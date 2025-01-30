#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

gen_shm_cmd.pl

=head1 SYNOPSIS

Perl script that reads in an XML file representation
of a linker map file and spits out MEMORY sections that are in 
shared memory, so they can be shared with another processor (eg Arm+DSP).
Basically avoids the double-duty problem ie if SHMEM section start
address or length changes, then you'd otherwise need to update the 
other processor's SHMEM org, len cmd file definitions (Arm or DSP)

A sample output in a generated .cmd might be: -

	/* This file is *auto-generated*. Do not edit. Mods risk being overwritten */ 
	/* These sections are shared between Arm & DSP */ 
	MEMORY {
		SHMEM1: origin = 0x00030000, len = 0x00010000 
		SHMEM2: origin = 0x00060000, len = 0x00020000 
	}

=head1 USAGE

   gen_shm_cmd.pl <--start_shm=0x1234> <--end_shm=0x5678> <xml file> <output_cmd_file>

=head1 ADDITIONAL OPTIONS

 --cg_xml_version : Print out the version of the cg_xml package in use

=head1 DESCRIPTION

Generate the XML version of the map file with the linker option
--xml_link_info=<file>

For execution on Unix, the first line must be modified to point to
the directory where Perl is installed.

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

Script assumes that the Shared Memory is contiguous ie will not work
if there's a strange case of SHMEM, NONSHMEM, SHMEM.

Script will output the actual physical start and length of the shared 
MEMORY sections in the xml file within the ranges specified by 
start_shm, end_shm ie its not just a straight cmd-style duplicate
of the start_shm, end_shm passed in. Hence, advice is to pass 
start_shm, end_shm addresses consistent with range of 
SHMEM in device data sheet, thus encompassing full range
and ensuring that all SHMEM sections will be captured.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

use strict;
use warnings;

use Getopt::Long;       # to enable args style --start_shm=0x1234

# use this perl module to enable simple xml tag structure traversal 
use XML::Simple;

#=============================================================================
# Tell Perl the module XML_TI_MAP.pm is in the same directory as this script
#=============================================================================
use FindBin;
use lib $FindBin::Bin;

use XML_TI_MAP 1.01 qw(cg_xml_version);

#=============================================================================
# Global variables
#=============================================================================
my ($xml_file, $xml_data);
my $start_shm_addr = 0x0;
my $end_shm_addr = 0xffffffff;
my $mem_map_ref;
my %mem_sect_hash;           # store a hash of the SHMEM sections
my $shm_output_file = "";         
my $debug_mode = 0;          # flip to 1 to see lots of debug info

#=============================================================================
# MAIN ROUTINE
#=============================================================================
if ($^V lt v5.8.3)
{
   print  "WARNING: Developed under Perl version 5.8.3.\n";
   printf "         May not work under version %vd\n\n", $^V;
}

process_cmd_line();

$xml_data   = XMLin($xml_file);
$mem_map_ref = $xml_data->{'placement_map'}->{'memory_area'};

# if ($debug_mode) { dump_out(\$mem_map_ref); }   # the whole shebang...

create_shm_cmd();
print_cmd_mem_to_file();

exit(0);

#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($print_usage);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, write that out to stderr
   #--------------------------------------------------------------------------
   cg_xml_version();

   $print_usage = 0;

   GetOptions ("h" => \$print_usage,
               "start_shm=o" => \$start_shm_addr,  # hex SHM start addr
               "end_shm=o" =>   \$end_shm_addr);   # hex SHM end addr
   die "Usage: $0 --start_shm=0x1234 --end_shm=0x5678 xml_file output_cmd_file \n" unless (@ARGV == 2);

   if ($print_usage)
   {
      die "Usage: $0 [-h] --start_shm=0x1234 --end_shm=0x5678 <xml file> <output_cmd_file>\n";
   }
   
   $xml_file = $ARGV[0];
   die "Cannot find $xml_file: $!" unless (-e $xml_file);
   
   if ($debug_mode) { printf ("SHM start= 0x%08x, SHM end= 0x%08x \n", $start_shm_addr, $end_shm_addr); }

   #--------------------------------------------------------------------------
   # Open file for output file for Shared Memory extract
   #--------------------------------------------------------------------------
   $shm_output_file = $ARGV[1];
   open(SHM_OUTFILE, ">$shm_output_file") || die "Can't open $shm_output_file : $!";
}

#=============================================================================
# CREATE_SHM_CMD
#=============================================================================
sub create_shm_cmd
{
   my ($mem_area_key, $mem_sect_origin_ref, $mem_sect_length_ref);

   # For each memory_area in the hash...
   foreach $mem_area_key (sort keys %$mem_map_ref)
   {
       if ($debug_mode) { print ("memory_area name : ", $mem_area_key , "\n"); }            
       
       # only include a given MEMORY section if its in the SHMEM range
       # ...to avoid too much logic, assume that if start addr is in 
       # range then its a valid SHMEM Memory range.
       $mem_sect_origin_ref = oct($mem_map_ref->{$mem_area_key}->{'origin'});
       $mem_sect_length_ref = oct($mem_map_ref->{$mem_area_key}->{'length'});
       if ($debug_mode) { printf ("o=0x%08x \t l=0x%08x \n", $mem_sect_origin_ref, $mem_sect_length_ref); }            
       
       if ( ($mem_sect_origin_ref >= $start_shm_addr) && ($mem_sect_origin_ref < $end_shm_addr) ) {	       
		   
           # store all data for each SHMEM section. Why not just print it straight to the file? 
           # Because storing it in a new hash enables output print to a file in eg a different format
           # hence could eg output for gcc toolchain instead of cmd MEMORY format
           $mem_sect_hash{$mem_area_key} = $mem_map_ref->{$mem_area_key};	       
       }       
   }
}

#=============================================================================
# PRINT_CMD_MEM_TO_FILE - prints out TI cmd file format for MEMORY sections.
# However if you have shared mem sections between Arm&Dsp and you are using
# eg gcc on Arm-side, simply modify this print-to-file function
# to output in the format you need for such a toolchain.
#=============================================================================
sub print_cmd_mem_to_file
{
   my ($mem_area_key, $mem_sect_origin_ref, $mem_sect_length_ref);

   printf SHM_OUTFILE ("/* This file is *auto-generated*. Do not edit. Mods risk being overwritten */ \n");
   printf SHM_OUTFILE ("/* These sections are shared between Arm & DSP */ \n");
   
   # begin the cmd file MEMORY section bracketing for Shared Mem sections
   print SHM_OUTFILE ("MEMORY {", "\n");

   if ($debug_mode) { dump_out(\%mem_sect_hash); }

   # traverse over all SHMEM memory_area's 
   foreach $mem_area_key (sort keys %mem_sect_hash)
   {
      $mem_sect_origin_ref = oct($mem_sect_hash{$mem_area_key}->{'origin'});
      $mem_sect_length_ref = oct($mem_sect_hash{$mem_area_key}->{'length'}); 
      printf SHM_OUTFILE ("\t%s: origin = 0x%08x, len = 0x%08x \n", $mem_area_key, $mem_sect_origin_ref, $mem_sect_length_ref);
   }

   # close out the cmd file MEMORY bracketing
   print SHM_OUTFILE ("}", "\n");
   
   # close the file handle for Shared Mem cmd file 
   close SHM_OUTFILE;
}


#=============================================================================
# DUMP_OUT - Debug routine.  Write to stdout the Perl data structures which
#    correspond to the XML.
#=============================================================================
sub dump_out
{
   my ($xml_data) = @_;
   use Data::Dumper;

   $Data::Dumper::Indent   = 1;
   $Data::Dumper::Sortkeys = 1;

   print "=============================================================\n";
   print "XML Data\n";
   print "=============================================================\n";
   print Dumper($xml_data);
   print "\n";

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

