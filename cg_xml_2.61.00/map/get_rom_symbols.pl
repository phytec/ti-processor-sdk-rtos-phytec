#!/usr/bin/env perl
#The above line invokes perl from the current path

# This code released under a license detailed at the end of the file.

#=============================================================================
# POD Documentation First ...
#=============================================================================

=head1 NAME

get_rom_symbols.pl

=head1 SYNOPSIS

Perl script that reads in an XML file representation
of a linker map file and spits out many .asm files as follows: -

   /* This file is *auto-generated*. Mods risk being overwritten */
   
   	.global _memset
   _memset	.set	0x3e600

The end output is a list of all the symbols and their addresses to which they
got ROM'ed.

This would typically be run in the context of a batch file / shell-script doing
e.g. (first ensure cgtools\bin is in your path!)
   del *tmp*.asm
   del *tmp*.obj

   perl c:\temp\cg_xml\map\get_rom_symbols.pl ..\rom_image\rom_rts.xml -s=.rom_rts

   cl6x -g -mv6400 *.asm

   ar6x r rom_rts_symbols.lib *.obj

   del *tmp*.asm
   del *tmp*.obj

Why do we put each in a separate .asm file? Because then we can patch on a per
function basis i.e. we simply explicitly link the patched RAM version
before linkage of the ROM symbols lib. Each asm file is built and archived
into a library thus enabling standard linker -priority switch selection
mechanisms.

Wildcards on section names can be specified by using the -e option e.g.
-e=.bios.* will pick up all of .bios, fbios, .bios_abc, .bios:abc
We separate this from -s because -s is an exact string match e.g. -s=.bios
will *only* pick up .bios whereas -e=.bios would pick up .bios and fbios.

We also support absolute symbols - what does this mean? BIOS 5.x for example
has several absolute symbols which are symbols in the sym-table with a constant
value like PIP_A_TABLEN yet they do not take up any target memory, nor are
they part of any section. We need to 'bring these forward' in same fashion as the
functions in sections. We use a different cmd line option (-a) to do this.
The script user has to pass in the name of the absolute symbols to generate
files with .set directives. 

NOTE : this script generates A LOT of files! These are temporary since
we can kill the .asm and their .obj's after we archive the objects into a lib.
But we dont do the 'cleanup' in this script because we wish to avoid
shell [delete, rename etc] commands inside this script which would tie
the script to a particular environment [windows, linux etc]
You probably want to run this script from an empty directory

Note that the i/p XML version of map file can be obtained via linker option
--xml_link_info=<file>

=head1 USAGE

   get_rom_symbols.pl link_xml_file -s=.rom_sect1 -s=.rom_sect2 -e=.sect.* -a=sym1

=head1 ADDTIONAL OPTIONS

 --cg_xml_version : Print out the version of the cg_xml package in use

=head1 PERL VERSION DETAILS

This script was written using Perl version 5.8.3.  It may not work
with earlier revisions of Perl.

=cut

#=============================================================================
# Code starts here ...
#=============================================================================

use strict;
use warnings;

use Getopt::Long;

#==============================================================================
# Tell Perl that the module XML_TI_MAP.pm in the same directory as this script
#==============================================================================
use FindBin;
use lib $FindBin::Bin;

# use this perl module to enable simple xml tag structure traversal 
use XML_TI_MAP 1.01 qw(process_xml_file cg_xml_version);

#=============================================================================
# Global variables
#=============================================================================
my $xml_file;
my @inp_rom_sects;               # what section names did ROM code, const go into?
my @inp_rom_re_sects;            # sect-names specified w/ regular expression syntax
my @inp_rom_abssyms;             # may also need to bring forward absolute symbols
my @rom_sections;                # complete list of sections to get symdata from
my @rom_syms;                    # end array of ROM sym names & addresses

my $bench = 0;                   # for benchmarking this script
my ($start_time, $end_time, $diff_time);
if ($bench) { use Benchmark; }


#=============================================================================
# MAIN ROUTINE
#=============================================================================
if ($^V lt v5.8.3)
{
   print  "WARNING: Developed under Perl version 5.8.3.\n";
   printf "         May not work under version %vd\n\n", $^V;
}

process_cmd_line();

if ($bench) { $start_time = new Benchmark; }

my $xml_data = process_xml_file($xml_file);

my $rom_filename = $xml_data->{'output_file'};
my $output_sects = $xml_data->{'logical_group_list'}->{'logical_group'};
my $sym_tbl = $xml_data->{'symbol_table'};

build_sects_list();

# get the input symbol names that went into our rom_sects   
get_rom_symbols();

# add any absolute symbols to our rom_syms list
get_rom_abs_symbols(\@inp_rom_abssyms);

# dump all the symbols w/ associated ROM addresses to separate asm files
dump_rom_syms($rom_filename);

if ($bench) {
   $end_time = new Benchmark;
   $diff_time = timediff($end_time, $start_time);
   print "It took ", timestr($diff_time), "\n";
}

exit(0);



#=============================================================================
# PROCESS_CMD_LINE 
#=============================================================================
sub process_cmd_line
{
   my ($print_usage);

   #-------------------------------------------------------------------------
   # If --cg_xml_version is requested, write that out to stderr
   #-------------------------------------------------------------------------
   cg_xml_version();

   $print_usage = 0;

   GetOptions ("h" => \$print_usage,
               "sect=s" => \@inp_rom_sects,      # what sect did ROM code, const go into?
               "esect=s" => \@inp_rom_re_sects,  # regular expression equivalent of above
               "abssym=s" => \@inp_rom_abssyms,  # bring forward absolute symbols as well
               );

   if ($print_usage || (@ARGV != 1))
   {
      die "\nUsage: $0 [-h] xml_file -s=.rom_sect1 -s=.rom_sect2 -e=.sect.* -a=sym1\n";
   }
     
   $xml_file = $ARGV[0];
   die "Cannot find $xml_file: $!" unless (-e $xml_file);
   
   die "Need valid rom_section_name(s) and/or symbol_name(s). See -h Help: $!" unless
      ((@inp_rom_sects > 0) || (@inp_rom_abssyms > 0) || (@inp_rom_re_sects > 0));

}

#=============================================================================
# BUILD_SECTS_LIST
# Given the input sections list we build a hash of all sections so we can
# access elements quicker without needing to continually re-scan all sections
# or symbols.
# (unfortunately we need more looping than strictly necessary - this is
# because we error check & store only the sections that truly exist in the XML)
#=============================================================================
sub build_sects_list
{
   # the 'standard' sections passed in via -s
   foreach my $sect (@inp_rom_sects) {
      my $seen = 0;
      foreach my $log_grp (values %{$output_sects}) {
         next unless ($log_grp->{'name'} eq $sect);
         push @rom_sections, $log_grp;
         $seen = 1;
         last;         # in -s exact-string list there's only 1 match hence bail         
      }
      warn "WARNING: couldnt find section name $sect\n" unless ($seen);
   }
   
   # 'regular expression' sections passed in via -e (may be multiple matches)
   foreach my $sect (@inp_rom_re_sects) {
      my $seen = 0;
      foreach my $log_grp (values %{$output_sects}) {
         next unless ($log_grp->{'name'} =~ m/^$sect/);
         push @rom_sections, $log_grp;
         $seen = 1;         
      }
      warn "WARNING: couldnt find section name matching expression $sect\n" unless ($seen);
   }
   
}


#=============================================================================
# GET_ROM_SYMBOLS
# Here we get a list of symbols that went into ROM.
# We then traverse the symbol table to get their addresses
#=============================================================================
sub get_rom_symbols
{
   my @rom_sects;             # all required ROM sect-names with lo & hi addrs

   #
   # construct array of sections with their lo & hi range
   # we'll use this to later match symbols within that section range
   #
   foreach my $sect (@rom_sections) {
      my $rec;    # must re-initialize as new record each time
      
      $rec->{run_addr} = str_to_num($sect->{'run_address'});
      my $size = str_to_num($sect->{'size'});
      $rec->{hi_range} = $rec->{run_addr} + $size;
      push @rom_sects, $rec;
   }
   
   # Go thru symbol table and grab list of symbols & addresses within that range
   foreach my $sym_name (keys %{$sym_tbl}) {
      my $rec;    # must re-initialize as new record each time
      
      $rec->{sym_addr} = $sym_tbl->{$sym_name}->{'value'};

      # only store symbols w/ addresses in ROM section memory range
      foreach my $sect (@rom_sects) {
         next unless ((str_to_num($rec->{sym_addr}) >= $sect->{run_addr}) &&
                      (str_to_num($rec->{sym_addr}) < $sect->{hi_range}));
         
         $rec->{sym_name} = $sym_name;
         
         # store ROM sects name & address in array for later printing
         push @rom_syms, $rec;
      }
   }

}

#=============================================================================
# GET_ROM_ABS_SYMBOLS
# Given a list of absolute symbols we find out their absolute value and add
# them to our ROM symbols list .
#=============================================================================
sub get_rom_abs_symbols
{
   my (@abs_sym_list) = @{$_[0]};
   
   foreach my $sym (@abs_sym_list)
   {
      # skip if non-existent symbol-name passed in
      unless ($sym_tbl->{$sym}) {
         warn "WARNING: couldnt find symbol $sym\n";
         next;                        
      }
      
      my $rec;    # must re-initialize as new record each time
      $rec->{sym_name} = $sym;
      $rec->{sym_addr} = $sym_tbl->{$sym}->{'value'};

      # store abs symbol name & value in array for later printing
      push @rom_syms, $rec;
   }

}


#=============================================================================
# DUMP_ROM_SYMS - prints out format of...
#   	.global _memset
#   _memset	.set	0x3e600
# i.e. for every ROM'ed symbol we get a separate .asm file
# This is where we open A LOT of files
# You probably want to run this script from an empty directory
#=============================================================================
sub dump_rom_syms
{
   my ($rom_fname) = @_;
   
   # Change any non-alphanumeric characters in section name to '_'
   $rom_fname =~ s/[^A-Za-z0-9]/_/g;
   
   my $ct = 0;
   my %syms;
   foreach my $rec (@rom_syms) {
      
      # skip file creation if we've aleady seen this symbol
      if (!($syms{$rec->{sym_name}})) {

         # open up a new file for each single symbol asm file
         my $asm_fname = $rom_fname . "_" . $rec->{sym_name} . "_tmp.asm";
         open (ASMFILE, ">$asm_fname") || die "Cannot open output file: $!";
         
         print ASMFILE ("; This file is *auto-generated*. Do not edit. Mods risk being overwritten \n");
         print ASMFILE ("; Assembly file : " . $asm_fname . "\n\n");
         
         print ASMFILE "\t .global\t" . $rec->{sym_name} . "\n";
         print ASMFILE ($rec->{sym_name} . "\t .set\t" . $rec->{sym_addr} . "\n");
         close ASMFILE;
         $ct++;
         
         # store the symname in a hash
         $syms{$rec->{sym_name}} = 1;
      }

   }
   print "$0 Done. Created $ct asm files for $rom_filename\n";

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

