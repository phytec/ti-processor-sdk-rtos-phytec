#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

coff_header_data.pl

=head1 SYNOPSIS

Shows the contents of a .out / .obj / .lib's file header COFF info.
Useful for checking that e.g. xyz.lib is indeed a 64 lib or 
is indeed big-endian or...

A sample output might be: -

    ===================================
    COFF File header data for hello.out
    ===================================
    target : 55L phase3
    endianNess : big
    targetFamily : 55x family
    isExecutable : true

=head1 USAGE

coff_header_data <filename.xml> 

Example : 
- from a DOS prompt: -

    [>] <ccstudiodir>\dosrun.bat
    [>] OFD -x -o=app.xml app.out
    [>] perl coff_header_data.pl app.xml

... OR ...

    [>] OFD -x <out file> | perl coff_header_data.pl

OFD stands for the object display utility, such as ofd6x for C6000 tools, 
or ofd55 for C5500 tools.

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -x --xml_indent=0 --obj_display=none,header

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

=head1 DESCRIPTION

Shows the contents of a .out or .obj or .lib's file header COFF info.
Useful for checking that e.g. xyz.lib is indeed a 64 lib or 
is indeed big-endian or...

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

use warnings;
use strict;

#----------------------------------------------------------------------------
# Include various perl packages
#----------------------------------------------------------------------------
use Getopt::Long;       # no command line options right now

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
# hashes of target type settings
#=============================================================================
my %target_types = (    
    0x97           => {
        name           => 'Arm family',
        # invalid combos for Arm include :-
        #    (a) >= Arm9 with TI-ABI

        # in older Arm CGT 2.x Arm version wasnt encoded. Defaults to Arm7
        0x0            => 'Arm7 : TI-ABI',
	    
        # Note 1 : -md no-dual-mode CGT switch only has effect when 
        # (i) its Arm7, and (ii) 'new' CGT ie v4.x Arm CGT
        0x20           => 'Arm7 : TI-ABI : No Dual Mode',
        0x30           => 'Arm7 : Arm9-ABI : No Dual Mode',
        0xA0           => 'Arm7 : TI-ABI : Dual-Mode',
        0xB0           => 'Arm7 : Arm9-ABI : Dual-Mode',
        0xD0           => 'Arm9 : Arm9-ABI',
        0xF0           => 'Arm11 : Arm9-ABI' 
    },
    0x99           => {
        name           => 'C6000 family',
        0x0            => 'C62',
        0x40           => 'C67',
        0x60           => 'C64P',
        0xA0           => 'C64 rev1.1',
        0xC0           => 'C67P'
    },
    0x98           => {
        name           => '54x family',
        # Note 1 : sadly cant read far/near attribute from file_header cpu_flags
        # Note 2 : only deduction we can make from C54 cpu_flags is if a build
        #          flipped the -v548 (or greater) flag. This changes the 
        #          cpu_flags value. But its somewhat redundant as the -v processor
        #          flag is not necessary on 54xx devices (as opposed to 54x).
        #          Hence we do not print specific device target data for C54.
    },
    0x9C           => {
        name           => '55x family',
        0xC0           => '55 phase3',
        0xE0           => '55L phase3',
        0x30           => '55H phase3'
    }
);


#=============================================================================
# Global variables
#=============================================================================
my $xml_file;           # the .xml file produced from running ofd6x -x
my $coff_file;          # COFF file so we can do seek, read to grab init'd data
my $xml_in_data;        # the hash tree structure returned by XMLin()
my $file_header;        # The TI COFF file header data
my %header_hash;        # hash accumulating all the file header stuff

my $debug_mode = 0;     # flip to 1 to see lots of debug info

my $all_lines;

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
# Remove parts of XML not needed.  Speeds up script considerably.
#--------------------------------------------------------------------------
ofd_strip_xml($xml_file,
	      \$all_lines,
	      GENERAL => [qw(string_table
			     raw_data
			     symbol_table
			     line_numbers
			     optional_file_header
			     section
			     dwarf)]);


$xml_in_data = ofd_process_xml_file($all_lines);

# Get name of .out file from the XML file
my $filename = ofd_get_input_file_name($xml_in_data);

print "===========================================\n";
print "COFF File header data for ", $filename,   "\n";
print "===========================================\n";

get_file_header();

process_file_header();

print_coff_data();

exit(0);



#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($print_usage);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   $print_usage = 0;

   GetOptions("h" => \$print_usage);

   if ($print_usage)
   {
      warn "Usage: $0 [-h] <xml file>   ... OR ...\n";
      die  "       ofdXX -x <obj or out file> | perl $0\n";
   }

   if ($ARGV[0])
   {
      $xml_file = $ARGV[0];
      die "Cannot find $xml_file: $!" unless (-e $xml_file);
   }
   else
   {
      $xml_file = '-';   # read from stdin
      warn "Reading from stdin ...\n";
   }
}


#=============================================================================
# GET_FILE_HEADER - Return the file header.  If a library, returns the file
#    header of from the first file in the library.
#=============================================================================
sub get_file_header
{
    my (@file_list, $file_data);

    @file_list = ofd_each_file($xml_in_data);
    $file_data = $file_list[0];

    if (exists $file_data->{elf})
    {
       die "$0 does not handle ELF.\n" .
           "For ELF, use a command similar to:\n" .
	   "   % ofd470 --obj_display=none,header,battrs file.out\n";
    }

    $file_header = $file_data->{'ti_coff'}->{'file_header'};
    
    if ($debug_mode) { XML_TI_OFD::dump_out(\$file_header); }   # all file_header data
}

#=============================================================================
# PROCESS_FILE_HEADER 
#=============================================================================
sub process_file_header
{  
    # get endianness of the file
    $header_hash{endianNess} = $file_header->{'endian'};
    if ($debug_mode) { print "endianNess : ", $header_hash{endianNess}, "\n"; }
    
    # store whether or not its an executable
    if (exists $file_header->{'exec'}) {
	$header_hash{isExecutable} = 'true';
    }
    else {
	$header_hash{isExecutable} = 'false';
    }

    if ($debug_mode) { print "Is this an executable? : ", $header_hash{isExecutable}, "\n"; }

    # in order to determine whether we're a 55 or a 64 or a...first need
    # to get the ISA family since some of the cpu_flags overlap across ISAs
    my $targ_family = hex($file_header->{'target_id'});
    if (exists $target_types{$targ_family}) {
	    
	$header_hash{targetFamily} = $target_types{$targ_family}{'name'};
	if ($debug_mode) { print "targetFamily : ", $header_hash{targetFamily}, "\n"; }
	
	my $target_value = hex($file_header->{'cpu_flags'});
	if (exists $target_types{$targ_family}{$target_value}) {
	    $header_hash{target} = $target_types{$targ_family}{$target_value};
	    if ($debug_mode) { print "target : ", $header_hash{target}, "\n"; }
	}
	else {
	    $header_hash{target} = "unknown target";
	}	    
    }
    else {
	$header_hash{targetFamily} = 'unknown CPU family';
        $header_hash{target} = 'unknown target';
    }
}


#=============================================================================
# PRINT_COFF_DATA - simply echoes the accumulated file header data to stdout
#=============================================================================
sub print_coff_data
{
    foreach my $key (sort keys %header_hash) {
        printf("%s : %s\n", $key, $header_hash{$key});
    }
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

