How to generate the Guide using Doxygen
***************************************

Installation
============
- Install doxygen for Windows from the below link.
  http://www.stack.nl/~dimitri/doxygen/download.html#latestsrc

- In Ubuntu 16.04 and above do below,
  # sudo apt-get install doxygen

- Current CFG works with 1.8.x Doxygen version

- In case doxygen.exe does not appear in your system PATH.
  Add it manually via windows control panel.

Generating the documention
==========================
- From \perception folder do 
  # make doxy_docs

- This will generate html files at 
  \perception\docs\ptk_api_guide

- Open \perception\docs\ptk_api_guide\index.html to start using the user guide.

