#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

objdiff.pl

=head1 SYNOPSIS

Compares two COFF or ELF files or libraries for equality

=head1 USAGE

objdiff.pl [options] <file1> <file2> [<OFD cmd>]

OFD cmd stands for Object File Display utility.  That is the name of the
object file display utility from the toolset used to create the object files.
Examples: ofd6x for C6000 or ofd55 for C5500.  If given, the script runs the
OFD command to create the XML version of the object file.  Creating the XML
in this manner avoids the need to create the XML files prior to running the
script, while keeping the script itself device independent.

Other Perl scripts that work with OFD output typically allow you to directly
pipe the output of the the OFD command into the script.  This script does not
work that way; it requires the OFD output from two files, not just one.

If you do not give an OFD command, then the script presumes the files are
already in XML format.

For example ...

   % perl objdiff.pl file1.obj file2.obj ofd6x

Or ...

   % ofd6x -x -o=file1.xml file1.obj
   % ofd6x -x -o=file2.xml file2.obj
   % perl objdiff.pl file1.xml file2.xml

=head1 OPTIONS

All of these options are disabled by default.

 --syms             : Compare symbols, relocations, and line number entries
 --dwarf            : Compare Dwarf debug sections
 --verbose          : Show raw data bytes that are different (if any)
 --config_file=file : Name file for specifying which sections to skip
 --quiet            : Suppress progress messages
 --cg_xml_version   : Print out the version of the cg_xml package in use

Except for --cg_xml_version, all the options can be abbreviated to a single
letter, i.e. -q is equivalent to --quiet.

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  Filtering the XML in this way reduces the
amount of data processed by this script, thus making it run faster.  Sometimes
much faster.  

The exact OFD options to use with this script vary based on conditions detailed 
below.  Note the fewer --obj_display option settings you use, the larger the
XML file becomes, and the slower the script executes.  

The options given below can be used when building the XML files as a separate
step.  You can also use those options on the objdiff.pl invocation line.
If you do that, you have to precede the OFD command with "--".  The "--"
breaks the command line in half; everything before is script options,
everything after is OFD options.  For example ...

  perl objdiff.pl file1.out file2.out -- ofd6x --xml_indent=0 --obj_display=norawdata,nosymbols,nostrings,norelocs,nolnnos

Yes, that's a long command line.  But it saves you from having to create
2 XML files in separate steps.  Note OFD -x is not needed here, because
objdiff.pl always adds it when it invokes OFD.  However, using the -x 
anyway does not cause any problems.

=head2 Default objdiff.pl Options and Comparing .out Files

  -x --xml_indent=0 --obj_display=norawdata,nosymbols,nostrings,norelocs,nolnnos

Note this means the raw data will not appear in the XML.  Thus the script
opens the two .out files, reads the raw data directly, and then compares.
It turns out this method of comparison is much faster than having the raw data
encoded in the XML.  Note each .out file must be available for access by
the script.

=head2 Default objdiff.pl Options and Comparing Libraries

  -x --xml_indent=0 --obj_display=nosymbols,nostrings,norelocs,nolnnos

Note the raw data must be kept in the XML in this case.  There is no
straightforward method for accessing the raw data from inside a library
archive file.

=head2 Using the objdiff.pl --syms Option and Comparing .out Files

  -x --xml_indent=0 --obj_display=norawdata,nostrings

The symbol table, relocation entries, and line number entries are kept
for comparison.

=head2 Using the objdiff.pl --syms Option and Comparing Libraries

  -x --xml_indent=0 --obj_display=nostrings

The symbol table, relocation entries, and line number entries are kept
for comparison.

=head1 COMPARING LIBRARIES

The script can automatically detect when libraries are being compared; no
option is necessary.

=head1 COMPARING SYMBOLIC INFORMATION

By default the symbolic information is not compared.  This includes the
symbol table, the relocations, and the line number entries.  Often such
information can have slight differences that do not affect the execution
of the underlying object code.  Whether to compare this information is
thus different from user to user.

Only globally defined symbols are compared.  Other symbols, which includes
function local and file static symbols, are ignored.

=head1 COMPARING DWARF DEBUG INFORMATION

By default, the Dwarf debug information is not compared.

Dwarf information is represented in two forms by the OFD generated XML.  It
is encoded in the raw data of the Dwarf debug sections (which are named
.debug_<something>).  And, it can be explicitly represented in the XML if the
-g option is used when invoking OFD.  

Use the --dwarf flag to compare the raw data in the Dwarf debug sections.

The explicit representation of the Dwarf debug information (possibly present)
in the XML is not examined.  This is admittedly a crude approach.  But it is
the best we can do without a much more sophisticated approach.  Comparing two
Dwarf representations is like comparing two C programs.  Even though they may
appear quite different, they may represent the same thing.  

=head1 COMPARING STABS DEBUG INFORMATION

Stabs debug information is encoded as a collection of cleverly arranged symbols
in the symbol table, plus line number entries.  No Dwarf-like sections contain
stabs debug information.  Because only globally defined symbols are ever
compared, there is no way to compare Stabs debug information.

=head1 USING THE CONFIGURATION FILE TO SKIP SECTIONS

You can choose to not compare sections either by name, or by section header
flags.  Write a configuration file to specify which sections to skip, and
supply the name of that file with the option --config_file=file_name.

=head2 SYNTAX OF THE CONFIGURATION FILE

 <property> = <value>

Valid syntax for a property includes: SKIP_BY_NAME, SKIP_BY_FLAG

If the property is SKIP_BY_NAME, the value need only be a string.  Note it
must match the name of an actual section to have any effect.  

If the property is SKIP_BY_FLAG, valid values include: COPY, UDATA, TEXT,
DATA, DUMMY, NOLOAD.  Note UDATA stands for uninitialized data, i.e. the "bss"
flag.  Any section which has one of these flags set in the section header is
skipped.  

You can list multiple SKIP_BY_NAME and SKIP_BY_FLAG entries.

The configuration file may also contain empty space, blank lines, and comments
that begin with '#'.  Only simple error checking is done when processing the
file.  Poke holes in it, if you must.

=head2 CONFIGURATION FILE EXAMPLE

 #------------------------------------------------------------------------
 # Configuration File Example
 #------------------------------------------------------------------------
 SKIP_BY_NAME = .one      # comments OK here too
 SKIP_BY_NAME = two       # sections do not have to start with '.'
 SKIP_BY_FLAG = COPY      # skip all sections that have the COPY flag set
 
=head2 LIMITATION OF SECTION SKIPPING

Suppose you want to compare two files, and you know one file has an additional
section named "extra" that you want to avoid comparing.  So, you use the
configuration file entry ...

 SKIP_BY_NAME = extra

to skip it.  The file comparison will still fail.  The I<file> header includes
a count of the number of sections in the file.  This count is unaffected by
which sections are skipped during comparison.  Thus, this section count will
be different between the files, even though the "extra" section is skipped.

=head1 CANNOT COMPENSATE FOR DIFFERENCES FOUND

Text based file comparison utilities like Unix diff can often adjust for
differences found and usefully continue the comparison.  Such is not the case
for objdiff.pl.  When a difference is found no attempt is made to somehow
compensate for that difference.  

For example, when the size of a section changes (presuming this section is
loaded to target memory), this causes all sections after it in memory to begin
on different addresses.  Further, all symbolic references to these sections
are different, which can show up as differences in any section.

=head1 CONFIGURE FOR YOUR SYSTEM

Make sure that the OFD command is in your executables path.

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
# Other useful modules
#=============================================================================
use Getopt::Long;              # for handling command line options
use MIME::Base64;              # for reading raw data encoded in XML file

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
   # From the command line ---------------------------------------------------
   $in_file1,          # Input file
   $in_file2,          # Input file
   @ofd_cmd,           # Exact OFD command run
   $compare_syms,      # Whether to compare symbols
   $compare_dwarf,     # Whether to compare Dwarf sections
   $verbose,           # Verbose mode - presently dumps raw data diffs
   $config_file,       # Config file for spec'ing which sections to skip
   $skip_empty_sects,  # Skip empty sections
   $quiet_mode,        # Suppress progress messages

   # Skip section settings from configuration file ---------------------------
   %skip_by_name,
   @skip_by_flag,

   # Global data -------------------------------------------------------------
   %strip_args,        # arguments used to strip unneeded parts of XML
   $xml_data1,         # Perl data structure that corresponds to XML
   $xml_data2,         # Perl data structure that corresponds to XML
   $basename1,         # Base file name w/o dir
   $basename2,         # Base file name w/o dir
   $lib_name1,         # Name of library
   $lib_name2,         # Name of library
   $comp_name1,        # Name of file or lib printed in error messages
   $comp_name2,        # Name of file or lib printed in error messages
   %diffs_found,       # Holds all diffs found in one file
   %lookup_array_with, # Fields used in looking up an array named by the key

   # Global state ------------------------------------------------------------
   $is_elf,            # Compare ELF files?
   $comparing_libs,    # Comparing libs?
   $reading_obj,       # Reading raw data directly from object file?
   $saw_diff,          # Any differences seen?
   $processing_stage,  # String that describes current processing stage
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

   $saw_diff = 0;

   process_cmd_line();
   process_config_file();
   build_strip_args();
      
   #--------------------------------------------------------------------------
   # Get XML form of OBJ file information, represented as Perl data structures
   # This can take a while, so keep user updated.
   #--------------------------------------------------------------------------
   warn "Processing $in_file1 ...\n" unless ($quiet_mode);
   $xml_data1 = get_xml_data($in_file1);
   warn "Processing $in_file2 ...\n" unless ($quiet_mode);
   $xml_data2 = get_xml_data($in_file2);
   warn "Comparing files ...\n\n" unless ($quiet_mode);

   $is_elf         = check_elf($xml_data1, $xml_data2);
   $comparing_libs = is_libs($xml_data1, $xml_data2); 
   init_tables();

   if ($comparing_libs)
      { compare_libs($xml_data1, $xml_data2); }
   else
      { compare_files($xml_data1, $xml_data2); }

   if (!$saw_diff)
      { print "\nFiles are the same\n"; }
   else
      { print "\nFiles are different\n"; }

   exit($saw_diff);
}

#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($pn);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   #--------------------------------------------------------------------------
   # Init cmd line vars.  Note "skip_empty_sects" is an undocumented option.
   # Grep the source to see how a user might discover it's existence.
   #--------------------------------------------------------------------------
   $compare_syms = $compare_dwarf = $verbose = $config_file = 
   $skip_empty_sects = $quiet_mode = undef;

   GetOptions("syms"             => \$compare_syms,
              "dwarf"            => \$compare_dwarf,
              "verbose"          => \$verbose,
	      "config_file=s"    => \$config_file,
	      "skip_empty_sects" => \$skip_empty_sects,
	      "quiet"            => \$quiet_mode);

   if (@ARGV < 2)
   {
      #-----------------------------------------------------------------------
      # Strip off path from program name
      #-----------------------------------------------------------------------
      $0 =~ m/([\w\.]+)$/;
      $pn = $1;
      die "Usage: $pn [options] <file1> <file2> [<ofd cmd>]\n";
   }

   $in_file1 = shift @ARGV;
   $in_file2 = shift @ARGV;
   
   #--------------------------------------------------------------------------
   # Getting OFD command into an array on purpose.  While there is no need
   # for it now, there may come a time when folks will want to pass OFD
   # specific args (like -g) to OFD.  By handling the OFD command as an
   # array, you can do it like this ...
   #     objdiff.pl file1.obj file2.obj -- ofd6x -g
   # Note the "--".  That tells the GetOptions function above to quit
   # consuming the command line options that begin with "-".  
   #--------------------------------------------------------------------------
   # Quoting ofd command itself in case it a full path with spaces like:
   # "\Program Files\Texas Instruments\and\so\on\ofd6x.exe".
   #--------------------------------------------------------------------------
   @ofd_cmd = @ARGV;
   if ($ofd_cmd[0]) { $ofd_cmd[0] = '"' . $ofd_cmd[0] . '"'; }

   #--------------------------------------------------------------------------
   # Panic checks.  Make sure the input files exist, and they are not
   # directories, or some other not-a-normal-file thing.
   #--------------------------------------------------------------------------
   die "Cannot find $in_file1: $!"  unless (-e $in_file1);
   die "Cannot find $in_file2: $!"  unless (-e $in_file2);
   die "Unusual file $in_file1: $!" unless (-f $in_file1);
   die "Unusual file $in_file2: $!" unless (-f $in_file2);

   #--------------------------------------------------------------------------
   # If no OFD command given, the input files cannot be binary.
   # It would be ideal to somehow make sure they are XML files, but there is
   # no easy way to do that.
   #--------------------------------------------------------------------------
   if (!@ofd_cmd && (-B $in_file1 || -B $in_file2))
   {
      die << "MUST_BE_XML";
Since no OFD command appears at the end of the command line, the input files
FILE1: $in_file1
FILE2: $in_file2
cannot be object or executables files, but must be XML files.  Either add the
the OFD command, or separately use OFD to convert them to XML.
MUST_BE_XML
   }
}

#=============================================================================
# PROCESS_CONFIG_FILE ... that specifies which sections to skip
#=============================================================================
sub process_config_file
{
   return if (!$config_file);

   my ($line, $orig_line, $prop, $val, %config_to_flag);

   #--------------------------------------------------------------------------
   # Hash that converts SKIP_BY_FLAG values to how those flags are found
   # in the XML
   #--------------------------------------------------------------------------
   %config_to_flag = (
      DUMMY  => 'dummy',
      NOLOAD => 'noload',
      COPY   => 'copy',
      TEXT   => 'text',
      DATA   => 'data',
      UDATA  => 'bss' );         # UDATA == uninitialized data

   open(CONFIG, "<$config_file") or die "Cannot open $config_file: $!";

   #--------------------------------------------------------------------------
   # Parse lines of the form "PROPERTY = VALUE"
   #--------------------------------------------------------------------------
   while ($line = <CONFIG>)
   {
      $orig_line = $line;        # save original line for error reporting

      $line =~ s/#.*//;          # remove comments
      $line =~ s/^\s+//;         # remove leading space
      $line =~ s/\s+$//;         # remove trailing space
      next if (!length($line));  # skip blank lines

      ($prop, $val) = split(/\s*=\s*/, $line, 2);

      if (!$prop || !$val)
      {
	 warn "$config_file: Syntax error on line:\n";
	 warn "\t$orig_line";  
	 warn "Ignored\n\n";
	 next;
      }
      elsif ($prop eq 'SKIP_BY_NAME')
      {
	 $skip_by_name{$val} = 1;
      }
      elsif ($prop eq 'SKIP_BY_FLAG')
      {
	 if (!$config_to_flag{$val})
	 {
	    warn "$config_file: Unrecognized flag name \"$val\" on line:\n";
	    warn "\t$orig_line";
	    warn "Ignored\n\n";
	    next;
	 }

	 push @skip_by_flag, $config_to_flag{$val};
      }
      else
      {
	 warn "$config_file: Unrecognized property \"$prop\" on line:\n";
	 warn "\t$orig_line";  
	 warn "Ignored\n\n";
	 next;
      }
   }
  
   close(CONFIG);
}

#=============================================================================
# BUILD_STRIP_ARGS - Build hash ultimately used as the final argument to
#    ofd_strip_xml.  Indicates what parts of the XML to remove.
#=============================================================================
sub build_strip_args
{
   my (@tags, @sections);

   #--------------------------------------------------------------------------
   # Always delete the string table.  The "dwarf" stuff comes in only if
   # the -g option is used on OFD.  Very unlikely to occur.  But delete if
   # it does.
   #--------------------------------------------------------------------------
   @tags = qw(string_table dwarf);

   #--------------------------------------------------------------------------
   # If not comparing symbols, then delete the symbol table, the relocation
   # entries, and the line number entries (which only occur in COFF/Stabs
   # files anyway)
   #--------------------------------------------------------------------------
   if (!$compare_syms)
      { push @tags, qw(symbol_table relocations line_numbers); }

   #--------------------------------------------------------------------------
   # If not comparing the raw data in the Dwarf debug sections, then delete
   # them.  The strip function can't accept a regular expression like
   # ".debug_*".  So, these are the names of all the Dwarf debug sections
   # known at this time, for either COFF or ELF.
   #--------------------------------------------------------------------------
   if (!$compare_dwarf)
   {
      @sections = qw(.debug_info .debug_frame .debug_abbrev .debug_line
                     .debug_str);
   }

   #--------------------------------------------------------------------------
   # Delete any sections the user said to skip in the config file
   #--------------------------------------------------------------------------
   push @sections, keys %skip_by_name;

   $strip_args{GENERAL}  = [ @tags ];
   $strip_args{SECTIONS} = [ @sections ] if (@sections);
}

#=============================================================================
# GET_XML_DATA - Convert one input file from:
#    - Object to XML (if needed)
#    - XML to Perl data structure
#=============================================================================
# These operations use lots of memory.  Take special care, when the XML file
# size exceeds 10 MB, to avoid having the XML and Perl data structure in
# memory at the same time.
#=============================================================================
sub get_xml_data
{
   my ($input_file) = @_;
   my ($all_lines, $ten_mb, $temp_fh);

   #--------------------------------------------------------------------------
   # If the user didn't specify an OFD command, presume the input file is
   # XML already.  ofd_filter_and_parse_xml is perfect for this case.
   #--------------------------------------------------------------------------
   if (!@ofd_cmd)
   {
      return
         ofd_filter_and_parse_xml(
	    $input_file,
	    STRIP => { %strip_args });
   }

   #--------------------------------------------------------------------------
   # Run OFD to convert the file to XML
   # $? handling is explained in Programming Perl, 3rd Ed., pg #659-660
   #--------------------------------------------------------------------------
   $all_lines = qx(@ofd_cmd -x $input_file);
   if ($? >> 8) { die "Running @ofd_cmd failed, stopped"; }

   #--------------------------------------------------------------------------
   # The rest of this code is an adaptation of what happens in ofd_filter_and
   # _parse_xml.  Given that $all_lines already is set up here, you can't call
   # it and reap the main benefit, which is that $all_lines is never in memory
   # at the same time as the big Perl data structure.
   #--------------------------------------------------------------------------
   ofd_strip_xml(undef,
		 \$all_lines,
		 %strip_args);

   $ten_mb = 10 * (1 << 20);
   if (length($all_lines) > $ten_mb)
   {
      #-----------------------------------------------------------------------
      # Copy the huge scalar out to a temporary file
      #-----------------------------------------------------------------------
      open ($temp_fh, "+>", undef) or die "Cannot create temp file: $!";
      print $temp_fh "$all_lines";
      seek $temp_fh, 0, 0;

      #-----------------------------------------------------------------------
      # Free the memory used for the huge scalar
      #-----------------------------------------------------------------------
      $all_lines = undef;

      #-----------------------------------------------------------------------
      # Convert XML to Perl data structures
      #-----------------------------------------------------------------------
      return ofd_process_xml_file($temp_fh);
   }
   else
   {
      return ofd_process_xml_file($all_lines);
   }
}

#=============================================================================
# CHECK_ELF - Returns FALSE if both files are COFF. Returns TRUE if both
#   files are ELF.  Dies with an error if one file is COFF and the other ELF.
#=============================================================================
sub check_elf
{
   my ($xml_data1, $xml_data2) = @_;
   my (@file_list, $file1, $file2);

   @file_list = ofd_each_file($xml_data1);
   $file1 = $file_list[0];
   @file_list = ofd_each_file($xml_data2);
   $file2 = $file_list[0];

   if ( exists($file1->{elf}) &&  exists($file2->{elf}))
      { return 1; }

   if (!exists($file1->{elf}) && !exists($file2->{elf}))
      { return undef; }

   if (exists($file1->{elf}))
      { die "$in_file1 is ELF, $in_file2 is COFF\n"; }
   else
      { die "$in_file2 is ELF, $in_file1 is COFF\n"; }
}

#=============================================================================
# IS_LIBS - Are we comparing libs?
#=============================================================================
sub is_libs
{
   my ($xml_data1, $xml_data2) = @_;

   $lib_name1 = ofd_find_lib_name($xml_data1);
   $lib_name2 = ofd_find_lib_name($xml_data2);

   if ($lib_name1 && !$lib_name2)
      { die "$lib_name1 is a library, $in_file2 is not a library\n"; }
   elsif (!$lib_name1 && $lib_name2)
      { die "$lib_name2 is a library, $in_file1 is not a library\n"; }
   elsif ($lib_name1 && $lib_name2)
      { return 1; }
   else
      { return undef; }
}

#=============================================================================
# INIT_TABLES
#=============================================================================
sub init_tables
{
   %lookup_array_with = (
      vendor_section => [ 'vendor_name' ],
      attribute_list => [ 'tag' ],
      elf32_reloc    => [qw(r_offset r_sym_string)],
      members        => [ 'sh_name_string' ],
      elf32_verdaux  => [ 'vda_name' ],
      elf32_vernaux  => [ 'vna_name' ],
      function       => [ 'name' ],
      line_entry     => [qw(line addr)],
      );
}

#=============================================================================
# COMPARE_LIBS - Compare two libs
#=============================================================================
sub compare_libs
{
   my ($xml_data1, $xml_data2) = @_;
   my ($file_data1, $file_data2, %files2, %files2_visited, $key);

   #--------------------------------------------------------------------------
   # Build hash of files in lib2, for easy lookup.  Also, keep track of 
   # which files in lib2 have been compared.
   #--------------------------------------------------------------------------
   foreach $file_data2 (ofd_each_file($xml_data2))
   {
      $files2{$file_data2->{'name'}} = $file_data2;
      $files2_visited{$file_data2->{'name'}} = undef;
   }

   #--------------------------------------------------------------------------
   # Loop through files from lib1, comparing against the same file in lib2
   #--------------------------------------------------------------------------
   foreach $file_data1 (ofd_each_file($xml_data1))
   {
      $file_data2 = $files2{$file_data1->{'name'}};

      if (!$file_data2)
      {
	 print "$file_data1->{'name'} is in $lib_name1, but not $lib_name2\n";
	 $saw_diff = 1;
	 next;
      }

      $files2_visited{$file_data1->{'name'}} = 1;

      compare_file_helper($file_data1, $file_data2);
   }

   #--------------------------------------------------------------------------
   # Make sure every file from lib2 has been compared
   # Sort so automated testing is easier
   #--------------------------------------------------------------------------
   foreach $key (sort keys %files2_visited)
   {
      if (!$files2_visited{$key})
      {
	 print "File $key is in $lib_name2, but not in $lib_name1\n";
	 $saw_diff = 1;
      }
   }
}

#=============================================================================
# COMPARE_FILES - Compare two files, not two libs
#=============================================================================
sub compare_files
{
   my ($xml_data1, $xml_data2) = @_;
   my ($file_data1, $file_data2);

   $file_data1 = (ofd_each_file($xml_data1))[0];
   $file_data2 = (ofd_each_file($xml_data2))[0];

   compare_file_helper($file_data1, $file_data2);
}

#=============================================================================
# COMPARE_FILE_HELPER - Core operations to compare exactly 2 files
#=============================================================================
sub compare_file_helper
{
   my ($file_data1, $file_data2) = @_;

   #-----------------------------------------------------------------------
   # Base names of files (without dir info) used in print statements
   #-----------------------------------------------------------------------
   $basename1 = $file_data1->{'name'};
   $basename2 = $file_data2->{'name'};

   $comp_name1 = ($comparing_libs) ? $lib_name1 : $basename1;
   $comp_name2 = ($comparing_libs) ? $lib_name2 : $basename2;

   #-----------------------------------------------------------------------
   # Zero out the diffs collected from the last file processed
   #-----------------------------------------------------------------------
   %diffs_found = ();

   #-----------------------------------------------------------------------
   # Compare one COFF file
   #-----------------------------------------------------------------------
   if (!$is_elf)
   {
      coff_compare_file_headers($file_data1, $file_data2);

      $processing_stage = 'Comparing Optional File Headers';
      compare_hash_record($file_data1->{ti_coff},
			  $file_data2->{ti_coff},
			  'optional_file_header',
			  { tool_version => 1});

      compare_sections($file_data1, $file_data2);

      if ($compare_syms)
         { coff_compare_symbols($file_data1, $file_data2); }
   }
   #-----------------------------------------------------------------------
   # Compare one ELF file
   #-----------------------------------------------------------------------
   else
   {
      $processing_stage = 'Comparing File Headers';
      compare_hash_record($file_data1->{elf},
			  $file_data2->{elf},
			  'elf32_ehdr',
			  { e_phoff => 1, e_shoff => 1, e_shstrndx => 1});

      $processing_stage = 'Comparing Segment Tables';
      compare_array_hash_records(
         $file_data1->{elf},
	 $file_data2->{elf},
	 'program_table',
	 [qw(p_vaddr p_type)],
	 { p_offset => 1, p_filesz => 1},
	 undef);

      compare_sections($file_data1, $file_data2);
   }

   #-----------------------------------------------------------------------
   # Print out any differences found in this file
   #-----------------------------------------------------------------------
   dump_diffs();
}

#=============================================================================
# compare_hash_record and compare_array_hash_records form the heart of the
# comparison process.  These routines can compare arbitrary data structures
# so long as they are composed of hashes and arrays of hashes.  
#=============================================================================
{
   #--------------------------------------------------------------------------
   # This is a stack of processing stages shared between these two
   # compare routines.  It is used to form the key for the %diffs_found
   # hash.  
   #--------------------------------------------------------------------------
   my (@sub_stages);
   
   #==========================================================================
   # COMPARE_HASH_RECORD - Compares one record that is in the form of a
   #    hash.  The fields in the record can be values, or references to
   #    other arrays or hashes.  
   #==========================================================================
   sub compare_hash_record
   {
      my ($base_ref1,     # ref to record from file1
          $base_ref2,     # ref to record from file2
	  $base_field,    # field within ref to begin comparison - optional
	  $skip_fields)   # ref to hash of field names to skip over - optional
	     = @_;

      my ($field, $ref1, $ref2, $diffs_key, $diffs, $ref1_type, $ref2_type);

      #-----------------------------------------------------------------------
      # If a base field is given, make sure both sides have it
      #-----------------------------------------------------------------------
      if (defined($base_field))
      {
	 my $ref1_ok = defined($base_ref1->{$base_field});
	 my $ref2_ok = defined($base_ref2->{$base_field});

	 push @sub_stages, $base_field;
	 $diffs_key = join ' : ', ($processing_stage, @sub_stages);

	 if    (!$ref1_ok && !$ref2_ok)
	 { 
	    pop @sub_stages;
	    return undef;   # OK if neither file has this base field
	 }
	 elsif ( $ref1_ok && !$ref2_ok)
	 {
	    push @{$diffs->{only_in_file1}}, $base_field;
	    push @{$diffs_found{$diffs_key}}, $diffs;
	    pop @sub_stages;
	    return $diffs;
	 }
	 elsif (!$ref1_ok &&  $ref2_ok)
	 {
	    push @{$diffs->{only_in_file2}}, $base_field;
	    push @{$diffs_found{$diffs_key}}, $diffs;
	    pop @sub_stages;
	    return $diffs;
	 }

	 $ref1 = $base_ref1->{$base_field};
	 $ref2 = $base_ref2->{$base_field};
      }
      else
      {
	 $diffs_key = join ' : ', ($processing_stage, @sub_stages);
	 $ref1 = $base_ref1;
	 $ref2 = $base_ref2;
      }

      #-----------------------------------------------------------------------
      # Expect both sides to be a hash record
      #-----------------------------------------------------------------------
      $ref1_type = ref($ref1);
      $ref2_type = ref($ref2);
      if ($ref1_type ne 'HASH' || $ref2_type ne 'HASH')
      {
	 warn "Do not see expected HASH references in compare_hash_record";
	 pop @sub_stages if (defined($base_field));
	 return undef;
      }

      #-----------------------------------------------------------------------
      # Go through the fields of the record from file1
      #-----------------------------------------------------------------------
      foreach $field (keys %$ref1)
      {
         #--------------------------------------------------------------------
	 # Skip certain fields
         #--------------------------------------------------------------------
	 next if (exists $skip_fields->{$field});

         #--------------------------------------------------------------------
	 # Make sure file2 has it
         #--------------------------------------------------------------------
	 if (!(exists $ref2->{$field}))
	 { 
	    push @{$diffs->{only_in_file1}}, $field;
	    next;
	 }

	 $ref1_type = ref($ref1->{$field});
	 $ref2_type = ref($ref2->{$field});

         #--------------------------------------------------------------------
	 # This field is a simple scalar
         #--------------------------------------------------------------------
	 if (!$ref1_type && !$ref2_type)
	 {
	    if (!defined($ref1->{$field}) && !defined($ref2->{$field}))
	       { next; }  # OK if both files have undef value for this field

	    if (   !defined($ref1->{$field})
		|| !defined($ref2->{$field})
		|| $ref1->{$field} ne $ref2->{$field}) # values compared here!
	       { push @{$diffs->{mismatch}}, $field; }
	 }
         #--------------------------------------------------------------------
	 # This field is a reference to something else
         #--------------------------------------------------------------------
	 else
	 {
	    if (!$ref1_type || !$ref2_type || $ref1_type ne $ref2_type)
	       { push @{$diffs->{mismatch}}, $field; }
	    elsif ($ref1_type eq 'HASH')
	       { compare_hash_record($ref1, $ref2, $field, $skip_fields); }
	    elsif ($ref1_type eq 'ARRAY')
	    {
	       compare_array_hash_records(
	          $ref1,
	          $ref2,
		  $field,
		  undef,
		  $skip_fields, 
		  undef);
	    }
	    else
	       { warn "Unexpected reference type $ref1_type"; }
	 }
      }

      #-----------------------------------------------------------------------
      # Make sure all the fields from file2 were compared
      #-----------------------------------------------------------------------
      foreach $field (keys %$ref2)
      {
	 next if (exists $skip_fields->{$field});

	 if (!(exists $ref1->{$field}))
	    { push @{$diffs->{only_in_file2}}, $field; }
      }

      #-----------------------------------------------------------------------
      # Record any differences found
      #-----------------------------------------------------------------------
      if ($diffs)
      {
	 $diffs->{ref1} = $ref1;
	 $diffs->{ref2} = $ref2;

	 push @{$diffs_found{$diffs_key}}, $diffs;
      }

      pop @sub_stages if (defined($base_field));
      return $diffs;
   }

   #==========================================================================
   # COMPARE_ARRAY_HASH_RECORDS - Compares an array of records.  This is 
   #    mostly about how to pick the elements of each array for comparison.
   #    Once the choice is made, compare_hash_record does the actual compare.
   #    This is tricky, since each array can have its elements in any order.
   #    The method is to form a hash of the elements from file2 keyed on the
   #    values of the fields given in the array $lookup_fields.  Each entry
   #    in this hash is an array of the elements from file2 that have the
   #    same values for those fields.  An array is used to handle duplicates.
   #    Then go through the elements from the file1 array, using that hash to
   #    grab the closest match from file2.  Then compare that pair.  If there
   #    are duplicate elements, then pull them out of the file2 set in the
   #    order they are first found.  While that is not guaranteed to match the
   #    order such duplicates occur in file1, there is no other technique to
   #    apply.  Just compare them anyway and hope for the best.  Most of the
   #    time, it will work out.  If it doesn't, then the two files are
   #    probably not a match anyway.  
   #==========================================================================
   sub compare_array_hash_records
   {
      my ($base_ref1,     # ref from file1
          $base_ref2,     # ref from file2
	  $base_field,    # field within the ref to start comparison - required!
	                  # The rest are optional
	  $lookup_fields, # ref to array of fields for looking up elements
	  $skip_fields,   # ref to hash of field names to skip over
          $skip_elem)     # ref to sub for whether to skip element
	     = @_;

      my ($diffs_key, $diffs, $ref1_ok, $ref2_ok, $ref1, $ref2,
          $ref1_type, $ref2_type, $lookup_key, $display_fields,
	  %refs2_by_field, $elem1, $elem2, $key);

      #-----------------------------------------------------------------------
      # If lookup fields is not given, then it must be found in the table
      # lookup_array_with
      #-----------------------------------------------------------------------
      if (!$lookup_fields)
         { $lookup_fields = $lookup_array_with{$base_field}; }

      if (!$lookup_fields)
      {
	 warn "No lookup fields";
	 return undef;
      }

      push @sub_stages, $base_field;
      $diffs_key = join ' : ', ($processing_stage, @sub_stages);

      #-----------------------------------------------------------------------
      # Make sure the array from each file is there.  
      #-----------------------------------------------------------------------
      $ref1_ok = defined($base_ref1->{$base_field});
      $ref2_ok = defined($base_ref2->{$base_field});

      if    (!$ref1_ok && !$ref2_ok)
      { 
         pop @sub_stages;
         return undef;        # OK if neither file has this array
      }
      elsif ( $ref1_ok && !$ref2_ok)
      {
	 push @{$diffs->{only_in_file1}}, $base_field;
	 push @{$diffs_found{$diffs_key}}, $diffs;
         pop @sub_stages;
	 return $diffs;
      }
      elsif (!$ref1_ok &&  $ref2_ok)
      {
	 push @{$diffs->{only_in_file2}}, $base_field;
	 push @{$diffs_found{$diffs_key}}, $diffs;
         pop @sub_stages;
	 return $diffs;
      }

      $ref1 = $base_ref1->{$base_field};
      $ref2 = $base_ref2->{$base_field};

      $ref1_type = ref($ref1);
      $ref2_type = ref($ref2);

      if ($ref1_type ne 'ARRAY' || $ref2_type ne 'ARRAY')
      {
	 warn "Do not see expected ARRAY references in " . 
	      "compare_array_hash_records";
         pop @sub_stages;
	 return undef;
      }

      #-----------------------------------------------------------------------
      # Form the hash of elements from file2
      #-----------------------------------------------------------------------
      foreach $elem2 (@{$ref2})
      {
         if (defined($skip_elem) && &{$skip_elem}($elem2))
	    { next; }

         #--------------------------------------------------------------------
	 # Note that a field name in $lookup_fields may not be present in
	 # the $elem2 record.  More comments on this code are just too hard.
	 # Read up on grep and map in your favorite Perl book.  Then run this
	 # under a debugger.  Then it should make sense.  :-)
         #--------------------------------------------------------------------
	 $lookup_key = join ' ',
	    map  { $elem2->{$_} } grep { $elem2->{$_} } @{$lookup_fields};

	 push @{$refs2_by_field{$lookup_key}}, $elem2;
      }

      #-----------------------------------------------------------------------
      # Go through all of file1 elements, comparing against its closest
      # match from file2
      #-----------------------------------------------------------------------
      foreach $elem1 (@{$ref1})
      {
         if (defined($skip_elem) && &{$skip_elem}($elem1))
	    { next; }

	 $lookup_key = join ' ',
	    map  { $elem1->{$_} } grep { $elem1->{$_} } @{$lookup_fields};
	 $display_fields = join ', ',
	    map { "$_ = $elem1->{$_}" } grep { $elem1->{$_} } @{$lookup_fields};

	 if (!(   exists $refs2_by_field{$lookup_key} 
	       && @{$refs2_by_field{$lookup_key}}))
	 {
	    push @{$diffs->{only_in_file1}}, $display_fields;
	    next;
	 }

	 $elem2 = shift @{$refs2_by_field{$lookup_key}};  # shift, not pop!

	 $ref1_type = ref($elem1);
	 $ref2_type = ref($elem2);

	 if ($ref1_type ne 'HASH' && $ref2_type ne 'HASH')
	 {
	    warn 'Do not see expected HASH references in ' . 
	         'compare_array_hash_records';
            pop @sub_stages;
	    return undef;
	 }

	 push @sub_stages, $display_fields;
	 compare_hash_record($elem1, $elem2, undef, $skip_fields);
	 pop @sub_stages;
      }

      #-----------------------------------------------------------------------
      # Make sure every element from file2 was compared.  Note that, in the
      # normal case, @{refs2_by_field{$key}} is empty.  Whatever is left 
      # was not compared.
      #-----------------------------------------------------------------------
      foreach $key (keys %refs2_by_field)
      {
	 foreach $elem2 (@{$refs2_by_field{$key}}) 
	 {
	    $display_fields = join ', ',
	       map { "$_ = $elem2->{$_}" } 
	       grep { $elem2->{$_} } @{$lookup_fields};
	    push @{$diffs->{only_in_file2}}, $display_fields;
	 }
      }

      #-----------------------------------------------------------------------
      # Record any diffs found
      #-----------------------------------------------------------------------
      if ($diffs)
         { push @{$diffs_found{$diffs_key}}, $diffs; }

      pop @sub_stages;
      return $diffs;
   }
}

#=============================================================================
# COFF_COMPARE_FILE_HEADERS
#=============================================================================
sub coff_compare_file_headers
{
   my ($file_data1, $file_data2) = @_;
   my (%skip_fields);

   $processing_stage = 'Comparing File Headers';

   #--------------------------------------------------------------------------
   # Set up fields that are not compared
   #--------------------------------------------------------------------------
   if (!$compare_syms)
   {
      %skip_fields = (
	 'file_length'      => 1,      # never compared
	 'byte_swapped'     => 1,      # never compared
	 'symbol_table_ptr' => 1,      # never compared
	 'string_table_ptr' => 1,      # never compared
	 'symbol_count'     => 1,      # never compared
	 'reloc_strip'      => 1,      # never compared
	 'localsym_strip'   => 1,      # never compared
	 'lnno_strip'       => 1,      # never compared
      );
   }
   else
   {
      %skip_fields = (
	 'file_length'      => 1,      # never compared
	 'byte_swapped'     => 1,      # never compared
	 'symbol_table_ptr' => 1,      # never compared
	 'string_table_ptr' => 1,      # never compared
      );
   }

   compare_hash_record($file_data1->{ti_coff},
                       $file_data2->{ti_coff},
		       'file_header',
		       \%skip_fields);
}

#=============================================================================
# COFF_SECT_CMP - For sorting COFF sections
#=============================================================================
sub coff_sect_cmp
{
   return    ofd_sect_name($a) cmp ofd_sect_name($b)
          || ofd_sect_size($a) <=> ofd_sect_size($b);
}

#=============================================================================
# ELF_SECT_CMP - For sorting ELF sections
#=============================================================================
sub elf_sect_cmp
{
   return    ofd_sect_name($a)         cmp ofd_sect_name($b)
          || ofd_sect_size($a)         <=> ofd_sect_size($b)
	  || str_to_num($a->{'index'}) <=> str_to_num($b->{'index'});
}

#=============================================================================
# COMPARE_SECTIONS
#-----------------------------------------------------------------------------
# This could potentially be handled by a call to compare_array_hash_records.
# However, there are too many things unique to comparing sections for that
# to make sense.  Chief among them is the fact that sections of the same
# name are possible, even common, in ELF.  So, given that sections can be
# in the object file in any order, choosing which two sections ought to be
# compared is tricky.  The algorithm here will work well when comparing files
# that are mostly the same.  Comparing wildly different files will result in
# lots of differences found here, because it is likely the wrong sections
# are paired up to compare.  The sections from each file are sorted
# according to the comparison routines just above.  Look at name, followed
# by size.  For ELF, also look at the index as well.  Looking at the index
# (not available in COFF) only because the more ties you can break, the more
# likely the sort will work well.  Note the index field is not compared.
# Then walk through those sorted section lists in lockstep, comparing each
# pair.  This is more likely to end up with poor pairing than the method
# used in compare_array_hash_records.  But it is the best way to handle
# potential duplication among array entries.
#=============================================================================
sub compare_sections
{
   my ($file_data1, $file_data2) = @_;

   my ($load_equals_run, $sort_method, $sect1, $sect2,
       @sections1, @sections2, $i);

   raw_data_setup($file_data1, $file_data2);

   #--------------------------------------------------------------------------
   # Keep track of whether the load and run addresses are always the same.
   # This is used only for COFF files.  If those addresses are always the
   # same, then do not compare the load addresses.  The results in somewhat
   # less output from the script when actual differences occur.
   #--------------------------------------------------------------------------
   $load_equals_run = !$is_elf;

   $sort_method = ($is_elf) ? \&elf_sect_cmp : \&coff_sect_cmp;

   #--------------------------------------------------------------------------
   # Sort the sections from file1
   #--------------------------------------------------------------------------
   foreach $sect1 (sort $sort_method ofd_each_section($file_data1))
   {
      next if (skip_section($sect1));

      $load_equals_run =
	    $load_equals_run
	 && $sect1->{'physical_addr'} eq $sect1->{'virtual_addr'};

      push @sections1, $sect1;
   }

   #--------------------------------------------------------------------------
   # Sort the sections from file2
   #--------------------------------------------------------------------------
   foreach $sect2 (sort $sort_method ofd_each_section($file_data2))
   {
      next if (skip_section($sect2));

      $load_equals_run =
            $load_equals_run
	 && $sect2->{'physical_addr'} eq $sect2->{'virtual_addr'};

      push @sections2, $sect2;
   }

   #--------------------------------------------------------------------------
   # Walk through the ordered lists of sections in lockstep
   #--------------------------------------------------------------------------
   for ($i = 0; $i < @sections1; $i++)
   {
      $sect1 = $sections1[$i];
      $sect2 = $sections2[$i];

      #-----------------------------------------------------------------------
      # A differing number of sections is caught in the file header check.
      # But that doesn't prevent this code from being run.  Because the file
      # header check will fail, there isn't anything here to find the extra
      # sections in either file1 or file2.  That would be a fair amount of
      # work, with little to show for it.
      #-----------------------------------------------------------------------
      last if (!$sect2);

      $processing_stage = 'Comparing Sections : section = ' .
                             ofd_sect_name($sect1);
      next unless sect_headers_match($sect1, $sect2, $load_equals_run);

      compare_raw_data($sect1, $sect2);
      compare_hash_record($sect1, $sect2, 'build_attributes', undef);

      if (!$is_elf)
      {
	 #--------------------------------------------------------------------
	 # Compare relocations and line number entries only if interested in
	 # symbols too
	 #--------------------------------------------------------------------
         next if (!$compare_syms);

	 compare_array_hash_records(
	    $sect1, 
	    $sect2,
	    'relocations',
	    [ 'virtual_addr' ],
	    undef,
	    undef);

	 compare_hash_record($sect1, $sect2, 'line_numbers', undef);
      }
      else
      {
	 #--------------------------------------------------------------------
	 # This call to compare_hash_record looks at all section fields
	 # except those mentioned in skip_fields.  OK, this skip_fields
	 # includes almost everything.  But, if more fields sneak in (and a 
	 # few have), this call will compare them.
	 #--------------------------------------------------------------------
         my %skip_fields = (
	    build_attributes  => 1,   # already compared
	    dynamic_table     => 1,   # compare later
	    elf32_shdr        => 1,   # already compared
	    'index'           => 1,   # never compare
	    raw_data          => 1,   # already compared
	    relocations       => 1,   # compare later
	    string_table      => 1,   # never compare
	    symbol_hash_table => 1,   # never compare
	    symbol_table      => 1,   # compare later
	    verdef_table      => 1,   # compare later
	    verneed_table     => 1,   # compare later
	    );
	 compare_hash_record($sect1, $sect2, undef, \%skip_fields);

	 next if (!$compare_syms);

	 #--------------------------------------------------------------------
	 # Really ought to write a function that skips over certain 
	 # dynamic table entries that are really just file pointers and
	 # sizes, and pass the function in just like the symbol table
	 # comparison call does.
	 #--------------------------------------------------------------------
	 compare_array_hash_records(
	    $sect1,
	    $sect2,
	    'dynamic_table',
	    [qw(d_tag d_val d_ptr)],
	    undef,
	    undef);

	 compare_array_hash_records(
	    $sect1,
	    $sect2,
	    'verdef_table',
	    [ 'vd_ndx' ],
	    { vd_ndx => 1 },
	    undef);

	 compare_array_hash_records(
	    $sect1,
	    $sect2,
	    'verneed_table',
	    [ 'vn_file' ],
	    undef,
	    undef);

	 compare_hash_record($sect1, $sect2, 'relocations', undef);
	 elf_compare_symbols($sect1, $sect2);
      }
   }
   
   raw_data_teardown();
}

#=============================================================================
# SECT_HEADERS_MATCH - Compare section headers.  Return 1 if the overall
#    section comparison should continue.  Return undef if the section headers
#    are so different that further comparison doesn't make sense.
#=============================================================================
sub sect_headers_match
{
   my ($sect1, $sect2, $load_equals_run) = @_;
   my (%skip_fields, $diffs);

   if (!$is_elf)
   {
      %skip_fields = (
         'file_offsets'     => 1,    # never compare
         'index'            => 1,    # never compare
	 'line_numbers'     => 1,    # compare later
	 'relocations'      => 1,    # compare later
	 'raw_data'         => 1,    # compare later
	 'build_attributes' => 1,    # compare later
      );

      if ($load_equals_run) { $skip_fields{'virtual_addr'} = 1; }
      $diffs = compare_hash_record( $sect1, $sect2, undef, \%skip_fields);
   }
   else
   {
      #-----------------------------------------------------------------------
      # Note 'sh_name' is not compared because it is an index into the
      # string table.  'sh_name_string' has the full name of the section,
      # and that is compared.
      #-----------------------------------------------------------------------
      %skip_fields = (
	 'sh_offset'  => 1,          # never compare
	 'sh_link'    => 1,          # never compare
	 'sh_info'    => 1,          # never compare
	 'sh_entsize' => 1,          # never compare
	 'sh_name'    => 1,          # never compare
      );

      $diffs = compare_hash_record($sect1, $sect2, 'elf32_shdr', \%skip_fields);
   }

   if ($diffs)
   {
      #-----------------------------------------------------------------------
      # If the only difference is run address, then section comparison
      # continues.  Otherwise, stop comparing now.
      #-----------------------------------------------------------------------
      if (   !exists($diffs->{only_in_file1})
	  && !exists($diffs->{only_in_file2})
	  && exists($diffs->{mismatch})
	  && scalar(@{$diffs->{mismatch}}) == 1
	  && ofd_sect_run_addr($sect1) != ofd_sect_run_addr($sect2))
	 { return 1; }
      else
         { return undef; }
   }

   return 1;
}

BEGIN {
   #==========================================================================
   # ELF sections always skipped over
   #==========================================================================
   my %elf_skip_section   = (
      '.dynstr'   => 1,
      '.strtab'   => 1,
      '.shstrtab' => 1,
      );

   #==========================================================================
   # ELF sections skipped over unless symbol comparison is requested
   #==========================================================================
   my %elf_symbol_section = (
      '.ARM.preemptmap' => 1,
      '.dynamic'        => 1,
      '.dynsym'         => 1,
      '.got'            => 1,
      '.hash'           => 1,
      '.plt'            => 1,
      '.symtab'         => 1,
      '.symtab_shndx'   => 1,
      );

   #==========================================================================
   # These sections are deliberately unmentioned in these tables, which
   # means these sections *are* compared.  Unless otherwise noted, these
   # sections are not expected to appear in TI object files any time soon.
   #
   # - <anything>.exidx - related to table driven exception handling
   # - <anything>.extab - related to table driven exception handling
   # - .ARM.debug_overlay 
   # - .ARM.overlay_table 
   # - .fini
   # - .fini_array 
   # - .init
   # - .init_array 
   # - .interp
   # - .line
   # - .note
   # - .preinit_array
   # - .rodata
   # - .rodata1
   # - .tbss
   # - .tdata
   #
   # All of these section names come from:
   # - General ELF spec
   #   + http://www.sco.com/developers/gabi/2003-12-17/contents.html
   #   + See "Special Sections"
   # - ARM specific additions to ELF
   #   + http://www.arm.com/products/DevTools/ABI.html
   #   + See "ELF for the ARM Architecture" section 4.4.4 "Special Sections"
   #==========================================================================

   #==========================================================================
   # SKIP_SECTION - Should this section be skipped?
   #==========================================================================
   sub skip_section
   {
      my ($sect) = @_;
      my ($name, $size, $flag);

      $name = ofd_sect_name($sect);
      $size = ofd_sect_size($sect);

      #-----------------------------------------------------------------------
      # Skip the NULL named section that occurs in ELF.
      # Unless the --dwarf cmd line flag was used, skip Dwarf debug sections.
      # Or ... User can request sections be skipped by name.
      # Or ... User can request that empty sections be skipped.
      #-----------------------------------------------------------------------
      if (   !$name
	  || !$compare_dwarf && ($name =~ m/^.debug/ || $name =~ m/^__TI_DW/)
	  || $skip_by_name{$name}
	  || $skip_empty_sects && !$size)
      {
	 return 1;
      }

      #-----------------------------------------------------------------------
      # For ELF, some sections are always skipped.  Others are skipped if
      # you are not comparing symbol info, including any section with a
      # name of the form ".rel<something>".
      #-----------------------------------------------------------------------
      if ($is_elf)
      {
         return 1 if (exists $elf_skip_section{$name});
	 if (!$compare_syms)
	 {
	    return 1 if (exists $elf_symbol_section{$name});
	    return 1 if ($name =~ m/^\.rel/);
	 }
      }

      #-----------------------------------------------------------------------
      # User can also request sections be skip by section header flag.
      # Examples include 'copy', 'bss', etc.
      #-----------------------------------------------------------------------
      if (!$is_elf)
      {
	 foreach $flag (@skip_by_flag)
	 {
	    if (exists $sect->{$flag} && $sect->{$flag} eq 'true')
	       { return 1; }
	 }
      }
      else
      {
	 foreach $flag (@skip_by_flag)
	 {
	    if ($flag eq 'text' && ofd_sect_is_code($sect))
	       { return 1; }
	    if (   $flag eq 'data'
	        && ofd_sect_is_data($sect)
		&& ofd_sect_is_init($sect))
	       { return 1; }
	    if (   $flag eq 'bss'
		&& ofd_sect_is_data($sect)
		&& !ofd_sect_is_init($sect))
	       { return 1; }
	    if ($flag eq 'copy' || $flag eq 'noload' || $flag eq 'dummy')
	       { warn "Skipping $flag sections is not supported for ELF"; }
	 }

         #--------------------------------------------------------------------
	 # Consider adding to config syntax: NOT_SHF_ALLOC.  For ELF that
	 # would correspond to a check here that the SHF_ALLOC flag is *not*
	 # set.  That is true of every section is which not used on the
	 # target.  The XDC section xdc.meta could be skipped in this
	 # manner.
         #--------------------------------------------------------------------
      }

      return undef;
   }
}

#=============================================================================
# RAW_DATA_SETUP - Called once at the start of the sections comparison func
#=============================================================================
sub raw_data_setup
{
   my ($file1_data, $file2_data) = @_;
   my ($sect_data, $sect_with_data, $f1_name, $f2_name);

   #--------------------------------------------------------------------------
   # Presume not reading directly from object file
   #--------------------------------------------------------------------------
   $reading_obj = undef;

   #--------------------------------------------------------------------------
   # Find first section that has raw data
   #--------------------------------------------------------------------------
   $sect_with_data = undef;
   foreach $sect_data (ofd_each_section($file1_data))
   {
      if (sect_has_raw_data($sect_data))
      {
	 $sect_with_data = $sect_data;
	 last;
      }
   }

   #--------------------------------------------------------------------------
   # Quit if raw data is in the XML.  Also, must account for rare case where
   # the file has no initialized sections (i.e. !$sect_with_data).
   #--------------------------------------------------------------------------
   return if (!$sect_with_data || ofd_sect_raw_data($sect_with_data));

   #--------------------------------------------------------------------------
   # If comparing libs, must issue error
   #--------------------------------------------------------------------------
   if ($comparing_libs)
   {
      warn << "NEED_RAW_DATA_FIELD";
>> When comparing data from object files contained in libraries, must use a
>> recent version of OFD which encodes the raw data directly into the XML.
>> Run 'perldoc objdiff.pl' for more information.
NEED_RAW_DATA_FIELD

      die "Stopped";  # Perl adds: at <file> <line num>
   }

   #--------------------------------------------------------------------------
   # OK to read raw data directly from object file
   #--------------------------------------------------------------------------
   $reading_obj = 1;

   $f1_name = $file1_data->{name};
   $f2_name = $file2_data->{name};
   open(FILE1, "<$f1_name") or die "Cannot open $f1_name: $!";
   open(FILE2, "<$f2_name") or die "Cannot open $f2_name: $!";

   binmode(FILE1);
   binmode(FILE2);
}

#=============================================================================
# RAW_DATA_TEARDOWN - Called once at the end of the sections comparison func
#=============================================================================
sub raw_data_teardown
{
   return if (!$reading_obj);

   close(FILE1);
   close(FILE2);
}

#=============================================================================
# COMPARE_RAW_DATA
#=============================================================================
# Note the section headers must mostly match before this routine is called.
# The length of the raw data, in particular, must be an exact match.
#=============================================================================
sub compare_raw_data
{
   my ($sect1, $sect2) = @_;
   my ($size, $data_ptr1, $data_ptr2, $buff1, $buff2, $diffs,
       @buff1_bytes, @buff2_bytes);

   #--------------------------------------------------------------------------
   # Quit if there is no raw data
   #--------------------------------------------------------------------------
   return if (!sect_has_raw_data($sect1));

   #--------------------------------------------------------------------------
   # If we have the MIME64 form of the raw data, compare it straight up
   #--------------------------------------------------------------------------
   if (!$reading_obj)
   {
      $buff1 = ${ofd_sect_raw_data($sect1)};
      $buff2 = ${ofd_sect_raw_data($sect2)};
   }
   #--------------------------------------------------------------------------
   # Otherwise, read the data from the obj file directly.  Note this is
   # not done for libraries.
   #--------------------------------------------------------------------------
   else
   {
      $size      = ofd_sect_size($sect1);
      $data_ptr1 = ofd_sect_raw_data_ptr($sect1);
      $data_ptr2 = ofd_sect_raw_data_ptr($sect2);

      seek(FILE1, $data_ptr1, 0) || die "Cannot seek: $!";
      read(FILE1, $buff1, $size) || die "Cannot read: $!";
      seek(FILE2, $data_ptr2, 0) || die "Cannot seek: $!";
      read(FILE2, $buff2, $size) || die "Cannot read: $!";
   }

   #--------------------------------------------------------------------------
   # Quit if raw data matches
   #--------------------------------------------------------------------------
   return if ($buff1 eq $buff2);

   #--------------------------------------------------------------------------
   # In default non-verbose mode, just record the difference
   #--------------------------------------------------------------------------
   if (!$verbose)
   {
      push @{$diffs_found{$processing_stage}}, { raw_data_mismatch => 1 };
      return;
   }

   #--------------------------------------------------------------------------
   # In verbose mode dump out all the raw data differences
   #--------------------------------------------------------------------------
   if ($comparing_libs)
   {
      print '*' x 70, "\n";
      print "Filename $basename1\n";
      print '*' x 70, "\n";
   }

   print '=' x 70, "\n";
   print "$processing_stage\n";
   print '=' x 70, "\n";

   print "Raw data is different\n";
   $saw_diff = 1;

   #-----------------------------------------------------------------------
   # Delay decoding the MIME64 string as long as possible
   #-----------------------------------------------------------------------
   if (!$reading_obj)
   {
      $buff1 = decode_base64($buff1);
      $buff2 = decode_base64($buff2);
   }

   #-----------------------------------------------------------------------
   # Comparing in terms of host bytes isn't quite correct for targets where
   # the minimum addressable unit (MADU) is bigger than an 8-bit byte.
   # But, this keeps things target independent, and it doesn't miss any
   # actual differences.  It is just that $i may not be the target address
   # offset of where the difference occurs.
   #-----------------------------------------------------------------------
   # Consult Perl docs on unpack to know what this does (a lot!)
   #-----------------------------------------------------------------------
   @buff1_bytes = unpack("C*", $buff1);
   @buff2_bytes = unpack("C*", $buff2);

   #-----------------------------------------------------------------------
   # Note loop iteration count is not $size above.  That is different
   # than the number of host bytes in the buffer on targets where the
   # MADU is bigger than an 8-bit byte.
   #-----------------------------------------------------------------------
   my ($i, $byte1, $byte2);
   for ($i = 0; $i < scalar(@buff1_bytes); $i++)
   {
      $byte1 = $buff1_bytes[$i];
      $byte2 = $buff2_bytes[$i];

      next if ($byte1 eq $byte2);

      print "   Byte $i is different\n";
      print "      $comp_name1: "; printf("0x%x\n", $byte1);
      print "      $comp_name2: "; printf("0x%x\n", $byte2);
   }
}

#=============================================================================
# SECT_HAS_RAW_DATA - Does section contain raw data?
#=============================================================================
sub sect_has_raw_data
{
   my ($sect) = @_;
   my ($size, $data_ptr);

   $size     = ofd_sect_size($sect);
   $data_ptr = ofd_sect_raw_data_ptr($sect);

   return (!ofd_sect_is_uninit($sect) && $size && $data_ptr);
}

#=============================================================================
# SYM_IS_NOT_GLOBAL
#=============================================================================
sub sym_is_not_global
{
   my ($sym) = @_;
   return !ofd_sym_is_global($sym);
}

#=============================================================================
# COFF_COMPARE_SYMBOLS
#=============================================================================
sub coff_compare_symbols
{
   my ($file_data1, $file_data2) = @_;

   $lookup_array_with{'dimension'} = [ 'dim_num' ];

   compare_array_hash_records($file_data1->{ti_coff},
                              $file_data2->{ti_coff},
			      'symbol_table',
			      [ 'name' ],
			      { 'index' => 1 },
			      \&sym_is_not_global);
}

#=============================================================================
# ELF_COMPARE_SYMBOLS
#=============================================================================
sub elf_compare_symbols
{
   my ($sect1, $sect2) = @_;

   compare_array_hash_records($sect1,
                              $sect2,
			      'symbol_table',
			      [ 'st_name_string' ],
			      { 'index' => 1, st_shndx => 1, st_name => 1 },
			      \&sym_is_not_global);
}

#=============================================================================
# DUMP_DIFFS - Print out the differences found in one pair of files
#-----------------------------------------------------------------------------
# - %diffs_found is keyed by the processing stage where the diff was found.
#   This key is chosen carefully because it is printed out here.  That is the
#   only way a user can figure out where the difference occurs.  See how it
#   is formed in compare_hash_record and compare_array_hash_records.  The
#   value of this hash is an array of "diff" records.  
# - Fields that *may* appear in a "diff" record are:
#   + mismatch - Refers to an array of fields that are different
#   + ref1 - Refers to part of the data from file1 where a mismatch occurs
#   + ref2 - Refers to part of the data from file2 where a mismatch occurs
#   + only_in_file1 - Refers to an array of fields only found in file1
#   + only_in_file2 - Refers to an array of fields only found in file2
#   + raw_data_mismatch - Indicates the raw data is different.  Value of
#     this field is not used.  Raw data mismatches are printed out only in
#     verbose mode, in the subroutine compare_raw_data.
#=============================================================================
# Why is everything sorted as it is printed out?  To make automated test
# regression possible. "sort keys" is a well known idiom.  But why are the
# arrays sorted?  Because the arrays were built by working through unsorted
# hashes (you might think of them as records). It is faster to delay sorting
# until now.  Rather than sorting everything, you only sort what is found
# to be different.
#=============================================================================
sub dump_diffs
{
   my ($stage, $diff, $field, $ref1, $ref2);

   return if (!%diffs_found);

   if ($comparing_libs)
   {
      print '*' x 70, "\n";
      print "Filename $basename1\n";
      print '*' x 70, "\n";
   }

   $saw_diff = 1;

   foreach $stage (sort keys %diffs_found)
   {
      print '=' x 70, "\n";
      print "$stage\n";
      print '=' x 70, "\n";
      
      foreach $diff (@{$diffs_found{$stage}})
      {
	 if (exists $diff->{mismatch} && @{$diff->{mismatch}})
	 {
	    printf("%20s: %20s\t %20s\n", "Differences",
	       $comp_name1, $comp_name2);
	    foreach $field (sort @{$diff->{mismatch}})
	    {
	       printf("%20s: %20s\t %20s\n",
		  $field,
		  field_string($diff->{ref1}, $field),
		  field_string($diff->{ref2}, $field));
	    }
	 }

	 if (exists $diff->{only_in_file1} && @{$diff->{only_in_file1}})
	 {
	    print "Only in $comp_name1\n";
	    print "\t", join("\n\t", sort @{$diff->{only_in_file1}}), "\n";
	 }

	 if (exists $diff->{only_in_file2} && @{$diff->{only_in_file2}})
	 {
	    print "Only in $comp_name2\n";
	    print "\t", join("\n\t", sort @{$diff->{only_in_file2}}), "\n";
	 }

	 if (exists $diff->{raw_data_mismatch})
	    { print "\tRaw data is different\n", }
      }
   }
}

#=============================================================================
# FIELD_STRING - Given a reference to a record and the name of a field within
#   that record, return a string which represents the value of that field.
#   Much error checking here.
#=============================================================================
sub field_string
{
   my ($ref, $field) = @_;
   my ($ref_type);

   if (!defined($ref) || !defined($ref->{$field}))
      { return '(MISSING)'; }
   
   $ref_type = ref($ref->{$field});
   if (!$ref_type)
      { return $ref->{$field}; }  # the "normal" case
   elsif ($ref_type eq 'HASH')
      { return '(RECORD)'; }
   elsif ($ref_type eq 'ARRAY')
      { return '(LIST)'; }
   else
      { return '(???)'; }
}

#=============================================================================
# STR_TO_NUM - Convert string to number.  Works regardless of the format the
#    number is written in.  Google "perl oct" for more detail.
#=============================================================================
sub str_to_num
{
   my ($str) = @_;

   return ($str =~ m/^0/) ? oct($str) : $str;
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

