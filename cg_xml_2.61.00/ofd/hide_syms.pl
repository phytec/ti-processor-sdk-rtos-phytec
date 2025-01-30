#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

hide_syms.pl

=head1 SYNOPSIS

Hides symbols in a COFF .out or .obj file

=head1 USAGE

hide_syms.pl [options] I<xml file> I<sym1 sym2 ...>

   ... OR ...

ofdXX -x I<out file> | perl hide_syms.pl [options] I<sym1 sym2 ...>

=head1 OPTIONS

 -h               : Output usage and quit
 --cg_xml_version : Print out the version of the cg_xml package in use

=head1 DESCRIPTION

The symbols to hide must be global and defined in the file (storage class
C_EXT).  Such symbols are associated with actual code or variable storage
in the file.  Global symbols only referenced in a file (storage class
C_EXTREF) are NOT hidden.  Hiding is implemented by changing the storage class
of these symbols to static (C_STAT).  Future links against this .out or .obj
file will result in these symbols not being used to resolve other external
references.

To verify the changes to the COFF file, perform steps similar to the following:

=over

=item *

Use the nmXX utility to see the symbols in the COFF file.  Pipe it through
grep (Unix) or find (DOS) to filter for the symbol of interest.  Example:

   % nm6x -l coff.out | find "_malloc"
   [1091]  |0x00005100|C_EXT   |.text              |_malloc

=item *

Run the script as described above.

=item *

Use the nmXX utility to see the symbol storage class change to C_STAT.
Example:

   % nm6x -l coff.out | find "_malloc"
   [1091]  |0x00005100|C_STAT  |.text              |_malloc

=back

=head1 USING OFD

OFD stands for Object File Display utility.  Examples include ofd6x
for C6000 and ofd55 for C5500.

Use the -x option of OFD to create the XML file.  Example ...
       
      ofd6x -x -o=file.xml file.out

You can avoid creating the XML file by piping the output from OFD into the
script ...

      ofd6x -x file.out | perl hide_syms.pl [options] I<sym1 sym2 ...>

=head1 OFD OPTIONS

Recent releases of OFD support options for filtering the XML output down to
what is strictly of interest.  The best options to use in combination with
this script are:

  -x --xml_indent=0 --obj_display=none,header,symbols

Filtering the XML in this way reduces the amount of data processed by this
script, thus making it run faster.


=head1 PERL VERSION DETAILS

This script was written using Perl version 5.8.4.  It may not work
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
    $symtab_start,      # File pointer to start of symbol table in COFF file
    %ext_symtab,        # Symbol table for C_EXT symbols
    $c_stat_val);       # Binary byte representation of C_STAT value

#----------------------------------------------------------------------------
# Constants from Appendix A of the Assembly Language Tools User's Guide
#----------------------------------------------------------------------------
my $SYMTAB_PTR_OFFSET      = 8;
my $SIZEOF_SYMBOL_REC      = 18;
my $OFFSETOF_STORAGE_CLASS = 16;
my $C_STAT                 = 3;

#----------------------------------------------------------------------------
# Declare the functions
#----------------------------------------------------------------------------
sub process_cmd_line;
sub build_ext_symtab;
sub open_files;
sub get_symtab_start;
sub hide_sym;
sub close_files;

#=============================================================================
# MAIN ROUTINE
#=============================================================================
{
   my ($xml_data, @file_list, $file_data, $sym_name);

   if ($^V lt v5.8.3)
   {
      print  "WARNING: Developed under Perl version 5.8.3.\n";
      printf "         May not work under version %vd\n\n", $^V;
   }

   process_cmd_line();
   $xml_data = ofd_process_xml_file($xml_file);

   if (ofd_find_lib_name($xml_data))
      { die "$0 does not handle libraries"; }

   @file_list = ofd_each_file($xml_data);
   $file_data = $file_list[0];

   #----------------------------------------------------------------------------
   # This script is seldom used.  And it has many presumptions about COFF in
   # it that cannot be handled in XML_TI_OFD.pm.  Anything related to writing
   # over the symbols in the object file (where the symbol table starts, how
   # big a symbol entry is, etc.) would have to be found and changed to support
   # both COFF and ELF.  It seems like a lot of work for little benefit.  
   # This decision may be revisited as customer needs change.
   #----------------------------------------------------------------------------
   if (exists $file_data->{elf})
      { die "$0 does not support ELF"; }

   build_ext_symtab($file_data);
   open_files($file_data);
   $symtab_start = get_symtab_start($file_data);

   #----------------------------------------------------------------------------
   # Use pack('C', $var) to convert from string to unsigned 8-bit binary value
   #----------------------------------------------------------------------------
   $c_stat_val = pack('C', $C_STAT);

   #----------------------------------------------------------------------------
   # Hide symbols one at a time
   #----------------------------------------------------------------------------
   foreach $sym_name (@ARGV) { hide_sym($sym_name); }

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

   #----------------------------------------------------------------------------
   # If --cg_xml_version is requested, print that out to stderr
   #----------------------------------------------------------------------------
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
Usage: $pn [options] <xml file> <sym1 sym2 ...>  ... OR ...
       ofdXX -x <obj or out file> | perl $pn <sym1 sym2 ...>
END_USAGE
   }

   #--------------------------------------------------------------------------
   # If first arg ends with '.xml', then it is the input XML file
   #--------------------------------------------------------------------------
   if ($ARGV[0] =~ m/.xml$/i)
   {
      $xml_file = shift @ARGV;
   }
   #--------------------------------------------------------------------------
   # Otherwise, presume XML file is being piped to stdin
   #--------------------------------------------------------------------------
   else
   {
      $xml_file = '-';
      warn "Reading from stdin ...\n";
   }

   #--------------------------------------------------------------------------
   # Note list of syms to hide is left in @ARGV
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
      if ($sym->{'storage_class'} eq 'C_EXT')
      {
         $ext_symtab{$sym->{'name'}} = $sym;
      }
   }
}

#=============================================================================
# OPEN_FILES
#=============================================================================
sub open_files()
{
   my ($file_data) = @_;

   #--------------------------------------------------------------------------
   # Get name of .out file from the XML file
   #--------------------------------------------------------------------------
   my $filename = $file_data->{'name'};

   #--------------------------------------------------------------------------
   # Open input COFF file (.out) for updating.  Note it is possible to get 
   # into a bit of trouble if you do the following ...
   # 
   #    ofdXX -x -o=file.xml file.out
   #    cd /some/other/dir
   #    hide_syms.pl /path/to/file.xml <sym1 sym2 ...>
   #
   # At this point $filename = 'file.out', and this open will fail.  This is
   # judged to be an unlikely scenario.
   #--------------------------------------------------------------------------
   open (OBJFILE, "+<$filename") or die "Cannot open $filename for update: $!";
   binmode(OBJFILE);
}

#=============================================================================
# GET_SYMTAB_START - The file pointer to the beginning of the COFF symbol
#    table is contained in the file header.  But is not exposed by OFD.
#    Thus, have to read it directly out of the COFF file itself.
#=============================================================================
sub get_symtab_start
{
   my ($file_data) = @_;
   my ($raw_data, $val);

   seek(OBJFILE, $SYMTAB_PTR_OFFSET, 0) or die "Seeking: $!";
   read(OBJFILE, $raw_data, 4)          or die "Reading: $!";

   #--------------------------------------------------------------------------
   # Use unpack('L', $var) to convert from unsigned 32-bit binary
   # representation to Perl's string representation of the number
   #--------------------------------------------------------------------------
   $val = unpack('L', $raw_data);

   #--------------------------------------------------------------------------
   # In COFF, raw data sections are in the endianness of the target machine.
   # The endianness of all other parts of the file, such as the file header,
   # are in the endianness of the host machine where the final build occurred.
   # For example, this script could execute on a PC (little endian) while
   # processing an .out file built on a Sparc (big endian).  In that case,
   # OFD notes that the file is "byte_swapped".  Which means the script must
   # byte swap the symbol table file pointer as shown.
   #--------------------------------------------------------------------------
   if (   exists $file_data->{'ti_coff'}->
                            {'file_header'}->{'byte_swapped'}
       &&        $file_data->{'ti_coff'}->
                            {'file_header'}->{'byte_swapped'} eq 'true')
   {
      $val =   (($val << 24) & 0xff000000)  
             | (($val <<  8) & 0x00ff0000) 
	     | (($val >>  8) & 0x0000ff00) 
	     | (($val >> 24) & 0x000000ff);
   }

   return $val;
}

#=============================================================================
# HIDE_SYM - Hide one symbol
#=============================================================================
sub hide_sym
{
   my ($sym_name) = @_;
   my ($sym, $fidx);

   $sym = $ext_symtab{$sym_name};
   if (!$sym)
   {
      warn "Cannot find symbol \"$sym_name\", ignored\n";
      return;
   }

   #--------------------------------------------------------------------------
   # File index of byte to overwrite is the sum of:
   # - Start of symbol table
   # - Index of the symbol in the table * sizeof(symbol record)
   # - Offset of the storage class byte in the symbol
   #--------------------------------------------------------------------------
   $fidx =   $symtab_start
           + oct($sym->{'index'}) * $SIZEOF_SYMBOL_REC
	   + $OFFSETOF_STORAGE_CLASS;

   #--------------------------------------------------------------------------
   # Overwrite the symbol storage class from C_EXT to C_STAT
   #--------------------------------------------------------------------------
   seek(OBJFILE, $fidx, 0)   or die "Seeking: $!";
   print OBJFILE $c_stat_val or die "Printing: $!";
}

#=============================================================================
# CLOSE_FILES
#=============================================================================
sub close_files
{
   close OBJFILE;
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

