#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([tsn_uniconf], [1.2.0], [shiro@excelfore.com])
AC_SUBST(PACKAGE_DESC,"Excelfore Universal Configuration Management Library")
AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT([static])
AC_CONFIG_SRCDIR([yangs/yang_db_access.c])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

# pkg-config dependency
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Check for command line options
AC_ARG_ENABLE([unittest],
	AS_HELP_STRING([--disable-unittest],[disable building unittest programs]))
AC_ARG_ENABLE([understpl],
	AS_HELP_STRING([--enable-understpl],[enable understpl mode build]))
AC_ARG_ENABLE([1588ptp],
	AS_HELP_STRING([--enable-1588ptp],[enable 1588ptp build]),
	[enable_1588ptp=$enableval], [enable_1588ptp="yes"])
AC_ARG_ENABLE([dot1qbridge],
	AS_HELP_STRING([--enable-dot1qbridge],[enable qbridge build]),
	[enable_dot1qbridge=$enableval], [enable_dot1qbridge="yes"])
AC_ARG_ENABLE([dot1ablldp],
	AS_HELP_STRING([--enable-dot1ablldp],[enable ablldp build]),
	[enable_dot1ablldp=$enableval], [enable_dot1ablldp="yes"])
AC_ARG_ENABLE([tsnuni],
	AS_HELP_STRING([--enable-tsnuni],[enable tsnuni build]),
	[enable_tsnuni=$enableval], [enable_tsnuni="yes"])
AC_ARG_ENABLE([tsnremote],
	AS_HELP_STRING([--enable-tsnremote],[enable tsnremote build]),
	[enable_tsnremote=$enableval], [enable_tsnremote="yes"])
AC_ARG_ENABLE([ncserver],
	AS_HELP_STRING([--enable-ncserver],[enable netconf server build]),
	[enable_ncserver=$enableval], [enable_ncserver="yes"])
AC_ARG_ENABLE([testyang],
	AS_HELP_STRING([--enable-testyang],[enable excelfore-test yang build]),
	[enable_testyang=$enableval])
AC_ARG_ENABLE([dbname],
	AS_HELP_STRING([--enable-dbname],[set which db is used]),
	[enable_dbname=$enableval], [enable_dbname="simpledb"])
AC_ARG_ENABLE([ncagent],
	AS_HELP_STRING([--enable-ncagent],[enable netconf agents build]),
	[enable_ncagent=$enableval])
AC_ARG_ENABLE([staticdata],
	AS_HELP_STRING([--enable-staticdata],[enable static config data build]),
	[enable_staticdata=$enableval], [enable_staticdata="yes"])
AC_ARG_ENABLE([runconf],
	AS_HELP_STRING([--disable-runconf],[disable runtime configurations]))
AC_ARG_ENABLE([staticmem],
	AS_HELP_STRING([--enable-staticmem],[use static memory allocation]))
AC_ARG_ENABLE([reldbdebug],
	AS_HELP_STRING([--enable-reldbdebug],[debug releasedb timing]))
AC_ARG_ENABLE([ucmonthread],
	AS_HELP_STRING([--enable-ucmonthread],[run uniconfmon as a thread in unicof]))

# Check platform specific headers
AC_CHECK_HEADERS([semaphore.h],
	[AM_CONDITIONAL(PTHREAD_SEMAPHORE,[true])])

AC_CHECK_HEADERS([linux/netlink.h],
	[AM_CONDITIONAL(LINUX_NETLINK,[true])])

AC_CHECK_HEADERS([dirent.h],
	[AM_CONDITIONAL(POSIX_DIRENT,[true])])

# Checks for libraries.
PKG_CHECK_MODULES([x4unibase],[tsn_unibase],,
	[AC_CHECK_HEADERS([tsn_unibase/unibase.h])]
)
PKG_CHECK_MODULES([x4combase],[tsn_combase],,
	[AC_CHECK_HEADERS([tsn_combase/combase.h])]
)

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h string.h stdint.h stdbool.h])

if test x"$enable_unittest" != "xno"; then
AC_CHECK_HEADERS([setjmp.h cmocka.h],[],
	[AC_MSG_ERROR([you may need to install libcmocka-dev package])],
	[[#include <string.h>
	#include <stdio.h>
	#include <setjmp.h>
	]])
fi

# check rtnetlink header
AC_CHECK_HEADERS([linux/rtnetlink.h],
	[AM_CONDITIONAL(NETLINK_AVAILABLE,[true])])

# conditional build
AM_CONDITIONAL(BUILD_IN_STPL, [test x"$enable_understpl" = "xyes"])
AM_CONDITIONAL(BUILD_1588_PTP, [test x"$enable_1588ptp" = "xyes"])
AM_CONDITIONAL(BUILD_DOT1Q_BRIDGE, [test x"$enable_dot1qbridge" = "xyes"])
AM_CONDITIONAL(BUILD_DOT1AB_LLDP, [test x"$enable_dot1ablldp" = "xyes"])
AM_CONDITIONAL(BUILD_TSNUNI, [test x"$enable_tsnuni" = "xyes"])
AM_CONDITIONAL(BUILD_TSNREMOTE, [test x"$enable_tsnremote" = "xyes"])
AM_CONDITIONAL(BUILD_NCSERVER, [test x"$enable_ncserver" = "xyes"])
AM_CONDITIONAL(BUILD_TESTYANG, [test x"$enable_testyang" = "xyes"])
AM_CONDITIONAL(BUILD_STATICDATA, [test x"$enable_staticdata" = "xyes"])
AM_CONDITIONAL(BUILD_SIMPLEDB, [test x"$enable_dbname" = "xsimpledb"])
AM_CONDITIONAL(BUILD_LMDB, [test x"$enable_dbname" = "xlmdb"])
AM_CONDITIONAL(BUILD_NCPROSILSA, [test x"$enable_ncagent" = "xncprosilsa"])
AM_CONDITIONAL(BUILD_RUNCONF, [test x"$enable_runconf" != "xno"])
AM_CONDITIONAL(STATIC_MEMALLOC, [test x"$enable_staticmem" = "xyes"])
AM_CONDITIONAL(BUILD_RELDBDEBUG, [test x"$enable_reldbdebug" = "xyes"])
AM_CONDITIONAL(BUILD_UCMONTHREAD, [test x"$enable_ucmonthread" = "xyes"])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNC([malloc], [], [AC_MSG_ERROR([malloc() function not found.])])
AC_CHECK_FUNC([realloc], [], [AC_MSG_ERROR([realloc() function not found.])])
AC_CHECK_FUNC([memmove], [], [AC_MSG_ERROR([memmove() function not found.])])
AC_CHECK_FUNC([memset], [], [AC_MSG_ERROR([memset() function not found.])])
AC_CHECK_FUNC([strtol], [], [AC_MSG_ERROR([strtol() function not found.])])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([
    Makefile
    yangs/Makefile
    doc/Makefile
    tsn_uniconf.pc])

AC_OUTPUT
