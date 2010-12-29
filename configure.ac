AC_INIT(rdiff-backup-fs)

rm --force config.h
echo "/* System dependent options; */" >> config.h
echo "" >> config.h

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

AC_CHECK_FUNCS(get_current_dir_name getline)

if test "$ac_cv_func_get_current_dir_name" == yes -a "$ac_cv_func_getline" == yes
then
	echo "#define _GNU_SOURCE" >> config.h
	echo "" >> config.h
fi

if test -z $TMP_DIR
then
	echo '#define DEFAULT_TMP_DIR "/tmp"' >> config.h
else
	echo "#define DEFAULT_TMP_DIR ${TMP_DIR}" >> config.h
fi
echo "" >> config.h

AC_OUTPUT
