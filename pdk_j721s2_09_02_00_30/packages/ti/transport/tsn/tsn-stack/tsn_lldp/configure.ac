#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT([xl4lldp], [1.1.2], [shiro@excelfore.com])
AC_SUBST(PACKAGE_DESC,"Excelfore LLDP Library")

AM_INIT_AUTOMAKE([-Wall -Werror subdir-objects])
AM_PROG_AR
LT_PREREQ([2.2])
LT_INIT([static])
AC_CONFIG_SRCDIR([lldpd.h])

# pkg-config dependency
PKG_PROG_PKG_CONFIG
PKG_INSTALLDIR

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AM_PROG_CC_C_O

# Check if posix platform
AC_CHECK_HEADERS([pthread.h unistd.h],
	[AM_CONDITIONAL(POSIX_PLATFORM,[true])])


# Check for command line options
AC_ARG_ENABLE([understpl],
	AS_HELP_STRING([--enable-understpl],[enable understpl mode build]))

# Checks for libraries.
PKG_CHECK_MODULES([x4unibase],[tsn_unibase],,
	[AC_CHECK_HEADERS([tsn_unibase/unibase.h])]
)
PKG_CHECK_MODULES([x4combase],[tsn_combase],,
	[AC_CHECK_HEADERS([tsn_combase/combase.h])]
)
PKG_CHECK_MODULES([x4gptp],[xl4gptp],,
	[AC_CHECK_HEADERS([xl4tsn/gptp2/gptpbasetypes.h])]
)

# Checks for header files.
AC_CHECK_HEADERS([stdio.h stdlib.h string.h])

# conditional build
AM_CONDITIONAL(BUILD_IN_STPL, [test x"$enable_understpl" = "xyes"])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_UINT8_T

# Checks for library functions.
AC_CHECK_FUNC([malloc], [], [AC_MSG_ERROR([malloc() function not found.])])
AC_CHECK_FUNC([realloc], [], [AC_MSG_ERROR([realloc() function not found.])])
AC_CHECK_FUNC([memset], [], [AC_MSG_ERROR([memset() function not found.])])
AC_CHECK_FUNC([memcpy], [], [AC_MSG_ERROR([memcpy() function not found.])])
AC_CHECK_FUNC([random], [], [AC_MSG_ERROR([random() function not found.])])
AC_CHECK_FUNC([pipe], [], [AC_MSG_ERROR([pipe() function not found.])])

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_FILES([
	Makefile
	xl4lldp.pc])

AC_OUTPUT
