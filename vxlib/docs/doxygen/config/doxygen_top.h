/* ======================================================================== *
 * VXLIB -- TI Vision Library                                               *
 *                                                                          *
 *   Doxygen generation header file                                         *
 *                                                                          *
 * Copyright (C) 2016- 2017 Texas Instruments Incorporated                  *
 *                          http://www.ti.com/                              *
 *                                                                          *
 *                                                                          *
 *  Redistribution and use in source and binary forms, with or without      *
 *  modification, are permitted provided that the following conditions      *
 *  are met:                                                                *
 *                                                                          *
 *    Redistributions of source code must retain the above copyright        *
 *    notice, this list of conditions and the following disclaimer.         *
 *                                                                          *
 *    Redistributions in binary form must reproduce the above copyright     *
 *    notice, this list of conditions and the following disclaimer in the   *
 *    documentation and/or other materials provided with the                *
 *    distribution.                                                         *
 *                                                                          *
 *    Neither the name of Texas Instruments Incorporated nor the names of   *
 *    its contributors may be used to endorse or promote products derived   *
 *    from this software without specific prior written permission.         *
 *                                                                          *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS     *
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT       *
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR   *
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT    *
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   *
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT        *
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,   *
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY   *
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT     *
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE   *
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.    *
 * ======================================================================== */

/**
 * @mainpage  @PACKAGE User's Manual (@TARGETN)
 *
 *  @HLINE
 *
 *  <div>
 *    <p>&nbsp;</p>
 *    @section start Getting Started
 *    <table @TSTYLE>
 *      <tr>
 *        <td><a @ASTYLE href="intro.html">Introduction</a></td>
 *        <td>@PACKAGE features and advantages</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE href="install.html">Package Contents</a></td>
 *        <td>What the installation provides and where</td>
 *      </tr>
 * @if src
 *      <tr>
 *        <td><a @ASTYLE href="building.html">Building</a></td>
 *        <td>How to build @PACKAGE using CCS Projects or GNU make</td>
 *      </tr>
 * @endif
 *      <tr>
 *        <td><a @ASTYLE href="integrate.html">Integration</a></td>
 *        <td>How to integrate @PACKAGE into your code</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE href="examples.html">Examples</a></td>
 *        <td>Example projects provided with @PACKAGE </td>
 *      </tr>
 *      <tr>
 *        <td @CSTYLE><a @ASTYLE target="_blank" href="@PKGMANLINK">API Reference</a></td>
 *        <td>Detailed usage for all @PACKAGE kernels</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE href="bamplugins.html">BAM Plugin Support</a></td>
 *        <td>List of kernels which have BAM plugins supported</td>
 *      </tr>
 *    </table>
 *  </div>
 *
 *
 *  <div>
 *    <p>&nbsp;</p>
 *    @section dox Additional Documents
 *    <table @TSTYLE>
 *      <tr>
 *        <td @CSTYLE><a @ASTYLE @BLANK href="../../../@PKGRELNOTES">Release Notes</a></td>
 *        <td>New features, device support, known issues, etc.</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE @BLANK href="../../@PKGSWMFEST">Software Manifest</a></td>
 *        <td>Link to manifests for all packages in delivery</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE @BLANK href="../../@PKGTESTREP">Cycle Benchmarks</a></td>
 *        <td>Cycle and memory benchmarking</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE @BLANK href="../../@PKGCOMPREP">Standard Compliance</a></td>
 *        <td>MISRA-C Compliance</td>
 *      </tr>
 *    </table>
 *  </div>
 *
 *
 *  <div>
 *    <p>&nbsp;</p>
 *    @section linx Helpful Links
 *    <table @TSTYLE>
 *      <tr>
 *        <td @CSTYLE><a @ASTYLE @BLANK href="https://www-a.ti.com/downloads/sds_support/TICodegenerationTools/download.htm">Code Gen Tools</a></td>
 *        <td>Download site for TI DSP code generation tools</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE @BLANK href="http://processors.wiki.ti.com/index.php/Download_CCS">Code Composer Studio</a></td>
 *        <td>Download site for Code Composer Studio IDE</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE @BLANK href="https://e2e.ti.com/support/arm/automotive_processors/f/1021">TI E2E Support Forum</a></td>
 *        <td>Forum for @PACKAGE questions or remarks</td>
 *      </tr>
 *      <tr>
 *        <td><a @ASTYLE @BLANK href="http://processors.wiki.ti.com/index.php/Software_libraries">Library Wiki</a></td>
 *        <td>Find and download the latest @PACKAGE release</td>
 *      </tr>
 *    </table>
 *  </div>
 *
 *  @SPACER
 */


/**
 * @page intro  Introduction
 *
 * <a @NSTYLE href="index.html">Back</a>
 *
 * @SPACER
 *
 * The Texas Instruments @PACKAGE is an optimized Vision Processing
 * Functions Library for C programmers using @TARGETX devices.
 * It includes many C-callable, assembly-optimized, general-purpose vision
 * processing routines. These routines are typically used in computationally
 * intensive real-time applications where optimal execution speed is critical.
 *
 * Using these routines assures execution speeds considerably faster than
 * equivalent code written in standard ANSI C language. In addition, by
 * providing ready-to-use DSP functions, TI @PACKAGE can significantly shorten
 * vision processing application development time.
 *
 * @SPACER
 *
 * @section features  Features and Benefits
 *  @SECSTART
 *  The TI @TARGETN @PACKAGE is a software library of more than 100 royalty-free kernels
 *  from Texas Instruments that accelerates common vision processing operations. @PACKAGE
 *  is an extensible library that is optimized for the C66 DSP core and is available
 *  royalty-free.  This collection of 100+ kernels provide the ability to perform:
 *
 *  - Image filtering
 *  - Statistics extraction
 *  - Feature Detection
 *  - Tracking & Recognition
 *  - Low-level Pixel Processing
 *
 *   VXLIB provides an extensible foundation for the following applications:
 *
 *  - Video Analytics
 *  - Video Surveillance
 *  - Automotive Vision
 *  - Embedded Vision
 *  - Game Vision
 *  - Machine Vision
 *  - Consumer Electronics
 *  - Augmented Reality
 *  @SECEND
 *
 *
 */


/**
 * @page install  Package Contents
 *
 * <a @NSTYLE href="index.html">Back</a>
 *
 * @SPACER
 *
 *  Unless otherwise specified, the @PACKAGE package installs under
 *  <tt>C:\\ti\\</tt> in directory <tt> @PKGDIR</tt>.
 *  The files that comprise the @PACKAGE installation can be categorized into the
 *  following categories.
 *
 *    -# Documentation
 *    -# Component Repository
 *    -# Kernel Directories
 *    -# Library Directory
 *    -# Eclipse Support
 *    -# Internal Meta Data
 *
 *  The sections that follow provide details for each category.
 *
 * @SPACER
 *
 * @section documents Documentation
 *  @SECSTART
 *  Installation places all @PACKAGE documentation in a @c @b Docs directory in the
 *  @PACKAGE root.  The following documentation comes with the delivery:
 *
 *  - @PKGMANHTML (this document)
 *  - <a @BLANK href="../../../@PKGRELNOTES">Release Notes</a>
 *  - <a @BLANK href="../../@PKGSWMFEST">Software Manifest</a></td>
 *  - <a @BLANK href="../../@PKGTESTREP">Cycle Benchmarks</a>
 *  @SECEND
 *
 * @section kerneldir  Kernel Directories
 *  @SECSTART
 *  The installation creates a @c @b Packages directory in the @PACKAGE root
 *  folder.  This directory holds all kernels contained within the @PACKAGE
 *  library.  This directory follows the standard TI directory structure.  As an
 *  example, the files that comprise the @PACKAGE kernel @c @b @KERNELEX will be
 *  located, relative to the @PACKAGE root installation directory, at
 *  <tt>packages/ti/@package/src/vx/@KERNELEX/</tt>.
 *
 *  Each Kernel is delivered with a small test application that illustrates
 *  the kernel API and performs several validation tests.  Each application provides
 *  an estimate of kernel cycle and program memory requirements.
 *
 *  Some selected kernels also contain a bam_plugin folder which contains the
 *  files necessary to plugin the respective kernel into the BAM block based processing
 *  framework.
 *
 *  Follow @ref ccs_compile_knl for building & testing kernel projects.
 *  @SECEND
 *
 * @section libdir  Library Directory
 *  @SECSTART
 *  The installation also creates a @c @b lib directory in the top
 *  folder with subpaths /<PLATFORM>/<CPU>/NO_OS/<release/debug>.  This directory holds the following:
 *   - @c @b vxlib_<CPU>.lib : Main optimized @package library
 *  In addition to to default optimized library, the following libraries can also be generated
 *  from the makefile:
 *   - @c @b vxlib_cn_<CPU>.lib : C Natural Implementation of @package library
 *   - @c @b vxlib_bamplugin_<CPU>.lib : Library of bam plugin interfaces to the VXLIB functions.
 *   - @c @b common_<CPU>.lib : Common functions required for testing kernels.
 *
 *  Supported Platforms and Cpus:
 *   - @b PLATFORM: PC, J721E, J721S2, J784S4, AM62A, J722S
 *   - @b CPU: x86_64, C66, C7120 C7504 C7524
 *
 */


/**
 * @page examples  Examples
 *
 * <a @NSTYLE href="index.html">Back</a>
 *
 * @SPACER
 *
 * @section kernelexamples  Stand-alone kernel examples/tests
 *  @SECSTART
 *  Each kernel directory has a stand-alone application that can be compiled and run
 *  on a bare-metal DSP (no OS or drivers).  The main test file is the *_d.c file.
 *  This file follows a common format for each kernel:
 *  - Load all the test vectors from the *_idat.c file of the same directory.
 *  - For each test case:
 *    - Allocate memory for each buffer based on the parameters specified from the test vector
 *    - Initialize buffers with data as specified from the test vector (random, static, etc.)
 *    - Initialize function parameters
 *    - Call the optimized version of the kernel from the c66 folder of the same directory
 *    - Call the natural c version of the kernel from the *_cn.c file of the same directory
 *    - Compare the results of these two versions to make sure they match
 *    - Optionally compare the results against a static expected output_iterator
 *    - Print test results, including performance
 *    - Free buffers
 *  - When all the test vectors are completed, print summary of performance and memory usage.
 *  @SECEND
 *
 * @section regressionexample  Full regression test
 *  @SECSTART
 *  The packages/ti/vxlib.main.c  contains
 *    - Regression application (Calls each of the kernel tests one after the other in a single executable)
 *  @SECEND
 *
 */


/**
 * @page integrate  Integration
 *
 * <a @NSTYLE href="index.html">Back</a>
 *
 * @SPACER
 *
 *  Since @PACKAGE is a collection of individual kernels, any combination of the
 *  kernels that comprise @PACKAGE may be integrated into a system individually.
 *  Integration, for single or multiple kernels, requires simple steps below:
 *
 *    -# @ref compile
 *    -# @ref kernelcalls
 *    -# @ref syscompile
 *    -# @ref linking
 *
 *  The sections that follow provide details regarding the above steps.
 *
 * @SPACER
 *
 *  @if src
 *  @section compile  Compile @PACKAGE
 *  @SECSTART
 *  On installation, the @PACKAGE optimized library is built and ready to link.  Therefore,
 *  this step is only required when the original kernel source code has been refined, standalong applications
 *  need to built, or contributions have been added.  See @ref building for details regarding building @PACKAGE.
 *  @SECEND
 *  @endif
 *
 *  @section kernelcalls  Add @PACKAGE API Calls
 *  @SECSTART
 *  Add calls to @PACKAGE kernels within the system source code as necessary.
 *  Any system source file that contains calls to an @PACKAGE kernel will require
 *  that the @PACKAGE header file <tt> @PKGHEADER </tt> is included.  See the
 *  <a href="@PKGMANLINK">@PACKAGE Function Reference</a> for details on individual
 *  kernel APIs.
 *  @SECEND
 *
 *  @section syscompile  System Compilation
 *  @SECSTART
 *  @par Include Paths
 *  To compile the system code with VXLIB integrated, the following two paths should be added to the
 *  compiler's include path search list:
 *  - The path to <tt> @PKGHEADER </tt>: <tt>\<@PACKAGE root\>\\packages\\ti\\vxlib</tt> directory
 *  - The path to the <tt>\<@PACKAGE root\>\\packages</tt> directory.
 *
 *  These paths will depend on the @PACKAGE root installation directory.  This allows the main
 *  @PACKAGE header file to be moved from within the installation directory and
 *  still recognize the individual kernel headers.
 *  @par Preprocessor Defines
 *  (Optional) When compiling the final executable as a PC target (X86 or x86_64), the
 *  following preprocessor definitions should be added to the compiler's preprocessor defines
 *  list:
 *  - _HOST_BUILD
 *  - _TMS320C6600
 *  - TMS320C66X
 *  - CORE_DSP
 *  - HOST_EMULATION
 *  @SECEND
 *
 *  @section linking  Linking @PACKAGE
 *  @SECSTART
 *  The path to the @PACKAGE required libraries must be provided to the linker.
 *  @note When compiling for PC target (X86 or x86_64), the common library also needs to be linked along with the vxlib library due to dependencies on emulated intrinsic that are
 *  part of the common library.
 *  @SECEND
 *
 */


/**
 * @if src
 * @page building  @PACKAGE Build Process
 *
 * <a @NSTYLE href="index.html">Back</a>
 *
 * @SPACER
 *
 *  @PACKAGE is provided with concerto makefile-based compilation/link (build).
 *
 *  The following can be built using the make command (along with specific targets).
 *
 *  - <b>Build libraries</b> : build vxlib, vxlib_cn, vxlib_bamplugin, and common libraries.
 *  - <b>Build Individual Kernel Examples</b> : Separate executable for each kernel.
 *  - <b>Build Full Kernel Regression</b> : Single executable which includes all kernel examples run back to back.
 *
 * @SPACER
 *
 * @section gnu_howto  GNU Makefile
 *  @SECSTART
 *  A concerto based @b makefile is provided for rebuilding @PACKAGE libraries via the GNU make utility.
 *  This file is located in the @b top folder. The makefile provides
 *  a list of all tools, common build utilities and build rules required for generating
 *  @PACKAGE libraries.
 *
 *  Follow the instructions at @ref gnu_build for more details.
 *  @SECEND
 * @endif
 */

/**
 * @if src
 * @page gnu_build  GNU Make Build Process
 *
 * <a @NSTYLE href="building.html">Back</a>
 *
 * @SPACER
 *
 *  The sections below provide all necessary details to get started.
 *
 *    -# @ref gnu_tools
 *    -# @ref gnu_compile_Linux
 *
 * @SPACER
 *
 * @section gnu_tools  Required Tools
 *  @SECSTART
 *  @PACKAGE is installed as part of a larger SDK, which should include or install all the needed tools, so this section lists the dependencies
 *  that is expected to come as part of the SDK so that you have the freedom to override if you need.
 *
 *  - Linux Build Machines
 *    - <a target=_blank href="https://www.ti.com/tool/C6000-CGT">C6000 DSP Code Generation Tools Version 8.3.7</a> - For building the C66 DSP target binaries.
 *    - <a target=_blank href="https://www.ti.com/tool/C7000-CGT">C7000 DSP Code Generation Tools Version 3.0.0</a> - For building the C7120 C7504 C7524 DSP target binaries.
 *    - GNU Make - validated using v4.1
 *    - GCC - validated using v7.5.0
 *    - GCC compiler with 32b mode support is required for using VXLIB in 32bit mode (if building in 64 bit mode, you can ignore below)
 *      - Make sure below linux packages are installed. This is required to use GCC in 32b mode on 64b systems.
 *        @code
 *        sudo apt-get install g++-multilib libc6 libc6-dev
 *        @endcode
 *
 *  @note  A "My.TI" account is required for download and can be registered at <a target=_blank href="http://my.ti.com">my.ti.com</a>
 *  @SECEND
 *
 *
 * @section gnu_compile_Linux  Build Procedure (Linux Build Machine)
 *  @SECSTART
 *  The makefile may be compiled on a Linux build machine using GNU make following the procedure below.
 *
 *   -# Open a linux terminal window in the top folder of vxlib
 *   -# This installation assumes it was installed as part of a larger SDK, and so the tools_paths.mak file in the top level is pointing
 *      to another file in tiovx.  If different tools than what is given in the SDK is used, then this file (or the one it points to) can be
 *      updated accordingly.
 *   -# Set the following environment variables:
 *     - @b TARGET_CPU: (supports x86_64, C66, C7120, C7504 C7524)
 *     - @b TARGET_PLATFORM: (supports PC, J721E, J721S2, J784S4, AM62A, J722S)
 *     - If TARGET_PLATFORM is PC:
 *       - @b TARGET_SCPU: (supports C66)
 *     - @b TARGET_BUILD: (supports release, debug)
 *   -# Build the makefile (full list of targets can be found by running "make targets")
 *     - Enter <b><tt>make all</tt></b> to build all supported versions of the the binaries (libraries, regression tests, individual tests).
 *     - Enter <b><tt>make vxlib</tt></b> to build only the DSP optimized version of the vxlib library needed to link to an application.
 *     - Enter <b><tt>make vxlib_cn</tt></b> to build only the natural-c version of the vxlib library.
 *     - Enter <b><tt>make regress</tt></b> to build the full regression tests.
 *     - Enter <b><tt>make vx/(kernel name)</tt></b> to build the specific kernel unit test
 *     - Enter <b><tt>make scrub</tt></b> to clean all generated object files and artifacts.
 *  @SECEND
 *  @endif
 */

/**
 * @page bamplugins  BAM Plugins (not currently supported on J7 SOCs)
 *
 * <a @NSTYLE href="index.html">Back</a>
 *
 * @SPACER
 *
 *  Some selected VXLIB kernels also contain a bam_plugin folder which contains the
 *  files necessary to plugin the respective kernel into the BAM block based processing
 *  framework.  In order to build the bam_plugin library, DMAUTILS_INSTALL_DIR should be
 *  defined in the make system due to dependent header files.  Currently, these bam plugins
 *  are utilized to provide DMA acceleration of VXLIB kernels within the TI OpenVX (TIOVX)
 *  framework.  If VXLIB is not being used in conjuction with TIOVX, then the bam_plugins
 *  can be ignored and left unused.
 *
 *  This table lists each @PACKAGE kernel and indicates if it has BAM plugin support.  The plugin files are located in the bam_plugin folder in each kernel directory.
 *
 *  <table>
 *    <tr bgcolor="lightgrey">
 *      <th> Kernel </th>
 *      <th> BAM Plugin Support </th>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_absDiff_i16s_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_absDiff_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_accumulateImage_i8u_io16s </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_accumulateSquareImage_i8u_io16s </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_accumulateWeightedImage_i8u_io8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_add_i16s_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_add_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_add_i8u_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_add_i8u_i8u_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_addSquare_i8u_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_addWeight_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_and_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_box_3x3_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_cannyNMS_i16s_i16s_i16u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelCopy_1to1_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelCombine_2to1_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelCombine_3to1_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelCombine_4to1_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelCombine_yuyv_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelExtract_1of2_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelExtract_1of3_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_channelExtract_1of4_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_IYUVtoNV12_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_IYUVtoRGB_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_IYUVtoRGBX_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_IYUVtoYUV4_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_NVXXtoIYUV_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_NVXXtoRGB_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_NVXXtoRGBX_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_NVXXtoYUV4_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBtoIYUV_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBtoNV12_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBtoRGBX_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBtoYUV4_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBXtoIYUV_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBXtoNV12_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBXtoRGB_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_RGBXtoYUV4_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_YUVXtoIYUV_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_YUVXtoNV12_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_YUVXtoRGB_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_colorConvert_YUVXtoRGBX_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convertDepth_i16s_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convertDepth_i8u_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_3x3_i8u_c16s_o16s </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_3x3_i8u_c16s_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_5x5_i8u_c16s_o16s </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_5x5_i8u_c16s_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_7x7_i8u_c16s_o16s </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_7x7_i8u_c16s_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_i8u_c16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_convolve_i8u_c16s_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_dilate_3x3_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_dilate_MxN_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_doubleThreshold_i16u_i8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_edgeTracing_i8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_equalizeHist_i8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_erode_3x3_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_erode_MxN_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_fastCorners_i8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_fastCornersDetect_i8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_fastCornersScore_i8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_fastCornersNMS_i8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_gaussian_3x3_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_gaussian_5x5_br_i8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_gaussian_5x5_i8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_halfScaleGaussian_5x5_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_halfScaleGaussian_5x5_i8u_o8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_halfScaleGaussian_5x5_br_i8u_o8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_harrisCornersDetect_i32f </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_harrisCornersNMS_i32f </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_harrisCornersScore_i16s_i16s_o32f </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_harrisCornersScore_i32s_i32s_o32f </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_histogram_i8u_o32u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_histogramCdfLut_i32u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_histogramSimple_i8u_o32u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_integralImage_i8u_o32u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_magnitude_i16s_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_meanStdDev_i8u_o32f </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_median_3x3_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_median_MxN_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_minMaxLoc_i16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_minMaxLoc_i8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_multiply_i16s_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_multiply_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_multiply_i8u_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_multiply_i8u_i8u_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_normL1_i16s_i16s_o16u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_normL2_i16s_i16s_o16u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_not_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_or_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_phase_i16s_i16s_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_remapBilinear_bc_i8u_i32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_remapNearest_bc_i8u_i32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_scaleImageBilinear_i8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_scaleImageBilinear_bc_i8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_scaleImageBilinear_br_i8u_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_scaleImageNearest_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_scharr_3x3_i8u_o16s_o16s </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_sobelX_3x3_i8u_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_sobelY_3x3_i8u_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_sobel_3x3_i8u_o16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_sobel_5x5_i8u_o16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_sobel_7x7_i8u_o16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_sobel_7x7_i8u_o32s_o32s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_subtract_i16s_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_subtract_i8u_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_subtract_i8u_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_subtract_i8u_i8u_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_tableLookup_i16s_o16s </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_tableLookup_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_thresholdBinary_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_thresholdRange_i8u_o8u </td>
 *      <td align="center"> YES </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_trackFeaturesLK_i8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpAffineBilinear_bc_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpAffineBilinear_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpAffineNearest_bc_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpAffineNearest_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpPerspectiveBilinear_bc_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpPerspectiveBilinear_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpPerspectiveNearest_bc_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_warpPerspectiveNearest_i8u_c32f_o8u </td>
 *      <td align="center">  </td>
 *    </tr>
 *    <tr>
 *      <td> VXLIB_xor_i8u_i8u_o8u
 *      <td align="center"> YES </td>
 *    </tr>
 *  </table>
 *
 */
