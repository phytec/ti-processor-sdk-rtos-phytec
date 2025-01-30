#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

func_info.pl

=head1 SYNOPSIS

It produces a listing of all functions ranges and sizes in CSV format e.g.

    HOSTclose, trgdrv.c, 0x000070e0, 0x0000719c, 188
    HOSTlseek, trgdrv.c, 0x00005ec0, 0x00005fd8, 280

Basically it shows each function, the C/asm file it came from, its
function range (ie start -> end PC), and its size (end - start)

Works on executables, object files & libraries. However, the Low & High
PC are basically meaningless on libraries. Only on out files (ie after
linker placement) do these make sense. We keep Low & High PC in the output
for libraries simply to aid programmers who want to write 1 script to
post-parse the CSV produced from this perl script.

Works for any ISA.

Should be fairly fast since we strip out unneeded elements of the
large XML file before we process it. In Windows, works fastest when run on an
existing XML file as compared to piping (ofdNN -xg a.out | perl b.pl). In
Linux/Solaris, piping is just as fast (and avoids storing big XML files)

=head1 LIMITATIONS

=over 

=item *

Requires code be built with a CGT that produces DWARF debug e.g c6x CGT >= 5.0

=back

=head1 USAGE

func_info.pl [options] I<xml_file>

   ... OR ...

ofdXX -g -x I<out_file> | perl func_info.pl [options]

=head1 OPTIONS

 -h                 : Dump usage and quit
 --no_header        : Disable writing out header row in CSV format
 --sorted           : Print in func name sorted order (handy for diff's)
 --cg_xml_version   : Print out the version of the cg_xml package in use

=head1 CREATING XML INPUT 

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

Use the -x and -g options of OFD to create the XML file.  Example ...
       
      ofd6x -x -g -o=file.xml file.out

Alternatively, pipe the output from OFD into the script ...

      ofd6x -x -g file.out | perl func_info.pl
   ...

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

   -xg --xml_indent=0 --obj_display=none --dwarf_display=none,dinfo

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

=head1 PERL VERSION DETAILS

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

#=============================================================================
# Standard Perl pragmas that provide checks against common errors
#=============================================================================
use strict;
use warnings;

#=============================================================================
# Module for handling command line options
#=============================================================================
use Getopt::Long;

#=============================================================================
# Tell Perl the module XML_TI_OFD.pm in the same directory as this script
#=============================================================================
use FindBin;
use lib $FindBin::Bin;

#=============================================================================
# Bring in the module that handles parsing the OFD XML file
#=============================================================================
use XML_TI_OFD 3.11;

#=============================================================================
# Global variables
#=============================================================================
my(
   $xml_file,              # name of XML file or '-' for stdin
   $csv_no_header,         # whether to not output header in CSV format
   $func_sorted,           # output sorted by function name (useful for diff's)
   @func_recs,             # func records in an array for easy sort
  );


#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   my ($all_lines, $xml_data, $file_data);

   if ($^V lt v5.8.3)
   {
      print  "WARNING: Developed under Perl version 5.8.3.\n";
      printf "         May not work under version %vd\n\n", $^V;
   }

   process_cmd_line();

   #--------------------------------------------------------------------------
   # Strip out parts of the XML file that are not needed.  Detailed knowledge
   # of the XML and what this script uses is required to get this part right.  
   # This step runs fast, and reduces script run time drastically.  All the
   # remaining lines of the XML file come back in $all_lines.
   # GENERAL : Anything enclosed by these XML tags is removed
   # SECTION : Any <section>'s with this name are removed
   # DIE_TAG : Any <die>'s with this name are removed
   #--------------------------------------------------------------------------
   ofd_strip_xml($xml_file,
		 \$all_lines,
		 GENERAL => [qw(string_table
				raw_data
				symbol_table
				line_numbers)],
		 SECTION => [qw(.debug_line
				.debug_frame)],
		 DIE_TAG => [qw(DW_TAG_TI_assign_register
				DW_TAG_formal_parameter
				DW_TAG_variable)]);

   #--------------------------------------------------------------------------
   # Convert XML to Perl data structures
   #--------------------------------------------------------------------------
   $xml_data = ofd_process_xml_file($all_lines);

   foreach my $file_data (ofd_each_file($xml_data)) {
       get_func_info($file_data);
   }
   dump_func_info();

   exit(0);
}

#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($print_usage, $pn);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to master
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   $print_usage = $csv_no_header = $func_sorted = undef;

   GetOptions("h"         => \$print_usage,
	      "no_header" => \$csv_no_header,
              "sorted"    => \$func_sorted);

   if ($print_usage)
   {
      #-----------------------------------------------------------------------
      # Strip off path from program name
      #-----------------------------------------------------------------------
      $0 =~ m/([\w\.]+)$/;
      $pn = $1;

      warn "Usage: $pn [options] <xml file>   ... OR ...\n";
      die  "       ofdXX -x -g <input file> | perl $pn [options]\n";
   }

   if ($ARGV[0])
   {
      $xml_file = $ARGV[0];
      die "Cannot find $xml_file: $!" unless (-e $xml_file);
   }
   else
   {
      $xml_file = '-';
      warn "Reading from stdin ...\n";
   }
}


#=============================================================================
# get_func_info
# store the list of functions we find in the program
# This is keyed from DW_TAG_subprogram dies
#=============================================================================
sub get_func_info
{
    my ($file_data) = @_;
 
    my ($debug_info, $compile_unit, $die, @func_dies, $func_die);
    my ($func_name, $start_addr, $end_addr, $func_file, $func_file_try2,
        $size, $is_tramp);
 
    #--------------------------------------------------------------------------
    # Insure Dwarf debug info is in the XML
    #--------------------------------------------------------------------------
    if (!ofd_has_dwarf($file_data))
       { die "No DWARF info available.  Insure you use OFD -g flag.  Stopped"; }
 
    #--------------------------------------------------------------------------
    #--------------------------------------------------------------------------
    foreach $debug_info (ofd_each_debug_info($file_data))
    {
       #-----------------------------------------------------------------------
       # Go through each compile unit in the section
       #-----------------------------------------------------------------------
       foreach $compile_unit (@{$debug_info->{compile_unit}})
       {
	  # store off the filename, which we'll list with the function
	  # name in o/p
	  $func_file = $func_file_try2 = "";

	  my $cu_die = ofd_find_compile_unit_die($compile_unit);
	  return if (!$cu_die);
	  
	  ofd_find_attrs($cu_die, DW_AT_name => \$func_file);
	  
	  #-----------------------------------------------------------------
	  # only need DIE TAG subprogram since it has the attrs we need
	  # Searching every top-level DIE is not necessary.  Only the
	  # DW_TAG_compile_unit DIE will contain the functions.
	  #-----------------------------------------------------------------
	  @func_dies = ();

	  ofd_put_dies_in_array($cu_die, DW_TAG_subprogram => \@func_dies);
	  # bail to next iteration if non-existent DW_TAG_subprogram
	  next if (!scalar(@func_dies));

	  #-----------------------------------------------------------------
	  # Go through the function DIE's
	  #-----------------------------------------------------------------
	  foreach $func_die (@func_dies)
	  {
	      #--------------------------------------------------------------
	      # Collect the function name, start & end address
	      # Note that DW_AT_trampoline changed to add '_TI_' in later
	      # OFD versions, likewise for begin_file, hence we check for both
	      #--------------------------------------------------------------
	      $func_name = $start_addr = $end_addr = $is_tramp = undef;
	      ofd_find_attrs($func_die,
			     DW_AT_name           => \$func_name,
			     DW_AT_low_pc         => \$start_addr,
			     DW_AT_high_pc        => \$end_addr,
			     DW_AT_TI_trampoline  => \$is_tramp,
			     DW_AT_trampoline     => \$is_tramp,
			     DW_AT_begin_file     => \$func_file_try2,
			     DW_AT_TI_begin_file  => \$func_file_try2,
			     );
	      next if (!$func_name || !$start_addr || !$end_addr);
     
	      #--------------------------------------------------------------
	      # Ignore trampolines
	      #--------------------------------------------------------------
	      next if ($is_tramp && $is_tramp eq 'true');
     
	      #
	      # 99% of the time $func_file should be valid ie there should be
	      # a valid DW_AT_name filename for a compile_unit. However
	      # if (a) program is built with e.g. -pm (prog-wide compilation)
	      # then all files are globbed together and DW_AT_name is
	      # not defined, or (b) lin asm files have bogus DW_AT_name.of
	      # 'serial_asm' (CQ 14134).  In these cases we use
	      # DW_AT_begin_file
	      #
	      if ((!$func_file) || ($func_file =~ /serial_asm/)) {
		 $func_file = $func_file_try2;
	      }
	      
	      $start_addr = str_to_num($start_addr);
	      $end_addr   = str_to_num($end_addr);
	      $size       = $end_addr - $start_addr;
     
	      #
	      # CSV style output - easy for subsequent scripts to parse. The
	      # file name may contain commas hence needs dbl-quotes.
	      # func_name is also quoted since C++ templates may contain commas
	      # in the func signature e.g. templateClassName<T1, T2> 
	      #
	      my $file_quoted = "\"" . $func_file . "\"";
	      my $func_quoted = "\"" . $func_name . "\"";
	      
	      #
	      # could potentially print here and be done...but ordering of
	      # functions is not determininistic...which makes diff-checking
	      # tricky. So we store func's info into an array and print sorted
	      # by function name if the user passed that command line option
	      # 
	      my $rec;    # must re-initialize as new record each time
	      $rec->{func} = $func_quoted;
	      $rec->{file} = $file_quoted;
	      $rec->{s_addr} = $start_addr;
	      $rec->{e_addr} = $end_addr;
	      $rec->{size} = $size;             
	      
	      push @func_recs, $rec;      
	  }
       }
    }
}


#=============================================================================
# DUMP_FUNC_INFO - Dump func_info using array built previously
#=============================================================================
sub dump_func_info
{
    if (!($csv_no_header)) {
        printf("\n");
        printf("FunctionName,FileName,PCStartAddr,PCEndAddr,SizeInDec\n");
        printf("------------,--------,-----------,---------,---------\n");
    }

    my @funcs = $func_sorted ? sort {$a->{func} cmp $b->{func}} @func_recs : @func_recs;

    foreach my $rec (@funcs) {
        printf("%s,%s,0x%x,0x%x,%d\n", $rec->{func}, $rec->{file},
                                       $rec->{s_addr}, $rec->{e_addr},
                                       $rec->{size} );
   }
}

#=============================================================================
# STR_TO_NUM - Convert string to number.  Works regardless of the format the
#    number is written in.  Google "perl oct" for more detail.
#=============================================================================
sub str_to_num
{
   my ($str) = @_;

   return ($str =~ m/^0/) ? oct($str) : $str;
}


__END__

/*
 *
 * Copyright (C) 2011 Texas Instruments Incorporated - http://www.ti.com/ 
 * 
 * 
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright 
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the 
 *    documentation and/or other materials provided with the   
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/

