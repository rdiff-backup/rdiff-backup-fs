CC = gcc -c
LD = gcc
LDFLAGS = -lz
OBJS = archfs.o \
       errors.o \
       parse.o \
       initialize.o \
       fuse.o \
       support/gstring.o \
       support/gpath.o \
       support/gtree.o \
       support/grdiff.o \
       support/gstats.o \
       structure/core.o \
       structure/full.o \
       structure/necessary.o \
       structure/support.o \
       retriever/retriever.o \
       retriever/simple.o \
       retriever/limit.o \
       retriever/support.o \
       layout/core.o \
       layout/all.o \
       layout/versions.o

EXECUTABLE = rdiff-backup-fs
FLAGS = -Wall `pkg-config --cflags fuse` -D_GNU_SOURCE
MAIN_FLAGS = ${FLAGS} `pkg-config --libs fuse`

all: ${OBJS}
	${LD} ${MAIN_FLAGS} ${OBJS} ${LDFLAGS} -o ${EXECUTABLE}

archfs.o: archfs.c headers.h
	${CC} ${FLAGS} archfs.c

errors.o: errors.c errors.h headers.h
	${CC} ${FLAGS} errors.c 

parse.o: parse.c parse.h headers.h
	${CC} ${FLAGS} parse.c

initialize.o: initialize.c initialize.h headers.h
	${CC} ${FLAGS} initialize.c

fuse.o: fuse.c fuse.h fuse.h
	${CC} ${FLAGS} fuse.c

support/gstats.o: support/gstats.c support/gstats.h headers.h
	${CC} ${FLAGS} support/gstats.c -o support/gstats.o

support/gstring.o: support/gstring.c support/gstring.h headers.h
	${CC} ${FLAGS} support/gstring.c -o support/gstring.o

support/gpath.o: support/gpath.c support/gpath.h headers.h
	${CC} ${FLAGS} support/gpath.c -o support/gpath.o

support/gtree.o: support/gtree.c support/gtree.h headers.h
	${CC} ${FLAGS} support/gtree.c -o support/gtree.o

support/grdiff.o: support/grdiff.c support/grdiff.h headers.h
	${CC} ${FLAGS} support/grdiff.c -o support/grdiff.o

structure/core.o: structure/core.c structure/core.h headers.h
	${CC} ${FLAGS} structure/core.c -o structure/core.o

structure/full.o: structure/full.c structure/full.h headers.h
	${CC} ${FLAGS} structure/full.c -o structure/full.o

structure/necessary.o: structure/necessary.c structure/necessary.h headers.h
	${CC} ${FLAGS} structure/necessary.c -o structure/necessary.o

structure/support.o: structure/support.c structure/support.h headers.h
	${CC} ${FLAGS} structure/support.c -o structure/support.o

retriever/retriever.o: retriever/retriever.c retriever/retriever.h headers.h
	${CC} ${FLAGS} retriever/retriever.c -o retriever/retriever.o

retriever/simple.o: retriever/simple.c retriever/retriever.h headers.h
	${CC} ${FLAGS} retriever/simple.c -o retriever/simple.o

retriever/limit.o: retriever/limit.c retriever/retriever.h headers.h
	${CC} ${FLAGS} retriever/limit.c -o retriever/limit.o

retriever/support.o: retriever/support.c retriever/support.h headers.h
	${CC} ${FLAGS} retriever/support.c -o retriever/support.o

layout/core.o: layout/core.c layout/core.h headers.h
	${CC} ${FLAGS} layout/core.c -o layout/core.o

layout/all.o: layout/all.c layout/all.h headers.h
	${CC} ${FLAGS} layout/all.c -o layout/all.o

layout/versions.o: layout/versions.c layout/versions.h headers.h
	${CC} ${FLAGS} layout/versions.c -o layout/versions.o

install:
	cp ${EXECUTABLE} /usr/bin/

clean:
	- find . -name "*\.o" | xargs rm
	- rm ${EXECUTABLE}
