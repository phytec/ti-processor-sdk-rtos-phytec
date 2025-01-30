#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

sectti.pl

=head1 SYNOPSIS

Prints the name, size, and base address of each section in an object
(.obj), executable (.out), or library (.lib) file.  Any section not loaded
or used on the target is ignored.

=head1 USAGE

sectti.pl [options] I<xml file>

   ... OR ...

ofdXX -x I<obj, out, or lib file> | perl sectti.pl [options]

=head1 OPTIONS

 -h               : Output usage and quit
 --csv            : CSV format
 --no_header      : Disable writing out header row in CSV format
 --cg_xml_version : Print out the version of the cg_xml package in use

=head1 CREATING XML INPUT 

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

Use the -x option of OFD to create the XML file.  Example ...
       
      ofd6x -x -o=file.xml file.out

Alternatively, pipe the output from OFD into the script ...

      ofd6x -x file.out | perl sectti.pl

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -x --xml_indent=0 --obj_display=none,sections,header,segments

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

If you are using an older version of OFD which does not support all those
options, try:

  -x --xml_indent=0 --obj_display=none,sections,header

That will work fine, so long as you are not working with ELF object files.  

If you are working with ELF object files, and you have an older version
of OFD, then try:

  -x --xml_indent=0 --obj_display=norawdata,nosymbols

These options are less than ideal, but still helpful.

=head1 SIZING UNITS

Sizes are always reported as the number of 8-bit bytes.  On targets that
can address 8-bit bytes, this is not an issue.  For other targets, this may
be an issue.  On C5400, for example, which can only address 16-bit words,
to compute the number of target words used by a section, divide the size
reported by sectti.pl by 2 i.e. 124 bytes == 62 words.

=head1 CSV FORMAT

CSV stands for Comma Separated Values.  It is a text-based file format
that can be loaded into many useful utilities, most notably MS-Excel.
Get the data in CSV format with the option --csv.

=head2 CSV FORMAT FIELDS

The fields you get vary with whether the input file is a library, and
whether it executable. 

If the file is a library, the library name is output at the beginning of
each line.  Otherwise, no library name is output.

If the file is not a library and is executable (as indicated by the
file header flags), the load and run address of each section is at the
end of each line.  Otherwise, addresses are not output.  

Fields always output are file name, section name, section type (CODE, 
DATA, UDATA == Uninitialized Data), and section size.

=head2 CSV HEADER ROW

By default, an appropriate CSV header row is output.  Disable with the option
--no_header.  Use this option to accumulate data across multiple files ...

   ofd6x -x first.lib  | perl sectti.pl --csv > results.csv
   ofd6x -x second.lib | perl sectti.pl --csv --no_header >> results.csv
   ...

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
   %stype_data,            # data bin'd by type of section
   $csv_format,            # enable CSV output
   $csv_no_header,         # whether to not output header in CSV format
  );

#=============================================================================
# MAIN ROUTINE
#=============================================================================
if ($^V lt v5.8.3)
{
   print  "WARNING: Developed under Perl version 5.8.3.\n";
   printf "         May not work under version %vd\n\n", $^V;
}

process_cmd_line();
process_sect_info();
print_totals();

exit(0);

#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($print_usage, $pn);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   $print_usage = $csv_format = $csv_no_header = undef;
   GetOptions("h"         => \$print_usage,
              "csv"       => \$csv_format,
	      "no_header" => \$csv_no_header);

   if ($print_usage)
   {
      #-----------------------------------------------------------------------
      # Strip off path from program name
      #-----------------------------------------------------------------------
      $0 =~ m/([\w\.]+$)/;
      $pn = $&;

      warn "Usage: $pn [options] <xml file>   ... OR ...\n";
      die  "       ofdXX -x <input file> | perl $pn [options]\n";
   }

   if (!$csv_format && $csv_no_header)
   {
      warn "Argument --no_header requires use of --csv. Ignored.\n";
      $csv_no_header = undef;
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
# PROCESS_SECT_INFO
#=============================================================================
sub process_sect_info
{
   my ($all_lines, $xml_data, $lib_name, $is_exec,
       $file_data, $sect_data, $file_name, $sname, $size, $load_addr,
       $run_addr, %stype_to_name, $stype, $flag);
   my ($sect_hdr);

   %stype_to_name = (
      text  => 'CODE',
      data  => 'DATA',
      bss   => 'UDATA',
      'N/A' => 'N/A',
      );

   #--------------------------------------------------------------------------
   # Remove parts of XML not needed.  Speeds up script considerably.
   #--------------------------------------------------------------------------
   ofd_strip_xml($xml_file,
                 \$all_lines,
                 GENERAL => [qw(string_table
		                raw_data
		                symbol_table
		                line_numbers
		                optional_file_header
		                dwarf)]);

   #--------------------------------------------------------------------------
   # Convert XML to Perl data structure
   #--------------------------------------------------------------------------
   $xml_data = ofd_process_xml_file($all_lines);

   #--------------------------------------------------------------------------
   # If a library, print library name
   #--------------------------------------------------------------------------
   if ($lib_name = ofd_find_lib_name($xml_data))
      { print_lib_header($lib_name); }

   #--------------------------------------------------------------------------
   # Set flag to print load and run addresses if file is executable
   #--------------------------------------------------------------------------
   $is_exec = ofd_is_executable($xml_data);

   #--------------------------------------------------------------------------
   # For CSV format, print header row
   #--------------------------------------------------------------------------
   print_csv_header($lib_name, $is_exec);

   #--------------------------------------------------------------------------
   # For each file (in what might be a library)
   #--------------------------------------------------------------------------
   foreach $file_data (ofd_each_file($xml_data))
   {
      $file_name = $file_data->{name};
      print_fname_and_header($file_name, $is_exec);

      #-----------------------------------------------------------------------
      # Process each section in the file
      #-----------------------------------------------------------------------
      foreach $sect_data (ofd_each_section($file_data))
      {
	 $size  = ofd_sect_size($sect_data);

	 if (   !$size
	     || !(ofd_sect_is_code($sect_data) || ofd_sect_is_data($sect_data)))
	    { next; }

         $sname    = ofd_sect_name($sect_data);
	 $stype    = get_stype($sect_data);
	 $run_addr = ofd_sect_run_addr($sect_data);

         if ($is_exec && ofd_sect_is_init($sect_data))
	    { $load_addr = ofd_sect_load_addr($sect_data, $file_data); }
	 else
	    { $load_addr = $run_addr; }

	 #--------------------------------------------------------------------
	 # Print size info
	 #--------------------------------------------------------------------
	 print_size_info(
	    lib_name   => $lib_name,
	    file_name  => $file_name,
	    sect_name  => $sname,
	    sect_type  => $stype_to_name{$stype},
	    size       => $size,
	    load_addr  => $load_addr,
	    run_addr   => $run_addr,
	    print_addr => $is_exec);

	 #--------------------------------------------------------------------
	 # Account for total by section type
	 #--------------------------------------------------------------------
         if ($stype ne 'N/A')
	    { $stype_data{$stype} += $size; }
      }
   }
}

#=============================================================================
# GET_STYPE
#=============================================================================
sub get_stype
{
   my ($sect_data) = @_;

   if (ofd_sect_is_code($sect_data))    { return 'text'; }
   if (ofd_sect_is_data($sect_data))
   {
      if (ofd_sect_is_init($sect_data)) { return 'data'; }
      else                              { return 'bss';  }
   }
    
   return 'N/A';
}

#=============================================================================
# PRINT_TOTALS
#=============================================================================
sub print_totals
{
   my ($key);
   my (%expanded_stype_names);

   return if ($csv_format);

   %expanded_stype_names = (
      text => 'Code',
      data => 'Initialized Data',
      bss  => 'Uninitialized Data');

   print "\n";
   print "------------------------------------------------------------\n";
   print "Totals by section type\n";
   print "------------------------------------------------------------\n";

   foreach $key (sort keys %stype_data) 
   {
      printf("%20s : %10d  0x%08x\n", $expanded_stype_names{$key},
             $stype_data{$key}, $stype_data{$key});
   }
}

#=============================================================================
# PRINT_LIB_HEADER
#=============================================================================
sub print_lib_header
{
   my ($lib_name) = @_;

   return if ($csv_format);

print "====================================================================\n";
print "REPORT FOR LIBRARY: $lib_name\n";
print "====================================================================\n";
print "\n";
}

#=============================================================================
# PRINT_CSV_HEADER
#=============================================================================
sub print_csv_header
{
   my ($lib_name, $print_addr) = @_;
   my (@header_columns);

   return if (!$csv_format || $csv_no_header);

   @header_columns = ();
   if ($lib_name)
      { push @header_columns, qw(Library); }

   push @header_columns, qw(Filename Section Type Size);

   if ($print_addr)
      { push @header_columns, qw(Load Run); }

   csv_print(@header_columns);
}

#=============================================================================
# PRINT_FNAME_AND_HEADER
#=============================================================================
sub print_fname_and_header
{
   my ($file_name, $print_addr) = @_;

   return if ($csv_format);

   if ($print_addr)
   {
   print << "END_HEADER1";

************************************************************
REPORT FOR FILE: $file_name
************************************************************
                Name : Size (dec)  Size (hex)  Type   Load Addr   Run Addr
-------------------- : ----------  ----------  ----   ----------  ----------
END_HEADER1
   }
   else
   {
   print << "END_HEADER2";

************************************************************
REPORT FOR FILE: $file_name
************************************************************
                          Name : Size (dec)  Size (hex)  Type
------------------------------ : ----------  ----------  ----
END_HEADER2
   }
}

#=============================================================================
# PRINT_SIZE_INFO
#=============================================================================
sub print_size_info
{
   my (%size_info) = @_;

   my (@columns);

   #--------------------------------------------------------------------------
   # Print size info for regular text format
   #--------------------------------------------------------------------------
   if (!$csv_format)
   {
      if ($size_info{print_addr})
      {
	 printf("%20s : %10d  0x%08x  %-5s  0x%08x  0x%08x\n",
		$size_info{sect_name},
		$size_info{size},
		$size_info{size},
		$size_info{sect_type},
		$size_info{load_addr},
		$size_info{run_addr});
      }
      else
      {
	 printf("%30s : %10d  0x%08x  %-5s\n",
		$size_info{sect_name},
		$size_info{size},
		$size_info{size},
		$size_info{sect_type});
      }
      
      return;
   }

   #--------------------------------------------------------------------------
   # Print for CSV
   #--------------------------------------------------------------------------
   @columns = ();
   if ($size_info{lib_name})
   {
      push @columns, $size_info{lib_name};
   }

   push @columns, $size_info{file_name},
                  $size_info{sect_name},
		  $size_info{sect_type},
		  $size_info{size};

   if ($size_info{print_addr})
   {
      #-----------------------------------------------------------------------
      # Output addresses in hex format.  Note Excel cannot sort hex numbers,
      # but that is unlikely to be an issue.
      #-----------------------------------------------------------------------
      push @columns, sprintf("0x%08x", $size_info{load_addr}),
                     sprintf("0x%08x", $size_info{run_addr});
   }

   csv_print(@columns);
}

#=============================================================================
# CSV_PRINT - Print an array's worth of data in CSV format
#=============================================================================
sub csv_print
{
   my (@columns) = @_;
   my ($need_quotes);

   $need_quotes = csv_check(@columns);

   if (!$need_quotes)
   {
      print join ',', @columns;
      print "\n";
   }
   else
   {
      print '"'; 
      print join '","', @columns;
      print '"', "\n";
   }
}

#=============================================================================
# CSV_CHECK - Check for unusual characters that can mess up CSV output.  These
#    are very unlikely to occur.  But if they do, the CSV output will be
#    wrong, and the error hard to diagnose.  So, just check for the weird
#    characters, and if present, quit immediately.  Also, return whether
#    any commas are in the data, and thus quotes are required when writing
#    the CSV line.
#=============================================================================
sub csv_check
{
   my (@columns) = @_;
   my ($check_string, $col);

   $check_string = join ' ', @columns;

   #--------------------------------------------------------------------------
   # I'm pretty sure the only chars to worry about are " and newline.  But 
   # more could be added over time.
   #--------------------------------------------------------------------------
   if ($check_string =~ m/["\n]/)
   {
      warn "Unexpected chars in output.  Dumping one column per line ...\n";

      foreach $col (@columns)
         { warn "\"$col\"\n"; }

      die "Stopped";
   }

   #--------------------------------------------------------------------------
   # Quotes are required if any of the data contains a comma
   #--------------------------------------------------------------------------
   return $check_string =~ m/,/;
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

