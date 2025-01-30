#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

load_bps.pl

=head1 SYNOPSIS

Generate load address breakpoints

=head1 USAGE

load_bps.pl [options] I<xml file> I<*.gel file> I<sym1 sym2 ...>

   ... OR ...

ofdXX -x I<out file> | perl load_bps.pl [options] I<*.gel file> I<sym1 sym2 ...>

=head1 OPTIONS

 -h               : Output usage and quit
 --cg_xml_version : Print out the version of the cg_xml package in use

=head1 DESCRIPTION

What follows is mostly accurate.  Needs some work ...

The symbols must be global and defined in the file (storage class C_EXT).
Such symbols are associated with actual code or variable storage in the file.
Global symbols only referenced in a file (storage class C_EXTREF) are NOT
supported.  Similarly, static symbols that are only defined and used in a
single file are NOT supported.  The scoping mechanism required to support such
static symbols is not easy to build.

=head1 USING OFD

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

Use the -x option of OFD to create the XML file.  Example ...
       
      ofd6x -x -o=file.xml file.out

You can avoid creating the XML file by piping the output from OFD into the
script ...

      ofd6x -x file.out | perl load_bps.pl [options] I<sym1 sym2 ...>

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -x --xml_indent=0 --obj_display=none,sections,symbols

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

However, as of this writing, there is a bug in OFD which means, for ELF
files, the above options do not work.  Eventually, this bug will be fixed.
In the mean time use these options:

  -x --xml_indent=0 --obj_display=norawdata,nosymbols

These options are less than ideal, but still effective.

=head1 PERL VERSION DETAILS

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

#----------------------------------------------------------------------------
# Standard pragmas that check common error conditions
#----------------------------------------------------------------------------
use warnings;
use strict;

#=============================================================================
# Tell Perl the module XML_TI_OFD.pm in the same directory as this script
#=============================================================================
use FindBin;
use lib $FindBin::Bin;

#----------------------------------------------------------------------------
# Bring in the module that handles parsing the OFD XML file
#----------------------------------------------------------------------------
use XML_TI_OFD 3.11;

#----------------------------------------------------------------------------
# Other useful Perl modules
#----------------------------------------------------------------------------
use Getopt::Long;       # For handling command line options

#----------------------------------------------------------------------------
# Global variables
#----------------------------------------------------------------------------
my ($xml_file,          # XML file name, or '-' for stdin
    $gel_file,		# Output GEL file name
    %ext_symtab,        # Symbol table for C_EXT symbols
    %secttab);          # Section table, indexed by section name

#----------------------------------------------------------------------------
# Declare the functions
#----------------------------------------------------------------------------
sub process_cmd_line;
sub build_ext_symtab;
sub build_secttab;
sub open_files;
sub load_addr;
sub add_gel_function;
sub close_files;

#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   my ($xml_data, $sym_name, $all_lines, @file_list, $file_data);

   if ($^V lt v5.8.3)
   {
      print  "WARNING: Developed under Perl version 5.8.3.\n";
      printf "         May not work under version %vd\n\n", $^V;
   }

   process_cmd_line();

   #--------------------------------------------------------------------------
   # Remove parts of XML not needed.  Speeds up script considerably.
   #--------------------------------------------------------------------------
   ofd_strip_xml($xml_file,
		 \$all_lines,
		 GENERAL => [qw(string_table
				raw_data
				line_numbers
				optional_file_header
				dwarf)]);

   $xml_data = ofd_process_xml_file($all_lines);

   if (ofd_find_lib_name($xml_data))
      { die "$0 does not handle libraries\n"; }

   @file_list = ofd_each_file($xml_data);
   $file_data = $file_list[0];

   build_ext_symtab($file_data);
   build_secttab($file_data);
   open_files();

   #--------------------------------------------------------------------------
   # Compute load address one sym at a time
   #--------------------------------------------------------------------------
   printf("%20s : Load address\n", 'Symbol Name');
   print '-' x 20;
   print      " : ------------\n";
   foreach $sym_name (@ARGV) { load_addr($sym_name, $file_data); }

   close_files();
   print "Done\n";
   exit(0);
}

#=============================================================================
# PROCESS_CMD_LINE - Process the command line
#=============================================================================
sub process_cmd_line
{
   my ($print_usage, $pn);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   $print_usage = 0;
   GetOptions("h" => \$print_usage);

   if ($print_usage || @ARGV == 0) 
   {
      #-----------------------------------------------------------------------
      # Strip off path from program name
      #-----------------------------------------------------------------------
      $0 =~ m/([\w\.]+$)/;
      $pn = $&;
      
      die << "END_USAGE";
Usage: $pn [options] <xml file> <*.gel file> <sym1 sym2 ...>  ... OR ...
       ofdXX -x <obj or out file> | perl $pn <*.gel file> <sym1 sym2 ...>
END_USAGE
   }

   #--------------------------------------------------------------------------
   # If first arg ends with '.xml', then it is the input XML file
   #--------------------------------------------------------------------------
   if ($ARGV[0] =~ m/\.xml$/i)
   {
      $xml_file = shift @ARGV;
   }
   #--------------------------------------------------------------------------
   # Otherwise, presume XML file is being piped to stdin
   #--------------------------------------------------------------------------
   else
   {
      $xml_file = '-';
      warn "Reading XML from stdin ...\n";
   }

   #--------------------------------------------------------------------------
   # If second arg ends with '.gel', then it is the output GEL file path/name
   #--------------------------------------------------------------------------
   if ($ARGV[0] =~ m/\.gel$/i)
   {
      $gel_file = shift @ARGV;
   }
   #--------------------------------------------------------------------------
   # Otherwise, set GEL file name/path to default location
   #--------------------------------------------------------------------------
   else
   {
      $gel_file = "load_bps.gel";
      warn "Invalid or no output GEL file name specified. Using default name (load_bps.gel) and current path ...\n";
   }
   
   #--------------------------------------------------------------------------
   # Note list of syms is left in @ARGV
   #--------------------------------------------------------------------------
}

#=============================================================================
# BUILD_EXT_SYMTAB - Build symbol table of C_EXT symbols.  Cannot use symbol
#    table directly from COFF file.  It is an array and not a hash, which
#    makes lookup clumsy.  It is an array because it is possible to have
#    multiple symbols with the same name in the table.  In C/C++, it is
#    possible for function local symbols to have the same name as a global
#    symbol.  When the file is built with COFF debug, both symbols will
#    appear in the symbol table.
#=============================================================================
sub build_ext_symtab
{
   my ($file_data) = @_;
   my ($sym);

   foreach $sym (ofd_each_symbol($file_data))
   {
      #-----------------------------------------------------------------------
      # Only keeping track of C_EXT symbols.  C_EXT symbols correspond to
      # global objects that have a definition (actual code or variable
      # storage) in the file.  Thus, no need to worry about name collisions.
      # By definition, a C_EXT sym is unique.
      #-----------------------------------------------------------------------
      next unless (ofd_sym_is_global($sym));
      $ext_symtab{ofd_sym_name($sym)} = $sym;
   }
}

#=============================================================================
# BUILD_SECTTAB - Build the hash of sections indexed by section name
#=============================================================================
sub build_secttab
{
   my ($file_data) = @_;
   my ($sect);

   foreach $sect (ofd_each_section($file_data))
      { $secttab{ofd_sect_name($sect)} = $sect; }
}

#=============================================================================
# OPEN_FILES - Open the file(s) that contain the GEL funcs
#=============================================================================
sub open_files()
{
	open( GELFILE, "> $gel_file" ) or die "Can't open $gel_file : $!";	

	print GELFILE <<"EOStr";
/* load_bps.gel	: set\\clear breakpoints on symbol load addresses	   */
/* This file is *auto-generated*. Do not edit. Mods risk being overwritten */

menuitem \"Manage Breakpoints\"
EOStr
}

#=============================================================================
# LOAD_ADDR - Compute the load address for one symbol and calls add_gel_function
#=============================================================================
sub load_addr
{
   my ($sym_name, $file_data) = @_;
   my ($sym, $sym_sect_name, $sect, $sect_run, $sect_load,
       $sym_run, $sym_load);

   $sym = $ext_symtab{$sym_name};
   if (!$sym)
   {
      warn "Cannot find symbol \"$sym_name\", ignored\n";
      return;
   }

   $sym_sect_name = ofd_sym_sect_name($sym);
   $sect = $secttab{$sym_sect_name};
   if (!$sect)
   {
      warn "Cannot find section \"$sym_sect_name\" for symbol \"$sym_name\", ignored\n";
      return;
   }

   $sect_load = ofd_sect_load_addr($sect, $file_data);
   if (!defined($sect_load))  
      { return; }

   $sect_run = ofd_sect_run_addr($sect);
   $sym_run  = ofd_sym_value($sym);

   $sym_load = $sect_load + ($sym_run - $sect_run);

   printf("%20s : 0x%08x\n", $sym_name, $sym_load);

   add_gel_function($sym_name, $sym_load);
}

#=============================================================================
# ADD_GEL_FUNCTION - Write a pair of gel function that will add and remove a 
#	breakpoint for the specified load address to the file(s) that contain the GEL funcs
#=============================================================================
sub add_gel_function
{
    my ($sym_name, $sym_load) = @_;

	print GELFILE "\n";
	print GELFILE "hotmenu set_load_bp$sym_name() \n";
	print GELFILE "{ \n";
	printf GELFILE "	GEL_BreakPtAdd(0x%08x); \n", $sym_load;
	print GELFILE "} \n";
	print GELFILE "\n";
	print GELFILE "hotmenu clr_load_bp$sym_name() \n";
	print GELFILE "{ \n";
	printf GELFILE "	GEL_BreakPtDel(0x%08x); \n", $sym_load;
	print GELFILE "} \n";
}

#=============================================================================
# CLOSE_FILES - Close the file(s) that contain the GEL funcs
#=============================================================================
sub close_files
{
	close (GELFILE);
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

