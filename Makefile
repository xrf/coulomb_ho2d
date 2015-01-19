.POSIX:
.SUFFIXES:

PREFIX=/usr/local

ARFLAGS=-cru
CPPFLAGS=-include dist/tmp/config.h -Iinclude -DNDEBUG -D_XOPEN_SOURCE=500
CFLAGS=-fPIC -fvisibility=hidden -g -O3 -mtune=native\
       -Wall -Wconversion -pedantic -std=c99
libmath=-lm
libpthread=-lpthread

NUM_SHELLS=3

major=2
version=$(major).0.0

all: \
    dist/bin/clh2-am \
    dist/lib/libclh2.a \
    dist/lib/libclh2.so

clean:
	rm -fr dist

check: dist/tmp/check dist/bin/example dist/bin/tabulate dist/bin/clh2-am
	if [ -f reference.mk ]; then $(MAKE) -f reference.mk; fi
	. tools/env && \
	    dist/bin/example >/dev/null && \
	    dist/bin/tabulate >/dev/null $(NUM_SHELLS) && \
	    dist/tmp/check $(PROVIDER)

check-compilers:
	CPPFLAGS='$(CPPFLAGS)' \
	warnflags='-Wall -Wconversion -pedantic' \
	tools/compile-check src/*.c

example: dist/bin/example dist/bin/clh2-am
	. tools/env && dist/bin/example

tabulate: dist/bin/tabulate dist/bin/clh2-am
	. tools/env && dist/bin/tabulate $(NUM_SHELLS) $(PROVIDER)

doc:
	. tools/conf && doc_init dist/doc
	doxygen

doc-upload: doc
	. tools/conf && doc_upload dist/doc

install: all
	install -d \
	    $(DESTDIR)$(PREFIX)/bin \
	    $(DESTDIR)$(PREFIX)/include \
	    $(DESTDIR)$(PREFIX)/lib
	install -m644 -t $(DESTDIR)$(PREFIX)/include include/clh2.h
	install -m644 -t $(DESTDIR)$(PREFIX)/lib dist/lib/libclh2.a
	install -m755 -t $(DESTDIR)$(PREFIX)/lib \
	    dist/lib/libclh2.so.$(version)
	cp -P \
	    dist/lib/libclh2.so \
	    dist/lib/libclh2.so.$(major) \
	    $(DESTDIR)$(PREFIX)/lib

uninstall:
	rm -f \
	    $(DESTDIR)$(PREFIX)/bin/clh2-am \
	    $(DESTDIR)$(PREFIX)/include/clh2.h \
	    $(DESTDIR)$(PREFIX)/lib/libclh2.a \
	    $(DESTDIR)$(PREFIX)/lib/libclh2.so \
	    $(DESTDIR)$(PREFIX)/lib/libclh2.so.$(major) \
	    $(DESTDIR)$(PREFIX)/lib/libclh2.so.$(version)

.PHONY: all check check-compilers clean doc doc-upload \
        example tabulate install uninstall

dist/bin/clh2-am: \
    dist/tmp/clh2-am.o \
    dist/tmp/am.o \
    dist/tmp/protocol.o \
    dist/tmp/util.o
	mkdir -p dist/bin
	$(CC) -o $@ \
	    dist/tmp/clh2-am.o \
	    dist/tmp/am.o \
	    dist/tmp/protocol.o \
	    dist/tmp/util.o \
	    $(libmath) $(libpthread)

dist/bin/example: \
    src/example.c \
    include/clh2.h \
    dist/lib/libclh2.so
	mkdir -p dist/bin
	$(CC) $(CPPFLAGS) $(CFLAGS) -Ldist/lib -o $@ \
	    src/example.c -lclh2

dist/bin/tabulate: \
    src/tabulate.c \
    include/clh2.h \
    dist/lib/libclh2.so
	mkdir -p dist/bin
	$(CC) $(CPPFLAGS) $(CFLAGS) -Ldist/lib -o $@ \
	    src/tabulate.c -lclh2

dist/lib/libclh2.a: \
    dist/tmp/clh2.o \
    dist/tmp/util.o
	mkdir -p dist/lib
	$(AR) $(ARFLAGS) $@ \
	      dist/tmp/clh2.o \
	      dist/tmp/util.o

dist/lib/libclh2.so: \
    dist/lib/libclh2.so.$(major) \
    dist/lib/libclh2.so.$(version)
	ln -fs libclh2.so.$(version) $@

dist/lib/libclh2.so.$(major): dist/lib/libclh2.so.$(version)
	ln -fs libclh2.so.$(version) $@

dist/lib/libclh2.so.$(version): \
    dist/tmp/clh2.o \
    dist/tmp/util.o
	mkdir -p dist/lib
	$(CC) -shared -Wl,-soname,libclh2.so.$(major) -o $@ \
	      dist/tmp/clh2.o \
	      dist/tmp/util.o $(libpthread)

dist/tmp/check: src/check.c include/clh2.h dist/lib/libclh2.so
	mkdir -p dist/tmp
	$(CC) $(CPPFLAGS) $(CFLAGS) -Ldist/lib -o $@ src/check.c -lclh2

dist/tmp/config.h: tools/conf
	mkdir -p dist/tmp
	rm -f $@
	. tools/conf && \
	    cc() { $(CC) $(CPPFLAGS) $(CFLAGS) "$$@"; } && \
	    detect_limits >>$@ signed off_t OFF sys/types.h

dist/tmp/am.o: \
    src/am.c \
    src/am.h
	mkdir -p dist/tmp
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c src/am.c

dist/tmp/clh2.o: \
    src/clh2.c \
    src/util.h \
    src/math.inl \
    src/protocol.h \
    include/clh2.h \
    dist/tmp/config.h
	mkdir -p dist/tmp
	$(CC) $(CPPFLAGS) $(CFLAGS) -DCLH2_BUILD -o $@ -c src/clh2.c

dist/tmp/clh2-am.o: \
    src/clh2-am.c \
    src/am.h \
    src/protocol.h \
    src/util.h \
    include/clh2.h \
    dist/tmp/config.h
	mkdir -p dist/tmp
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c src/clh2-am.c

dist/tmp/protocol.o: \
    src/protocol.c \
    src/protocol.h \
    src/util.h \
    include/clh2.h \
    dist/tmp/config.h
	mkdir -p dist/tmp
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c src/protocol.c

dist/tmp/util.o: \
    src/util.c \
    src/util.h \
    src/math.inl \
    dist/tmp/config.h
	mkdir -p dist/tmp
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ -c src/util.c
