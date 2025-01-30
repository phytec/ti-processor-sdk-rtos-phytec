#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

mkhex4bin.pl

=head1 SYNOPSIS

Creates a command file for the TI hex conversion utility (such as hex470)
that is useful for converting a TI COFF or ELF .out file to a binary
.bin file.

=head1 USAGE

Rarely invoked alone.  Usually invoked as part of running the accompanying
batch file tiobj2bin.bat.  Instructions on using tiobj2bin.bat are at the top
of that file.  

If invoked directly ...

mkhex4bin.pl [options] I<xml file>

   ... OR ...

ofdXX -x I<out file> | perl mkhex4bin.pl [options]

=head1 OPTIONS

 -h               : Output usage and quit
 --cg_xml_version : Print out the version of the cg_xml package in use

=head1 CREATING XML INPUT 

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

Use the -x option of OFD to create the XML file.  Example ...
       
      ofd6x -x -o=file.xml file.out

Alternatively, pipe the output from OFD into the script ...

      ofd6x -x file.out | perl mkhex4bin.pl

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -x --xml_indent=0 --obj_display=none,sections,header,segments

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
   $xml_data,              # XML data structure
   $file_data,             # Points to first (usually the only) file in the XML
   $target,                # Target specific information
   $low_addr,              # lowest address of initialized memory
   $high_addr,             # highest address of initialized memory
  );

#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   if ($^V lt v5.8.3)
   {
      print  "WARNING: Developed under Perl version 5.8.3.\n";
      printf "         May not work under version %vd\n\n", $^V;
   }

   process_cmd_line();
   process_xml_file();
   check_and_get_target_props();
   process_sect_info();
   print_hex_file();

   exit(0);
}

#=============================================================================
# PROCESS_CMD_LINE - Handle the command line
#=============================================================================
sub process_cmd_line
{
   my ($print_usage, $pn);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   GetOptions("h" => \$print_usage);

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
# PROCESS_XML_FILE
#=============================================================================
sub process_xml_file
{
   my ($all_lines, @file_list);

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
   # Punt if it is a library.  If it is an .obj file, well, that doesn't make
   # much sense, but it is handled just like an executable .out file.
   #--------------------------------------------------------------------------
   if (ofd_find_lib_name($xml_data))
   {
      die "Cannot handle libraries, stopped";
   }

   @file_list = ofd_each_file($xml_data);
   $file_data = $file_list[0];
}

#=============================================================================
# GET_TARGET_PROPS - The way addresses and section sizes are counted is
#    inconsistent among targets.  Thus, this table of target specific
#    scaling factors.  The operations within the script are in terms of 8-bit
#    bytes.  The input values, when multiplied by the input scale factors,
#    change them to 8-bit bytes.  Output values are similarly scaled.  The
#    interesting ones are C5400 and C2000.  For reasons I don't get, the
#    addresses in the OFD XML output are in terms of 16-bit words, but
#    section lengths are in terms of 8-bit bytes.  And while you might expect
#    the hex utility to operate in terms of 16-bit words, and thus the output
#    scale factor to be 0.5, it isn't.  The C5400 and C2000 hex utilities
#    expect the values in the command file to be in terms of 8-bit bytes.
#    I carefully checked all this out.  Perhaps it is a bug.  But it has 
#    been this way for many years, and cannot be changed.
#=============================================================================
BEGIN {
   my %illegal_target = (
      C5400 => 1,
      C5500 => 1,
      'PRU' => 1,
   );

   my %target_table = (
      ARM => {
	 input_addr_scale  => 1,
	 input_size_scale  => 1,
	 output_addr_scale => 1,
	 output_size_scale => 1,
      },
      C6000 => {
	 input_addr_scale  => 1,
	 input_size_scale  => 1,
	 output_addr_scale => 1,
	 output_size_scale => 1,
      },
      C2000 => {
	 input_addr_scale  => 2,
	 input_size_scale  => 1,
	 output_addr_scale => 1,
	 output_size_scale => 1,
      },
      MSP430 => {
	 input_addr_scale  => 1,
	 input_size_scale  => 1,
	 output_addr_scale => 1,
	 output_size_scale => 1,
      },
      'C5500+' => {
	 input_addr_scale  => 1,
	 input_size_scale  => 1,
	 output_addr_scale => 1,
	 output_size_scale => 1,
      },
   );

   sub check_and_get_target_props
   {
      my $cpu = ofd_machine_name($xml_data);

      #-----------------------------------------------------------------------
      # C54x, C55x, and PRU do not have a linear address space.  Each has
      # separate program and memory pages.  The same numeric address can have
      # different contents on each page.  Binary files cannot support this
      # scheme.  Binary files presume every memory address has unique contents.
      #-----------------------------------------------------------------------
      # That die intentionally ends with \n.  This prevents Perl from adding
      # the source line number to the diagnostic.  This is not a panic message.
      # Users are likely to see this one.  Showing a line number makes an
      # already confusing situation that much worse.
      #-----------------------------------------------------------------------
      if (exists($illegal_target{$cpu}))
         { die "Binary files do not work for target $cpu\n"; }

      if (!exists($target_table{$cpu}))
         { die "Unexpected target: $cpu"; }

      $target = $target_table{$cpu};
   }
}

#=============================================================================
# PROCESS_SECT_INFO - Process all the sections in the file to find the lowest
#    and highest load (not run!) address of initialized memory
#=============================================================================
sub process_sect_info
{
   my ($sect_data, $size, $load_addr);

   $low_addr = undef;
   foreach $sect_data (ofd_each_section($file_data))
   {
      $size = ofd_sect_size($sect_data) * $target->{input_size_scale};

      #-----------------------------------------------------------------------
      # Only handle sections of non-zero size that are initialized.  This
      # skips sections like .bss, .debug_<something>, etc.
      #-----------------------------------------------------------------------
      next unless ($size && ofd_sect_is_init($sect_data));

      $load_addr =   ofd_sect_load_addr($sect_data, $file_data)
                   * $target->{input_addr_scale};

      if (!defined($low_addr))
      {
         $low_addr  = $load_addr;
	 $high_addr = $load_addr + $size;
      }
      else
      {
	 if ($load_addr < $low_addr)
	    { $low_addr = $load_addr; }
	 if ($load_addr + $size > $high_addr)
	    { $high_addr = $load_addr + $size; }
      }
   }

   if (!defined($low_addr))
   {
      $low_addr = $high_addr = 0;
   }
}

#=============================================================================
# PRINT_HEX_FILE - Emit hex utility command file syntax which defines a single
#    memory range with the limits of the lowest and highest address of
#    initialized memory range.
#=============================================================================
sub print_hex_file
{
   my ($orig_str, $len_str);

   $orig_str = sprintf("0x%x", $low_addr * $target->{output_addr_scale});
   $len_str  = sprintf("0x%x",   ($high_addr - $low_addr)
                               * $target->{output_size_scale});

   print << "END_HEX";
ROMS {
   all_mem: o = $orig_str, l = $len_str
}
END_HEX
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

