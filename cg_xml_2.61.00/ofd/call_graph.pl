#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

call_graph.pl

=head1 SYNOPSIS

Generates a call graph, including the worst case stack depth.

=head1 LIMITATIONS

=head2 LIMITATIONS RELATED TO COMPUTING STACK DEPTH

Older compiler versions, when building with -g, prevent computing the stack
depth.  If you build with a compiler version >= than those shown here, then
this limitation does NOT apply to you.

 Device Family         Minimum Compiler Version
 -------------         ------------------------
 ARM                   4.6.0
 C2000                 5.2.0
 C6000                 7.0.0
 MSP430                3.1.0

If you use an older compiler version, then none of the code can be built with
the -g switch.  If the code is built with -g, a call graph can still be built.
But the stack depth information is inaccurate.

=head2 LIMITATIONS FOR C6000

=over

=item *

Only tested with C6000 compiler tools v6.1.0 or higher.  It is likely to work
with older compiler revisions, but there may be an occasional inaccuracy,
especially regarding which functions are called indirectly.

=item *

A function call optimization performed for C64+, when compiling for minimal
code size, can limit the ability to construct the call graph.  Details below.

=back

=head2 LIMITATIONS FOR ARM (TMS470)

=over

=item *

Must use compiler version 4.4.3 or later to get accurate results for most
devices.  The script cannot detect which compiler version was used.

=item *

In some cases, an indirect call is incorrectly found in a function which makes
no indirect calls.

=item *

For ARM7, only works for TIABI

=item *

The utility ofd470 is used to create the XML input for this script.  That
utility was not released until v4.1.0.  However, it is possible to combine
the use of a new ofd470 executable with older ARM toolsets.

=back

=head2 LIMITATIONS FOR C5500

=over

=item *

The code must be built with compiler version 3.2.X or higher.

=back

=head1 USAGE

call_graph.pl [options] I<xml file>

   ... OR ...

ofdXX -g -x I<file> | perl call_graph.pl [options]

=head1 OPTIONS

 -h                   : Dump usage and quit
 --entry_point=<name> : Name root function for the call graph
 --i_cfg=<file>       : Configuration file for indirect function calls
 --verbose            : Enable warnings
 --func_info          : Show the file name and size for each function
 --stack_max          : Print the function which uses the most stack
 --cg_xml_version     : Print out the version of the cg_xml package in use

The module Getopt::Long is used to parse the options, thus you can substitute
single letter versions of the options, i.e. -e for --entry_point.  Except
you can never abbreviate --cg_xml_version.

You can use --i_cfg multiple times to supply multiple configuration files.

C6000 specific ... To see a complete call graph starting at the application
entry point, you may need to use -i=c60_rts_indirect.txt.  Details below.

=head1 CREATING XML INPUT 

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

Use the -x and -g options of OFD to create the XML file.  Example ...
       
      ofd6x -xg -o=file.xml file.out

Alternatively, pipe the output from OFD into the script ...

      ofd6x -xg file.out | perl call_graph.pl

On Windows systems, piping the output from OFD tends to be slower; sometimes
much slower.

The -g option is required.  Otherwise, the XML does not contain the Dwarf
debug information this script uses.

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  When processing a .out file, the best options
to use in combination with this script are:

  -xg --xml_indent=0 --obj_display=none,header,optheader,symbols,battrs --dwarf_display=none,dinfo

When processing a library, symbols are not needed, so the best options to
use are:

  -xg --xml_indent=0 --obj_display=none,header,optheader,battrs --dwarf_display=none,dinfo

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

=head1 DESCRIPTION OF THE OUTPUT

The section presumes you are not using the --stack_max option.

=head2 MAIN CALL GRAPH

The first thing you see is the main call graph rooted at the entry point
of the module.  How the entry point is determined is described later.
Here is an example ...

 Call Graph for main.obj
 **********************************************************************
 _main : wcs = 72
 |  _level1 : wcs = 64
 |  |  _level2 : wcs = 48

After each function name the worst case stack (wcs) depth is shown.  That
is the maximum amount stack memory used by that function and all the functions
it calls.  The values are in 8-bit bytes for C6000 and ARM, 16-bit words for
C5500.  As you can see, the child function(s) called by the parent are
indented.  

If you use the option --func_info, you additionally see the file name that
supplies the function, and the size of the function.  

 Call Graph for main.obj
 **********************************************************************
 _main : wcs = 72 : fn = main.c : sz = 32
 |  _level1 : wcs = 64 : fn = level.c : sz = 64
 |  |  _level2 : wcs = 48 : fn = level.c : sz = 64

Note the file name shown is not 100% reliable.  In instances of function
inlining, building with -pm, or linear assembly, you may see ??? instead
of a file name, or an incorrect file name.

If the input file is a library, then there is no entry point and you do not
see a main call graph.

=head2 REPEAT FUNCTION DISPLAY

For code like this:

 top_level()
 {
    first();
    second();
 }

 first()
 {
    big_tree();   // calls many functions
 }

 second()
 {
    big_tree();   // calls those same functions
 }

The output will look like this:

 top_level
 |  first
 |  |  big_tree
 |  |  |  many
 |  |  |  |  function
 |  |  |  |  calls
 |  |  |  appear
 |  |  |  |  here
 |  second
 |  |  big_tree
 |  |  |  <repeat ...>

All the functions that appear under "big_tree" are always the same, and always
in the same order.  Rather than print it out all again under "second", 
<repeat ...> appears instead.  Printing it out again takes lots of space. 
For very large apps, this technique saves lots of execution time and output
file space.

=head2 ORPHANED CALL GRAPHS

Followed by the main call graph are the orphaned call graphs.  Functions which
are never called directly are termed orphans because their parent cannot be
found.  There are two possible explanations for orphans.  One, the function
really is never called.  Two, the function is called, but indirectly, and the
parent that makes that indirect call is not noted in a configuration file
described below.  The full call graph of each orphan, including all the
functions it calls, is shown.

Note interrupt functions are legitimate orphans.

=head2 FUNCTIONS THAT MAKE INDIRECT CALLS

Last, is a list of functions known to make indirect function calls, but
which have no information in the configuration file described below. 

To distinguish orphans and functions that make indirect calls, here is
an example ...

 - Consider two functions _top_level and _next_level
 - _top_level is called directly from _main
 - _top_level makes an indirect call to _next_level
 - There are no other calls to _next_level
 - No configuration file about indirect calls is used

In that case, _top_level is listed as making an indirect call, and 
_next_level is an orphan.

=head2 WHEN GRAPHING A LIBRARY

When the input file is a library, all of the code in the library is examined
and combined into a single call graph with many entry points.  The entry
points to the graph are all the functions which are not called directly.
The file boundaries in the library are not considered.  Suppose file1.c 
has a function named _level1, which calls a function in file2.c named _level2.
Suppose further there are no other calls to _level1 or _level2.  Then,
_level1 is an entry point into the graph, and the corresponding graph will
show the call to _level2.  Neither function will appear elsewhere in the
output.

Note it is possible that an entry point into the graph is called indirectly.
That will occur when that indirect call is not listed in the configuration
file provided with the i_cfg=file option.

=head2 FUNCTION SYMBOL ALIASING

One optimization performed by the compiler is called Function Symbol Aliasing.
For this code ...

 int bbb(int arg1, char *arg2);

 int aaa(int n, char *str)
 {
    return bbb(n, str);
 }

The compiler makes aaa an alias of bbb.  The symbols aaa and bbb have the
same address.  

This is represented in the call graph with ...

 |  |  main : wcs = 1356
 |  |  |  ( aaa
            bbb ) : wcs = 1356
 |  |  |  |  printf : wcs = 1348

And it appears in the orphan function listing ...

 one_orphan
 ( aaa bbb )
 another_orphan

If you use the --verbose option, a table of Function Symbol Aliases is output
to stderr.

 Function Symbol Aliases
 -----------------------
 ( aaa bbb )
 ( level1 level2 )

Function symbol aliases are represented internally with a name like "aaa:bbb".
Thus, you may see names of that form in diagnostics. 

=head3 FUNCTION SYMBOL ALIASING AND CONFIGURATION FILES

Consider the example above with the functions aaa and bbb.  Suppose your
call_graph configuration file contains ...

   aaa : bbb

This indicates that the function aaa calls bbb.  Now that call_graph combines
aaa and bbb together in the graph, this is the same as writing ...

   (aaa bbb) : (aaa bbb)

The configuration entry, in effect, says that this function calls itself.  
In nearly all such cases, that is not true.  

To find this situation, run call_graph with --verbose (or just -v) and 
look for ( aaa bbb ) in the Function Symbol Aliases table.  That entry means
aaa and bbb cannot be written to call each other in the configuration file.

=head1 DESCRIPTION OF --stack_max OUTPUT

Under the option --stack_max the output is only one line like this ...

 _name_of_function : wcs = 1172

The function name is the root of the call graph, whether the main call 
graph or an orphan call graph, that uses the most stack.

This option is intended to support automatically finding maximum stack
usage.  Care must be taken, however.  There is no guarantee that the number
output is, in all cases, the maximum amount of stack used.  These two 
factors must be considered: the effect of possible indirect calls,  and
possible nesting of interrupts.

=head2 EFFECT OF INDIRECT CALLS

It is possible that some function in the maximum stack call graph makes
an indirect call to some other function, thus increasing stack usage
even more than what is reported.  To know this is not occuring, you have
to run without --stack_max, and insure that the last section of output
is empty.  The last section lists the functions known to make indirect calls
but have no configuration file information about those calls.  Note, further,
the information in the configuration file(s) about those indirect calls
must be complete and correct.

=head2 NESTING OF INTERRUPTS

If building the call graph for a library, this section can be ignored.  When
building the call graph for a complete application, you have to consider the
possibility of nesting interrupts that use the same stack.  In most
applications, interrupts that could nest use separate stacks.  But it is 
possible for nested interrupts to use the same stack.  The case not addressed
in the --stack_max output is the possiblity that while the functions in the
maximum stack graph are executing, an interrupt is taken, that interrupt uses
the same stack, and thus causes the stack to grow even more.

=head2 CANNOT COMBINE --stack_max AND --func_info

When the options combination --stack_max and --func_info is attempted, a
warning message is issued, and --func_info is ignored.

=head1 ARM SUPPORT DETAILS

This table summarizes ARM support ...

                                      All
 ABI         |   ARM7   ARM9   ARM11  Cortex
 ------------+------------------------------
 TIABI       |   Y      NA     NA     NA
 TI_ARM9_ABI |   N      4.4.3  4.4.3  4.4.3
 EABI        |   N      4.4.3  4.4.3  4.4.3


First, a general issue which affects all ARM devices is described.  The table
entries are described next.

=head2 FALSE INDIRECT CALLS

Due to a bug in the compiler, this script may incorrectly display an indirect
call in a function which makes no indirect function calls.  The ClearQuest
entry is SDSCM00020700.  If you know you have no indirect calls in your 
code, then you can presume any indirect calls found are these false ones.

=head2 Y

Generally supported, even when builds are done with older toolsets.

=head2 N

Not supported.  When the combination is detected, a diagnostic is issued.
The script tries to execute anyway.  But the results could be incorrect, 
and there is no easy way detect any errors.

Why?  The compiler has a bug where an indirect call sequence is not encoded
as a call in the Dwarf debug information.  This means an indirect call is
ignored by this script.  The ClearQuest entry is SDSCM00020565.

If you know you have no indirect calls in your code, then you can ignore
the warning.

=head2 NA

Not applicable.  You cannot build code for TIABI while specifying any
target other than ARM7.  Note, however, you can execute ARM7/TIABI code
on other ARM devices.

=head2 4.4.3

Supported only if all the code is built with tools version 4.4.3 or later.
The script cannot detect the version of the tools used to do the build.

=head1 C64+ FUNCTION CALL OPTIMIZATION LIMITS OUTPUT

A code size optimization, performed when building for C64P with the build
options -ms2 or -ms3, changes how some functions are called.  The net effect
is that a direct function call at source level is changed to an indirect
function call in assembly.  The Dwarf information used to build the call
graph shows such a call as an indirect function call.   Such a call will
appear in the call graph similar to this ...

 _user_func1 : wcs = 0
 |  __local_call_stub : wcs = 0
 |  |  __call_stub : wcs = 0

In this case, _user_func1 does a direct call to _user_func2.  The
optimization, however, changes it to an indirect function call performed by
the two functions _local_call_stub and __call_stub.  In many cases,
_user_func2 will show up later in the call graph output as an orphan.

=head1 CONFIGURATION FILE FOR INDIRECT FUNCTION CALLS

Supply information about what functions are called through indirection with
configuration files.  The option -i_cfg=file gives the name of the
configuration file.  You can use the option multiple times to supply, for
instance, one configuration file for each library you use.

=head2 SYNTAX OF THE CONFIGURATION FILE

General syntax ...

   parent : child1 child2 child3 ...

Where the "parent" function may make one or more indirect function calls to
the listed "child" functions.  It must all appear on one line.  Multiple
lines that start with the same parent are OK.  Comments begin with '#'.
Blank lines are ignored.  All other space is normalized.

The function names must be written as they appear in *assembly*, not C.  For
all targets except MSP430, when building with the default COFF ABI, this means
the function name must begin with an '_'.  For MSP430 or when building with
EABI, the C function name and the assembly name is the same, which means
a leading '_' is usually not present.  At this writing, only the ARM and
C6000 compilers provide support for EABI.  Other TI compilers will add EABI
support over time.

=head2 CONFIGURATION FILES FOR COMPILER RTS FUNCTIONS

The compiler runtime support library code makes indirect function calls.
The configuration files ti_rts_indirect.txt, c60_rts_indirect.txt,
arm_rts_indirect.txt, and c55_rts_indirect.txt supply information for these
indirect function calls.  The file ti_rts_indirect.txt supplies information
that is common across those different RTS libs.  For full correctness, it
must be used in combination with a target specific file.  For example ...

   call_graph.pl -i=ti_rts_indirect.txt -i=c60_rts_indirect.txt c60_app.xml

These files also serve as examples of how you can write your own such
configuration files.

=head2 DIAGNOSTICS

These diagnostics are emitted only under the option --verbose.

If you name a parent function for which there is no information on the
main source of the function (i.e. declarations don't count), then you
get a warning message and the parent function, and all the children listed,
are ignored.

If you name a child function for which there is similarly no information,
you get a diagnostic.  But, since this could legitimately occur in cases
where you are processing only part of an application, or the routine is
written in assembly, the child function is still entered in the call graph.  

=head1 ASSEMBLY FUNCTIONS

The call graph is developed entirely from Dwarf debug information.  The
assembler will encode proper Dwarf debug information for an assembly
function if you follow these steps.

 - Use the .asmfunc and .endasmfunc directives
 - Indicate stack usage with the stack_usage(<num>) argument
   to .asmfunc.  Specify the number of bytes for C6000 and ARM.  Specify
   the number of words for C5500.
 - For C6000, use the CALL and RET mnemonics instead of simple B

Here is a C6000 assembly example ...

 in_asm:     .asmfunc    stack_usage(16)     ; uses 16 bytes of stack
             ...
 loop:
             ...
	     b     loop    ; normal branches need no special handling
	     ...
	     call  func    ; instead of b   func
	     ...
	     ret   b3      ; instead of b   b3
	     .endasmfunc

=head1 RECURSION

Recursion is detected.  It is stopped in the graph with the first function
that begins the cycle of recursion.  Under --verbose, you get a warning
message about how the script cannot compute worst case stack depth in this
case.

=head1 ENTRY POINT DETAILS

This section does not apply to libraries, but only to full application builds.

The script attempts to automatically find the entry point of the application,
and root the call graph there.  This doesn't always work.

In COFF files, the entry point is given in the optional file header.  The
optional file header is not always present.  Simple object (.obj) files
created by the assembler do not have an optional file header.

In C6000, there is an additional issue.  In compiler version v7.2.0, there is
a bug in the Dwarf information created by the compiler.  The chain of calls
from the entry point to main is: _c_int00 -> args_main -> main.  Because of
the bug, the Dwarf information about the call from args_main to main is not
present.  The bug is SDSCM00039952 in the ClearQuest system.  There are
entries in the configuration file c60_rts_indirect.txt to make up for this
bug.  Thus the option ...

  -i=c60_rts_indirect.txt

is a workaround for the problem.

=head1 PERL VERSION DETAILS

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

#=============================================================================
# TBDs ...
# - Handle static functions
# - Output source function name (attribute DW_AT_TI_symbol_name) instead
#=============================================================================

#=============================================================================
# Standard Perl pragmas that provide checks against common errors
#=============================================================================
use strict;
use warnings;

#=============================================================================
# Module for handling command line options
#=============================================================================
use Getopt::Long;

#=============================================================================
# Tell Perl to find XML_TI_OFD.pm in the same directory as this script
#=============================================================================
use FindBin;
use lib $FindBin::Bin;

#=============================================================================
# Bring in the module that handles parsing the OFD XML file
#=================1===========================================================
use XML_TI_OFD 3.11;

#=============================================================================
# Global variables
#=============================================================================
my(
   # From the command line ---------------------------------------------------
   $xml_file,              # name of XML file or '-' for stdin
   @i_cfg_files,           # config files for indirect function calls
   $verbose,               # whether to emit warnings
   $want_func_info,        # whether to emit file name and size for each func
   $stack_max,             # Find function that uses the most stack

   # General -----------------------------------------------------------------
   $targ_name,             # name of the CPU target
   %tag_or_attr,           # all the Dwarf tags or attributes in the XML file
   $is_lib,                # processing a library?
   $is_elf,                # processing an ELF file?

   # Related to the call graph -----------------------------------------------
   %call_graph,            # call graph - keyed by function name
   %call_seen,             # functions that are known to be called
   %is_veneer,             # functions that are veneers
   $entry_addr,            # address of the entry point
   $entry_name,            # name of the entry point

   # Function symbol aliases -------------------------------------------------
   %function_aliases,      # multiple names for the same function

   # Used when dumping the call graph ----------------------------------------
   %call_in_path,          # call seen in current path from root
   %no_stk_info,           # funcs for which there is no stack info
   @graph_in_order,        # func names in order of call graph traversal
  );

#=============================================================================
# Description of the call graph data structure ...
#
# The call graph is a hash indexed by function name.  The full source to
# a function must be found before it is entered in the call graph.  Thus,
# a function declaration, or a call to a function, does not enter it in the
# call graph.  The graph is always indexed with what is termed a "parent".
# Much of the time, that parents call some functions termed "kids".  Some
# functions make no calls and thus have no kids.  It is possible for a kid
# to not have an entry in the call graph.  Such is the case when only a part
# of an application is being profiled, or a function is defined in assembly
# without using the .asmfunc/.endasmfunc directives.
#
# All the fields an entry in the call graph may have are:
# 
# - kids            : Maintained as a hash, so multiple calls to the same func
#                     are automatically collapsed to a single entry
# - frame_size      : How much stack func uses
# - has_indirect    : Whether func makes any indirect function calls
# - has_config_info : Whether func has any info in config file
# - wcs             : Worst case stack usage.  Maximum amount of the stack
#                     needed for this function and all its descendants.
#                
# Note the call graph has no natural "root".  You can start a traversal
# with any function name.  The only traversal that could possibly visit
# every function must start at the application entry point.  But that
# presumes the graph is fully connected, which is rarely the case.
#
#=============================================================================
# Note on %call_seen hash ...
#
# The call_seen property cannot be maintained in the call graph.  Why? 
# Because a function ($parent) is entered in the call graph only when the
# definition (i.e. the actual code) for the function is found.  It is possible
# to call a function ($kid) for which the definition is not in the file being
# processed.
#=============================================================================

#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   my ($xml_data, @file_list, $first_file, $file_data);

   if ($^V lt v5.8.3)
   {
      print  "WARNING: Developed under Perl version 5.8.3.\n";
      printf "         May not work under version %vd\n\n", $^V;
   }
   
   process_cmd_line();

   #--------------------------------------------------------------------------
   # Filter XML even further, and convert to Perl data structure
   #--------------------------------------------------------------------------
   $xml_data = 
      ofd_filter_and_parse_xml(
	 $xml_file,
         FILTER => { SECTION => [qw(.debug_info)],
		     DIE_TAG => [qw(DW_TAG_TI_branch
		                    DW_TAG_compile_unit
		                    DW_TAG_TI_reserved_3
		                    DW_TAG_subprogram)] },
         STRIP => { GENERAL => [qw(string_table
		                   raw_data
				   line_numbers)] },
	 XML_PEEK => \&xml_peek);

   #--------------------------------------------------------------------------
   # Is the input from a library?
   #--------------------------------------------------------------------------
   if (ofd_find_lib_name($xml_data))
      { $is_lib = 1; }

   #--------------------------------------------------------------------------
   # Get list of files (potentially just one).  
   #--------------------------------------------------------------------------
   @file_list = ofd_each_file($xml_data);
   $first_file = $file_list[0];

   $is_elf = exists $first_file->{elf};

   #--------------------------------------------------------------------------
   # Determine the CPU architecture
   #--------------------------------------------------------------------------
   check_target($xml_data);

   #--------------------------------------------------------------------------
   # For complete .out files, and not libraries, find the entry point, and
   # build the function symbol aliases table.  See the comment at the top
   # of build_function_aliases for the details.
   #--------------------------------------------------------------------------
   if (!$is_lib)
   {
      $entry_addr = get_entry_addr($first_file);
      build_function_aliases($first_file);
   }

   #--------------------------------------------------------------------------
   # Build up one call graph regardless of how many input files
   #--------------------------------------------------------------------------
   foreach $file_data (@file_list)
      { build_call_graph($file_data); }

   #--------------------------------------------------------------------------
   # Odd little thing just for ARM ...
   #--------------------------------------------------------------------------
   remove_arm_veneers();

   #--------------------------------------------------------------------------
   # Add info on indirect calls from --i_cfg files
   #--------------------------------------------------------------------------
   add_indirect_calls();              

   #--------------------------------------------------------------------------
   # Dump out function symbol aliases
   #--------------------------------------------------------------------------
   dump_function_aliases();

   if (!$stack_max)
   {
      if (%call_graph)
      {
         #--------------------------------------------------------------------
         # Dump the call graph starting with the entry point symbol
         #--------------------------------------------------------------------
         if (!$is_lib) { dump_main_call_graph($first_file); }

         #--------------------------------------------------------------------
         # Dump orphan call graphs, and names of funcs that make indirect calls
         #--------------------------------------------------------------------
         audit_call_graph();
      }
      else
      {
         print "No graph to output\n";
      }
   }
   else
   {
      #-----------------------------------------------------------------------
      # Print out only the one function that uses the most stack
      #-----------------------------------------------------------------------
      find_stack_max();
   }

   exit(0);
}

#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($print_usage, $pn);

   #--------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out on stderr
   #--------------------------------------------------------------------------
   ofd_cg_xml_version();

   $print_usage = $entry_name = $verbose = $want_func_info =
   $stack_max = undef;
   @i_cfg_files = ();

   GetOptions('h'               => \$print_usage,
              '--entry_point=s' => \$entry_name,
	      '--i_cfg=s'       => \@i_cfg_files,
	      '--verbose'       => \$verbose,
	      '--func_info'     => \$want_func_info,
	      '--stack_max'     => \$stack_max,
	     );

   if ($print_usage)
   {
      #-----------------------------------------------------------------------
      # Strip off path from program name
      #-----------------------------------------------------------------------
      $0 =~ m/([\w\.]+)$/;
      $pn = $1;

      warn "Usage: $pn [options] <xml file>   ... OR ...\n";
      die  "       ofdXX -x -g <input file> | perl $pn [options]\n";
   }

   if ($stack_max && $want_func_info)
   {
      warn "Cannot use --func_info in combination with --stack_max.\n" .
           "--func_info disabled.\n";
      $want_func_info = undef;
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
# XML_PEEK - Called back from ofd_filter_and_parse_xml
#=============================================================================
sub xml_peek
{
   my ($ref_all_lines) = @_;
   my ($name);

   #--------------------------------------------------------------------------
   # Save off the exact names of the Dwarf tags and attributes that are
   # in the XML file.  Used in build_call_graph.
   #--------------------------------------------------------------------------
   foreach $name ($$ref_all_lines =~ m/\b(DW_(?:AT|TAG)_\w+)/g)
      { $tag_or_attr{$name} = 1; }
}

#=============================================================================
# CHECK_TARGET - Determine the target the app runs on.  Issue warning if
#    not a supported target.
#=============================================================================
sub check_target
{
   my ($xml_data) = @_;

   $targ_name = ofd_machine_name($xml_data);

   if ($targ_name eq 'ARM')
      { check_arm_target($xml_data); return; }
   return if $targ_name eq 'C6000';
   return if $targ_name eq 'C5500';
   return if $targ_name eq 'C5500+';
   return if $targ_name eq 'C2000';
   return if $targ_name eq 'MSP430';

   warn << "END_WARN";
$0 Only tested for C6000, C5500, ARM, C2000, and MSP430 not: $targ_name
END_WARN

   #--------------------------------------------------------------------------
   # Intentionally keeping a low profile on support for C5500+.  This script
   # goes to customers who do not have an NDA that covers that device.  That
   # said, this script says very little about it.  It is hardly a big secret
   # that some next-gen C5500 thing might be lurking about.
   #--------------------------------------------------------------------------
}

#=============================================================================
# CHECK_ARM_TARGET
#=============================================================================
sub check_arm_target
{
   my ($xml_data) = @_;
   my (@file_list, $file_data, $cpu, $abi);

   @file_list = ofd_each_file($xml_data);
   $file_data = $file_list[0];

   if (!$is_elf)
   {
      my ($cpu_flags, $cpu_bits);
      $cpu_flags = str_to_num($file_data->{ti_coff}->{file_header}->{cpu_flags});

      #-----------------------------------------------------------------------
      # Very old compilers do not define the CPU header flags, and thus can
      # only support ARM7, TIABI
      #-----------------------------------------------------------------------
      if ($cpu_flags == 0)
      {
	 $abi = 'TIABI';
	 $cpu = 'ARM7';
      }
      #-----------------------------------------------------------------------
      # CPU flags are documented in the linker source file coff_trg.h
      #-----------------------------------------------------------------------
      else
      {
	 $abi = ($cpu_flags & 0x10) ? 'TI_ARM9_ABI' : 'TIABI';

	 $cpu_bits = $cpu_flags & 0x60;
	 if    ($cpu_bits == 0x20) { $cpu = 'ARM7';    }
	 elsif ($cpu_bits == 0x40) { $cpu = 'ARM9';    }
	 elsif ($cpu_bits == 0x60) { $cpu = 'ARM11';   }
	 else                      { $cpu = 'unknown'; }
      }
   }
   else
   {
      my ($sect, $attribute_sect, $vendor, $ti_attributes, $attribute,
          $cpu_tag, %tag_to_name);
      
      foreach $sect (ofd_each_section($file_data))
      {
         next if (!$sect->{build_attributes});
	 $attribute_sect = $sect;
	 last;
      }

      if (!$attribute_sect)
      {
         warn "No build attributes to check";
	 return;
      }

      foreach $vendor (@{$attribute_sect->{build_attributes}->{vendor_section}})
      {
         next if ($vendor->{vendor_name} ne 'TI');
	 $ti_attributes = $vendor;
	 last;
      }

      $cpu_tag = 0;
      foreach $attribute
         (@{$ti_attributes->{attribute_section}->{attribute_list}})
      {
         next if ($attribute->{tag} ne 'Tag_ISA');
	 $cpu_tag = str_to_num($attribute->{arguments}->{const});
	 last;
      }

      #-----------------------------------------------------------------------
      # This table comes from the spec "Addenda to, and Errata in, the ABI
      # for the ARM Architecture".  The section is titled "Target-related
      # Attributes".  That describes the architecture choices for the
      # vendor "aeabi" and tag "Tag_CPU_arch".  This code uses build
      # attributes for vendor "TI" and tag "Tag_ISA".  The vendor "TI"
      # is preferred because attributes are present even in (newer) COFF
      # files, not just ELF.  The encoding of Tag_ISA is the same as
      # Tag_CPU_arch.
      #-----------------------------------------------------------------------
      %tag_to_name = (
         2  => 'ARM7',
	 4  => 'ARM9',
	 6  => 'ARM11',
	 10 => 'Cortex',
      );

      $cpu = ($tag_to_name{$cpu_tag}) ? $tag_to_name{$cpu_tag} : 'unknown';
      $abi = 'EABI';
   }

   #--------------------------------------------------------------------------
   # See POD section ARM SUPPORT DETAILS about this warning
   #--------------------------------------------------------------------------
   if ($cpu eq 'ARM7' && ($abi eq 'TI_ARM9_ABI' || $abi eq 'EABI'))
      { warn "WARNING: $0 unlikely to work for ARM7 and $abi"; }
}

#=============================================================================
# GET_ENTRY_ADDR - Get the entry point address out of the optional file header
#=============================================================================
sub get_entry_addr
{
   my ($file_data) = @_;
   my ($entry_point);

   #--------------------------------------------------------------------------
   # Quit if entry point supplied on the command line
   #--------------------------------------------------------------------------
   if ($entry_name)
   {
      $call_seen{$entry_name} = 1;
      return undef;
   }

   $entry_point = ofd_entry_point($file_data);
   if (!(defined $entry_point))
      { die "Cannot find entry point.  Stopped"; }

   return $entry_point;
}

#=============================================================================
# BUILD_FUNCTION_ALIASES
#
# For background on "function symbol aliasing", see the section of that name
# above in the POD.  The %function_aliases hash is keyed by the symbol
# name.  One entry maps "aaa" to "aaa:bbb", and a separate entry maps "bbb"
# to "aaa:bbb".  The values are records that contain these fields:
# 
# - alias_name - The full name of the function alias, i.e. "aaa:bbb"
# - used - Whether that particular symbol name is ever used as a function.
#   This flag is set whenever the function name is seen in the Dwarf as 
#   either a fully defined function, or is called by that name.  It is also
#   set if the name is used in a configuration file.  This prevents confusing
#   the output with alias names that never get called.  The poster child
#   example is the four names for memcpy.  This field means the output will
#   almost always just show memcpy, and none of its never used alias names.
#
# This function is not called if the input is a library.  The symbols in an
# object library are not at their final address.  Many symbols are defined
# with the value 0.  Symbol aliases don't make sense in that case.
#=============================================================================
sub build_function_aliases
{
   my ($file_data) = @_;
   my (%temp_rev, %rev_symtab);

   foreach my $sym (ofd_each_symbol($file_data))
   {
      #-----------------------------------------------------------------------
      # Only keep global symbols
      #-----------------------------------------------------------------------
      next unless (ofd_sym_is_global($sym));

      #-----------------------------------------------------------------------
      # Build a reverse symbol table.  %temp_rev maps any address back to a 
      # symbol.  %rev_symbtab maps only those addresses for which 2+ symbols
      # are found.  Note %temp_rev values are symbols, while %rev_symtab
      # values are symbol *names*.
      #-----------------------------------------------------------------------
      my $addr = ofd_sym_value($sym);
      if (!$temp_rev{$addr})             # first sym found at this address
	 { $temp_rev{$addr} = $sym; }
      else                               # 2+ syms found at this address
      {
	 if (!$rev_symtab{$addr})
	    { push @{$rev_symtab{$addr}}, ofd_sym_name($temp_rev{$addr}); }

	 push @{$rev_symtab{$addr}}, ofd_sym_name($sym);
      }
   }

   #--------------------------------------------------------------------------
   # %temp_rev can get huge.  It is no longer needed.  Save memory by
   # emptying it out.
   #--------------------------------------------------------------------------
   %temp_rev = ();

   #--------------------------------------------------------------------------
   # Create the function_alias entry for every address that has 2+ symbols.
   # Only the alias_name field is set here.  The used field is set whenever a
   # use appears.  The sort is important in creating stable output that can
   # be reliably compared.  The function walk_call_graph outputs child
   # functions in sorted order.  The alias name created here is one of the
   # entries in that sort.
   #--------------------------------------------------------------------------
   foreach my $addr (keys %rev_symtab)
   {
      my $alias_name = join(':', sort @{$rev_symtab{$addr}});
      foreach my $sym_name (@{$rev_symtab{$addr}})
      {
         $function_aliases{$sym_name} = { alias_name => $alias_name };
      }
   }
}

#=============================================================================
# BUILD_CALL_GRAPH
#=============================================================================
sub build_call_graph
{
   my ($file_data) = @_;

   my ($debug_info, $file, $cu_die, $die, @func_dies, $func_die);
   my ($parent, $start_addr, $is_tramp, $is_veneer, $frame_size);
   my (@branch_dies, $branch_die, $is_call, $is_indirect, $kid);
   my ($tramp_attr, $call_attr, $indirect_attr, $branch_tag, %func_attrs);
   my ($err_msg, @debug_info_sections, $file_name, $end_addr, $func_size);

   #--------------------------------------------------------------------------
   # Insure Dwarf debug info is in the XML, and the .debug_info scn is there
   #--------------------------------------------------------------------------
   $err_msg = undef;
   if (!ofd_has_dwarf($file_data))
      { $err_msg = 'No DWARF info available.  Insure you use OFD -g flag.'; }
   if (!(@debug_info_sections = ofd_each_debug_info($file_data)))
      { $err_msg = 'No .debug_info sections found.'; }

   #--------------------------------------------------------------------------
   # Emit possible error message, and quit as needed
   #--------------------------------------------------------------------------
   if ($err_msg)
   {
      if (!$is_lib)
         { die "$err_msg Stopped"; }
      else
      {
	 warn "Skipping file: $file_data->{name}.  $err_msg\n"; 
	 return;
      }
   }

   #--------------------------------------------------------------------------
   # The functions ofd_find_all_dies and ofd_find_attrs end up taking lots
   # of execution time.  Minimize the arguments passed to those funcs by
   # peeking into the XML to see which of mutually exclusive possiblities
   # are actually there.  Note it is possible there are no trampolines.
   #--------------------------------------------------------------------------
   $tramp_attr = which_tag_or_attr(qw(DW_AT_TI_trampoline DW_AT_trampoline));
   $branch_tag = which_tag_or_attr(qw(DW_TAG_TI_branch DW_TAG_TI_reserved_3));
   $call_attr  = which_tag_or_attr(qw(DW_AT_TI_call DW_AT_TI_reserved_9));
   $indirect_attr =
                 which_tag_or_attr(qw(DW_AT_TI_indirect DW_AT_TI_reserved_10)); 


   #--------------------------------------------------------------------------
   # Compute the attributes searched for function dies.
   #--------------------------------------------------------------------------
   # Using DW_AT_TI_symbol_name (the compiler internal name) for the
   # function and not DW_AT_name (the user name) because that is how
   # the calls to this function appear elsewhere.  Printing out the name
   # with user name instead means saving off the DW_AT_name ignored here.
   # Getting the start address for two reasons:  1) Finding it means this
   # is a real function, and not just a declaration, and 2) It is compared
   # against the entry point to know if this function is the entry point.
   #--------------------------------------------------------------------------
   %func_attrs = (
      DW_AT_TI_symbol_name    => \$parent,
      DW_AT_low_pc            => \$start_addr,
      DW_AT_frame_base        => \$frame_size,
      DW_AT_TI_max_frame_size => \$frame_size,
   );

   if ($tramp_attr)     { $func_attrs{$tramp_attr}     = \$is_tramp; }
   if ($want_func_info) { $func_attrs{'DW_AT_high_pc'} = \$end_addr; }

   if (exists $tag_or_attr{DW_AT_TI_veneer})
      { $func_attrs{DW_AT_TI_veneer} = \$is_veneer; }

   #--------------------------------------------------------------------------
   # If there are no branches or calls (very unlikely), these guys will be
   # undef and cause failures later.  So, set them to something harmless.
   #--------------------------------------------------------------------------
   if (!$branch_tag)    { $branch_tag    = 'DW_TAG_TI_branch';  }
   if (!$call_attr)     { $call_attr     = 'DW_AT_TI_call';     }
   if (!$indirect_attr) { $indirect_attr = 'DW_AT_TI_indirect'; }

   #--------------------------------------------------------------------------
   #--------------------------------------------------------------------------
   foreach $debug_info (@debug_info_sections)
   {
      #-----------------------------------------------------------------------
      # Go through each file (AKA compile unit) in the section
      #-----------------------------------------------------------------------
      foreach $file (@{$debug_info->{compile_unit}})
      {
	 #--------------------------------------------------------------------
	 # Get the DW_TAG_compile_unit DIE for this file
	 #--------------------------------------------------------------------
	 if (!($cu_die = ofd_find_compile_unit_die($file)))
	    { die "Cannot find DW_TAG_compile_unit DIE, stopped"; }

	 #--------------------------------------------------------------------
	 # Possibly save off the file name
	 #--------------------------------------------------------------------
	 if ($want_func_info)
	 {
	    $file_name = undef;
	    ofd_find_attrs($cu_die, DW_AT_name => \$file_name);
	 }

	 #--------------------------------------------------------------------
	 # Find all the function DIE's defined in this file
	 #--------------------------------------------------------------------
	 @func_dies = ();
	 ofd_put_dies_in_array($cu_die, DW_TAG_subprogram => \@func_dies);

	 #--------------------------------------------------------------------
	 # Go through the function DIE's
	 #--------------------------------------------------------------------
	 foreach $func_die (@func_dies)
	 {
	    #-----------------------------------------------------------------
	    # Get the function attributes.  See big comment before the loop
	    # about it.  Worth noting here is that this call may define all
	    # the variables set to undef here.
	    #-----------------------------------------------------------------
	    $parent = $start_addr = $end_addr = $frame_size = $is_tramp =
	    $is_veneer = undef;
	    ofd_find_attrs($func_die, %func_attrs);

	    #-----------------------------------------------------------------
	    # Skip if don't find a name or starting address
	    #-----------------------------------------------------------------
	    next if (!$parent || !$start_addr);
	    
	    #-----------------------------------------------------------------
	    # Check whether this is a function symbol alias
	    #-----------------------------------------------------------------
	    $parent = function_alias_name($parent);

	    #-----------------------------------------------------------------
	    # Ignore trampolines
	    #-----------------------------------------------------------------
	    next if ($is_tramp && $is_tramp eq 'true');

            $frame_size = parse_frame_size($frame_size, $parent);

            #-----------------------------------------------------------------
	    # Note if this is a veneer (only occurs on ARM7).  The is_veneer
	    # property could be maintained in the call graph.  But since so 
	    # few functions are veneers, a separate hash uses less memory.
	    #-----------------------------------------------------------------
	    if ($is_veneer) { $is_veneer{$parent} = 1; }

	    #-----------------------------------------------------------------
	    # Note if this is the entry point
	    #-----------------------------------------------------------------
	    if (   $entry_addr
		&& !$entry_name
		&& str_to_num($start_addr) == $entry_addr)
	    {
	       $entry_name = $parent;
	       $call_seen{$parent} = 1;
	    }

	    #-----------------------------------------------------------------
	    # Place in graph as a leaf function, i.e. do not add any "kids"
	    #-----------------------------------------------------------------
	    # "leaf function" is a compiler term for a function that makes no
	    # function calls.  It is at the end, or the leaves, of the call
	    # graph tree.
	    #-----------------------------------------------------------------
	    $call_graph{$parent}->{frame_size} = $frame_size;

	    #-----------------------------------------------------------------
	    # Assign function info stuff
	    #-----------------------------------------------------------------
	    if ($want_func_info)
	    {
	       $func_size = str_to_num($end_addr) - str_to_num($start_addr);
	       $call_graph{$parent}->{file_name} = $file_name; # possibly undef
	       $call_graph{$parent}->{func_size} = $func_size;
	    }

	    #-----------------------------------------------------------------
	    # Find the branches in this function.  Note $branch_tag is computed
	    # at the top of this function.
	    #-----------------------------------------------------------------
	    @branch_dies = ();
	    ofd_put_dies_in_array($func_die, $branch_tag => \@branch_dies);

	    #-----------------------------------------------------------------
	    # Go through the branches
	    #-----------------------------------------------------------------
	    foreach $branch_die (@branch_dies)
	    {
	       #--------------------------------------------------------------
	       # Get attributes and check if this branch is a call. 
	       #--------------------------------------------------------------
	       # The DW_AT_name attribute here is the compiler internal name
	       # for the function, not the user name.  Note $call_attr and
	       # $indirect_attr are set at the top of this function. 
	       #--------------------------------------------------------------
	       $kid = $is_call = $is_indirect = undef;
	       ofd_find_attrs($branch_die,
			      DW_AT_name => \$kid,
			      $call_attr => \$is_call,
			      $indirect_attr => \$is_indirect,
			     );

	       next if (!($is_call && $is_call eq 'true'));

	       handle_kid($parent, $kid, $is_indirect);
	    }
	 }
      }
   }
}

#=============================================================================
# FUNCTION_ALIAS_NAME - If this name appears in %function_alias, then use
#   its alias name instead.  Note that this name has been used.  See the
#   comment in build_function_aliases for background details.
#=============================================================================
sub function_alias_name
{
   my ($name) = @_;

   if (exists $function_aliases{$name})
   {
      $function_aliases{$name}->{used} = 1;
      return $function_aliases{$name}->{alias_name};
   }
   else
      { return $name; }
}

#=============================================================================
# WHICH_TAG_OR_ATTR - Given a list of Dwarf tags or attributes, says which
#    of those, if any, actually appears in the XML
#=============================================================================
sub which_tag_or_attr
{
   my (@list) = @_;
   
   foreach (@list)
      { return $_ if (exists $tag_or_attr{$_}); }

   return undef;
}

#=============================================================================
# PARSE_FRAME_SIZE - Handle frame size info.  This is intended to be target
#   independent.  Nonetheless, this needs to be checked each time a new
#   target is supported by this script.
#=============================================================================
sub parse_frame_size
{
   my ($frame_size, $parent) = @_;

   if (!(defined $frame_size))   # '0' is valid value
   {
      if ($verbose)
      {
         warn << "NO_FRAME_SIZE";
Frame size for function \"$parent\" cannot be found.  Built with -g?
Assuming frame size of 0.
NO_FRAME_SIZE
      }

      return 0;
   }

   #--------------------------------------------------------------------------
   # The frame size string can have these forms:
   #    "register_name size_num"
   #    "register_name other_stuff size_num"
   #    "size_num"
   # Notice it always ends with a number.  This regex picks off the last bit
   # of non-space as the size.
   #--------------------------------------------------------------------------
   if ($frame_size !~ m/(\S+)$/)
      { die "Unexpected pattern in frame size, stopped"; }

   $frame_size = str_to_num($1);

   #--------------------------------------------------------------------------
   # The C2000 compiler puts out frame size as a negative number
   #--------------------------------------------------------------------------
   if ($frame_size < 0) { $frame_size = -$frame_size; }

   #--------------------------------------------------------------------------
   # Some variants of OFD (C2000 v4.3.0 is one example) print out negative
   # values in hex like '0xfffffffc' for -4.  That does not look like a
   # negative number to Perl.  So this code catches that and negates it the
   # hard way.  This works only for 32-bit hex values.  Note 64-bit hex
   # values causes the 'oct' call in 'str_to_num' to complain.
   #--------------------------------------------------------------------------
   if ($frame_size & 0x80000000)
      { $frame_size = (~$frame_size + 1) & 0xffffffff; }

   return $frame_size;
}

#=============================================================================
# HANDLE_KID - Enter a parent/kid relation into the call graph.  Several
#    target dependent factors are considered.
#=============================================================================
sub handle_kid
{
   my ($parent, $kid, $is_indirect) = @_;

   #--------------------------------------------------------------------------
   # Recent releases of the compiler mark indirect function calls with the
   # attribute DW_AT_TI_indirect.  If that attribute is not detected, check
   # and see if older compiler methods of encoding indirect function calls
   # in the Dwarf are being used.
   #--------------------------------------------------------------------------
   if (!$is_indirect)
   {
      #-----------------------------------------------------------------------
      # C6000: Is indirect if the function name matches a register name
      #-----------------------------------------------------------------------
      if    ($targ_name eq 'C6000')
      {
	 return if (!$kid);

	 if ($kid =~ m/^[A|a|B|b]\d\d?$/)
	    { $is_indirect = 1; }
      }
      #-----------------------------------------------------------------------
      # ARM7 TIABI: Is indirect if function name is IND_CALL or IND$CALL
      # Note for ARM<any> TI_ARM9_ABI, depend upon TI_indirect attribute.
      #-----------------------------------------------------------------------
      elsif ($targ_name eq 'ARM')
      {
	 return if (!$kid);

	 if ($kid =~ m/^IND[_\$]CALL$/)
	 {
	    $call_seen{$kid} = 1;   # so IND_CALL doesn't look like an orphan
	    $is_indirect = 1;
	 }
      }
      #-----------------------------------------------------------------------
      # C55 or C2000: Is indirect if the name of the function is absent
      #-----------------------------------------------------------------------
      elsif (   $targ_name eq 'C5500'
             || $targ_name eq 'C5500+'
             || $targ_name eq 'C2000')
      {
	 if (!$kid)
	    { $is_indirect = 1; }
      }
   }

   #--------------------------------------------------------------------------
   # If indirect, mark parent
   #--------------------------------------------------------------------------
   if ($is_indirect)
      { $call_graph{$parent}->{has_indirect} = 1; }
   #--------------------------------------------------------------------------
   # Otherwise add kid to graph under the parent.  Note that maintaining the
   # list of kids as a hash means multiple calls to the same kid collapse to
   # the same entry in the graph.
   #--------------------------------------------------------------------------
   else
   {
      return if (!$kid);

      #-----------------------------------------------------------------------
      # Check whether this a function symbol alias
      #-----------------------------------------------------------------------
      $kid = function_alias_name($kid);

      $call_graph{$parent}->{kids}->{$kid} = 1;
      $call_seen{$kid} = 1;
   }
}

#=============================================================================
# REMOVE_ARM_VENEERS - Get rid of veneers by changing a call to a veneer to
#    look like a call to the real function the veneer is for
#-----------------------------------------------------------------------------
# Veneers occur only under ARM7 TIABI.  Consult the ARM compiler docs for
# background.  Veneers are worth removing because they clutter up the call
# graph.  A change in state is easy to see, even without a veneer, i.e. a
# function whose name starts with '_' calls a function whose name starts
# with '$', or vice versa.  
#
# A veneer is detected at the function definition site.  That doesn't help
# this problem much.  Removing veneers requires changes at all the *call*
# sites for the veneer.  That can only be done once all the veneers and
# their call sites are known.  That is, the call graph must be completed, and
# the work done in a separate step.  That is what this function does.  It
# visits every *call* site in the call graph, i.e. the kids, and if they are
# a veneer, changes that call site from a call to the veneer, to a call to
# the function the veneer is for.
#-----------------------------------------------------------------------------
# When processing libraries, this function often doesn't do much.  Note that,
# for a veneer to get removed, there has to be some function that makes a
# call through the veneer.  It is common, in a library, for a veneer to have
# no callers.  
#=============================================================================
sub remove_arm_veneers
{
   return if ($targ_name ne 'ARM' || !(scalar keys %is_veneer));

   my ($parent, $veneer, $real_func);

   #--------------------------------------------------------------------------
   # Visit every kid that is veneer ...
   #--------------------------------------------------------------------------
   foreach $parent (keys %call_graph)
   {
      next if (!exists($call_graph{$parent}->{kids}));

      foreach $veneer (keys %{$call_graph{$parent}->{kids}})
      {
	 next if (!$is_veneer{$veneer});

         #--------------------------------------------------------------------
	 # The real function name is the name of the veneer with the
	 # leading '$' or '_' characters swapped.
         #--------------------------------------------------------------------
	 $real_func = $veneer;
	 if (substr($veneer, 0, 1) eq '_')
	    { substr($real_func, 0, 1) = '$'; }
	 else
	    { substr($real_func, 0, 1) = '_'; }

         #--------------------------------------------------------------------
	 # Remove the call to the veneer, and add in the call to the real func
         #--------------------------------------------------------------------
	 # The veneer is intentionally left in the %call_seen hash.  This means
	 # that, even though no calls to the veneer remain, it will not be
	 # identified as an orphan.
         #--------------------------------------------------------------------
	 delete($call_graph{$parent}->{kids}->{$veneer});
	 $call_graph{$parent}->{kids}->{$real_func} = 1;
	 $call_seen{$real_func} = 1;

	 #--------------------------------------------------------------------
	 # A veneer from Thumb to ARM state, where the function is a leaf,
	 # does not use any stack.  All other veneers use 4-bytes of stack.  
	 # Conservatively model it here by always adding 4 bytes of stack
	 # usage to the parent that calls the veneer.
	 #--------------------------------------------------------------------
	 $call_graph{$parent}->{frame_size} += 4;
      }
   }
}

#=============================================================================
# ADD_INDIRECT_CALLS ... from the config files to the call graph
#=============================================================================
sub add_indirect_calls
{
   my ($i_cfg_file, $line, $orig_line, $parent, @kids, $kid);

   foreach $i_cfg_file (@i_cfg_files)
   {
      open(CONFIG, "<$i_cfg_file") or die "Cannot open $i_cfg_file: $!";

      #-----------------------------------------------------------------------
      # Syntax of configuration file:
      #    parent_func : kid1 kid2 kid3 ...
      # You can have multiple lines that start with the same parent function.
      # Comments and spacing are just like in Perl itself.
      #-----------------------------------------------------------------------
      while ($line = <CONFIG>)
      {
	 $orig_line = $line;        # save original line for error reporting

	 $line =~ s/#.*//;          # remove comments
	 $line =~ s/^\s+//;         # remove leading space
	 $line =~ s/\s+$//;         # remove trailing space
	 next if (!length($line));  # skip blank lines

         #--------------------------------------------------------------------
	 # Check for the ':' here just so someone does not mistakenly write ...
	 #    parent : kid1
	 #             kid2
         #--------------------------------------------------------------------
	 # Symbol names are just like in Perl, except '$' is also accepted.
	 # For C6x only, the single quote character (') can be part of a
	 # symbol name.  But that's pretty odd.  So, not accepting it here.
	 # Will adjust if it becomes an issue.
         #--------------------------------------------------------------------
	 if ($line !~ m/^[\w\$]+\s*:/)
	 {
	    cfg_err_msg($i_cfg_file,
	       "Syntax error, ignoring this line ...\n",
	       "\t$orig_line"
	       );

	    next;
	 }

         #--------------------------------------------------------------------
	 # Note not explicitly parsing off the ':'.  The regex reads every
	 # word off the line.
         #--------------------------------------------------------------------
	 ($parent, @kids) = $line =~ m/([\w\$]+)/g;

         #--------------------------------------------------------------------
	 # Check whether this a function symbol alias.  Do this same check
	 # below for the child functions.
         #--------------------------------------------------------------------
	 $parent = function_alias_name($parent);

	 if (!$call_graph{$parent})
	 {
	    cfg_err_msg($i_cfg_file,
	       "Function \"$parent\" not found, ignoring this line ...\n",
	       "\t$orig_line"
	       );

	    next;
	 }

	 $call_graph{$parent}->{has_config_info} = 1;

	 foreach $kid (@kids)
	 {
	    $kid = function_alias_name($kid);

	    if (!$call_graph{$kid})
	    {
	       cfg_err_msg($i_cfg_file,
		  "Body of function \"$kid\" not found.  Spelling error?\n",
		  "OK to call functions not in this module.\n",
		  "Thus, will appear in graph.\n"
		  );
	    }

	    $call_graph{$parent}->{kids}->{$kid} = 1;
	    $call_seen{$kid} = 1;
	 }
      }

      close(CONFIG);
   }
}

#=============================================================================
# CFG_ERR_MSG - Helper to add_indirect_calls, for emitting error messages
#=============================================================================
{
   my (%emitted_header);  # persists across calls like C 'static'

   sub cfg_err_msg
   {
      return if (!$verbose);

      my ($file, @lines) = @_;
      my ($line);

      if (!$emitted_header{$file})
      {
	 warn '-' x 70, "\n";
	 warn "Processing configuration file: $file\n";
	 $emitted_header{$file} = 1;
      }
      
      warn '-' x 70, "\n";
      foreach $line (@lines)
         { warn "$line"; }
   }
}

#=============================================================================
# DUMP_FUNCTION_ALIASES - Under verbose, dump out a table of the function
#   symbol aliases.  See the comment in build_function_aliases for background.
#=============================================================================
sub dump_function_aliases
{
   return unless ($verbose);

   my (%by_alias_name, $alias_seen);

   #--------------------------------------------------------------------------
   # The %function_aliases hash is keyed by individual function names.
   # Create %by_alias_name, which is keyed by the full alias name, and the
   # values are an array of the individual function names.  Skip over any
   # individual function names that are never used.
   #--------------------------------------------------------------------------
   foreach my $sym_name (keys %function_aliases)
   {
      next unless (exists $function_aliases{$sym_name}->{used});
      my $alias_name = $function_aliases{$sym_name}->{alias_name};
      push @{$by_alias_name{$alias_name}}, $sym_name;
      $alias_seen = scalar(@{$by_alias_name{$alias_name}}) > 1 || $alias_seen;
   }

   #--------------------------------------------------------------------------
   # Saw any function symbol aliases?
   #--------------------------------------------------------------------------
   return if (!$alias_seen);

   #--------------------------------------------------------------------------
   # Dump it out to stderr.  Sort so the output is stable and easy to compare.
   # Don't dump anything if only 1 symbol alias is ever used.
   #--------------------------------------------------------------------------
   warn "Function Symbol Aliases\n";
   warn "-----------------------\n";
   foreach my $alias_name (sort keys %by_alias_name)
   {
      next unless (scalar(@{$by_alias_name{$alias_name}}) > 1);
      warn "( ", join(' ', (sort @{$by_alias_name{$alias_name}})), " )\n";
   }

   warn "\n";
}

#=============================================================================
# DUMP_MAIN_CALL_GRAPH
#=============================================================================
sub dump_main_call_graph
{
   my ($file_data) = @_;

   if (!$entry_name)
   {
      if ($verbose) { warn "Entry point function never found\n"; }
      return;
   }

   print "Call Graph for $file_data->{name}\n";
   print '*' x 70, "\n";
   @graph_in_order = ();
   walk_call_graph($entry_name, 0);
   dump_call_graph();
}

#=============================================================================
# WALK_CALL_GRAPH ... both to dump the graph and compute stack usage
#=============================================================================
sub walk_call_graph
{
   #--------------------------------------------------------------------------
   # $parent  : Name of current function
   # $depth   : How many levels deep in the call graph
   #--------------------------------------------------------------------------
   my ($parent, $depth) = @_;

   #--------------------------------------------------------------------------
   # $kid     : Name of function called by current function
   # $max_kid : Maximum stack usage of any single kid
   # $kid_frame : Stack usage needed by one kid
   # $wcs     : Worst case stack usage needed by this func and all descendents
   #--------------------------------------------------------------------------
   my ($kid, $max_kid, $kid_frame, $wcs);

   #--------------------------------------------------------------------------
   # Not in graph at all?
   #--------------------------------------------------------------------------
   if (!exists($call_graph{$parent}))
   {
      if ($verbose && !$no_stk_info{$parent})
      {
	 warn << "NO_STK_INFO";
Not possible to compute worst case stack depth because information for function
\"$parent\" is not available.  Presuming frame size for
\"$parent\" is 0.
NO_STK_INFO
         $no_stk_info{$parent} = 1;
      }

      save_graph_order($parent, $depth);
      return 0;
   }

   #--------------------------------------------------------------------------
   # Leaf function.  WCS is same as the frame size.
   #--------------------------------------------------------------------------
   if (!exists($call_graph{$parent}->{kids}))
   {
      $wcs = $call_graph{$parent}->{frame_size};
      $call_graph{$parent}->{wcs} = $wcs;
      save_graph_order($parent, $depth);
      return $wcs;
   }

   #--------------------------------------------------------------------------
   # If a wcs value exists, this function has been visited.  Save off the
   # order with a flag (the ",1") that indicates, when it is printed out, to
   # emit "<repeat ...>" on the next line.
   #--------------------------------------------------------------------------
   if (exists $call_graph{$parent}->{wcs})
   {
      save_graph_order($parent, $depth, 1);
      return $call_graph{$parent}->{wcs};
   }

   #--------------------------------------------------------------------------
   # Finally, all the odd special cases are done, and a normal visit to this
   # function can begin.
   #--------------------------------------------------------------------------
   save_graph_order($parent, $depth);

   #--------------------------------------------------------------------------
   # Note this function is in the path of the call graph currently being
   # traversed.  Used to prevent circular recursion.
   #--------------------------------------------------------------------------
   $call_in_path{$parent} = 1;

   #--------------------------------------------------------------------------
   # Go through all the functions called by the current function.  Keep track
   # of the max stack usage of any of the kids.  Go through in sorted order
   # so the functions called are listed alphabetically in standard output.
   # Makes test comparison much easier.
   #--------------------------------------------------------------------------
   $max_kid = 0;
   foreach $kid (sort keys %{$call_graph{$parent}->{kids}})
   {
      #-----------------------------------------------------------------------
      # Normal case: Kid is not in the path of the call graph currently being
      # traversed, so recurse to visit.
      #-----------------------------------------------------------------------
      if (!(exists $call_in_path{$kid}))
      {
         $kid_frame = walk_call_graph($kid, $depth+1);
      }
      #-----------------------------------------------------------------------
      # Otherwise, kid function calls the parent, directly or by calling other
      # functions that call the parent, and recursing would cause an endless
      # loop.  Just save it off at depth+1.
      #-----------------------------------------------------------------------
      else
      {
	 if ($verbose)
	 {
	    warn << "RECURSION";
Not possible to compute worst case stack depth because of recursion.  Function
\"$parent\" calls \"$kid\", which (eventually) calls \"$parent\".
RECURSION
	 }

         save_graph_order($kid, $depth+1);

         #--------------------------------------------------------------------
	 # Do not attempt to compute accurate stack usage under recursion.
	 # Just return frame size for this function, if it is there.
         #--------------------------------------------------------------------
	 if (   exists $call_graph{$kid}
	     && exists $call_graph{$kid}->{frame_size})
	 {
	    $kid_frame = $call_graph{$kid}->{frame_size};
	 } 
	 else
	 {
	    $kid_frame = 0;
	 }
      }

      #-----------------------------------------------------------------------
      # Keep track of maximum frame seen among the kids
      #-----------------------------------------------------------------------
      if ($kid_frame > $max_kid)
         { $max_kid = $kid_frame; }
   }

   #--------------------------------------------------------------------------
   # Function no longer in the path of the call graph currently being traversed
   #--------------------------------------------------------------------------
   delete($call_in_path{$parent});

   #--------------------------------------------------------------------------
   # Worst case stack usage is func frame size plus max stack needed by
   # any one kid.
   #--------------------------------------------------------------------------
   $wcs = $call_graph{$parent}->{frame_size} + $max_kid;
   $call_graph{$parent}->{wcs} = $wcs;
   return $wcs;
}

#=============================================================================
# SAVE_GRAPH_ORDER - Could print out func name at this point.  But stack
# usage is not computed yet.  Have to complete traversal for that.  Rather
# than walking the graph twice (very expensive), save the func name and depth
# off in an ordered array, and print it all later.
#=============================================================================
sub save_graph_order
{
   #--------------------------------------------------------------------------
   # $func_name : Name of the function
   # $depth     : Depth it appears in the call graph
   # $is_repeat : Whether this is a repeat appearance in the graph
   #--------------------------------------------------------------------------
   my ($func_name, $depth, $is_repeat) = @_;
   my $rec;

   $rec->{func}      = $func_name;
   $rec->{depth}     = $depth;
   $rec->{is_repeat} = 1 if ($is_repeat);

   push @graph_in_order, $rec;
}

#=============================================================================
# DUMP_CALL_GRAPH - Dump call graph using array built in save_graph_order
#=============================================================================
sub dump_call_graph
{
   my ($marker, $rec, $graph_entry);

   #--------------------------------------------------------------------------
   # Printed once for each level of depth in call graph
   #--------------------------------------------------------------------------
   $marker = '|  ';

   foreach $rec (@graph_in_order)
   {
      print $marker x $rec->{depth};
      my $padding = "\n  " . ' ' x (length($marker) * $rec->{depth});
      dump_func_name($rec->{func}, $padding);

      if ($graph_entry = $call_graph{$rec->{func}})
      {
	 if (defined $graph_entry->{wcs})
	    { print " : wcs = $graph_entry->{wcs}"; }
	 else
	    { print " : wcs = ???"; }

	 if ($want_func_info)
	 {
	    if ($graph_entry->{file_name})
	       { print " : fn = $graph_entry->{file_name}"; }
	    else
	       { print " : fn = ???"; }

	    if (defined $graph_entry->{func_size})
	       { print " : sz = $graph_entry->{func_size}"; }
	    else
	       { print " : sz = ???"; }
	 }
      }
      else
      {
         print " : wcs = ???";
	 if ($want_func_info)
	    { print " : fn = ??? : sz = ???"; }
      }

      print "\n";

      #-----------------------------------------------------------------------
      # If this is a repeat appearance in the graph, use this very abbreviated
      # form of representing all the descendents of this function.
      #-----------------------------------------------------------------------
      if (exists $rec->{is_repeat})
      {
	 print $marker x ($rec->{depth} + 1);
	 print "<repeat ...>\n";
      }
   }
}

#=============================================================================
# DUMP_FUNC_NAME - The normal case is trivial.  If the function name
# contains ':', then it is due to "function symbol aliasing".  For background
# on that, see the POD section above by that title.  
#=============================================================================
sub dump_func_name
{
   my ($func_name, $padding) = @_;

   #--------------------------------------------------------------------------
   # Handle trivial normal case
   #--------------------------------------------------------------------------
   if ($func_name !~ m/:/)
   {
      print "$func_name";
      return;
   }
   
   #--------------------------------------------------------------------------
   # Split the alias name up into an array of the individual names.  Ignore
   # any names never used.  The check for whether the name is in 
   # %function_aliases at all is a panic check.  There might be some weird
   # case where a non-alias name contains ':'.  
   #--------------------------------------------------------------------------
   my @aliases_used = grep {     exists $function_aliases{$_}
                             &&  exists $function_aliases{$_}->{used} } 
                         (split /:/, $func_name);

   #--------------------------------------------------------------------------
   # If only one of the aliased names is used, handle like the normal case.
   # If none of them are used, something odd is happening; handle like the
   # normal case because, well, what else can you do?  The interesting case
   # is where more than one name is output.  In that case, print with padding
   # in between each one, all surrounded by ( ).  
   #--------------------------------------------------------------------------
   if (scalar(@aliases_used) == 1)
      { print "$aliases_used[0]"; }
   elsif (scalar(@aliases_used) == 0)
      { print "$func_name"; }
   else
      { print "( ", join($padding, @aliases_used), " )"; }
}

#=============================================================================
# AUDIT_CALL_GRAPH - Run through the call graph and do the following:
# - Dump out graphs of functions not called directly
# - Report on functions that contain indirect calls but are not mentioned
#   in the configuration file for such indirect calls
#=============================================================================
sub audit_call_graph
{
   my ($func_name, @orphans, @no_config);

   #--------------------------------------------------------------------------
   # Go through all the functions in the call graph
   #--------------------------------------------------------------------------
   foreach $func_name (keys %call_graph)
   {
      #-----------------------------------------------------------------------
      # Save off funcs never called, thus have no parent, AKA orphans
      #-----------------------------------------------------------------------
      if (!$call_seen{$func_name})
      {
	 push @orphans, $func_name;
      }

      #-----------------------------------------------------------------------
      # Save off funcs that make indirect calls, but have no config info
      #-----------------------------------------------------------------------
      if (    exists($call_graph{$func_name}->{has_indirect})
	  && !exists($call_graph{$func_name}->{has_config_info}))
      {
	 push @no_config, $func_name;
      }
   }

   #--------------------------------------------------------------------------
   # Dump the full call graph for each orphan
   #--------------------------------------------------------------------------
   # Dump in order of the func names only to make the output the same 
   # across runs, making it easier to test this script.
   #--------------------------------------------------------------------------
   if (scalar(@orphans))
   {
      print "\n";
      if (!$is_lib)
      {
         print << "NO_CALLS";
The roots of the following graphs are functions that: 
- Are never called ... OR ...
- Are called indirectly and are not listed among the functions called
  indirectly in the configuration file specified with --i_cfg=file.
  Run "perldoc call_graph.pl" for more information.
NO_CALLS
      }
      else
      {
	 print << "NO_CALLS_LIB";
The roots of the following graphs are functions that:
- Are not called from elsewhere in the library ... OR ...
- Are called from elsewhere in the library, but such calls are indirect and
  are not listed among the functions in the configuration file specified
  with --i_cfg=file.  Run "perldoc call_graph.pl" for more information.
NO_CALLS_LIB
      }
      print '=' x 70, "\n\n";

      foreach $func_name (sort @orphans)
      {
         @graph_in_order = ();
	 walk_call_graph($func_name, 0);
         dump_call_graph();
	 print "\n";
      }
   }

   #--------------------------------------------------------------------------
   # Dump the name only of funcs with indirect calls, but no config info
   #--------------------------------------------------------------------------
   if (scalar(@no_config))
   {
      print "\n";
      print << "NO_CONFIG";
The following functions are known to contain indirect function calls, but
do not contain any information about those indirect calls in the configuration
file specified with --i_cfg=file.  Run "perldoc call_graph.pl" for more
information.
NO_CONFIG
      print '=' x 70, "\n\n";

      foreach $func_name (sort @no_config)
      {
         dump_func_name($func_name, ' ');
         print "\n";
      }
   }
}

#=============================================================================
# FIND_STACK_MAX - Find the call tree (starts with a function which has no
#    callers) that uses the most stack.  Report just the name and stack usage
#    for that tree.  Do NOT display the whole tree.  This is useful for
#    automated scripts that want to know how much stack a library uses.
#=============================================================================
sub find_stack_max
{
   my ($func_name, @orphans, $stack_max);

   if (!%call_graph)
   {
      warn "No graph to output\n";
      print "NULL : wcs = 0\n";
      return;
   }

   #--------------------------------------------------------------------------
   # Find all the functions which have no caller, AKA no parent, AKA orphans
   #--------------------------------------------------------------------------
   foreach $func_name (keys %call_graph)
   {
      next if ($call_seen{$func_name});
      push @orphans, $func_name;
   }

   #--------------------------------------------------------------------------
   # Entry point, if any, must be considered as well
   #--------------------------------------------------------------------------
   if ($entry_name)
      { push @orphans, $entry_name; }

   if (!scalar(@orphans))
   {
      warn "All of the functions have callers.  Do not know where to \n" .
           "start traversing the call graph.  No output.\n";
      return;
   }

   #--------------------------------------------------------------------------
   # Build the call graph for each orphan.  Keep track of the one with the
   # largest stack usage.
   #--------------------------------------------------------------------------
   $stack_max = { wcs  => 0, 
                  name => '' };
   foreach $func_name (@orphans)
   {
      @graph_in_order = ();
      walk_call_graph($func_name, 0);
      if (   exists $call_graph{$func_name}->{wcs}
          && $call_graph{$func_name}->{wcs} > $stack_max->{wcs})
      {
          $stack_max = { wcs  => $call_graph{$func_name}->{wcs}, 
	                 name => $func_name };
      }
   }

   #--------------------------------------------------------------------------
   # Report function that uses the most stack
   #--------------------------------------------------------------------------
   print "$stack_max->{name} : wcs = $stack_max->{wcs}\n";

   #--------------------------------------------------------------------------
   # If func that uses the most stack make no direct function calls (i.e.
   # has no kids, then something real weird is going on.  For C6000, they
   # may have built everything with -ml3.  
   #--------------------------------------------------------------------------
   if (!exists($call_graph{$stack_max->{name}}->{kids}))
   {
      warn << "MAX_NO_KIDS";
The function which uses the most stack also makes no direct function calls.
The max stack size is almost certainly too low.  Please view the online
documentation with the command 'perldoc call_graph.pl' for possible
explanations.
MAX_NO_KIDS

   }

=begin comment
This code prints out ALL the orphan roots, sorted by wcs.  It is relatively
inexpensive to execute.  I don't know if this would ever be useful.  It
might.  So I thought I would preserve it like this.

   foreach $func_name (sort {     $call_graph{$b}->{wcs} 
                              <=> $call_graph{$a}->{wcs}} @orphans)
   {
      print "$func_name : wcs = $call_graph{$func_name}->{wcs}\n";
   }
=end comment

=cut
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

