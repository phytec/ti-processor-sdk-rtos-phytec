******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.61.00
readme.txt
June 2018
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.61.00 - 1. What's New?
2.61.00 - 2. Bugs Resolved
2.61.00 - 3. Known Issues

------------------------------------------------------------------------------
2.61.00 - 1. What's New?
------------------------------------------------------------------------------

The sole purpose of this release is to fix issue CG_XML-103.  However, this
issue prevents all the Windows executables from running.

------------------------------------------------------------------------------
2.61.00 - 1. Bugs Resolved
------------------------------------------------------------------------------
These issues are maintained in a Jira system which can only be accessed from
within Texas Instruments.  This is different from the Bugzilla system used
in older releases.

ID          Summary
--          -------
CG_XML-103  Executables do not work on Windows systems


------------------------------------------------------------------------------
2.61.00 - 3. Known Issues
------------------------------------------------------------------------------
These issues are maintained in the same Jira system.

ID          Summary
--          -------
CG_XML-102  Linux or Mac tiobj2bin : No parameter can contain a space character
CG_XML-101  global_types_gen.pl does not handle Dwarf from GCC compilers
CG_XML-25   Investigate making download size smaller
CG_XML-23   tiobj2bin fails to change ofd470 to armofd under some circumstances
CG_XML-22   objdiff.pl: Does not handle all possible TI extensions to ELF
CG_XML-21   call_graph.pl: can't handle XML files bigger than 100MB
CG_XML-20   global_types_gen.pl: Handle enumerations in addition to structs
CG_XML-19   Change all instances of ofd470 to armofd
CG_XML-18   find_dup_syms.pl: Cannot handle spaces in path
CG_XML-17   XML_TI_OFD.pm: Document all the new functions added for ELF
CG_XML-16   call_graph.pl: Does not handle static functions with the same name
CG_XML-15   need -h to show options for each cg_xml script
CG_XML-14   Call_graph.pl should allow multiple lib file to be inputed.
CG_XML-13   call_graph.pl : need graceful err handling if wrong options passed
CG_XML-11   XML_TI_OFD.pm should handle missing XML elements more gracefully
CG_XML-10   Update XML_TI_MAP.pm WRT not exposing variables, etc.
CG_XML-9    call_graph.pl: Make function prefix configurable in config files
CG_XML-8    call_graph.pl : sort call call paths by worst stack usage
CG_XML-7    global_types_gen.pl : need to support enumerated types
CG_XML-6    funcs_ranges_size.pl : differentiate RTS funcs from other funcs
CG_XML-5    cannot detect duplicates in anonymous structures
CG_XML-3    global_types.pl : trunc lines to 80 chars
CG_XML-2    global_types.pl : need to handle bit-field types


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.60.00
readme.txt
March 2018
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.60.00 - 1. What's New?
2.60.00 - 2. Bugs Resolved
2.60.00 - 3. Known Issues

------------------------------------------------------------------------------
2.60.00 - 1. What's New?
------------------------------------------------------------------------------

New option --cg_xml_version
---------------------------
Every script now supports an option named --cg_xml_version.  When used, it
causes the script to print out, on the stderr stream, the version of the
cg_xml package it comes from.  Note there is no shortcut version of this
option, like -c.  You have to completely spell it out.

XML::Simple Version
-------------------
If you only use the standalone executable forms of the scripts, you can ignore
this section.

To run the Perl scripts in the /ofd directory, the XML::Simple module in
your Perl installation must be version 2.22 or higher.  To fix issue
CG_XML-100 requires a change that does not work with older versions of
XML::Simple.

Infrastructure
--------------
Much of the infrastructure behind the package is different.  While there
is no difference in interface, it does mean things are more robust, more
secure, etc.  Perl modules are updated, the install packages are built with
another tool, the bug tracking system changed, and the like.  


------------------------------------------------------------------------------
2.60.00 - 1. Bugs Resolved
------------------------------------------------------------------------------
These issues are maintained in a Jira system which can only be accessed from
within Texas Instruments.  This is different from the Bugzilla system used
in older releases.

ID          Summary
--          -------
CG_XML-100  Using recent versions of XML::Simple lead to error: Use of
            uninitialized value in join or string
CG_XML-26   Include a version string that can be seen from each script
CG_XML-24   objdiff: Improve error handling when user puts two object files on
            cmd line
CG_XML-4    global_types_gen.pl : need to support unions
CG_XML-1    global_types_gen.pl : cannot find DIE id


------------------------------------------------------------------------------
2.60.00 - 3. Known Issues
------------------------------------------------------------------------------
These issues are maintained in the same Jira system.

ID          Summary
--          -------
CG_XML-102  Linux or Mac tiobj2bin : No parameter can contain a space character
CG_XML-101  global_types_gen.pl does not handle Dwarf from GCC compilers
CG_XML-25   Investigate making download size smaller
CG_XML-23   tiobj2bin fails to change ofd470 to armofd under some circumstances
CG_XML-22   objdiff.pl: Does not handle all possible TI extensions to ELF
CG_XML-21   call_graph.pl: can't handle XML files bigger than 100MB
CG_XML-20   global_types_gen.pl: Handle enumerations in addition to structs
CG_XML-19   Change all instances of ofd470 to armofd
CG_XML-18   find_dup_syms.pl: Cannot handle spaces in path
CG_XML-17   XML_TI_OFD.pm: Document all the new functions added for ELF
CG_XML-16   call_graph.pl: Does not handle static functions with the same name
CG_XML-15   need -h to show options for each cg_xml script
CG_XML-14   Call_graph.pl should allow multiple lib file to be inputed.
CG_XML-13   call_graph.pl : need graceful err handling if wrong options passed
CG_XML-11   XML_TI_OFD.pm should handle missing XML elements more gracefully
CG_XML-10   Update XML_TI_MAP.pm WRT not exposing variables, etc.
CG_XML-9    call_graph.pl: Make function prefix configurable in config files
CG_XML-8    call_graph.pl : sort call call paths by worst stack usage
CG_XML-7    global_types_gen.pl : need to support enumerated types
CG_XML-6    funcs_ranges_size.pl : differentiate RTS funcs from other funcs
CG_XML-5    cannot detect duplicates in anonymous structures
CG_XML-3    global_types.pl : trunc lines to 80 chars
CG_XML-2    global_types.pl : need to handle bit-field types


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.50.00
readme.txt
November 2015
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.50.00 - 1. What's New?
2.50.00 - 2. Bugs Resolved
2.50.00 - 3. Known Issues

------------------------------------------------------------------------------
2.50.00 - 1. What's New?
------------------------------------------------------------------------------
This release introduces support for Apple Mac systems.

------------------------------------------------------------------------------
2.50.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
No bugs were resolved in this release.

------------------------------------------------------------------------------
2.50.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these issues are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is not publicly available.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF
1835  find_dup_syms.pl: Cannot handle spaces in path
2073  global_types_gen.pl: Handle enumerations in addition to structs
2779  call_graph.pl: can't handle XML files bigger than 100MB
2903  objdiff.pl: Does not handle all possible TI extensions to ELF
3053  tiobj2bin fails to change ofd470 to armofd under some circumstances



******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.41.00
readme.txt
June 2015
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.41.00 - 1. What's New?
2.41.00 - 2. Bugs Resolved
2.41.00 - 3. Known Issues

------------------------------------------------------------------------------
2.41.00 - 1. What's New?
------------------------------------------------------------------------------
This releases fixes one bug in tiobj2bin. CPU architectures which do not have
a single linear address space cannot have their memory system modeled by
a binary file.  This includes the PRU and C5500 devices.  tiobj2bin is updated
to detect when a user makes this mistake and issues a diagnostic.

Note the recently released 2.40.00 introduces the Linux shell script tiobj2bin
(no file extension).  Just like the existing Windows batch file tiobj2bin.bat,
it converts a TI ELF or COFF executable into a binary file.  See the full
description of tiobj2bin.bat in the 2.20.00 release notes below.

------------------------------------------------------------------------------
2.41.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is not publicly available.

ID    Status Headline
--    ------ --------
3048  FIXED  tiobj2bin must reject PRU and C5500 input files

------------------------------------------------------------------------------
2.41.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF
1835  find_dup_syms.pl: Cannot handle spaces in path
2073  global_types_gen.pl: Handle enumerations in addition to structs
2779  call_graph.pl: can't handle XML files bigger than 100MB
2903  objdiff.pl: Does not handle all possible TI extensions to ELF

******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.40.00
readme.txt
April 2015
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.40.00 - 1. What's New?
2.40.00 - 2. Bugs Resolved
2.40.00 - 3. Known Issues

------------------------------------------------------------------------------
2.40.00 - 1. What's New?
------------------------------------------------------------------------------
This release introduces the Linux shell script tiobj2bin (no file extension).
Just like the existing Windows batch file tiobj2bin.bat, it converts a TI
ELF or COFF executable into a binary file.  See the full description of
tiobj2bin.bat in the 2.20.00 release notes below.

------------------------------------------------------------------------------
2.40.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Status Headline
--    ------ --------
2908  FIXED  tiobj2bin cannot handle MSP430 EABI files
2935  FIXED  global_types_gen: Doesn't handle duplicate structures with
             different typedef names

------------------------------------------------------------------------------
2.40.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF
1835  find_dup_syms.pl: Cannot handle spaces in path
2073  global_types_gen.pl: Handle enumerations in addition to structs
2779  call_graph.pl: can't handle XML files bigger than 100MB
2903  objdiff.pl: Does not handle all possible TI extensions to ELF


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.32.00
readme.txt
September 2013
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.32.00 - 1. What's New?
2.32.00 - 2. Bugs Resolved
2.32.00 - 3. Known Issues

------------------------------------------------------------------------------
2.32.00 - 1. What's New?
------------------------------------------------------------------------------
The call_graph script, to address a bug, has new functionality related to
"function symbol aliasing".  For all the details, please read the section of
that name in the HTML documentation.  Open the file index.htm, located in the
root directory of the cg_xml installation, with your favorite browser.  Then
click the entry for call_graph.

------------------------------------------------------------------------------
2.32.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Status Headline
--    ------ --------
2887  FIXED  call_graph.pl: Gets fooled by comipler function name aliasing
2902  FIXED  objdiff.pl does not compare TI specific section flags like
             TI_SHF_RETAIN and TI_SHF_RETAINREFS

------------------------------------------------------------------------------
2.32.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF
1835  find_dup_syms.pl: Cannot handle spaces in path
2073  global_types_gen.pl: Handle enumerations in addition to structs
2779  call_graph.pl: can't handle XML files bigger than 100MB
2903  objdiff.pl: Does not handle all possible TI extensions to ELF


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.31.00
readme.txt
April 2013
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.31.00 - 1. What's New?
2.31.00 - 2. Bugs Resolved
2.31.00 - 3. Known Issues

------------------------------------------------------------------------------
2.31.00 - 1. What's New?
------------------------------------------------------------------------------

tiobj2bin.bat Updated
---------------------
The batch file tiobj2bin.bat converts a TI executable object file into a
simple binary format.  Details are below in the 2.20.00 readme section.

The ARM compiler changed executable names.  What was ofd470.exe is now
armofd.exe, and what was hex470.exe is now armhex.exe.  These executables
are used (actually passed in as command line parameters) in tiobj2bin.bat.
The script checks to see which set of executables is available, and 
automatically uses the right one.  Thus, ofd470.exe may get changed to
armofd.exe, or vice versa.  

This change is transparent to users.  The interface to tiobj2bin.bat is
exactly the same.  

------------------------------------------------------------------------------
2.31.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
No bugs were resolved since the last release.

------------------------------------------------------------------------------
2.31.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF
1835  find_dup_syms.pl: Cannot handle spaces in path
2073  global_types_gen.pl: Handle enumerations in addition to structs
2779  call_graph.pl: can't handle XML files bigger than 100MB


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.30.00
readme.txt
June 2011
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.30.00 - 1. What's New?
2.30.00 - 2. Bugs Resolved
2.30.00 - 3. Known Issues

------------------------------------------------------------------------------
2.30.00 - 1. What's New?
------------------------------------------------------------------------------

BSD License
-----------
The cg_xml package is now released under the BSD license.  For details see
the software manifest in cg_xml_2_30_00_Manifest.pdf.  In addition, the BSD
license is contained in each of the Perl source files.

Control How Standalone Executables use Temporary Directories and Files
----------------------------------------------------------------------
The file getting_started.txt contains the details about how the standalone
executable forms of the scripts make use of temporary directories and files.
A section was added which describes how you can control where those temporary
items are placed, and whether they are deleted upon script completion.

------------------------------------------------------------------------------
2.30.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Status Headline
--    ------ --------
2156  FIXED  Document temp file settings for standalone executables
2158  FIXED  call_graph.pl: Add support for MSP430
2252  FIXED  tiobj2bin.bat handles C2000 files incorrectly
2256  FIXED  objdiff.pl: For ELF avoid comparing sections not directly
             relevant to target

------------------------------------------------------------------------------
2.30.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF
1835  find_dup_syms.pl: Cannot handle spaces in path
2073  global_types_gen.pl: Handle enumerations in addition to structs



******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.20.00
readme.txt
November 2009
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.20.00 - 1. What's New?
2.20.00 - 2. Bugs Resolved
2.20.00 - 3. Known Issues

------------------------------------------------------------------------------
2.20.00 - 1. What's New?
------------------------------------------------------------------------------

Convert TI Executable to .bin Format
------------------------------------
The Windows batch file tiobj2bin.bat converts a TI executable file, COFF
or ELF, to a "binary" file.  This binary format is the one defined by the
"objcopy" Unix command.  Here is a description of the format from the
objcopy man page ...

       When objcopy generates a raw binary file, it will essentially produce
       a memory dump of the  contents of  the input object file.  All symbols
       and relocation information will be discarded.  The memory dump will
       start at the load address of the lowest section copied into the output
       file.

This batch file makes use of a new script mkhex4bin.pl to create a hex
utility command file.  The hex utility does the heavy lifting.  Documentation
on how to use tiobj2bin.bat is contained in comments at the top of the
file.  To see a command invocation summary, run tiobj2bin.bat with no
parameters.

Works Better on Fedora Linux
----------------------------
Some Fedora Linux users could not run the standalone executable version of
the Perl scripts.  The build process for Linux was modified to insure those
executables will run on a wider variety of platforms, including Fedora
Linux.

------------------------------------------------------------------------------
2.20.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Status Headline
--    ------ --------
1397  FIXED  call_graph.pl should consider not requiring symbols  
1733  FIXED  sectti.pl: OFD options recommendation does not work for ELF 
1745  FIXED  call_graph help should mention not including syms in XML 
1794  FIXED  objdiff.pl: Skips over Dwarf sections even with --dwarf 

------------------------------------------------------------------------------
2.20.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.12.00
readme.txt
October 2008
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.12.00 - 1. What's New?
2.12.00 - 2. Bugs Resolved
2.12.00 - 3. Known Issues

------------------------------------------------------------------------------
2.12.00 - 1. What's New?
------------------------------------------------------------------------------

This release fixes the bugs noted below.  There are no changes to user
functionality.

------------------------------------------------------------------------------
2.12.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Status Headline
--    ------ --------
1660  FIXED  Linux file permissions are too restrictive
1663  FIXED  Update ofd6x in utils directory so it can read gcc libs

------------------------------------------------------------------------------
2.12.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1397  call_graph.pl should consider not requiring symbols  
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.11.00
readme.txt
October 2008
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.11.00 - 1. What's New?
2.11.00 - 2. Bugs Resolved
2.11.00 - 3. Known Issues

------------------------------------------------------------------------------
2.11.00 - 1. What's New?
------------------------------------------------------------------------------

Works with MV GCC
-----------------
The scripts are now tested against libraries built by the MontaVista GCC ARM
compiler.  The code in these libraries is in ELF format.  A few minor issues
were fixed.  Making these fixes available is the point of this release.  If
you do not use the MontaVista GCC ARM compiler, then you will see no
differences versus release 2.10.00.

------------------------------------------------------------------------------
2.11.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Status Headline
--    ------ --------
1658  FIXED  ofd6x in utils directory is out of date

------------------------------------------------------------------------------
2.11.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1397  call_graph.pl should consider not requiring symbols  
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF


******************************************************************************
cg_xml - Code Generation Tools XML Processing Scripts
Release Version 2.10.00
readme.txt
October 2008
******************************************************************************

------------------------------------------------------------------------------
Introduction
------------------------------------------------------------------------------
This is a package of Perl scripts used to process the XML files that come
from the TI code generation tools.  Use it to do things like build a
spreadsheet that details the size of all the sections, or figure out how much
of the memory map is taken up by specific libraries.

Details for a first time user are in the file getting_started.txt.  The rest
of this file contains release specific information.

Table of Contents
-----------------
2.10.00 - 1. What's New?
2.10.00 - 2. Bugs Resolved
2.10.00 - 3. Known Issues

------------------------------------------------------------------------------
2.10.00 - 1. What's New?
------------------------------------------------------------------------------

ELF Support
-----------
ELF is an object file format.  TI compilers recently began replacing COFF
with ELF, a process that will take years.  At this writing, the only the ARM
(TMS470) compiler supports ELF, and even then only if you build with
--abi=eabi.  

ELF support in cg_xml was introduced in release 2.00.00 ALPHA.  This is the
first wide release of ELF support.  In general, the scripts auto-detect the
object file format.  Script by script details regarding ELF can be found in
section 2.00.00 - 2 below.  

If you do not use ELF, then you will see no changes.  Other than this ...

objdiff.pl Replaces coffdiff.pl
-------------------------------
The script coffdiff.pl has been replaced by a new script objdiff.pl.  Even
though coffdiff.pl is still in the package, please discontinue using it in
favor of objdiff.pl.  Future releases of cg_xml will not include coffdiff.pl.

There are two important improvements over coffdiff.pl: ELF support and
comparing build attributes.

ELF support was not added to coffdiff.pl.  Instead, the new script objdiff.pl
can compare files of either format.  

Build attributes are an encoding, in the object format, of the options used
to build the file.  It includes things like the memory model, endianness,
and so on.  Only the latest TI compilers include build attributes in the
object file.  Support for comparing build attributes was not added to 
coffdiff.pl.  Like all such improvements, it was added to objdiff.pl instead.

------------------------------------------------------------------------------
2.10.00 - 2. Bugs Resolved
------------------------------------------------------------------------------
Details on these bugs are in the SDO applications team bugzilla database
located at http://bugzilla.sanb.design.ti.com/bugzilla/index.cgi .  Note this
web link is only available from the TI intranet.  

ID    Status Headline
--    ------ --------
 938  FIXED  coffdiff.pl: comparison of reloc entries and line nums is lacking
1495  FIXED  coffdiff.pl: Compare build attributes
1616  FIXED  hide_syms.pl: Detect library as input and fail gracefully
1620  FIXED  Handle multiple .debug_info sections in COFF 

------------------------------------------------------------------------------
2.10.00 - 3. Known Issues
------------------------------------------------------------------------------
Details on these bugs are in the same bugzilla database.

ID    Headline
----  --------
 959  global_types.pl : need to handle bit-field types
 962  global_types.pl : trunc lines to 80 chars
 969  global_types.pl : need to support unions
1104  cannot detect duplicates in anonymous structures
1242  funcs_ranges_size.pl : differentiate RTS funcs from other funcs
1271  call_graph.pl: sort call paths by worst stack usage
1294  global_types_gen.pl: need to support enumerated types
1315  call_graph.pl: Make function prefix configurable in configuration files
1319  Update XML_TI_MAP.pm WRT not exposing variables, etc.
1397  call_graph.pl should consider not requiring symbols  
1398  XML_TI_OFD.pm should handle missing XML elements more gracefully  
1403  coffdiff.pl: Section .xref should be ignored by default  
1425  call_graph.pl : need graceful err handling if wrong options passed  
1436  call_graph.pl : Should allow multiple lib files to be input
1505  Need -h to show options for each cg_xml script
1575  call_graph.pl: Does not handle static functions with the same name.
1650  XML_TI_OFD.pm: Document all the new functions added for ELF
