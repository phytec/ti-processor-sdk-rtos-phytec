#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

bootimage.pl

=head1 SYNOPSIS

Converts a TI executable file (.out) into hexadecimal data contained in
arrays written in C.  The executable can be in COFF or ELF format.

=head1 USAGE

bootimage.pl [-h] I<xml file>

   ... OR ...

ofdXX -x I<out file> | perl bootimage.pl

=head1 OPTIONS

 -h               : Output usage and quit
 --section=name   : Only process named sections
 --pragma=name    : Use a DATA_SECTION pragma to allow placing the section
 --cg_xml_version : Print out the version of the cg_xml package in use

If --section is not used, all initialized sections are processed.  If --pragma
is used, then the number of --pragma options must equal the number of --section
options.  The Nth section is affected by the Nth pragma.

Short options are also accepted, e.g. -s for --section.  Except
--cg_xml_version can never be abbreviated.

=head1 DESCRIPTION

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

Use the -x option of OFD to create the XML file.  Example ...
       
      ofd6x -x -o=file.xml file.out

You can avoid creating the XML file by piping the output from OFD into the
script ...

      ofd6x -x file.out | perl bootimage.pl

On Unix systems, piping the output from OFD works just fine.  On Windows
systems, however, the pipe method can be much slower.

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -x --xml_indent=0 --obj_display=none,sections

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

=head1 OUTPUT FILES

There are two output files: a C file contains the arrays and a header file
contains extern declarations of those arrays.  The output file names are
formed by appending '.c' and '.h', respectively, to the name of the .out
file.  For example ...

      ofd6x -x -o=file.xml file.out
      bootimage.pl file.xml

creates 'file.out.c' and 'file.out.h'.  If you pipe the the XML in ...

     ofd6x -x file.out | perl bootimage.pl

the output files are the same: 'file.out.c' and 'file.out.h'.  Note the 
name of the of the .out file is one of the data items pulled from the XML
information.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

#=============================================================================
# Standard pragmas that check common error conditions
#=============================================================================
use warnings;
use strict;

#=============================================================================
# Declare the functions 
#=============================================================================
sub process_cmd_line;
sub open_files;
sub process_coff_file;
sub process_elf_file;
sub close_files;

#=============================================================================
# Modules used
#=============================================================================
use Getopt::Long;                     # for handling command line options
use MIME::Base64;                     # for decoding raw data in XML file

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
my (
    $xml_file,         # from the command line
    $reading_obj,      # reading data directly from OBJ?
    %sect_to_pragma,   # key: only process these sections, value: name of
                       # section to put the array in, if any
   );

#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   my ($all_lines, $xml_data, @file_list, $file_data);
   process_cmd_line();

   #--------------------------------------------------------------------------
   # Remove parts of XML not needed.  Speeds up script considerably.
   #--------------------------------------------------------------------------
   ofd_strip_xml($xml_file,
                 \$all_lines,
                 GENERAL => [qw(string_table
		                symbol_table
		                line_numbers
		                optional_file_header
		                dwarf)]);

   #--------------------------------------------------------------------------
   # Convert XML to Perl data structure
   #--------------------------------------------------------------------------
   $xml_data = ofd_process_xml_file($all_lines);

   if (ofd_find_lib_name($xml_data))
   {
      die "Cannot handle libraries, stopped";
   }
   @file_list = ofd_each_file($xml_data);
   $file_data = $file_list[0];

   open_files($file_data);
   process_out_file($file_data);
   close_files();

   exit(0);
}

#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($print_usage, $pn, @section_names, @pragma_names,
       $num_sects, $num_pragmas, $i);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   $print_usage = 0;
   GetOptions("h" => \$print_usage,
              "section=s" => \@section_names,
	      "pragmasection=s" => \@pragma_names);

   if ($print_usage)
   {
      #-----------------------------------------------------------------------
      # Strip off path from program name
      #-----------------------------------------------------------------------
      $0 =~ m/([\w\.]+)$/;
      $pn = $1;
      
      die << "END_USAGE";
Usage: $pn <options> <xml file>   ... OR ...
       ofdXX -x <obj or out file> | perl $pn <options>
END_USAGE
   }

   $num_sects   = scalar(@section_names);
   $num_pragmas = scalar(@pragma_names);

   if ($num_sects && $num_pragmas && $num_sects != $num_pragmas)
      { die "Must specify a pragma for all sections, or none of them"; }

   for ($i = 0; $i < $num_sects; $i++)
   {
      if ($pragma_names[$i])
         { $sect_to_pragma{$section_names[$i]} = $pragma_names[$i]; }
      else
         { $sect_to_pragma{$section_names[$i]} = undef; }
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
# OPEN_FILES
#=============================================================================
sub open_files()
{
   my ($file_data) = @_;

   my ($filename, $sect);

   #--------------------------------------------------------------------------
   # Get name of .out file from the XML file
   #--------------------------------------------------------------------------
   $filename = $file_data->{'name'};

   #--------------------------------------------------------------------------
   # Open output files for writing.  Note these files will end up in the
   # same directory as the .out file.  That is, if the script is invoked ...
   # 
   #      ofdXX -x /path/to/file.out | perl bootimage.pl
   #      
   # then $filename = '/path/to/file.out'.
   #--------------------------------------------------------------------------
   open (OUTC, ">$filename.c") or die "Cannot open $filename.c for output: $!";
   open (OUTH, ">$filename.h") or die "Cannot open $filename.h for output: $!";

   #--------------------------------------------------------------------------
   # Check if raw data is directly embedded in the XML.  If it is, no need
   # to open the COFF file.  Note this method it almost always slower than
   # reading the raw data from the COFF file.
   #--------------------------------------------------------------------------
   $reading_obj = undef;

   foreach $sect (ofd_each_section($file_data))
      { return if (defined ofd_sect_raw_data($sect)); }

   #--------------------------------------------------------------------------
   # Raw data is not embedded in the XML, so open input COFF file (.out) for
   # reading.  Note it is possible to get into a bit of trouble if you do the
   # following ...
   # 
   #    ofdXX -x -o=file.xml file.out
   #    cd /some/other/dir
   #    bootimage.pl /path/to/file.xml
   #
   # At this point $filename = 'file.out', and this open will fail.  This is
   # judged to be an unlikely scenario.
   #--------------------------------------------------------------------------
   $reading_obj = 1;
   open (OBJFILE, "<$filename") or die "Cannot open $filename for input: $!";
   binmode(OBJFILE);
}

#=============================================================================
# PROCESS_OUT_FILE
#=============================================================================
sub process_out_file
{
   my ($file_data) = @_;
   my($sect, $size, $paddr, $sectname, $ptr, $buff, $pragma);

   #--------------------------------------------------------------------------
   #--------------------------------------------------------------------------
   foreach $sect (ofd_each_section($file_data))
   {
       $size     = ofd_sect_size($sect);
       $ptr      = ofd_sect_raw_data_ptr($sect);
       $sectname = ofd_sect_name($sect);
       $pragma   = undef;

       next unless ($size && defined($ptr) && ofd_sect_is_init($sect));
       if (%sect_to_pragma)
       {
          next unless (exists $sect_to_pragma{$sectname});
	  $pragma = $sect_to_pragma{$sectname};
       }

       $paddr = ofd_sect_run_addr($sect);

       if ($reading_obj)
       {
	  seek(OBJFILE, $ptr, 0)      || die "seek: $!";
	  read(OBJFILE, $buff, $size) || die "read: $!";
       }
       else
       {
	  $buff = decode_base64(${ofd_sect_raw_data($sect)});
       }

       print_c_struct($sectname, $size, $paddr, $buff, $pragma);
   }
}

#=============================================================================
# PRINT_C_STRUCT - Output contents of one section
#=============================================================================
sub print_c_struct
{
    my($sectname, $size, $paddr, $buff, $pragma) = @_;
    my(@buff_bytes, $i);

    #-------------------------------------------------------------------------
    # Change any non-alphanumeric characters in section name to '_'
    #-------------------------------------------------------------------------
    $sectname =~ s/[^A-Za-z0-9]/_/g;

    #-------------------------------------------------------------------------
    # Output section header to C file
    #-------------------------------------------------------------------------
    print  OUTC "/*********************************************************\n";
    printf OUTC "** %s[0x%x]: paddr = 0x%08x \n",
                          $sectname, $size, $paddr;
    print  OUTC "**********************************************************/\n";

    if ($pragma)
       { print OUTC "#pragma DATA_SECTION($sectname, \"$pragma\")\n"; }

    printf OUTC "const unsigned char %s[0x%x] = {", $sectname, $size;

    #-------------------------------------------------------------------------
    # Output declaration to header file
    #-------------------------------------------------------------------------
    printf OUTH "extern const unsigned char %s[0x%x];\n", $sectname, $size;

    #-------------------------------------------------------------------------
    # Convert the binary data into an array of unsigned 8-bit bytes
    #-------------------------------------------------------------------------
    @buff_bytes = unpack("C*", $buff);

    for ($i = 0; $i < scalar(@buff_bytes); $i++)
    {
        #---------------------------------------------------------------------
        # Start a new line every 13 bytes.  Comes to about 80 chars per line.
        #---------------------------------------------------------------------
        print  OUTC "\n" if (($i % 13) == 0);
	printf OUTC "0x%02x, ", $buff_bytes[$i];
    }

    print OUTC "};\n\n";  
}

#=============================================================================
# CLOSE_FILES
#=============================================================================
sub close_files
{
   close OUTH;
   close OUTC;

   close OBJFILE if ($reading_obj);
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

