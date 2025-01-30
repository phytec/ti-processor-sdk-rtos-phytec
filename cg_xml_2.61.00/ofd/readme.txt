
Dir or File            Contains
-----------            --------
XML_TI_OFD.pm          Perl module that converts XML to data structures

dump_ofd.pl            Script that dumps Perl data structure built from
                          the XML file

bootimage.pl           Creates a boot image of .out - Perl version

bootimage.vb           Creates a boot image of .out - Visual Basic version
                          This file is for *example* purposes only.  It shows
                          how you might process OFD XML with languages other
                          than Perl.  It is not maintained.  Any bugs found
                          will not be fixed.

extract_sections.pl    Similar to bootimage but has per-section 
                          command line granularity. Useful for ROMing.

sectti.pl              Similar to sectti.exe that comes with CCS.  Prints
                          out load and run address, as well as totals by
                          section type.

coff_header_data.pl    Simply displays certain data from the COFF file header
                          such as endianness, exact target type (eg 64P)

hide_syms.pl           Hide symbols in a COFF .out or .obj file

coffdiff.pl            Compares two COFF files for equality

load_bps.pl            Generates load address of given symbols.  For setting
                          breakpoints at load address that are then copied to
                          the run address

global_types_gen.pl    Displays structs & their members & types in a program

lib_versions.pl        Prints out version of compiler used to build a library

list_interrupts.pl     Prints out list of functions that illegally use
                       interrupt keyword in a BIOS application

call_graph.pl          Prints call graph and stack usage

<nm>_rts_indirect.txt  Configuration files for use with call_graph.pl

func_info.pl           Produces a report listing functions, their sizes, and
                          starting addresses

tiobj2bin.bat          DOS batch file which converts a executable .out file
                          to .bin format.  Makes use of mkhex4bin.pl to create
			  a hex utility command file.  The hex utility does
			  the heavy lifting.  Note Linux users cannot run
			  this file.  A Unix shell version of this batch file
			  may be provided if requests justify it.

mkhex4bin.pl           Creates a hex command file used by tiobj2bin.bat
