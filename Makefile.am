bin_PROGRAMS=rdiff-backup-fs

LDADD = layout/liblayout.a structure/libstructure.a retriever/libretriever.a support/libsupport.a

rdiff_backup_fs_SOURCES=rdiff-backup-fs.c errors.c parse.c initialize.c operations.c

SUBDIRS = layout structure retriever support

man1_MANS = rdiff-backup-fs.man
