#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

global_types_gen.pl

=head1 SYNOPSIS

Reconstructs the global types used in a program. Can be used
for example to see some of the data types used internally in DSP/BIOS
or other libraries where the source code is typically not available.
Useful for debugging e.g. postmortem situations. Conceivably
host tools could be written which use this data to match up 
last-state-before-crash with the data structures used in the code.

If you have all the source code available to all the libraries
in your program this script is useless - but thats rarely the case.

Works on executables, object files & libraries.

=head1 USAGE

global_types_gen <filename.xml> 

Example (from a DOS prompt): -

    [>] <ccstudiodir>\dosrun.bat
    [>] ofd6x -xg -o=app.xml app.out
    [>] perl global_types_gen.pl app.xml

... OR ...

    [>] ofdXX -xg <out file> | perl global_types_gen.pl 

=head1 OPTIONS

None

=head1 DEPENDENCIES

You need to first run OFD which is only available
in Codegen Tools >= v5.0 for c6000, v3.0 for C55, v4.0 for C54

Also, you *MUST* run ofdXX -g. The -g flag is essential, otherwise
the DWARF DW_AT_stuff tags will *NOT* be in the XML file.

Additional limitations are: -

    > only works on code built with 'new' Codegen ie code built for
      DWARF-Debug (e.g. >= CGT 5.0 for c6x, >= CGT 3.0 for c55x...)
    > only displays C type data. Will not work on C++ classes etc.

=head1 DESCRIPTION

Reconstructs the global types used in a program. Can be used
for example to see some of the data types used internally in DSP/BIOS
or other libraries where the source code is typically not available.
Useful for debugging e.g. postmortem situations. Conceivably
host tools could be written which use this data to match up 
last-state-before-crash with the data structures used in the code.

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -xg --xml_indent=0 --obj_display=none --dwarf_display=none,dinfo,types

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

use warnings;
use strict;

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
# Module for handling command line options
#=============================================================================
use Getopt::Long; 

#=============================================================================
# Global variables
#=============================================================================
my $xml_file;           # the .xml file produced from running ofdNN -x -g
my $xml_in_data;        # the hash tree structure returned by XMLin()
my $dwarf_hash_ref;     # reference to tree of DWARF info
my @struct_data;        # accumulate structure data of each object file
my %typedef_seen;       # checks (and enables elimination of) duplicate typedefs
my $all_lines;          # a slurped scalar of all stripped XML content

my %typedef_id_to_names;
my %modifiers;

my $debug_mode = 0;     # flip to 1 to see lots of debug info
        
#=============================================================================
# MAIN ROUTINE
#=============================================================================
if ($^V lt v5.8.3)
{
   print  "WARNING: Developed under Perl version 5.8.3.\n";
   printf "         May not work under version %vd\n\n", $^V;
}

# get cmd line args and store OFD XML as hash reference
process_cmd_line();     

#
# Strip out parts of the XML file that are not needed.  Detailed knowledge
# of the XML and what this script uses is required to get this part right.  
# This step runs fast, and reduces script run time drastically.  All the
# remaining lines of the XML file come back in $all_lines.
# GENERAL : Anything enclosed by these XML tags is removed
# SECTION : Any <section>'s with this name are removed
# DIE_TAG : Any <die>'s with this name are removed
#
ofd_strip_xml($xml_file,
              \$all_lines,
              GENERAL => [qw(string_table
                             raw_data
                             symbol_table
                             line_numbers)],
              SECTION => [qw(.debug_line
                             .debug_frame)],
              DIE_TAG => [qw(DW_TAG_TI_assign_register
                             DW_TAG_formal_parameter
                             DW_TAG_variable
			     DW_TAG_TI_reserved_3
			     DW_TAG_TI_branch
			     DW_TAG_TI_reserved_2
			     DW_TAG_TI_loop_range
			     DW_TAG_subprogram)]);

# get well-formatted hash courtesy of the XML_TI_OFD module
$xml_in_data = ofd_process_xml_file($all_lines);

# Get name of .out / .obj / .lib from the XML file
dump_filename();        

foreach my $file_data (ofd_each_file($xml_in_data)) {

    # ensure ofd<ISA> -g was run to get dwarf section
    if (!ofd_has_dwarf($file_data)) {
        die "No DWARF info in XML file : make sure you use -g flag with OFD\n";
    }

    my (@debug_info_sections);
    if (!(@debug_info_sections = ofd_each_debug_info($file_data)))
    {
       warn "No .debug_info sections found in $file_data->{name}\n";
       next;
    }
    
    %modifiers = ();
    %typedef_id_to_names = ();

    # Index all the type information needed later on
    foreach my $debug_info_ref (@debug_info_sections)
        { index_type_dies($debug_info_ref); }

    foreach my $debug_info_ref (@debug_info_sections)
    {
        # use typedef names and additionally get struct names, member names & datatypes
        my $file_struct_data = undef;
        $file_struct_data = get_structs_members($debug_info_ref);
        
        # push the file structure data onto our storage array
        if ((defined($file_struct_data)) && (scalar(@{$file_struct_data}))) {
            push(@struct_data, $file_struct_data);
        }
    }
}

# flip e.g. int volatile x to volatile int x since 'typically' users wrote that
twiddle_type_modifiers(\@struct_data);

if ($debug_mode) { XML_TI_OFD::dump_out(\@struct_data); };

# print complete struct data (to STDOUT but can be piped on cmd line to file)
print_structs(\@struct_data);

exit(0);


#=============================================================================
# DUMP_FILENAME
# Simply finds the name of the .out file from the OFD XML and prints it
#=============================================================================
sub dump_filename
{
    my $filename = ofd_get_input_file_name($xml_in_data);
    
    $filename =~ m/([\w\.]+$)/;        # strip off the path
    print ("\nGlobal Data Types for ", $1, "\n");
    print ("-----------------", "\n\n");
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
# GET_TYPES_DEBUG_SECTION_REF
# Returns a reference to the section which holds all the types data
#=============================================================================
sub get_types_debug_section_ref
{  
    my($debug_sect, $file_data_ref) = @_;
 
    my $sect;    
    my $success;
         
    foreach $sect (@{$dwarf_hash_ref->{'section'}}) {
        
        $success = 0;
        
        #
        # if user compiled with --symdebug:none it produces a .debug_info
        # section with just 1 entry - a 'name' tag. If we find > 1 hash entry
        # its a 'real' section
        # 
        my $num_sect_hash_entries = keys(%$sect);
             
        if (($sect->{'name'} eq $debug_sect) && ($num_sect_hash_entries > 1)) {
            $success = 1;           # found the section we're looking for
            return $sect;
        }
        
    }
    
    # if we've reached here, then we didnt find a 'real' .debug_info 
    warn "missing or empty ", $debug_sect, " section in ", $file_data_ref->{'name'}, "\n\n";
    
    return undef;
}


#=============================================================================
# GET_TYPEDEF_DATA
#
# This function builds up data structures, keyed by DIE ID, used later on.  
# The idea is to get as much of this stuff as possible in one pass through
# the Dwarf data structure, thus avoid the need to do it again.  Keep in
# mind walking through the entire Dwarf data structure as this function does
# is an expensive operation.
#
# Data structures built here ...
# - typedef_id_to_names - Maps a DIE ID for typedef to the name (or names) for
#   the typedef
# - modifiers - All the basic type modifiers like "int", "const", "volatile",
#   etc. that are chained together to form an actual user type.
#=============================================================================
sub index_type_dies
{  
   my($debug_type_data_ref) = @_;
   my ($file, $die, %typedef_dies, $name, $id);

   #--------------------------------------------------------------------------
   # Walk through all the Dwarf DIE's.  Collect the typedef DIES in one spot,
   # and all the other type modifiers in another.
   #--------------------------------------------------------------------------
   foreach $file (@{$debug_type_data_ref->{compile_unit}})
   {
      foreach $die (@{$file->{die}})
      {
	 ofd_find_all_dies($die, undef,
			   DW_TAG_typedef          => \%typedef_dies,

			   DW_TAG_base_type        => \%modifiers,
			   DW_TAG_pointer_type     => \%modifiers,
			   DW_TAG_const_type       => \%modifiers,
			   DW_TAG_restrict_type    => \%modifiers,
			   DW_TAG_ioport_type      => \%modifiers,
			   DW_TAG_onchip_type      => \%modifiers,
			   DW_TAG_volatile_type    => \%modifiers,
			   DW_TAG_structure_type   => \%modifiers,
			   DW_TAG_union_type       => \%modifiers,
			   DW_TAG_subroutine_type  => \%modifiers,
			   DW_TAG_array_type       => \%modifiers,
			   DW_TAG_unspecified_type => \%modifiers,

      #-----------------------------------------------------------------------
      # Enumerated types are ignored throughout, except here.  An object
      # of an enumerated type will look that type up in the modifiers
      # hash.  Make sure it will find something.
      #-----------------------------------------------------------------------
			   DW_TAG_enumeration_type => \%modifiers,
			  );
      }
   }

   #--------------------------------------------------------------------------
   # Go through the typedef DIE's just collected
   #--------------------------------------------------------------------------
   foreach $die (values %typedef_dies)
   {
      #-----------------------------------------------------------------------
      # The typedef DIE's are needed in the modifier hash as well
      #-----------------------------------------------------------------------
      $modifiers{$die->{id}} = $die;

      #-----------------------------------------------------------------------
      # Collect the attributes to create the hash that maps the DIE ID to
      # the typedef name
      #-----------------------------------------------------------------------
      $name = $id = undef;
      ofd_find_attrs($die,
		     DW_AT_name => \$name,
		     DW_AT_type => \$id);
      next if (!$name || !$id);

      #-----------------------------------------------------------------------
      # Fix for BZ 2935.  It is possible for more than one typedef name to
      # refer to the same type information for a structure.  This hash
      # maps a type ID number to an array of names.  Add one name to the array.
      #-----------------------------------------------------------------------
      push @{$typedef_id_to_names{$id}}, $name;
   }
}

#=============================================================================
# GET_STRUCTS_MEMBERS
# DWARF2 Spec : Chapter 5
# Get the structure names and their members
#=============================================================================
sub get_structs_members
{
    my($debug_type_data_ref) = @_;
 
    my $struct_ct = 0;
    my $struct_data_ref;

    #-------------------------------------------------------------------------
    # As part of fixing BZ 2935, this entire loop was refactored.  In theory,
    # the fix does not require the refactor.  But rewriting the code was a
    # good way to deeply understand how it worked, and thus how to fix it.
    #-------------------------------------------------------------------------
    # Loop through each file's worth of type information
    #-------------------------------------------------------------------------
    foreach my $file (@{$debug_type_data_ref->{compile_unit}})
    {
        #---------------------------------------------------------------------
	# Collect all of the structure and union DIE's
        #---------------------------------------------------------------------
	my @su_dies = ();
        foreach my $die (@{$file->{die}})
	{
	    ofd_put_dies_in_array($die, 
	                          DW_TAG_structure_type => \@su_dies,
				  DW_TAG_union_type     => \@su_dies);
	}

        #---------------------------------------------------------------------
	# Loop through the structure and union DIE's
        #---------------------------------------------------------------------
	foreach my $su_die (@su_dies)
	{
            #-----------------------------------------------------------------
	    # It is somehow possible to have forward references to a struct
	    # that does not have a DW_AT_declaration.  It does not have a
	    # sub-die with information about struct members.  Skip it here and
	    # expect it to show up later with the sub-die member info.
            #-----------------------------------------------------------------
	    next unless (exists $su_die->{die});
            
            #-----------------------------------------------------------------
	    # Collect some attributes on this structure.  More efficient to
	    # do this with one call to ofd_find_attrs.
            #-----------------------------------------------------------------
	    my ($is_decl, $name_from_attr, $struct_size);
	    $is_decl = $name_from_attr = $struct_size = undef;
           
	    ofd_find_attrs($su_die, 
	        DW_AT_declaration => \$is_decl,
		DW_AT_name        => \$name_from_attr,
	        DW_AT_byte_size   => \$struct_size);

            #-----------------------------------------------------------------
            # Ignore incomplete/opaque structs that have DW_AT_declaration
            #-----------------------------------------------------------------
	    next if (defined $is_decl);

            #-----------------------------------------------------------------
            # Check if this struct has one or more typedef names.  If so,
	    # pick up the first one (index 0).
            #-----------------------------------------------------------------
	    my $typedef_name = undef;
	    if (exists $typedef_id_to_names{$su_die->{id}})
	    {
	        $typedef_name = ${$typedef_id_to_names{$su_die->{id}}}[0];
	    }

            #-----------------------------------------------------------------
	    # Maybe use the name from the structure attributes
            #-----------------------------------------------------------------
            if (!(defined $typedef_name))
	    {
	        $typedef_name = $name_from_attr;
	    }

            #-----------------------------------------------------------------
	    # Save off whatever struct name is found.  Skip any struct seen
	    # before.
            #-----------------------------------------------------------------
	    if (defined $typedef_name)
	    {
		next if (exists $typedef_seen{$typedef_name});
		$typedef_seen{$typedef_name} = 1;

		$struct_data_ref->[$struct_ct]->{typedefName} = $typedef_name;
	    }
	    else
	    {
	        $struct_data_ref->[$struct_ct]->{typedefName} = "";
	    }

            #-----------------------------------------------------------------
	    # Get the size of the structure
            #-----------------------------------------------------------------
	    if (defined $struct_size)
	    {
	        $struct_data_ref->[$struct_ct]->{structSz} =
		    str_to_num($struct_size);
	    }

            #-----------------------------------------------------------------
	    # Collect the tag: struct or union.
            #-----------------------------------------------------------------
	    $struct_data_ref->[$struct_ct]->{tag} = $su_die->{tag};

            #-----------------------------------------------------------------
	    # Collect info on members of the structure
            #-----------------------------------------------------------------
            my $member_ct = 0;
            my @member_dies = ();
	    ofd_put_dies_in_array($su_die, DW_TAG_member => \@member_dies);

	    foreach my $member_die (@member_dies)
	    {
	        my ($name, $type, $offset);
	        $name = $type = $offset = undef;

	        ofd_find_attrs($member_die, 
	                       DW_AT_name => \$name,
			       DW_AT_type => \$type,
			       DW_AT_data_member_location => \$offset);

	        next unless (   defined($name)
	                     && defined($type)
			     && defined($offset));

                # An offset looks like: DW_OP_plus_uconst 0x2
		# This regex picks off the last bit of non-space
		if ($offset !~ m/(\S+)$/)
		    { die "Unexpected pattern, stopped"; }
		$offset = str_to_num($1);

		$struct_data_ref->[$struct_ct]->{members}->[$member_ct]->
		    {memberName} = $name;
		$struct_data_ref->[$struct_ct]->{members}->[$member_ct]->
		    {typeMods}->[0]->{idRef} = $type;
		$struct_data_ref->[$struct_ct]->{members}->[$member_ct]->
		    {offSet} = $offset;
		
		$member_ct++;
	    }

            #-----------------------------------------------------------------
	    # More new code for BZ 2935 ...
	    # If the name of this struct is a typedef, then the first typedef
	    # name is used above for the name of the struct.  This code
	    # handles additional typedef names for the same struct.  In the
	    # common case of only one typedef name, the for loop does nothing.
	    # Otherwise, it creates a new record in $struct_data_ref for each
	    # additional typedef name.
            #-----------------------------------------------------------------
	    if (exists $typedef_id_to_names{$su_die->{id}})
	    {
		my $orig_sc = $struct_ct;
		my $idx;
		for ($idx = 1;  # not 0! That name is already saved off.
		     $idx < scalar(@{$typedef_id_to_names{$su_die->{id}}});
		     $idx++)
		{
		    my $name = ${$typedef_id_to_names{$su_die->{id}}}[$idx];

		    next if (exists $typedef_seen{$name});
		    $typedef_seen{$name} = 1;

                    #---------------------------------------------------------
		    # Create the new record in $struct_data_ref.
                    #---------------------------------------------------------
		    # In Perl this code "$ref = { %hash };" means to allocate
		    # memory from the heap, init it with all the key/value
		    # pairs in that hash, and assign the address to $ref.  
                    #---------------------------------------------------------
		    $struct_ct++;
		    $struct_data_ref->[$struct_ct] =
		        { %{$struct_data_ref->[$orig_sc]} };

                    #---------------------------------------------------------
		    # The name in the new record is the first typedef.  Change
		    # it to this additional typedef name.
                    #---------------------------------------------------------
		    $struct_data_ref->[$struct_ct]->{typedefName} = $name;
		}
	    }
            
            $struct_ct++;
        } # each structure
    } # each file

    #-------------------------------------------------------------------------
    # End of refactor for BZ 2935
    #-------------------------------------------------------------------------

    my $type_mod_ct = 0;
    W_IDREF: while (1) {
        # bail if we've no work to do i.e. if prev loop was a NOP
        last unless ($struct_ct > 0);

	my ($struct_key, $member_key);    
	my $done_with_idrefs = 1;
	
	# find the type associated with the id in our stored reference
	foreach $struct_key (@{$struct_data_ref}) {
	
	    foreach $member_key (@{$struct_key->{members}}) {
		if ( exists($member_key->{typeMods}->[$type_mod_ct]) && 
		     defined($member_key->{typeMods}->[$type_mod_ct]->{idRef}) ) {
		    $done_with_idrefs = 0;    # still more types work to do
		    my $die_tag = $modifiers{$member_key->{typeMods}->[$type_mod_ct]->{idRef}};
		    if ($die_tag) {
			my $next_type_mod_ct = $type_mod_ct + 1;
			c_type_modifiers($member_key, $die_tag, $next_type_mod_ct);
		    }
		    else
		    {
		       warn "Did not find DIE for ID: ",
		           $member_key->{typeMods}->[$type_mod_ct]->{idRef}, "\n";
		    }
		}
	    }
	} # end id-type-check for-loop
	if ($done_with_idrefs) {       # no idRefs left in any members of any struct
	    last W_IDREF;
	}

        $type_mod_ct++;       # increment C modifiers count for next R->L storage
    }
      
    return $struct_data_ref;
}


#=============================================================================
# C_TYPE_MODIFIERS - Given a DIE TAG type, return/store its C representation 
# Type modifiers are things like 'const' or 'volatile' or '*'
#=============================================================================
sub c_type_modifiers
{
    #
    # index from Right->Left for type modifier 
    #     ie 0 is R-most index, 1 is second R-most index...
    #
    my ($member_ref, $dw_tag, $right_assoc_index) = @_;       

    my $dw_tag_type = $dw_tag->{'tag'};
    
    #
    # Handle the Simple Types
    # By not storing the idRef of of a DW_TAG_typedef we stop the potentially
    # recursive chain i.e. we typically want to print something like: -
    # typedef struct XYZ_Obj {
    #     Int *bufend;
    #     XYZ_Event *curptr;
    # } LOG_Obj;
    # i.e. we dont want to further traverse XYZ_Event since its listed out anyway
    #
    if ($dw_tag_type eq 'DW_TAG_base_type') {
        $member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = get_attr($dw_tag, 'DW_AT_name');
    } elsif ($dw_tag_type eq 'DW_TAG_pointer_type') {        
        # default to void * : the void * case has no DW_AT_type attribute
        $member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'void *';
        foreach my $attr (@{$dw_tag->{'attribute'}}) {
            if ($attr->{'type'} eq 'DW_AT_type') {      # its not a void * ptr
                $member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = '*';
                $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
                last;      # found DW_AT_type attribute so bail out of loop
            }
        }                
    } elsif ($dw_tag_type eq 'DW_TAG_const_type') {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'const';
        $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
    } elsif ($dw_tag_type =~ m/DW_TAG_restrict_type|DW_TAG_TI_restrict_type/) {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'restrict';
        $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
    } elsif ($dw_tag_type =~ m/DW_TAG_ioport_type|DW_TAG_TI_ioport_type/) {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'ioport';
        $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
    } elsif ($dw_tag_type =~ m/DW_TAG_onchip_type|DW_TAG_TI_onchip_type/) {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'onchip';
        $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
    } elsif ($dw_tag_type eq 'DW_TAG_volatile_type') {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'volatile';
        $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
    } elsif ($dw_tag_type eq 'DW_TAG_typedef') {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = get_attr($dw_tag, 'DW_AT_name');
    } elsif ($dw_tag_type eq 'DW_TAG_structure_type') {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'struct ' . get_attr($dw_tag, 'DW_AT_name');
    } elsif ($dw_tag_type eq 'DW_TAG_union_type') {
	$member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'union ' . get_attr($dw_tag, 'DW_AT_name');
    } elsif ($dw_tag_type eq 'DW_TAG_subroutine_type') {
        #
        # too tricky to expand full fxn pointers with args. Using short-hand.
        # default to void * : the void * case has no DW_AT_type attribute
        #
        $member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = "void " . "A-FUNC-PTR";
        foreach my $attr (@{$dw_tag->{'attribute'}}) {
            if ($attr->{'type'} eq 'DW_AT_type') {      # its not a void * fxnptr
                $member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = 'A-FUNC-PTR';
                $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
                last;      # found DW_AT_type attribute so bail out of loop
            }
        }        
    } elsif ($dw_tag_type eq 'DW_TAG_array_type') {                
        $member_ref->{typeMods}->[$right_assoc_index]->{typeMod} = get_array_dimensions($dw_tag);
        $member_ref->{typeMods}->[$right_assoc_index]->{idRef} = get_attr($dw_tag, 'DW_AT_type');
        # array bracketing always goes after memberName
        $member_ref->{typeMods}->[$right_assoc_index]->{typePos} = 'end';
    } 

}    
    

#=============================================================================
# GET_ARRAY_DIMENSIONS - Determines the dimensions of single & multi-dimension
# arrays e.g. Int x[5] or MyType y[0xA0][20];
# multiple-dimensions are stored in DWARF section as successive
# DW_TAG_subrange_type DIE tags.
#=============================================================================
sub get_array_dimensions()
{
    my ($dw_tag) = @_;
    
    my $arr_entry;
    
    # access DW_AT_upper_bound (last array index) of DW_TAG_subrange_type
    foreach my $die_subr_enum_tag (@{$dw_tag->{'die'}}) {
        my $val = get_attr($die_subr_enum_tag, 'DW_AT_upper_bound');
        $val = oct($val) if $val =~ /^0/;
        my $arr_sz = $val + 1; 
        $arr_entry .= '[ ' . $arr_sz . ' ]';       # concat multi-dimensions 
    }
        
    return $arr_entry;
}


#=============================================================================
# TWIDDLE_TYPE_MODIFIERS
# Make sure that e.g. we print
# volatile  int     busy
# instead of 
# int	volatile	busy
# Note that this is purely an attempt to match 'typical' declarations
# a user makes ie typicaly volatile is specified first. volatile int .v.
# int volatile are both legal C.
#=============================================================================
sub twiddle_type_modifiers
{ 
    my ($files_struct_data_ref) = @_;

    foreach my $files_struct_key (@{$files_struct_data_ref}) {
        
        foreach my $struct_key (@{$files_struct_key}) {
            foreach my $member_key (@{$struct_key->{members}}) {            
    
                for (my $ctr = 0; $ctr < scalar(@{$member_key->{typeMods}}); $ctr++) {
                    if (($member_key->{typeMods}->[$ctr]->{typeMod}) &&
                        ($member_key->{typeMods}->[$ctr]->{typeMod} =~ m/volatile|const/) &&
                        (!($member_key->{typeMods}->[$ctr+1]->{idRef})) ) {
                        # mark the typeMod entries to be swapped
                        $member_key->{typeMods}->[$ctr]->{typePos} = 'beg';
                    }
                }
            }
        }
    }
}

#=============================================================================
# GET_ATTR - Given the attribute name finds its {'value'}->{'ref'}
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
    
}

#=============================================================================
# PRINT_STRUCTS
# Print the typedef structure names and their members
#=============================================================================
sub print_structs
{ 
    my ($files_struct_data_ref) = @_;
    my @flat_struct_arr;    # an array with 1 less nesting level for easy sort
    
    print "Structure Data...(sizes in Minimum Addressable Data Units)\n";
    
    # flatten the data structure first so we can sort it on typedefName
    foreach my $files_struct_key (@{$files_struct_data_ref}) {
        foreach my $struct_key (@{$files_struct_key}) {
            push(@flat_struct_arr, $struct_key);
        }
    }    
    @flat_struct_arr = sort {$a->{typedefName} cmp $b->{typedefName}} @flat_struct_arr;
        
    foreach my $struct_key (@flat_struct_arr) {

        # print structure name & its size in MADUs
        if ($struct_key->{typedefName}) {
            printf("\n%-30s", $struct_key->{typedefName});
        }
        else {  # truly an anonymous structure...pretty rare
            printf("\n%-30s", "ANONYMOUS_STRUCT");
        }

        print ' // Size in MADUs == ', $struct_key->{structSz}; 
        if ($struct_key->{tag} eq 'DW_TAG_union_type')
	    { print ' --- UNION ---'; }
	print "\n";

        # build up the type modifiers as per C ordering rules
        foreach my $member_key (@{$struct_key->{members}}) {

            my $c_type_str = ' ';

            my $mem_type_str = $member_key->{memberName};
            my $offset_str = ' // Offset == ' . $member_key->{offSet};

            # R->L associative storage in DWARF re the type modifiers
            foreach my $c_modifier (reverse(@{$member_key->{typeMods}})) {
                if ($c_modifier->{typeMod}) {
                    if ($c_modifier->{typePos} && $c_modifier->{typePos} eq 'beg') {
                        $c_type_str = " " . $c_modifier->{typeMod} . $c_type_str;
                    } elsif ($c_modifier->{typePos} && $c_modifier->{typePos} eq 'end') {
                        $mem_type_str = $mem_type_str . $c_modifier->{typeMod};
                    } else {
                        $c_type_str = $c_type_str . $c_modifier->{typeMod} . " ";
                    }
                }
            }
            # now print out the final type string, member name & its offset
            printf("\t%-30s %-20s %-15s\n", $c_type_str, $mem_type_str, $offset_str);            
        }
    }

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

