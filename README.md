# rdiff-backup-fs

> **CAUTION:** the code in this repository has been imported 1-to-1 from
the archive at https://code.google.com/archive/p/rdiff-backup-fs and is
not guaranteed to work in any way or form, and can be dangerous to use.

## WE'RE LOOKING FOR A CO-MAINTAINER!

Check the rdiff-backup-users mailing list
mailto:rdiff-backup-users@nongnu.org, join at
https://lists.nongnu.org/mailman/listinfo/rdiff-backup-users

## Description

Anyone who at least once lost his project knows – I personally do - how
important it is to create backups. Rdiff-backup 
(https://rdiff-backup.net/) allows user to incrementally
backup one directory into another. Increments make disk usage relatively
low, as only differences between two consecutive backups are stored. 
This has its drawbacks, however. Although user can access files from
most recent backup easily, simply by entering the backup directory,
it is much harder to access and browse through older versions.
Rdiff-backup allows to access them only by set of instructions,
that require precise knowledge of file names and backup times. To
investigate creation process of certain project one has to invest a lot
of time and energy. This reduces rdiff-backup directory to simple copy 
of a project rather than repository of all its versions.

Possible solution is to create userspace filesystem based on
rdiff-backup repository data. As these information are well and
explicitly stored, they can be easily accessed by the filesystem
and presented to user in clear and friendly manner. Instead of
doing a lot typing and bookkeeping, user will be able to freely browse
through any revision of the backed up directory.

The easiest – in my opinion – way to create one, is to use FUSE
(Filesystem in userspace - http://fuse.sourceforge.net/) library that
allows the developer to focus on high level aspects of the filesystem
(e.g. Accessing rdiff-back information and files), while it does all
the necessary low level work itself. Since using FUSE does not require
root privileges it can be used by any user to create personal
filesystem on demand.

As soon as filesystem is correctly mounted, it will provide access to
snapshot of the repository for every increment that had been done.
Directory structure will be the same as the backed up directory at the
time of the increment. Any file added to the repository at the will be
accessible, yet only the accessed files will be replicated, so disk
space usage stays low.

Idea for this filesystem was first presented by Lukasz Sznuk from
University of Warsaw as one of the projects for course in Operating
Systems.

For usage information check man rdiff-backup-fs.
