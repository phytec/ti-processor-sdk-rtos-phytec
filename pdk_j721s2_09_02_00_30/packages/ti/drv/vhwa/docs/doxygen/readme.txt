Steps to add New module for Doxygen API guide generation
1. At "apiguide\content_pages" create a configuration file for modules and add the interface header file path for which API document should be generated. Take reference from "udma.cfg"
2. Open "apiguide\doxyfile" and add newly created '.cfg' as INPUT. You can search for 'udma.cfg' and add your cfg in similar way
3. In one of the interface file define a top level group for module for example
	/**
	 *  \defgroup DRV_UDMA_MODULE UDMA Driver
	 *
	 *  @{
	 */
	/* @} */

	Group thr API guide content accordingly
4.  Add the module in 'api_guide\content_pages\pdk_top.h'

Building the API guide
1. Open 'makefile' and make sure DOXYGEN path is correct you need to edit "export DOXYGEN ?=" if required
2. MAke sure 'PDK_INSTALL_PATH' in "packages\ti\build\Rules.make" correct.
3. use 'make api_guide' to build api guide from 'internal_docs\doxygen' location
4. API guide will be generated at "docs\api_guide"
