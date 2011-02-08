AC_PREREQ(2.61)
AC_INIT(rdiff-backup-fs, 1.0.0)

AC_CANONICAL_HOST
AC_PROG_RANLIB

AM_INIT_AUTOMAKE(rdiff-backup-fs, 1.0.0)
AC_CONFIG_HEADER(config.h)
AC_CONFIG_FILES([Makefile layout/Makefile structure/Makefile retriever/Makefile support/Makefile])

dnl checks for programs

AC_PROG_CC

dnl checks for libraries

AC_CHECK_LIB([z], [gzgets],,[AC_MSG_ERROR(No zlib library!)])
AC_CHECK_LIB(fuse, fuse_main, FUSE_LIBS="-lfuse", AC_MSG_ERROR(No fuse library!))

dnl checks for header files

AC_CHECK_HEADER(zlib.h,,[AC_MSG_ERROR("No zlib.h header!")])
AC_CHECK_HEADER(stdio.h)
AC_CHECK_HEADER(errno.h)
AC_CHECK_HEADER(stdarg.h)
AC_CHECK_HEADER(dirent.h)
AC_CHECK_HEADER(sys/wait.h)
AC_CHECK_HEADER(pthread.h)

dnl check for functions

AC_CHECK_FUNCS([getcwd])
AC_CHECK_FUNCS([memset])
AC_CHECK_FUNCS([mkdir])
AC_CHECK_FUNCS([rmdir])
AC_CHECK_FUNCS([strtol])
AC_CHECK_FUNCS([get_current_dir_name])
AC_CHECK_FUNCS([getline])

AC_FUNC_FORK
AC_FUNC_MKTIME
AC_PREREQ
AC_TYPE_OFF_T
AC_TYPE_SIZE_T

dnl checking type of system to provide proper compile and linking flags

case ${host} in
    *-*-linux-*)	AC_SUBST(CFLAGS, ["-Wall -O3 `pkg-config --cflags fuse`"])
			AC_SUBST(LDFLAGS, ["`pkg-config --cflags --libs fuse` -lz"]);;
    *-*-bsd-*)		AC_SUBST(CFLAGS, ["-Wall -O3 `pkg-config --cflags fuse`"])
			AC_SUBST(LDFLAGS, ["`pkg-config --cflags --libs fuse` -lz"]);;
    *-*-darwin*)	AC_SUBST(CFLAGS, ["-Wall -O3 `pkg-config --cflags fuse`"])
			AC_SUBST(LDFLAGS, ["`pkg-config --cflags --libs fuse` -lz"]);;
    *)			AC_MSG_WARN(No automatic flags for this host system; set compile/linking flags manually);;
esac

if test -z $TMP_DIR
then
	AC_DEFINE([DEFAULT_TMP_DIR], ["/tmp"], 
              [Define default temporary directory])
else
    AC_DEFINE_UNQUOTED([DEFAULT_TMP_DIR], [${TMP_DIR}], 
                       [Define temporary directory from evnironment variable])
fi

AC_OUTPUT
