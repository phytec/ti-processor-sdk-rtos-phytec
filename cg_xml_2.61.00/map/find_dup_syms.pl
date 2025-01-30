#!/usr/bin/env perl
# The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#==============================================================================
# POD Documentation First ...
#==============================================================================

=head1 NAME

find_dup_syms.pl

=head1 SYNOPSIS

Checks for multiply defined symbols. Helps developer determine if he/she *might*
accidentally be picking up the wrong version of a function.

For example, suppose there is a user-defined version of clock() in an
application that also includes an off-the-shelf library that defines
the same function.  If the wrong version gets linked in, host time might be
read and real-time behavior could be messed up.

=head1 USAGE

    find_dup_syms.pl <options> <XML-file> <NM-cmd>

NM-cmd is the utility that prints the name list of an object file.
Examples: nm6x for C6000 or nm55 for C5500. See "HOW TO CREATE THE XML FILE"
(below) for details on generating the XML file.

=head1 OPTIONS

    --help                      : Print usage message
    --quiet                     : Quiet mode (suppress warnings)
    --other_symbols_too         : Check for duplicates of bss, data, and
                                  weak symbols
    --short                     : Short output format - faster run time
                                  mutually excl. with --ignore_pulled_from
    --ignore_pulled_from=<file> : Suppress printing of duplicates which are
                                  pulled from <file>
                                  mutually exclusive with --short
    --basedir=<dir>             : Link command was run from directory <dir>
    --cg_xml_version            : Print out the version of the cg_xml package

=head1 EXAMPLES

If the final link command was performed in the directory
C:\MyProjectDir\release, use

    find_dup_syms.pl --basedir=C:\MyProjectDir\release xmlfile.xml nm6x

Suppose you do not care about duplicate symbol definitions as long as the symbol
was pulled in from file a.lib.  Use

    find_dup_syms.pl --ignore_pulled_from=a.lib xmlfile.xml nm6x

In this case, if the script can determine conclusively that a symbol was
pulled in from a.lib, it will silently ignore duplicate definitions of that
symbol. However, if the script can only narrow down the source of the pulled in
symbol to the set consisting of a.lib and b.lib then it will still print out the
information for that symbol.

=head1 DETAILS

Script reads in an XML file representation of a linker map file
and, with the aid of nmXX, reports global symbols which are defined in
multiple XML files. Script also reports which version of a symbol was actually
selected by the linker.

The script must be able to locate each object file or archive. Hence, the script
must be run from the same directory from which the final link was performed or
the --basedir=<dir> option must be used to specify the directory where the
final link was performed.

Normally, the XML file must be specified as an absolute path or relative to the
current directory.  As a convenience, if the --base_dir=<dir> option is present,
the XML file can also be specified relative to <dir>.

Other Perl scripts that work with utilities typically allow you to directly
pipe the output of the utility directly into the script.  This script does not
work that way; it requires the NM output from multiple files, not just one.

By default, the script checks for duplicate definitions of text (code), data
and bss symbols.  Absolute symbol definitions are ignored. The reason is that on
some targets, the compiler generates duplicate absolute symbol definitions by
default. Handling this case would either generate a lot of false positives on
some targets or yield a highly compiler-version dependent script). Duplicate
weak definitions (Elf only feature) are considered by request only
(--weak option).  Weak definitions provide a method for a library developer to
define library symbols that a user might want to override.

Also by default, when duplicate symbols are discovered, the script prints out
which definition was linked in (i.e, the name of the archive/object file which
contains the definition that was linked in).  This computation takes about 30%
of the total run time.  If this information is not needed, run the script with
--short to get faster turnaround time.

=head1 HOW TO CREATE THE XML FILE

Generate the XML version of the map file with the linker option

    --xml_link_info=<file>

You must use a relatively recent linker which supports the --xml_link_info
option.  However, there is NO requirement as to which version of the tools
were used to create the libraries being linked in.  In other words, this
script will work even if some of the libraries were built with old compiler
tools.

=head1 REQUIREMENTS

=over

=item *

XML version of map file.

=item *

nm command

=item *

All libraries and object files must exist under the same name and in the same
location as when the application was linked and must be readable.

=back

=head1 LIMITATIONS

=over

=item *

Script must either be run from same directory in which final link was performed
or the --basedir option must be used to specify directory from which final link
was performed.

=item *

If there are 0 calls to a lib, that lib will not show up in the .xml file and
hence, will not be checked for duplicate symbol definitions.

=item *

Libraries that are linked into a partial .out and then relinked will generally
not be checked for duplicate symbol definitions.  For example, after the
following series of link steps, the files p1.lib and p2.lib will not show up in
full.xml and hence will not be checked.  Only references to members that needed
to be resolved to build the partial .out will get checked.

    cl6x -z -r [-a] p1.lib p2.lib partial.out
    cl6x -z --xml_link_info=full.xml partial.out ... -o full.out

=item *

An attempt is made to track down the archive/object file from which the
linked-in definition of a (multiply-defined) symbol originated.  Usually, the
script can precisely identify the file from which the definition was pulled.
In a few cases it cannot (e.g., when the output section containing the symbol
is part of a union).  If the script cannot identify a unique file, it will
print out a list of all candidates instead.

=back

=head1 CONFIGURE FOR YOUR SYSTEM

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=cut

#==============================================================================
# Code starts here ...
#==============================================================================

#------------------------------------------------------------------------------
# Organization
#------------------------------------------------------------------------------
# PRIMARY FUNCTIONS
#
# main                     : main driver
#   process_cmd_line       : reads in command line options
#   read_in_xml            : read xml file into xml data structure
#   search_for_dups        : creates list of files defining each ref'd symbol
#     build_file_path      : builds hash (keys are libs/objs, values are paths)
#     process_file_syms    : for sgl sym, builds list of libs/objs that def it
#   print_dup_syms         : print out info on all multiply-defined symbols
#     report_dup_sym       : for sgl symbol, print out duplicate def info
#       find_linked_in_def : for sgl sym, find file which contains pulled-in def
#
# DEBUG ROUTINES
#
#   dump_out_file_path
#   dump_out_sym_files
#   dump_out_xml_data

use strict;
use warnings;
use Getopt::Long;

#==============================================================================
# Tell Perl that the module XML_TI_MAP.pm in the same directory as this script
#==============================================================================
use FindBin;
use lib $FindBin::Bin;

use XML_TI_MAP 1.01 qw(process_xml_file cg_xml_version);

#==============================================================================
# Global variables
#==============================================================================
my ($nm_cmd, $xml_file);

#==============================================================================
# $xml_data - central hash of data from xml file.
#==============================================================================
my $xml_data;

#==============================================================================
# %file_path - central hash of scalars built by script.  Key is base file name.
# Value is absolute or relative path.  Path may be empty if file resides in
# current directory. The full path to the file is then
# "$file_path{$file} . $file". Based on data from nmXX. Warning is generated if
# the same base path name is associated with multiple paths.  All but one is
# ignored.  Call dump_out_file_path() to dump this data structure.
#==============================================================================
my %file_path;

#==============================================================================
# %sym_files - central hash of arrays built by script. Key is symbol name.
# Value is an array of base file names which define symbol. An array of
# files assoc. with a sym can be extracted as @files = @{sym_files{$sym}}.
# Maps symbols from libraries and object files to the list of libraries/object
# files where the symbol is declared. By construction, each array must contain
# at least 1 file name. Call dump_out_sym_files() to dump this data structure.
#==============================================================================
my %sym_files;

#==============================================================================
# Modes associated with options to find_dup_syms.pl
#==============================================================================
my $debug_mode;          # --debug
my $verbose_mode;        # --verbose
my $quiet_mode;          # --quiet
my $other_symbols;       # --other-symbols_too
my $short_output;        # --short
my $basedir;             # --basedir
my %ignore_pulled_from;  # --ignore_pulled_from

#==============================================================================
# MAIN ROUTINE
#==============================================================================
if ($^V lt v5.8.3 && !$quiet_mode)
{
  print STDERR  "WARNING: Developed under Perl version 5.8.3.\n";
  printf STDERR "         May not work under version %vd\n\n", $^V;
}

process_cmd_line(); # read in command line.
read_in_xml();      # read in xml file.
search_for_dups();  # find duplicates.
print_dup_syms();   # print info.
exit(0);
#end main

#==============================================================================
# PROCESS_CMD_LINE():
# PARAMETERS:         none
# RETURN VALUE:       none
# DESCRIPTION:
#   Reads in command line options and xml file:
#       find_dup_syms.pl <XML-file> <NM-cmd>
#   Exposed options:
#       --help                      : Print usage message
#       --quiet                     : Quiet mode (suppress warnings)
#       --other_symbols_too         : Check for duplicates of bss, data
#                                     and weak symbols
#       --short                     : Short output format - faster run time
#       --ignore_pulled_from=<file> : Suppress printing of duplicates which are
#                                     pulled from <file>
#       --basedir=<dir>             : Link command was run from directory <dir>
#   Hidden options:
#       --debug [1-9]               : Debug mode (currently, 1-3 in use).
#       --verbose                   : Verbose mode
#==============================================================================
sub process_cmd_line
{
    my $pn;
    my $print_usage = 0;
    my (@temp_file_list) = ();
    my ($file);

    #--------------------------------------------------------------------------
    # If --cg_xml_version is requested, write that out to stderr
    #--------------------------------------------------------------------------
    cg_xml_version();

    #--------------------------------------------------------------------------
    # Get options (described above).
    #--------------------------------------------------------------------------
    $debug_mode   = $verbose_mode  = $quiet_mode = 0;
    $short_output = $other_symbols = 0;
    $basedir = "";
    GetOptions("debug=i"                     => \$debug_mode,
               "help"                        => \$print_usage,
               "verbose"                     => \$verbose_mode,
               "quiet"                       => \$quiet_mode,
               "short"                       => \$short_output,
               "basedir=s"                   => \$basedir,
               "ignore_pulled_from=s"        => \@temp_file_list,
               "other_symbols_too"           => \$other_symbols);
    $debug_mode = 0   if ($debug_mode < 0);
    print "Debug enabled at level $debug_mode.\n" if ($debug_mode != 0);

    #--------------------------------------------------------------------------
    # Script does not check where symbol was pulled from in short output mode.
    # So, ignore --short if --ignore_pulled_from is specified.
    #--------------------------------------------------------------------------
    if (@temp_file_list > 0 && $short_output) {
        warn "WARNING: --short ignored. " .
             "Mutually exclusive with --ignore_pulled_from\n"
             unless ($quiet_mode);
        $short_output = 0;
    }

    #--------------------------------------------------------------------------
    # Convert "ignore_pulled_from" list into hash.
    #--------------------------------------------------------------------------
    foreach $file (@temp_file_list) {
        $ignore_pulled_from{$file} = 1;
    }

    #--------------------------------------------------------------------------
    # Debug mode implies verbose mode.
    #--------------------------------------------------------------------------
    $verbose_mode = 1 if ($debug_mode > 0);

    if ($print_usage != 0 || @ARGV != 2) {
        #----------------------------------------------------------------------
        # Strip off path from program name
        #----------------------------------------------------------------------
        $0 =~ m/([\w\.]+$)/;
        $pn = $1;

        print <<"END_COMMENT";
Usage: $pn <options> <XML-file> <NM-cmd>
    --help                      : Print usage message
    --quiet                     : Quiet mode (suppress warnings)
    --other_symbols_too         : Check for duplicates of bss, data, and weak
                                  symbols
    --short                     : Short output format - faster run time
    --ignore_pulled_from=<file> : Suppress printing of duplicates which are
                                  pulled from <file>
    --basedir=<dir>             : Link command was run from directory <dir>
END_COMMENT
        exit 0;
    }

    #--------------------------------------------------------------------------
    # Check whether XML file exists.
    # XML file can be specified as absolute path, relative to current directory
    # or relative to $base_dir.  Check for absolute path or relative to current
    # directory first. If these fail, check if path is relative to $basedir.
    #--------------------------------------------------------------------------
    $xml_file = shift @ARGV;
    unless (-e $xml_file) {
        die "ERROR: Cannot find $xml_file: $!"
            unless (-e $basedir . "/" . $xml_file);
        $xml_file = $basedir . "/" . $xml_file;
    }

    #--------------------------------------------------------------------------
    # Check whether $nm_cmd exists in current path by trying to execute it.
    # $? handling is explained in Programming Perl, 3rd Ed., pg #659-660
    # and on pg #134 of Programming Perl, 2nd Ed.
    #--------------------------------------------------------------------------
    $nm_cmd = $ARGV[0];
    qx($nm_cmd -h);
    if ($? >> 8) { exit 1; }
}  # end process_cmd_line

#==============================================================================
# READ_IN_XML()
# PARAMETERS:        none
# RETURN VALUE:      none
# DESCRIPTION:
#   Read data from XML file into %xml_data.
# NOTE:
#   This is the slowest function in the script.
#==============================================================================
sub read_in_xml
{
    #--------------------------------------------------------------------------
    # Slurp in xml data generated by linker.
    #--------------------------------------------------------------------------
    print "Reading in \"$xml_file\"\n" if ($verbose_mode);
    $xml_data = process_xml_file($xml_file);
    dump_out_xml_data($xml_data) if ($debug_mode >= 3);
} # end read_in_xml

#==============================================================================
# SEARCH_FOR_DUPS()
# PARAMETERS:        none
# RETURN VALUE:      none
# DESCRIPTION:
#   Search for duplicate symbols.
#   Step 1: Build hash of all object files and libraries.  Value is full path.
#   Step 2: Successively process each file in %file_path:
#       2a: Run NM cmd on each file to generate list of symbols.
#       2b: Add each symbol to hash $sym_files if not already present.
#       2c: Add name of (lib/obj) file defining sym to file list assoc. w/sym.
#==============================================================================
sub search_for_dups
{
    my $file;

    build_file_path();
    foreach $file (sort keys %file_path) {
        print "Processing \"$file\"\n" if ($verbose_mode);
        process_file_syms($file);
    }
    dump_out_sym_files() if ($debug_mode >= 2);
} # end search_for_dups


#==============================================================================
# BUILD_FILE_PATH()
# PARAMETERS:        none
# RETURN VALUE:      none
# DESCRIPTION:
#   Builds %file_path. $file_path{$file} = $path. $file is an archive/obj
#   linked into executable.
# NOTES:
#   If there are multiple full paths associated with the same basename,
#   all but the first one in alphabetical order will be silently ignored.
#   This method of selecting which one to process makes script output less
#   dependent on changes in future versions of Perl or the compiler.
#==============================================================================
sub build_file_path
{
    my ($files, $file_rec, $file, $path, $actual_path);

    #--------------------------------------------------------------------------
    # build hash of base file names and associated paths
    #--------------------------------------------------------------------------
    $files = $xml_data->{'input_file_list'};
    foreach $file_rec (values %$files) {
        #----------------------------------------------------------------------
        # Read file and path from xml data file.
        #----------------------------------------------------------------------
        my ($file) = $file_rec->{'file'};
        my ($path) = $file_rec->{'path'};

        #----------------------------------------------------------------------
        # Make sure that $file and $path are both defined.
        # If $path not defined, then path is current dir, so set to "".
        # This avoids alot of script checks later on.
        # Error if $file not defined.
        #----------------------------------------------------------------------
        $path = "" unless (defined $path);
        die "ERROR: Bad XML file. Missing file name." if (!defined $file);

        #----------------------------------------------------------------------
        # File should be an archive or normal .obj file. There should not be
        # anything else.
        #----------------------------------------------------------------------
        if ($file_rec->{kind} ne 'archive' && $file_rec->{kind} ne 'object') {
            die "ERROR: Unexpected file type: $file_rec->{kind}";
        }

        #----------------------------------------------------------------------
        # Search for file. If file cannot be found, skip it.
        #----------------------------------------------------------------------
        $actual_path = find_file($path, $file);
        next unless (defined $actual_path);

        #----------------------------------------------------------------------
        # Duplicates could be same file or different file with same base
        # name. Skip the one that comes later in alphabetical order.  This
        # preserves output order across Perl versions and compiler releases.
        #----------------------------------------------------------------------
        if (exists $file_path{$file}) {
            if ($actual_path lt $file_path{$file}) {
                ($file_path{$file}, $actual_path) =
                    ($actual_path, $file_path{$file});
            }
            if ($actual_path ne $file_path{$file}) {
                warn_dup_file($file, $file_path{$file}, $actual_path);
            }
            next;
        }

        $file_path{$file} = $actual_path;
    }

    dump_out_file_path() if ($debug_mode >= 2);
} # end build_file_path

#==============================================================================
# %nofind_warning_generated - hash of scalars built by script.  key is full path
# to file. Value is 1. Whenever a warning is generated for that file,
# current directory. Whenever a warning is generated for that file, the file
# and its associated path are stored in this hash. That prevents the same
# warning from being generated multiple times. Enclosing braces make
# %nofind_warning_generated a static local to build_file_path().
# See Programming Perl, 3rd Ed., p.223 for more info.
#==============================================================================
{
    my %nofind_warning_generated = ();

    #==============================================================================
    # FID_FILE($path, file)
    # PARAMETERS:        $path - path from XML file
    #                    $file - base file name from XML file
    # RETURN VALUE:      actual path if file is found, undef otherwise
    # DESCRIPTION:
    #   XML file can include relative or absolute paths.  For the script to locate
    #   lib/obj files with relative paths, either the script must be run from the
    #   directory where final link was performed or basedir must be supplied by
    #   user. To avoid the complexity of checking whether path from XML file is
    #   absolute or relative, simply check for it both with and without the
    #   prepending the user-supplied basedir, if any.
    #==============================================================================
    sub find_file
    {
        my ($path, $file) = @_;
        my ($expanded_path);

        #-------------------------------------------------------------------------
        # First check for easy case where path is absolute or relative to current
        # current directory.  If file is found, return path.
        #-------------------------------------------------------------------------
        return $path if (-e $path . $file);

        #-------------------------------------------------------------------------
        # If user supplied basedir, check for existence of file relative to
        # basedir. If file
        #-------------------------------------------------------------------------
        if ($basedir ne "") {
            $expanded_path = $basedir . "/" . $path;
            return $expanded_path if (-e $expanded_path . $file);
        }

        #-------------------------------------------------------------------------
        # Generate warning unless warnings suppressed or already generated.
        #-------------------------------------------------------------------------
        unless ($quiet_mode) {
            warn "WARNING: Could not find \"$path$file\".\n"
                unless (exists $nofind_warning_generated{$path . $file});
            $nofind_warning_generated{$path . $file} = 1;
        }

        #-------------------------------------------------------------------------
        # Return undefined path.
        #-------------------------------------------------------------------------
        return undef;
    }  # end find_file
} #end scope %nofind_warning_generated

#==============================================================================
# %dup_warning_generated - hash of scalars built by script.  key is full path to
# file. Value is 1. Whenever a warning is generated for that file,
# the file and its associated path are stored in this hash. That
# prevents the same warning from being generated multiple times. Enclosing
# braces make %dup_warning_generated a static local to warn_dup_file().
# See Programming Perl, 3rd Ed., p.223 for more info.
#==============================================================================
{
    my %dup_warning_generated = ();

    #==========================================================================
    # WARN_DUP_FILE($file, $path1, $path2)
    # PARAMETERS:        $file  - (string) base file name
    #                    $path1 - (string) first path to file
    #                    $path2 - (string) second path to file
    # RETURN VALUE:      none
    # DESCRIPTION:
    #   Prints the following:
    #       WARNING: Skipping <full-path-to-file-2> because it has the same base
    #       name as <full-path-to-file1> (already checked).  Unsure if these two
    #       file are the same.
    #==========================================================================
    sub warn_dup_file
    {
        return if ($quiet_mode);

        #----------------------------------------------------------------------
        # Compute full path to both files.
        #----------------------------------------------------------------------
        my ($base_file_name, $path1, $path2) = @_;
        my ($file1) = $path1 . $base_file_name;
        my ($file2) = $path2 . $base_file_name;

        #----------------------------------------------------------------------
        # Generate warning about ignore duplicate base file name unless one has
        # already been generated.
        #----------------------------------------------------------------------
        if (!(exists $dup_warning_generated{$file2})) {
            warn  "WARNING: Skipping \"$file2\" because it has the same base " .
                  " name as \"$file1\" (already checked).  Unsure if these " .
                  " two files are the same.\n";
        }

        #----------------------------------------------------------------------
        # Updated $warning_generated, to prevent duplicate warnings.
        # Don't care about value for hash.  Only key checked.
        #----------------------------------------------------------------------
        $dup_warning_generated{$file2} = 1;
    } # end warn_dup_file
} # end scope of %dup_warning_generated

#==============================================================================
# PROCESS_FILE_SYMS($file)
# PARAMETERS:        $file - (string) base name of file to process
# RETURN VALUE:      none
# DESCRIPTION:
#   Process symbols definitions from sgl. lib/obj.
#   Step 1: Run nmXX -g $filename and collect output
#   Step 2: For each symbol in nmXX output:
#       2a: Add sym to hash of symbols ($sym_files) if not already present.
#       2b: Add file to list of files associated with symbol.
# NOTES:
#   The memory for $nm_lines is freed up automatically upon return from
#   this function.
#==============================================================================
sub process_file_syms
{
    my ($file)   = @_;
    my $fullpath = $file_path{$file} . $file;
    my $ln       = 0;
    my (@nm_lines, $nm_ln);
    my ($addr, $type, $sym);

    #--------------------------------------------------------------------------
    # $? handling is explained in Programming Perl, 3rd Ed., pg #659-660
    # and on pg #134 of Programming Perl, 2nd Ed.
    #--------------------------------------------------------------------------
    @nm_lines = qx($nm_cmd -g $fullpath);
    if ($? >> 8) { die "ERROR: Running $nm_cmd failed."; }

    foreach $nm_ln (@nm_lines) {
        $ln = $ln + 1;

        #----------------------------------------------------------------------
        # Skip first two lines.
        #----------------------------------------------------------------------
        next if ($nm_ln =~ /^\s*$/);        # 1st line: blank
        next if ($nm_ln =~ /^ARCHIVE:/);    # 2nd line: ARCHIVE: <lib>

        #----------------------------------------------------------------------
        # Each non-empty line should be a triplet consisting solely of triplet
        # <addr> <type> <sym>. <addr> is an 8 character hex address. <type> is
        # one of "T" (text), "B" (bss), "D" (data), "U" (undefined),
        # "A" (absolute symbol), "W" (weak definition - Elf only),
        # "w" (weak reference - Elf only). In theory, "C" (common) is also
        # supported but this is not currently generated by the compiler.
        # Issue warning message and skip line if it did not parse into these
        # three tokens.
        #----------------------------------------------------------------------
	# The check for optional space \s* at the end of the line ...  This
	# comes up when a Cygwin Perl is reading output generated by a DOS
	# nmXX executable.  You need to skip over the control-M character.
        #----------------------------------------------------------------------
        ($type, $sym) = ($nm_ln =~
                            m{
                                ^                   # start of line
                                [0-9A-Fa-f]{8}      # 8 character hex address
                                \s+                 # white space separator
                                ([A-Za-z])          # single character ($type)
                                \s+                 # white space separator
                                ([\.\/\$\'\w]+)     # identifier ($sym)
				\s*                 # optional space
                                $                   # end of line
                            }x);

        if (!(defined $type && defined $sym))
        {
       	    if (!$quiet_mode)
	    {
                warn  "WARNING: Unexpected output from $nm_cmd -g $fullpath. " .
                      "Skipping\n   line $ln: $nm_ln";
	    }

            next;
        }

        print "type = \"$type\", sym = \"$sym\"\n" if ($debug_mode >= 3);

        #----------------------------------------------------------------------
        # Ignore undefined symbols ("U").  No def implies cannot be a dup.
        # Ignore absolute symbols ("A"). For some targets, the compiler inserts
        # the same absolute symbol into every .obj file (e.g., on C55x.)
        # This does not cause a problem.  Flagging it would generate a lot of
        # false positives.  Trying to exclude specific compiler-generated
        # symbols would make this script more dependent on specific compiler
        # versions.  This capability can easily be added later if needed.
        # Ignore weak references ("w").  Consider weak definitions ("W") upon
        # request. Weak definitions prevail only if there is no non-weak
        # definition of a symbol.  Weak definitions provide a method for
        # defining library symbols that a user might want to override.  May
        # become important later whe Elf usage picks up. Added now because it
        # was easy.
        #
        # Bottom line: type must be one of "T", "B", "D" or "W".
        # Latter three permitted only in presence of --weak option.
        #----------------------------------------------------------------------
        next unless ($type eq "T" || $type =~ /^[WBD]$/ && $other_symbols);

        #----------------------------------------------------------------------
        # Check symbol table to determine if symbol is ever referenced. Symbol
        # appears in symbol table only if it is linked in by the application.
        # Ignore unreferenced symbols.
        #----------------------------------------------------------------------
        next unless (exists $xml_data->{'symbol_table'}->{$sym});

        #----------------------------------------------------------------------
        # Add $file to list of files defining symbol.
        #----------------------------------------------------------------------
        push @{$sym_files{$sym}}, $file;
    }
} #end process_file_syms

#==============================================================================
# PRINT_DUP_SYMS()
# PARAMETERS:        none
# RETURN VALUE:      none
# DESCRIPTION:
#   Prints out list of duplicates and total # of duplicates.
#   Searches $sym_files for symbols which have more than one file associated
#   with them and reports those symbols as duplicates.
#   Sorts symbols before processing to preserve output order across Perl
#   versions and releases.
#==============================================================================
sub print_dup_syms
{
    my $num_dups = 0;
    my ($sym);

    for $sym (sort keys %sym_files) {
        #----------------------------------------------------------------------
        # If >1 file associated with symbol, report duplicate symbol.
        #----------------------------------------------------------------------
        next if (@{$sym_files{$sym}} < 2);

        $num_dups = $num_dups + report_dup_sym($sym, @{$sym_files{$sym}});
    }

    print "Done. $num_dups Symbol(s) have multiple definitions.\n";
} # end print_dup_syms

#==============================================================================
# REPORT_DUP_SYM($sym, @files)
# PARAMETERS:        $sym   - (string) symbol to process
#                    @files - (array of strings) files which define symbol
# RETURN VALUE:      1 if dup reported, 0 otherwise.
# DESCRIPTION:
#   Prints
#       >>> "<symbol-name>" <cnt> defs within "<file1> <file2> ...". Pulled
#       from <file(s)>.
# NOTES:
#   This routine should never be called with a file list <2 in length.
#   The line "Pulled from..." is omitted under the short output format.
#   If the script cannot identify a unique file from which the definition was
#   pulled (e.g., in case of a union in linker command file), then all
#   candidates will be printed.
#==============================================================================
sub report_dup_sym
{
    my ($sym, @files) = @_;
    my ($count)       = scalar(@files);
    my (%unique_names, $file);
    my (@pulled_from);

    #--------------------------------------------------------------------------
    # File names might not be unique (e.g., if two object files in same
    # archive define the same symbol.) However, we only want to print the
    # file name once. Do this by converting the array to a hash before printing
    # and then printing out the hash keys.
    # Originally stored in array not hash, because we need to keep count of
    # number of defs (duplicate files too) to tell whether a symbol is multiply
    # defined. Make sure to wait until after checking whether there are
    # duplicate defs which should have been done in caller.
    #--------------------------------------------------------------------------
    foreach $file (@files) {
        $unique_names{$file} = 1;
    }
    @files = (sort keys %unique_names);

    #--------------------------------------------------------------------------
    # Short output format is about 30% faster.  Does not identify which file
    # contains the def that was linked in.  So, done if short output format.
    #--------------------------------------------------------------------------
    if ($short_output) {
        print ">>> \"$sym\": $count defs within \"@files\".\n";
        return 1;
    }

    #--------------------------------------------------------------------------
    # Get printable list of file(s) which could be source of linked in sym def.
    # If list is empty, user chose not to report this dup, so skip it.
    # Report 0 so that dup does NOT get counted in total.
    #--------------------------------------------------------------------------
    @pulled_from = find_linked_in_def($sym, %unique_names);
    return 0 if (@pulled_from == 0);

    print ">>> \"$sym\": $count defs within \"@files\".";
    print " Pulled from @pulled_from.\n"            if (@pulled_from == 1);
    print " Pulled from one of @pulled_from.\n"     if (@pulled_from > 1);
    return 1;
} # end report_dup_sym

#==============================================================================
# FIND_LINKED_IN_DEF($sym, %files)
# PARAMETERS:        $sym   - (string) symbol to process
#                    %files - (hash) keys are names of files which define symbol
# RETURN VALUE:      array of file names which might be source of linked in def
# DESCRIPTION:
#   Determine which def of symbol is linked in. In most cases, the script can
#   precisely pinpoint the file from which the definition was pulled.
#   However, in a few cases the script cannot narrow down the set of candidates
#   to a single file (e.g., output section containing symbol involved in
#   union).  In this case, the script identifies all candidates.
#   The script returns an array of printable file names (e.g., "a.lib(b.obj)"
#   in the case of archives or "c.obj" in case of plain object files).
#   If no candidates are found, print warning and return "(file not found)".
#   If all "pulled from" candidates were excluded by user, return empty list.
#==============================================================================
sub find_linked_in_def
{
    my ($sym, %files) = @_;     # symbol name, hash of files containing defs
    my ($sym_addr);             # run address of symbol
    my ($file_rec);             # file containing symbol
    my ($input_sections);       # hash of all input sections
    my ($start, $size);         # start addr and size of input section
    my (@file_ids);             # ids of files that might contain linked in def
    my ($sect_rec);             # input section record
    my ($file_id);		# id of file which might contain pulled-in def
    my ($lib, $obj, $full_name);# name of file which might contain pulled-in def
    my (@name_list) = ();	# printable list of candidate file names
    my ($found)     = 0;

    #--------------------------------------------------------------------------
    # Find run address of symbol.
    #--------------------------------------------------------------------------
    $sym_addr = oct $xml_data->{'symbol_table'}->{$sym}->{'value'};

    #--------------------------------------------------------------------------
    # Find list of all input sections.
    #--------------------------------------------------------------------------
    $input_sections  = $xml_data->{'object_component_list'};

    #--------------------------------------------------------------------------
    # Search all input sections. For each one that spans symbol's run address
    # and defines symbol, add the corresponding file name to the array.
    # Normally, there will be exactly one.  However, there may be more than one
    # candidate (e.g., in case of unions), so check the whole array each time.
    # Add printable name of each candidate to @name_list array.
    #--------------------------------------------------------------------------
    foreach $sect_rec (values %$input_sections) {
        #----------------------------------------------------------------------
        # Find start and size of input section $sect_rec.
        # Convert from hex (e.g., "0x43a4bbc9") to decimal.
        #----------------------------------------------------------------------
        $start     = oct $sect_rec->{'run_address'};
        $size      = oct $sect_rec->{'size'};

        #----------------------------------------------------------------------
        # Check whether input section address range spans symbol run address.
        # If not, done with this input section.
        #----------------------------------------------------------------------
        next unless ($sym_addr >= $start && $sym_addr < ($start + $size));

        #----------------------------------------------------------------------
        # Find name of file containing this input section.  For the case of
        # archives, $lib = archive, $obj = obj file.  For case of plain object
        # files, $lib = $obj = obj file.
        #----------------------------------------------------------------------
        $file_id     = $sect_rec->{'input_file_ref'};
        $file_rec    = ${$xml_data->{'input_file_list'}}{$file_id};
        ($lib, $obj) = ($file_rec->{'file'}, $file_rec->{'name'});

        #----------------------------------------------------------------------
        # There are few files that span the address range but do not actually
        # define the symbol.  (E.g., on c62x, these include
        # rts6200.lib(boot.obj) and rts6200(args_main.obj).) To eliminate these
        # "spurious" files, check whether the file is in the list of files
        # that actually define the symbol. Skip those that are not.
        #----------------------------------------------------------------------
        next unless (exists $files{$lib});

        #----------------------------------------------------------------------
        # Script will only report duplicates if at least one candidate was
        # found which user did not request script to ignore.
        #----------------------------------------------------------------------
        $found = 1 unless (exists $ignore_pulled_from{$lib});

        #----------------------------------------------------------------------
        # Found legitimate file that defines $sym.  Add printable version of
        # name to list of file names that define $sym. Usually, there will just
        # be one.
        #----------------------------------------------------------------------
        $full_name   = $obj                    if ($lib eq $obj); #plain .obj
        $full_name   = $lib . "(" . $obj . ")" if ($lib ne $obj); #archive

        push @name_list, $full_name;
    }

    #--------------------------------------------------------------------------
    # Check that there is at least one file in list.
    #--------------------------------------------------------------------------
    unless (@name_list > 0) {
        warn  "WARNING: Could not find file that contains pulled in def of " .
              "\"$sym\".\n" unless ($quiet_mode);
        push @name_list, "(file not found)";
    }

    #--------------------------------------------------------------------------
    # If all file matches were excluded by user, return the empty set.
    # Otherwise, return list of files that might be sources of pulled in symbol
    # version.  I.e., if user only excluded some of the files in the list,
    # return all files.  Otherwise, results could be misleading.  In particular,
    # script might report that a symbol was definitely pulled from a file when
    # in fact that file was only one of several candidates.
    #--------------------------------------------------------------------------
    return () if ($found == 0);
    return sort @name_list;
} # find_linked_in_def

#==============================================================================
# DUMP_OUT_FILE_PATH()
# PARAMETERS:        none
# RETURN VALUE:      none
# DESCRIPTION:
#   Debug routine.  Write to stdout the Perl data structures which correspond to
#   %file_path.
#==============================================================================
sub dump_out_file_path
{
    print "=============================================================\n";
    print "file_path:\n";
    print "--------------\n";

    my $file;
    for $file (sort keys %file_path) {
        print "file = \"$file\", path = \"$file_path{$file}\"\n";
    }

    print "=============================================================\n";
} # end dump_out_file_path

#==============================================================================
# DUMP_OUT_SYM_FILES()
# PARAMETERS:        none
# RETURN VALUE:      none
# DESCRIPTION:
#   Debug routine.  Write to stdout the Perl data structures which correspond to
#   %sym_files.
#==============================================================================
sub dump_out_sym_files
{
    print "=============================================================\n";
    print "sym_files:\n";
    print "--------------\n";

    my $sym;
    for $sym (sort keys %sym_files) {
        print "sym = \"sym\", files = \"@{$sym_files{$sym}}\"\n";
    }
    print "=============================================================\n";
} # end dump_out_sym_files

#==============================================================================
# DUMP_OUT_XML()
# PARAMETERS:        none
# RETURN VALUE:      none
# DESCRIPTION:
#   Debug routine.  Write to stdout the Perl data structures which correspond to
#   %xml_data.
#==============================================================================
sub dump_out_xml_data
{
    my ($xml_data) = @_;
    use Data::Dumper;

    $Data::Dumper::Indent   = 1;
    $Data::Dumper::Sortkeys = 1;

    print "=============================================================\n";
    print "xml_data:\n";
    print "---------\n";

    print Dumper($xml_data);

    print "\n";
    print "=============================================================\n";
} # end dump_out_xml_data


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

