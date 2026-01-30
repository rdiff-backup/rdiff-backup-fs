bin_PROGRAMS=rdiff-backup-fs

LDADD = layout/liblayout.a structure/libstructure.a retriever/libretriever.a support/libsupport.a $(FUSE_LIBS)

rdiff_backup_fs_SOURCES=rdiff-backup-fs.c errors.c errors.h parse.c parse.h \
						initialize.c initialize.h operations.c operations.h \
						headers.h externs.h constants.h

SUBDIRS = layout structure retriever support

man1_MANS = rdiff-backup-fs.man

TESTS = tests.py

check_SCRIPTS = tests.py

EXTRA_DIST = rdiff-backup-fs.man
