#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

extract_sections.pl

=head1 SYNOPSIS

Generates a C array of the data in an initialized
section e.g. will generate an array 
const unsigned char _cinit[0x7fc] = { 0x12, 0x34, 0x56... }
A pragma CODE_SECTION or DATA_SECTION is added for link placeability

=head1 USAGE

extract_sections --sect=.initSect1 --sect=.initSect2
--pragma=.xyz:initSect1 --pragma=.xyz:initSect2
filename.xml

Note that --sect can be shortform replaced with -s. Likewise for --pragma as -p

The --pragma (-p) is optional.

Example : 
- from a DOS prompt: -

[>] <ccstudiodir>\dosrun.bat

[>] ofd6x -x -o=irom.xml irom.out

[>] perl extract_sections.pl --sect=.const --pragma=.constrom irom.xml

If you need more initialized sections to be generated, just add them
via additional '--sect', '--pragma' entries e.g. '--sect=.text'

Alternatively, pipe the output from OFD into the script ...

ofd6x -x irom.out | perl extract_sections.pl --sect=.const --pragma=.constrom

If you pass in a garbage section (ie one that does not exist in the COFF 
file, the script will skip it and move on. However, it might be easiest
to run 'sectti app.out' simply to see the list of sections in your
output file.

Make sure to match up the number of --sect entries with
--pragma entries else results are undefined.

Output of above commands would be 2 files :- irom.out.c, irom.out.h

Note that internally, this script still needs the .out file because it 
has to seek & read the raw data from the COFF file (since the data itself is not in
the .xml). It gets the .out COFF filename internally from a field in the .xml.

=head1 OPTIONS

None

=head1 DEPENDENCIES

None except that you need to first run OFD which is only available
in Codegen Tools >= v5.0 for c6000, v3.0 for C55, v4.0 for C54

=head1 DESCRIPTION

Generates a C array of the data in an initialized
section e.g. will generate an array _const[] = {0x12, 0x34, 0x56...}

Useful in cases such as ROMing where you e.g. want to get LOG_printf
result data from ROM'ed code, hence need to preserve the .const
contents which hold the string data, when you build the final application

Uses ofd<ARCH> XML output's raw_data_ptr, raw_data_size fields 
in each output section to achieve this.

At a push, this could be used to generate a complete bootimage if you
passed in all initialized sections via '--sect=.const --sect=.text' etc
However you'd be better served using bootimage.pl which does this for you.

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -x --xml_indent=0 --obj_display=none,sections

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

=head1 KNOWN PROBLEM

You may see diagnostics similar to ...

 Warning: <symbol> element has non-unique value in 'name' key attribute:
      .text at extract_sections.pl line 154.
 Warning: <symbol> element has non-unique value in 'name' key attribute:
      .bss at extract_sections.pl line 154.
 Warning: <symbol> element has non-unique value in 'name' key attribute:
      .data at extract_sections.pl line 154.

These diagnostics do not result in the script failing or producing incorrect
output.

Fixing this problem requires a significant rewrite of this script.  Since
it is likely this script is no longer used, the work will not be done.  If
this judgement proves wrong, it will be revisited.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

#=============================================================================
# extract_sections.pl  
#=============================================================================

use warnings;
use strict;

#----------------------------------------------------------------------------
# Include various perl packages
#----------------------------------------------------------------------------
use XML::Simple;        # to enable hash tree access of XML content
use Getopt::Long;       # to enable args style --sect=.initializedSection1

#----------------------------------------------------------------------------
# Tell Perl the module XML_TI_OFD.pm in the same directory as this script
#----------------------------------------------------------------------------
use FindBin;
use lib $FindBin::Bin;
use XML_TI_OFD 3.11;    # for ofd_cg_xml_version

#=============================================================================
# Global variables
#=============================================================================
my $xml_file;           # the .xml file produced from running ofd6x -x
my $coff_file;          # COFF file so we can do seek, read to grab init'd data
my $xml_in_data;        # the hash tree structure returned by XMLin()
my @sect_names;         # 1 or more initialized sections user wants extracted
my @pragma_names;       # 1 or more pragmas to tag the initialized sections
my $sections;           # all sections in the TI output file 
my $cmd_line = "";      # command line passed for echoing to C file as comments

#=============================================================================
# MAIN ROUTINE
#=============================================================================
if ($^V lt v5.8.3)
{
   print  "WARNING: Developed under Perl version 5.8.3.\n";
   printf "         May not work under version %vd\n\n", $^V;
}

process_cmd_line();    # check command line options are valid

#--------------------------------------------------------------------------
# XML::Simple itself does not directly parse XML.  Instead, it relies on
# yet other Perl modules.  Experimentation has shown that XML::Parser is
# much faster than other modules.   So, test to see if XML::Parser is
# available.  If it is, make sure it gets used.  Use 'perldoc XML::Simple'
# to get more info on the PREFERRED_PARSER setting.
#--------------------------------------------------------------------------
eval { require XML::Parser; };
if (!$@) { $XML::Simple::PREFERRED_PARSER = 'XML::Parser'; }

$xml_in_data = XMLin($xml_file);  # Parse the XML output (comes from XML::Simple)

if (exists $xml_in_data->{object_file}->{elf})
   { die "$0 does not handle ELF files.\n"; }

$sections = $xml_in_data->{'object_file'}->{'ti_coff'}->{'section'};

$coff_file = $xml_in_data->{'object_file'}->{'name'};

open_output_files();    # open the .c result file
open_input_file($coff_file);     # open COFF file so we can seek, read init'd data

# print out the command line for archive purposes as comment header in C file
print  OUTC "// Command line used to generate this file was...\n";
print  OUTC "$cmd_line", "\n";

# get initialized sections and send to .[ch] with pragma CODE/DATA_SECTION
extract_initialized_sections(@sect_names);   

close_all_files();     

exit(0);



#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{  
   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   my $sect_prefix = "--sect";
   my $pragma_prefix = "--pragma";
   my ($sect_entry, $pragma_entry, $argv_entry); 	
   my ($print_usage, $pn);

   print("\n");

   # Store command line to later echo to the C file as comments for archiving
   $cmd_line = "// " . "$0" . "\n";          # store the script name
   foreach $argv_entry (@ARGV) {             # get args before GetOptions stripping
       $cmd_line .= "// " . $argv_entry . "\n";
   }   	
   
   $print_usage = 0;

   GetOptions ("h" => \$print_usage,         # -h prints out help for the script
               "section=s" => \@sect_names,  # (string) initialized section names
               "pragmasection=s" => \@pragma_names   # (string) sect names for pragma
               );   

   if ($print_usage)
   {
      #-----------------------------------------------------------------------
      # Strip off path from program name
      #-----------------------------------------------------------------------
      $0 =~ m/([\w\.]+$)/;
      $pn = $&;

      # Recall that you can pass in multiple --sect=, --pragma= pairs
      warn "Usage: $pn [-h] <xml file> $sect_prefix=.sect1 $pragma_prefix=.xyz:sect1 ... OR ...\n";
      die  "       ofdXX -x <obj or out file> | perl $pn $sect_prefix=.sect1 $pragma_prefix=.xyz:sect1 \n";
   }   

   if ($ARGV[0])    # 2 separate steps ie run OFD, then run perl script
   {
      $xml_file = $ARGV[0];
      die "Cannot find $xml_file: $!" unless (-e $xml_file);      
   }
   else             # here we pipe the XML output of OFD thru this script
   {
      $xml_file = '-';
      warn "Reading from stdin ...\n";
   }
   
   # pointless if you dont pass in at least 1 initialized section
   die "Need valid section_name(s). See -h Help: $!" unless (@sect_names > 0);
   
}


#=============================================================================
# OPEN_OUTPUT_FILES e.g. app.out.[ch]
#=============================================================================
sub open_output_files
{  
   open (OUTC, ">$coff_file.c") || die "Cannot open output file: $!";
   open (OUTH, ">$coff_file.h") || die "Cannot open output file: $!";
}


#-----------------------------------------------------------------------------
# Open input COFF file (.out) for reading
#-----------------------------------------------------------------------------
sub open_input_file
{  
   my ($in_coff_file) = @_;	           # grab subroutine paramaters
	
   open (OBJFILE, "<$in_coff_file") || die "Could not open input file $!";
   binmode(OBJFILE);
}   

#-----------------------------------------------------------------------------
# EXTRACT_INITIALIZED_SECTIONS - Extract contents of 1+ initialized sections
#-----------------------------------------------------------------------------
sub extract_initialized_sections
{
    my (@init_sects) = @_;              # grab subroutine paramaters
    my $buff;             				# raw initialized section data
    my $init_sect_entry;                # individual intialized section entry
    my ($sect, $size, $paddr, $ptr);    # section name, len, addr, ptr-to-data
    my ($is_data_section);              # is it a CODE or DATA section?
    my $loop_ctr = 0;                   # keep track of sect names .v. pragmas
            
    #-----------------------------------------------------------------------------
    # For each 'section' entity
    #-----------------------------------------------------------------------------
    foreach $init_sect_entry (@init_sects)
    {			
        #-------------------------------------------------------------------------
        # Find section's name, length and address
        #-------------------------------------------------------------------------
        $sect  = $sections->{$init_sect_entry};

        #-------------------------------------------------------------------------
        # check if sect name user passed in really exists (ie if its defined)
        #-------------------------------------------------------------------------
        if (not defined $sect) {
            print ("sect ", $init_sect_entry, " not in ", $coff_file, ". Skipping...\n"); 
            next;
        }
        
        $size  = hex($sect->{'raw_data_size'});
        $paddr = hex($sect->{'physical_addr'});
    
        #-------------------------------------------------------------------------
        # Find pointer to section's raw data inside the COFF file
        #-------------------------------------------------------------------------
        $ptr   = hex($sect->{'file_offsets'}->{'raw_data_ptr'});
    
        # is it a Code or a Data section?
        if (defined $sect->{'data'}) {
            $is_data_section = 1;
        } 
        else {
            $is_data_section = 0;
        }
    
        #-------------------------------------------------------------------------
        # if section is non-zero length and there is data in the file ...
        # and it is an initialized section ...
        # and it is not a copy section ...
        # (copy sections are information of some kind that is not to be
        # loaded to the target.  Best example is .cinit when linked under
        # -cr.  Dwarf debug sections are also copy sections.)
        # and not a dummy section ...
        # (dummy is a legacy section type typically not used)
        # and not a noload section.
        # (noload sections come up when you are doing particularly complicated
        # partial linking tricks.  As the name implies, the section is not to
        # loaded.)
        #-------------------------------------------------------------------------
        if ($size != 0 && $ptr != 0    &&
            (not defined $sect->{'bss'})   &&
            #    (not defined $sect->{'copy'})  &&
            (not defined $sect->{'dummy'}) &&
            (not defined $sect->{'noload'}))
        {
            #---------------------------------------------------------------------
            # only initialized sections will pass through here.
            # copy it's raw data from the COFF file into the .c file
            # and copy section's name and size into the .h file.
            # Change any non-alphanumeric characters in section name to '_' 
            # Preface the above with a pragma for placeability
            #---------------------------------------------------------------------
            seek(OBJFILE, $ptr, 0) || die "seek:$!";
            read(OBJFILE, $buff, $size);
            $init_sect_entry =~ s/[^A-Za-z0-9]/_/g;	        
            
            # --pragma is an optional cmd line param
            if (@pragma_names) {
                print_pragma($init_sect_entry, $is_data_section, $pragma_names[$loop_ctr]);
            }
            
            print_c_struct($init_sect_entry, $size, $paddr,  $buff);
        }
        
        $loop_ctr++;  # unconditionally increment ctr to track sect names .v. pragmas
    }
    
    print "Done. Created $coff_file.c and $coff_file.h \n";
}


#-----------------------------------------------------------------------------
# PRINT_PRAGMA - Add a pragma CODE_SECTION or DATA_SECTION
#-----------------------------------------------------------------------------
sub print_pragma
{
    my ($sectname, $is_data, $pragmaname) = @_;
	
    #-------------------------------------------------------------------------
    # Output pragma CODE_SECTION or DATA_SECTION to C file, depending on what 
    # type of section it is. Use the section pragma name passed in on cmd line
    #-------------------------------------------------------------------------
    if ($is_data) {
        printf OUTC "#pragma DATA_SECTION(%s, \"%s\")\n", $sectname, $pragmaname;
    }
    else {
	printf OUTC "#pragma CODE_SECTION(%s, \"%s\")\n", $sectname, $pragmaname;
    }    
}


#-----------------------------------------------------------------------------
# PRINT_C_STRUCT - Output contents of one section
#-----------------------------------------------------------------------------
sub print_c_struct
{
    my($sectname, $size, $paddr, $buff) = @_;
    my($i);

    # Output comment banner to C file
    print  OUTC "/*********************************************************\n";
    printf OUTC "** %s[0x%x]: paddr = 0x%08x \n",
                          $sectname, $size, $paddr;
    print  OUTC "**********************************************************/\n";
    
    #-------------------------------------------------------------------------
    # Output section header to C file
    #-------------------------------------------------------------------------
    printf OUTC "const unsigned char %s[0x%x] = {", $sectname, $size;

    #-------------------------------------------------------------------------
    # Output declaration to header file
    #-------------------------------------------------------------------------
    printf OUTH "extern const unsigned char %s[0x%x];\n", $sectname, $size;

    for ($i=0; $i<$size; $i++)
    {
        #---------------------------------------------------------------------
        # Start a new line every 13 bytes.  Comes to about 80 chars per line.
        #---------------------------------------------------------------------
        print OUTC "\n" if (($i % 13) == 0);
        print OUTC "0x". sprintf("%02x", ord(substr($buff, $i, 1))) . ", ";
    }

    print OUTC "};\n\n";    
}


#=============================================================================
# CLOSE_ALL_FILES - close any filehandles
#=============================================================================
sub close_all_files
{  
   close OBJFILE;
   close OUTC;
   close OUTH;
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

