#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

lib_versions.pl

=head1 SYNOPSIS

Prints out version of code generation tools used to compile each library.

A sample output might be: -

Library: c:\chan.l64

Built by:
TMS320C6x C/C++ Codegen Unix v5.1.0 Copyright (c) 1996-2004 Texas Instruments Incorporated

=head1 USAGE

lib_versions [options] <list of libraries or xml files> 

Example : 
- from a DOS prompt: -

    [>] <ccstudiodir>\dosrun.bat
    [>] perl lib_versions.pl rts6000.l*

.. OR ..

    [>] <ccstudiodir>\dosrun.bat
    [>] ofd6x -xg -o=rts6000.lib.xml rts6000.lib
    [>] perl lib_versions.pl -x rts6000.lib.xml

=head1 OPTIONS

 -h: show usage
 -o: show version info on a per object file basis
 -x: do not run ofd internally. Arguments are a list of xml files from ofd utility
 --cg_xml_version : Print out the version of the cg_xml package in use

=head1 DEPENDENCIES

The script will only work on library files built with code generation tools
>= v5.0 for c6000, v3.0 for C55, v4.0 for C54

The script could take awhile to run depending on the size of the libraries. It
takes time to process the xml output from the ofd utility, which typically can
get as large as tens of megabytes.
    
=head1 DESCRIPTION

For each library listed, the script would print out the version of the codegen
tools used to build it. By default, it prints out the version used for the
first object file it finds in the library file.

Use the -o switch to get version information on a per object file basis in
case the library is a combination of C and assembly code, and the assembly 
code modules do not have any Dwarf information.  The compiler RTS library
is such library.  You may also use -o for the rare case where different object
files were compiled using different code generation tools.

Specifying the -x switch allows users to pass in ofd-generated xml files to the
script instead of the library files. This is generally faster since ofd does
not need to be run by the script.

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
my $xml_in_data;        # the hash tree structure returned by XMLin()
my $dwarf_hash_ref;     # reference to tree of DWARF info
my $current_file;       # name of file currently processed
my @version_data;       # version data collected
my $print_objs;         # print version info on a per object basis
my $xml_mode;           # arguments are xml files produced by ofd

my $debug_mode = 0;     # flip to 1 to see lots of debug info

   
#=============================================================================
# MAIN ROUTINE
#=============================================================================
if ($^V lt v5.8.3)
{
   print  "WARNING: Developed under Perl version 5.8.3.\n";
   printf "         May not work under version %vd\n\n", $^V;
}

process_cmd_line();  # get options and check command line options/args are valid


# Print out cgtools version used for each lib in each group
print "\n=============================================================\n";
print ("Codegen tools version reporting utility for libraries \n");
print "=============================================================\n";
foreach my $arg (@ARGV) {
    get_lib_info($arg);
}

print_version_info(\@version_data);

exit(0);


#=============================================================================
# GET_LIB_INFO
# Get version info for a particular group of files
#=============================================================================
sub get_lib_info
{
    my ($group_of_files) = @_;
   
    my @list_of_files = <${group_of_files}>; #create an array of filenames

    # For each file in the group, get its version info.
    foreach my $file (@list_of_files) {
        my ($xml_lines, $success);
        if (!$xml_mode) {

            #-------------------------------------------------------------------
	    # If use a newer OFD command that supports the new options for
	    # filtering the XML, change this next line to ...
	    #$xml_lines = `ofd6x -xg --xml_indent=0 --obj_display=none --dwarf_display=none,dinfo $file`;
            #-------------------------------------------------------------------
	    $xml_lines = `ofd6x -x -g $file`;
        }

        #-----------------------------------------------------------------------
        # Filter out parts of the XML file that are not needed. Detailed
	# knowledge of the XML and what this script uses is required to get this
	# part right.  
        # This step runs fast, and reduces script run time drastically.  All the
        # remaining lines of the XML file come back in $all_lines.
	# The first call filters out all the sections and DIE's except those
	# listed.  The second call removes XML chunks with that name.
        #-----------------------------------------------------------------------
	ofd_filter_xml(($xml_mode) ? $file : undef,
	               \$xml_lines,
	               SECTION => [qw(.debug_info)],
		       DIE_TAG => [qw(DW_TAG_compile_unit)]);

        ofd_strip_xml(undef,
		      \$xml_lines,
		      GENERAL => [qw(string_table
				     symbol_table
				     line_numbers)]);
				
        print "Processing ", $file, ".......\n";
        eval {$xml_in_data = ofd_process_xml_file($xml_lines)};
        if ($@) {
            warn "ofd_process_xml_file failed ($@)...\n";
            warn "Input file is invalid. Remember to use the -x switch for xml input files.\n";
            warn "failed to get version from ", $file, ". Skipping...\n\n";
            next;
        }
        $success = get_version_info($file);
        if (!$success) {
            warn "failed to get version from ", $file, ". Skipping...\n";
        }
    }
}


#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
    my ($print_usage);

    #-------------------------------------------------------------------------
    # If --cg_xml_version is requested, print that out to stderr
    #-------------------------------------------------------------------------
    ofd_cg_xml_version();
 
    GetOptions( "h" => \$print_usage,
                "o" => \$print_objs,
                "x" => \$xml_mode);
    
    if ($print_usage) {
        die  "Usage: $0 [options] <list of library files (or xml files)>\n";
    }
 
    my @list_of_files = <$ARGV[0]>;  #globbing to ensure at least 1 valid list
    
    die "Invalid file specification: $!" unless (-e $list_of_files[0]);
  
}


#=============================================================================
# GET_VERSION_INFO
# DWARF2 Spec : Sec 3.1
# Get the cgtools version used for compiling each library and populate the
#     @version_data array. Each entry is a hash with lib name and version
# Returns 0 on failure, 1 on success
#=============================================================================
sub get_version_info
{  
    ($current_file) = @_;
    my ($success);
    
    if ($print_objs) {  # Get the version info for each object file in library
        foreach my $objfile_data (ofd_each_file($xml_in_data))
        {
            $success += get_version_info_of_obj($objfile_data);
        }
    }
    else {  # if we only want to look at the first object file in each lib
        $success = get_version_info_of_obj((ofd_each_file($xml_in_data))[0]);
    }

    if ($debug_mode) { XML_TI_OFD::dump_out(\@version_data); };
    return ($success) ? 1 : 0;
}


#=============================================================================
# GET_VERSION_DATA_OF_OBJ
# Get version data for an entry in the object file array built by ofd_each_file()
# Returns 0 on failure, 1 on success
#=============================================================================
sub get_version_info_of_obj
{
    my ($objfile_data) = @_;
    my ($libname, $objname, $saw_debug_info, $cu_die, $version,
        @debug_info_sections, $debug_sect_ref, $compile_unit,
        $version_hash_ref);

    # Get the library name from xml data
    $libname= ofd_find_lib_name($xml_in_data);    
    $objname = $objfile_data->{name};

    # Make sure Dwarf info is present
    if (   !ofd_has_dwarf($objfile_data)
        || !(@debug_info_sections = ofd_each_debug_info($objfile_data))) {
       report_dwarf_error();
       return 0;
    }

    $saw_debug_info = undef;

    foreach $debug_sect_ref (@debug_info_sections)
    {
       $saw_debug_info = 1;

       # Only look at the first compile unit (i.e. file) in the section
       # Look at the DW_TAG_compile_unit DIE for that file. 
       $compile_unit = $debug_sect_ref->{compile_unit}->[0];
       $cu_die = ofd_find_compile_unit_die($compile_unit);
       next if (!$cu_die);

       # Compiler build info is in the DW_AT_producer attribute
       $version = undef;
       ofd_find_attrs($cu_die, DW_AT_producer => \$version);
       next if (!$version);
       
       $version_hash_ref = {
	   libname  => $libname,
	   objname  => $objname,
	   version  => $version
       };

       push(@version_data, $version_hash_ref);

       # In ELF files can have multiple .debug_info sections.  Quit now to
       # avoid reporting on info found in another .debug_info section.
       return 1;
    }

    if (!$saw_debug_info)
    {
       warn "No .debug_info section found in file $objname\n";
       return 0;
    }

    if (!$cu_die)
    {
       warn "No compile unit DIE found in file $objname\n";
       return 0;
    }

    if (!$version)
    {
       warn "No DW_AT_producer attribute found in $objname\n";
       return 0;
    }

    return 1;
}

#=============================================================================
# PRINT_VERSION_INFO
# Print the version_data structure names and their members
#=============================================================================
sub print_version_info
{ 
    my ($version_data_ref) = @_;

    print "\nVersion Info...\n";
    
    if (@{$version_data_ref} eq 0) {
        print "Nothing to report.\n";    
    }
    
    foreach my $vdata (@{$version_data_ref}) {
        print "\n***************************************************\n";
        print "Library: ", $vdata->{libname}, "\n";
        if ($print_objs) {
            print "Object: ", $vdata->{objname}, "\n";
        }
        print "Built by: \n", $vdata->{version}, "\n";
    } 
}

#=============================================================================
# REPORT_DWARF_ERROR - Reports error when expected dwarf info is not found.
#=============================================================================
sub report_dwarf_error
{
    warn "DWARF info missing/incomplete. Possibly invalid input file. ",
        "May be Stabs debug format.\n";
    if ($xml_mode) {
        warn "Make sure you use -g flag with OFD. \n";
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

