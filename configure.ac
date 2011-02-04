AC_INIT(rdiff-backup-fs, 1.0.0b3)

AC_CONFIG_HEADER(config.h)

AC_PROG_CC
dnl checks for programs

dnl checks for libraries

AC_CHECK_LIB(fuse, fuse_main, FUSE_LIBS="-lfuse", AC_MSG_ERROR(No fuse library!))

dnl checks for header files

AC_CHECK_HEADER(zlib.h,,[AC_MSG_ERROR("No zlib.h header!")])
AC_CHECK_HEADER(stdio.h)
AC_CHECK_HEADER(errno.h)
AC_CHECK_HEADER(stdarg.h)
AC_CHECK_HEADER(dirent.h)
AC_CHECK_HEADER(sys/wait.h)
AC_CHECK_HEADER(pthread.h)

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

if test -z $TMP_DIR
then
	AC_DEFINE([DEFAULT_TMP_DIR], ["/tmp"], 
              [Define default temporary directory])
else
    AC_DEFINE_UNQUOTED([DEFAULT_TMP_DIR], [${TMP_DIR}], 
                       [Define temporary directory from evnironment variable])
fi

AC_OUTPUT
