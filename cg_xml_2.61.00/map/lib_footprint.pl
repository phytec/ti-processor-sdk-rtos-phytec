#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

lib_footprint.pl

=head1 SYNOPSIS

Perl script that reads in an XML file representation of a linker map file
and reports how much memory each library occupies.

=head1 USAGE

   lib_footprint.pl [options] <xml file>

=head1 OPTIONS

 --sect_classes=<file> : Configuration file that classifies sections
 --cg_xml_version      : Print out the version of the cg_xml package in use

=head1 HOW TO CREATE THE XML FILE

Generate the XML version of the map file with the linker option
--xml_link_info=<file>

You must use a relatively recent linker which supports the --xml_link_info
option.  However, there is NO requirement as to which version of the tools
were used to create the libraries being linked in.  In other words, this
script will work even if some of the libraries were built with old compiler
tools.

=head1 NOTES ON THE OUTPUT

=head2 STACK AND HEAP SIZES

Gets fooled on overall stack and heap (used by malloc) sizes.  That is not
defined in input sections where all the size information is gathered.  Those
sizes are assigned directly by the linker to the output section.  

=head2 DIFFERENT RESULTS VS. OTHER METHODS

Other methods for measuring the size of a library may get a different
result than this script.  This script only measures the size of library
modules actually B<used> by the application.  If a library module is
never referenced, the linker does not bring it in, this script does not
see that module, and it does not count when summing up the size of the
library.  Other methods for measuring a library typically add up the
the size of every module in the library with no consideration for the
possibibility one or more modules may not be linked in.

=head2 HOLES ARE NOT COUNTED

This script considers the size of each section as it is pulled out of library,
before it is combined with other input sections to form a larger output section.
It is possible for holes to occur between input sections.  Such holes are
not counted by this script.

Consider the following example:

=over

=item *

A library named example.lib has 2 files in it

=item *

Each file contains one code section that is 50 bytes long

=item *

Each code section is required to be aligned on a 64 byte boundary

=item *

Both code sections are linked in to the final .out file being measured

=item *

The code sections are combined into an output section named .example_text

=item *

The output section .example_text does not include any other input sections

=back

This script will say that example.lib contributes 100 bytes to the final link.
Measuring the size of the output section .example_text with other methods 
(such as using sectti.pl from the OFD directory) will show that the section
.example_text is 114 bytes.  Why the difference?

A hole of 64 - 50 = 14 bytes, in the section .example_text, lies between the
code sections from the two files in example.lib.  That hole is not counted by
this script.  

It is, in general, impossible for such holes to be counted by this script.
They can occur anywhere, for nearly any reason.  They can occur when input
sections from libraries (which are counted by this script) are combined
together with input sections from normal object files (which are not counted
by this script).  In practice, size of such holes is rarely a factor.

=head2 NOTE ON LINKING WITH -CR

If you link with -cr, use --sect_classes to mark .cinit as a section
to IGNORE (details below).

=head1 HOW SECTIONS ARE CLASSIFIED

The final report shows how much code, data, and uninitialized data each
library contributes to overall code size.  This is achieved by classifying
each output section as one of CODE, DATA, or UDATA.  The classification
scheme is a simple one based on the names of the sections.  The default
scheme is illustrated in the subroutine "default_sect_classes".  This should
cover many common cases.  Since not all cases are covered, the option
"--sect_classes=file" is provided to give you the ability to change
the section classification (file syntax below).  This script should be
accompanied by the example section classification file "classes.txt".  It
defines the section classifications equivalent to the script default.  Thus,
it serves as a good starting point for additions or modifications.

Note you can also mark sections as IGNORE.  As the name implies, such
sections are ignored.

If -cr (instead of -c) is used on the link, then the .cinit section
should be marked IGNORE.  This script cannot tell the difference
between a .cinit section that is loaded in memory (linked with -c)
and one that is only a copy section (linked with -cr) the loader reads
to perform the global variable initialization at load time.  

=head2 SYNTAX OF SECTION CLASSIFICATION CONFIGURATION FILE

   <section name> <space or ',' or '='> <CODE|DATA|UDATA|IGNORE>

Each section specification must be contained on a single line

Optional space can surround the section name or classification

Blank lines are ignored

'#' is for comments.  Anything on a line after '#' is ignored.

=head1 PERL VERSION REQUIRED

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

#=============================================================================
# Possible Improvements:
#
# - A sure-fire method for classifying sections is to go read the OFD utility
#   XML output and look at the section header flags for the section.  That
#   clearly defines how each output section is classified.
# - An enhancement filed with DDTS (SDSsq39046) requests that the section
#   section header flag information be included in the XML map file.  When
#   that request is met, this script can be changed to use that information.
#=============================================================================

use strict;
use warnings;
use Getopt::Long;

#=============================================================================
# Tell Perl the module XML_TI_MAP.pm is in the same directory as this script
#=============================================================================
use FindBin;
use lib $FindBin::Bin;

use XML_TI_MAP 1.01 qw(process_xml_file cg_xml_version);

#=============================================================================
# Global variables
#=============================================================================
my ($output_sections, $input_sections, $files);
my ($xml_file, $sect_class_file);
my (%is_code, %is_data, %is_udata, %is_ignore);

#=============================================================================
# %lib_data is the central hash data structure built by the script.  It is
# keyed by the name of the library.  The value is a reference to hash keyed
# by section type, which is in turn a reference to hash keyed by section
# name.  You could think of it as a 3 dimensional hash:
#    lib_data{library_name}->{section_type}->{section_name}
#=============================================================================
my (%lib_data);

#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   my ($xml_data);

   if ($^V lt v5.8.3)
   {
      print  STDERR "WARNING: Developed under Perl version 5.8.3.\n";
      printf STDERR "         May not work under version %vd\n\n", $^V;
   }

   process_cmd_line();
   process_sect_classes($sect_class_file);
   $xml_data = process_xml_file($xml_file);

   #--------------------------------------------------------------------------
   # Set up some key references into the XML-based data structure
   #--------------------------------------------------------------------------
   $output_sections = $xml_data->{'logical_group_list'}->{'logical_group'};
   $input_sections  = $xml_data->{'object_component_list'};
   $files           = $xml_data->{'input_file_list'};

   compute_lib_footprint();
   print_lib_footprint();

   exit(0);
}

#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($pn);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, write that out to stderr
   #--------------------------------------------------------------------------
   cg_xml_version();

   $sect_class_file = undef;
   GetOptions("sect_classes=s" => \$sect_class_file);

   if (@ARGV != 1)
   {
      #----------------------------------------------------------------------
      # Strip off path from program name
      #----------------------------------------------------------------------
      $0 =~ m/([\w\.]+$)/;
      $pn = $1;

      die "Usage: $pn [--sect_classes=file] <xml file>\n";
   }

   $xml_file = $ARGV[0];
   die "Cannot find $xml_file: $!" unless (-e $xml_file);
}

#=============================================================================
# PROCESS_SECT_CLASSES - Process the file which defines the classifications
#    of the section names.  Syntax of the file described in POD documentation
#    above.
#
#=============================================================================
sub process_sect_classes
{
   my ($sect_class_file) = @_;
   
   my ($line, $line_no, $sect_name, $class, $rest, $severe_err, $err_msg);
   my (%sect_seen);

   #--------------------------------------------------------------------------
   # Build default hashes if no specification is supplied
   #--------------------------------------------------------------------------
   if (!$sect_class_file)
   {
      default_sect_classes();
      return;
   }

   $severe_err = 0;
   $line_no    = 1;
   $err_msg    = undef;

   #--------------------------------------------------------------------------
   # Open file
   #--------------------------------------------------------------------------
   open(SECT_INFO, "<$sect_class_file") ||
                         die "Can't open $sect_class_file : $!\n";

   #--------------------------------------------------------------------------
   # Process each line
   #--------------------------------------------------------------------------
   while ($line = <SECT_INFO>)
   {
      #-----------------------------------------------------------------------
      # Remove comments.  Ignore empty or blank lines.
      #-----------------------------------------------------------------------
      $line =~ s/\#.*$//;
      next if (!$line || $line =~ m/^\s*$/);

      #-----------------------------------------------------------------------
      # Remove space at the beginning or end of the line
      #-----------------------------------------------------------------------
      $line =~ s/^\s*//;
      $line =~ s/\s*$//;

      #-----------------------------------------------------------------------
      # Split into section name and class
      #-----------------------------------------------------------------------
      ($sect_name, $class, $rest) = split(/\s*[,=\s]\s*/, $line);

      #-----------------------------------------------------------------------
      # Error checking
      #-----------------------------------------------------------------------
      if ($rest)
      {
         $err_msg = "text after \"$class\" ignored";
      }

      if ($sect_name && defined($sect_seen{$sect_name}))
      {
         $err_msg = "ignored re-definition of section \"$sect_name\"";
         next;
      }
      
      if (!$sect_name || !$class)
      {
         $err_msg = "syntax error";
         $severe_err = 1;
         next;
      }

      #-----------------------------------------------------------------------
      # Put the section name in the correct hash
      #-----------------------------------------------------------------------
      if    ($class eq 'CODE')   { $is_code{$sect_name}   = 1; }
      elsif ($class eq 'DATA')   { $is_data{$sect_name}   = 1; }
      elsif ($class eq 'UDATA')  { $is_udata{$sect_name}  = 1; }
      elsif ($class eq 'IGNORE') { $is_ignore{$sect_name} = 1; }
      else
      {
         $err_msg = "illegal section classification \"$class\"";
         $severe_err = 1;
         next;
      }

      $sect_seen{$sect_name} = 1;
   }
   continue
   {
      if ($err_msg)
      {
         print "\"$sect_class_file\", line $line_no, $err_msg\n";
         $err_msg = undef;
      }

      ++$line_no;
   }

   close(SECT_INFO);

   exit(1) if ($severe_err);
}

#=============================================================================
# COMPUTE_LIB_FOOTPRINT
#=============================================================================
sub compute_lib_footprint
{
   my($os_key, $osect, $osect_class, $osect_name, $is_key, $isect);
   my($file_key, $file_rec, $lib_name, %reported_err, $err_key, $first_err);

   $first_err = 1;

   #--------------------------------------------------------------------------
   # For each output section
   #--------------------------------------------------------------------------
   foreach $os_key (sort keys %$output_sections)
   {
      $osect = $output_sections->{$os_key};

      #-----------------------------------------------------------------------
      # Skip empty sections
      #-----------------------------------------------------------------------
      next if (!oct($osect->{'size'}));

      #-----------------------------------------------------------------------
      # Determine the output section type: 
      #     CODE  - Obvious
      #     DATA  - Initialized data
      #     UDATA - Uninitialized data
      #     OTHER - A problem only if some input sections come from libs
      #     undef - Debug sections and others it is okay to skip
      #-----------------------------------------------------------------------
      $osect_name = $osect->{'name'};
      $osect_class = classify_section($osect_name);
      next if (!$osect_class);

      #-----------------------------------------------------------------------
      # For each input section in the output section
      #-----------------------------------------------------------------------
      foreach $is_key (@{$osect->{'contents'}->{'object_component_ref'}})
      {
         #--------------------------------------------------------------------
         # Trampolines reside in linker created input sections that are
         # not included in the input section list from the map file.  This
         # is arguably a bug in the linker.  It doesn't matter to this 
         # script since such an input section could never be associated 
         # with a file, much less a library.  Skip them.
         #--------------------------------------------------------------------
         next if (!($isect = $input_sections->{$is_key}));

         #--------------------------------------------------------------------
         # Overlay sections are created by the linker and have no input
         # file associated with them.  Skip them.  BTW, I would expect the
         # trampoline sections to get the same handling.
         #--------------------------------------------------------------------
         next if (!($file_key = $isect->{'input_file_ref'}));

         $file_rec = $files->{$file_key};

         #--------------------------------------------------------------------
         # If the corresponding file is a library (called an 'archive')
         #--------------------------------------------------------------------
         if ($file_rec->{'kind'} eq 'archive')
         {
            $lib_name = $file_rec->{'file'};

            #-----------------------------------------------------------------
            # If lib not seen before create new entry 
            #-----------------------------------------------------------------
            if (!defined($lib_data{$lib_name}))
            {
               $lib_data{$lib_name} = {
		  CODE  => {},
		  DATA  => {},
                  UDATA => {},
                  OTHER => {} };
            }

            #-----------------------------------------------------------------
            # Found an unclassified input section in a library.  That
            # should not happen.  Output diagnostic.
            #-----------------------------------------------------------------
            if ($osect_class eq 'OTHER')
            {
               $err_key = $lib_name . $osect->{'name'};
               if (!defined($reported_err{$err_key}))
               {
                  print ">> Unclassified section name: $osect->{'name'}\n";
                  print "   found in library: $lib_name\n";
                  if ($first_err)
                  {
                     print <<END_OF_MSG;
   -----------------------------------------------------------------------
   Change how sections are classified by  editing "classes.txt", then use
   the option "--sect_classes=classes.txt"
   -----------------------------------------------------------------------
END_OF_MSG
                     $first_err = undef;
                  }

                  #-----------------------------------------------------------
                  # Use this hash to avoid repeated messages on same
                  # combination of lib/section
                  #-----------------------------------------------------------
                  $reported_err{$err_key} = 1;
               }
            }

            #-----------------------------------------------------------------
            # Add the input section size from that lib, with that output
            # section type and output section name
            #-----------------------------------------------------------------
            $lib_data{$lib_name}->{$osect_class}->{$osect_name} +=
	                                            oct($isect->{'size'});
         }
      }
   }
}

#=============================================================================
# PRINT_LIB_FOOTPRINT
#=============================================================================
sub print_lib_footprint
{
   my ($lib_name, $total, $osect_class, $osect_name, $size);

   print "\n";

   foreach $lib_name (sort keys %lib_data)
   {
      print "$lib_name\n";
      $total = 0;

      foreach $osect_class (qw(CODE DATA UDATA OTHER))
      {
	 $total += print_osect_class($lib_name, $osect_class);
      }

      printf("                           ==========   ==========\n");
      printf("%-25s: %10ld   0x%08lx\n", '    LIB TOTAL', $total, $total);
      print "\n";
   }
}

#=============================================================================
# PRINT_OSECT_CLASS - Helper to print_lib_footprint.  Prints out data for
#    one section classification of one library.  Returns total seen for
#    that class.
#=============================================================================
sub print_osect_class
{
   my ($lib_name, $osect_class) = @_;
   my ($class_hash, $osect_name, $size, $total);


   $total = 0;
   $size = 0;
   $class_hash = $lib_data{$lib_name}->{$osect_class};

   #--------------------------------------------------------------------------
   # Check if class is empty.  IF so, quit.
   #--------------------------------------------------------------------------
   foreach $osect_name (sort keys %$class_hash)
   {
      $size = $class_hash->{$osect_name};
      last if ($size);
   }

   return 0 if (!$size);

   #--------------------------------------------------------------------------
   # Non-empty class.  Print it.
   #--------------------------------------------------------------------------
   if ($osect_class ne 'OTHER')
   {
      printf("%10s ---------------------------------------\n",
	 $osect_class);
   }
   else
   {
      printf("%15s ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n",
	 'NOT CLASSIFIED!');
   }

   foreach $osect_name (sort keys %$class_hash)
   {
      $size = $class_hash->{$osect_name};
      next if (!$size);
      printf("%25s: %10ld   0x%08lx\n", $osect_name, $size, $size);
      $total += $size;
   }

   return $total;
}

#=============================================================================
# CLASSIFY_SECTION - Returns a string used to classify the section.  Returns
#   undef if the section should not be considered.  Returns 'OTHER' if
#   classification fails.
#=============================================================================
sub classify_section
{
   my($name) = @_;

   #--------------------------------------------------------------------------
   # Name matches from one of the hashes
   #--------------------------------------------------------------------------
   return 'CODE'  if (defined($is_code{$name}));
   return 'DATA'  if (defined($is_data{$name}));
   return 'UDATA' if (defined($is_udata{$name}));
   return undef   if (defined($is_ignore{$name}));

   #--------------------------------------------------------------------------
   # Sections that contain debug information.  Skip them.
   #--------------------------------------------------------------------------
   return undef   if ($name =~ m/^\.debug_/);
   return undef   if ($name eq '$BRID');

   #--------------------------------------------------------------------------
   # Groups are collections of other output sections.  Those other output
   # output sections will be processed when they are seen, so it is okay
   # to skip a group.
   #--------------------------------------------------------------------------
   return undef   if ($name =~ m/^GROUP_/);

   #--------------------------------------------------------------------------
   # Could not classify
   #--------------------------------------------------------------------------
   return 'OTHER';
}

#=============================================================================
# DEFAULT_SECT_CLASSES - The default classification of the sections used if
#    the user does not specify one with --sect_classes=file
#=============================================================================
sub default_sect_classes
{
   %is_code = (
      '.text'      => 1,
      '.bios'      => 1,
      '.sysinit'   => 1,
      '.hwi_vec'   => 1,
      '.rtdx_text' => 1 );

   %is_data = (
      '.data'      => 1,
      '.cinit'     => 1,
      '.pinit'     => 1,
      '.switch'    => 1,
      '.const'     => 1,
      '.gblinit'   => 1,
      '.rtdx_data' => 1,
      '.trcdata'   => 1 );

   %is_udata = (
      '.bss'       => 1,
      '.far'       => 1,
      '.sysmem'    => 1,
      '.stack'     => 1,
      '.sysstack'  => 1,
      '.args'      => 1,
      '.cio'       => 1,
      '.sysdata'   => 1 );

   %is_ignore = ();
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
   print Dumper($xml_data);
   print "\n";
   print "=============================================================\n";
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

