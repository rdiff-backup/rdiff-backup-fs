bin_PROGRAMS=rdiff-backup-fs

rdiff_backup_fs_SOURCES=rdiff-backup-fs.c errors.c parse.c initialize.c fuse.c
rdiff_backup_fs_SUBDIRS = layout structure retriever support
rdiff_backup_fs_LDADD = layout/liblayout.a structure/libstructure.a retriever/libretriever.a support/libsupport.a
