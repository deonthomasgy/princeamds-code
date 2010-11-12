# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(ewallset, 0.1.0.0, princeamd.elive@gmail.com)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)
AC_CANONICAL_BUILD
AC_CANONICAL_HOST

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)

AC_C_BIGENDIAN
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AC_HEADER_STDC
AC_C_CONST

PKG_CHECK_MODULES([ELEMENTARY], [
                   eina-0
                   ecore-file
		   edbus
                   elementary
])


AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
])

echo
echo
echo
echo "------------------------------------------------------------------------"
echo
echo "Now type 'make' ('gmake' on some systems) to compile $PACKAGE,"
echo "and then afterwards as root (or the user who will install this), type"
echo "'make install'. Change users with 'su' or 'sudo' appropriately."
echo
