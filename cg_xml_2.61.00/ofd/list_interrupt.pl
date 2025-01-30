#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

list_interrupt.pl

=head1 SYNOPSIS

Prints out the list of functions that use the interrupt keyword in a BIOS
application.

A sample output might be: -
 
 =============================================================
 Interrupt keyword detector utility
 =============================================================

 Scanning foo.out

 List of functions that uses the interrupt keyword:

 Function: foo
 File: foo.c
 Line: 30

 Function: foo1
 File: foo1.c
 Line: 9
 
=head1 USAGE

 list_interrupt <filename.xml> 

 Example (from a DOS prompt): -

    [>] <ccstudiodir>\dosrun.bat
    [>] ofd6x -xg -o=app.xml app.out
    [>] perl list_interrupt.pl app.xml

... OR ...

    [>] ofdXX -xg <out file> | perl list_interrupt.pl 

=head1 OPTIONS

 -h               : show usage
 --cg_xml_version : Print out the versio of the cg_xml package in use

=head1 DEPENDENCIES

The script will only work on executable files built with code generation tools
>= v5.1.6 for c6000, v3.2.3 for C55, v4.1.1 for C54, v4.1.2 for C2000

The script could take awhile to run depending on the size of the binary. It
takes time to process the xml output from the ofd utility, which typically can
get as large as tens of megabytes.
    
=head1 DESCRIPTION

The script prints out the list of functions that use the interrupt
keyword in a given application. C functions that use HWI_enter/HWI_exit
macro pairs or that are plugged into the HWI dispatcher should not use the
interrupt keyword, hence this script helps track down the culprits. Moreover,
CLK functions should similarly not use the interrupt keyword. See the DSP/BIOS
user guide's chapter on hardware interrupts and the HWI module documentation in
the DSP/BIOS API guide for more details.

For execution on Unix, the first line must be modified to point to
the directory where Perl is installed.

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

   -xg --xml_indent=0 --obj_display=none --dwarf_display=none,dinfo

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

use warnings;
use strict;

#----------------------------------------------------------------------------
# Include various perl packages
#----------------------------------------------------------------------------
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
my $dwarf_hash_ref;     # reference to tree of DWARF info
my $debug_sect_ref;     # reference to the debug info section in DWARF info
my $xml_file;           # file currently being processed
my @interrupt_data;     # interrupt keyword usage data collected

my $debug_mode = 0;     # flip to 1 to see lots of debug info

   
#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   my ($xml_in_data, $file_data, @debug_info_sections, $debug_info);

   if ($^V lt v5.8.3)
   {
      print  "WARNING: Developed under Perl version 5.8.3.\n";
      printf "         May not work under version %vd\n\n", $^V;
   }

   process_cmd_line();

   print "\n=============================================================\n";
   print ("Interrupt keyword detector utility \n");
   print "=============================================================\n";

   print "Processing ofd info...\n";

   # get well-formatted hash courtesy of the XML_TI_OFD module
   eval {$xml_in_data = ofd_process_xml_file($xml_file)};
   if ($@) {
       warn "ofd_process_xml_file failed ($@)...\n";
       warn "Input file is invalid. \n";
   }

   # Get name of .out file from the XML file
   print ("\nScanning ", ofd_get_input_file_name($xml_in_data), "\n");

   # ensure ofd<ISA> -g was run to get dwarf section
   $file_data = (ofd_each_file($xml_in_data))[0];

   if (!ofd_has_dwarf($file_data))
      { die "No DWARF info found. Make sure you use -g flag with OFD\n"; }
   if (!(@debug_info_sections = ofd_each_debug_info($file_data)))
      { die "No .debug_info sections found.  Stopped"; }

   foreach $debug_info (@debug_info_sections)
   {
      interrupt_search($debug_info);
   }

   # Print out results
   print_results(\@interrupt_data);

   exit(0);
}

#=============================================================================
# PROCESS_CMD_LINE
# Script can operate on XML file produced via ofd<ISA> -g or be piped
# via the ofd<ISA> command to avoid large intermediate XML file
#=============================================================================
sub process_cmd_line
{
    my ($print_usage);

    #-------------------------------------------------------------------------
    # If --cg_xml_version is requested, print that out to stderr
    #-------------------------------------------------------------------------
    ofd_cg_xml_version();
 
    $print_usage = 0;
 
    GetOptions("h" => \$print_usage);
 
    if ($print_usage) {
        warn "Usage: $0 [-h] <xml file>   ... OR ...\n";
        die  "       ofdXX -xg <obj or out file> | perl $0\n";
    }
 
    if ($ARGV[0]) {
        $xml_file = $ARGV[0];
        die "Cannot find $xml_file: $!" unless (-e $xml_file);
    }
    else {
        $xml_file = '-';   # read from stdin
        warn "Reading from stdin ...\n";
    }
   
}

#=============================================================================
# GET_DEBUG_SECT_REF
# Set a reference to the section which holds all the DWARF debug data
#=============================================================================
sub get_debug_sect_ref
{  
    my ($debug_sect_name) = @_;
    my $success = 0;
         
    foreach my $sect (@{$dwarf_hash_ref->{'section'}}) { 
        if ($sect->{'name'} eq $debug_sect_name) {
            $success = 1;           # found the section we're looking for
            $debug_sect_ref = $sect;
        }
    }
    
    # if we've reached here, then we didnt find .debug_info so bail out
    die "No ", $debug_sect_name, " section found. Error...\n" unless ($success eq 1);
}

#=============================================================================
# INTERRUPT_SEARCH
# Construct a list of functions that uses the interrupt keyword. See application
# note SPRAAB5 for details on DW_AT_TI_interrupt tag.
#=============================================================================
sub interrupt_search
{
    my ($debug_sect_ref) = @_;

    foreach my $compile_unit (@{$debug_sect_ref->{'compile_unit'}}) {
        foreach my $die_tag (@{$compile_unit->{'die'}}) {         
            next if ($die_tag->{'tag'} ne 'DW_TAG_compile_unit');	    
            foreach my $sub_die_tag (@{$die_tag->{'die'}}) {
                next if ($sub_die_tag->{'tag'} ne 'DW_TAG_subprogram');		
                my $int_attr = get_attr($sub_die_tag, 'DW_AT_TI_interrupt');
                my $beg_line_attr = get_attr($sub_die_tag, 'DW_AT_TI_begin_line');
                #
                # interrupt attribute is defined and set to true for functions
                # that use interrupt keyword
                #
                if (defined($int_attr) && ($int_attr eq 'true')) {
                    my $interrupt_hash_ref = {
                        'filename' => get_attr($die_tag, 'DW_AT_name'),
                        'line_num' => $beg_line_attr,
                        'function_name' => get_attr($sub_die_tag, 'DW_AT_name'),
                    };			
                    push(@interrupt_data, $interrupt_hash_ref);
                }
            }            
        }
    }

    if ($debug_mode) { XML_TI_OFD::dump_out(\@interrupt_data); };
}

#=============================================================================
# GET_ATTR - Given the attribute name find its {'value'}->{'ref'}
# or {'value'}->{'string'} or...
#=============================================================================
sub get_attr()
{
    my ($dw_tag, $attr_type) = @_; 

    foreach my $attr (@{$dw_tag->{'attribute'}}) {
        if ($attr->{'type'} eq $attr_type) {
            # return the single 'value' in an attribute DIE tag
            return ((values(%{$attr->{'value'}}))[0]);
        }
    }
    
    return undef;
}


#=============================================================================
# PRINT_RESULTS
# Print results from search
#=============================================================================
sub print_results
{ 
    my ($interrupt_data_ref) = @_;

    print "\nList of functions that uses the interrupt keyword:\n";
    
    if (@{$interrupt_data_ref} eq 0) {
        print "Nothing to report.\n";    
    }

    foreach my $idata (@{$interrupt_data_ref}) {
        print "\nFunction: ", $idata->{function_name}, "\n";
        print "File: ", $idata->{filename}, "\n";
        print "Line: ", hex($idata->{line_num}), "\n";
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

