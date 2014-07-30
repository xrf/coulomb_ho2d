.POSIX:
VERSION=1.2.3
BASEDIR=.
DESTDIR=/usr/local
OUTDIR=$(BASEDIR)/dist
DOCDIR=$(OUTDIR)/doc
INTDIR=$(OUTDIR)/tmp
ARFLAGS=-cru
WARNFLAGS=-Wall -Wsign-conversion -pedantic
OPTFLAGS=-ffast-math -O3 -DNDEBUG
FPICFLAG=-fPIC
ALLFLAGS=$(WARNFLAGS) $(OPTFLAGS) $(FPICFLAG)
CFLAGS=$(ALLFLAGS) -std=c99
CXXFLAGS=$(ALLFLAGS) -std=c++11 # TODO switch to c++03 later on
REMOTE=git@github-xrf:xrf/coulomb_ho2d.git
LLAPACK=-llapack

# choice of implementation for alias
IMPL=openfci

# OpenFCI (dependency of the OpenFCI-based interface)
OPENFCI_VER=0.6
OPENFCI=openfci-$(OPENFCI_VER)
OPENFCI_URL=http://folk.uio.no/simenkva/openfci/$(OPENFCI).tar.gz

# LAPACK Plus Plus (dependency of OpenFCI)
LPP_VER=0.2.b
LPP_URL2=lpp-$(LPP_VER)/lpp.$(LPP_VER).tgz/download
LPP_URL=http://sourceforge.net/projects/lpp/files/lpp/$(LPP_URL2)

.PHONY: alias all check check-compilers clean doc doc-upload install shared \
        shared-am shared-openfci static static-am static-openfci uninstall

all: static shared alias

alias: $(OUTDIR)/libcoulombho2d.a $(OUTDIR)/libcoulombho2d.so

static: static-am static-openfci

static-am: $(OUTDIR)/libcoulombho2d_am.a

static-openfci: $(OUTDIR)/libcoulombho2d_openfci.a

shared: shared-am shared-openfci

shared-am: $(OUTDIR)/libcoulombho2d_am.so

shared-openfci: $(OUTDIR)/libcoulombho2d_openfci.so

install: all
	install -d $(DESTDIR)/include $(DESTDIR)/lib
	install -m644 -t $(DESTDIR)/include \
	    coulomb_ho2d.h \
	    coulomb_ho2d_compat.h
	install -m644 -t $(DESTDIR)/lib \
	    $(OUTDIR)/libcoulombho2d.a \
	    $(OUTDIR)/libcoulombho2d_am.a \
	    $(OUTDIR)/libcoulombho2d_openfci.a
	install -m755 -t $(DESTDIR)/lib \
	    $(OUTDIR)/libcoulombho2d.so \
	    $(OUTDIR)/libcoulombho2d_am.so \
	    $(OUTDIR)/libcoulombho2d_am.so.$(VERSION) \
	    $(OUTDIR)/libcoulombho2d_openfci.so \
	    $(OUTDIR)/libcoulombho2d_openfci.so.$(VERSION)

uninstall:
	rm -f \
	    $(DESTDIR)/include/coulomb_ho2d.h \
	    $(DESTDIR)/include/coulomb_ho2d_compat.h \
	    $(DESTDIR)/lib/libcoulombho2d.a \
	    $(DESTDIR)/lib/libcoulombho2d_am.a \
	    $(DESTDIR)/lib/libcoulombho2d_openfci.a \
	    $(DESTDIR)/lib/libcoulombho2d.so \
	    $(DESTDIR)/lib/libcoulombho2d_am.so \
	    $(DESTDIR)/lib/libcoulombho2d_am.so.$(VERSION) \
	    $(DESTDIR)/lib/libcoulombho2d_openfci.so \
	    $(DESTDIR)/lib/libcoulombho2d_openfci.so.$(VERSION)

check:
	cd test && $(MAKE)

check-compilers:
	cppcheck coulomb_ho2d_am.c
	gcc $(WARNFLAGS) -x c -c coulomb_ho2d_am.c
	g++ $(WARNFLAGS) -x c -c coulomb_ho2d_am.c
	g++ $(WARNFLAGS) -std=c89 -x c -c coulomb_ho2d_am.c
	g++ $(WARNFLAGS) -std=c99 -x c -c coulomb_ho2d_am.c
	g++ $(WARNFLAGS) -x c++ -c coulomb_ho2d_am.c
	g++ $(WARNFLAGS) -std=c++03 -x c++ -c coulomb_ho2d_am.c
	g++ $(WARNFLAGS) -std=c++11 -x c++ -c coulomb_ho2d_am.c
	clang $(WARNFLAGS) -x c -c coulomb_ho2d_am.c
	clang++ $(WARNFLAGS) -x c -c coulomb_ho2d_am.c
	clang++ $(WARNFLAGS) -std=c89 -x c -c coulomb_ho2d_am.c
	clang++ $(WARNFLAGS) -std=c99 -x c -c coulomb_ho2d_am.c
	clang++ $(WARNFLAGS) -x c++ -c coulomb_ho2d_am.c
	clang++ $(WARNFLAGS) -std=c++03 -x c++ -c coulomb_ho2d_am.c
	clang++ $(WARNFLAGS) -std=c++11 -x c++ -c coulomb_ho2d_am.c
	rm coulomb_ho2d_am.o

	cppcheck coulomb_ho2d_openfci.cc
	g++ $(WARNFLAGS) -x c++ -c coulomb_ho2d_openfci.cc
	g++ $(WARNFLAGS) -std=c++03 -x c++ -c coulomb_ho2d_openfci.cc
	g++ $(WARNFLAGS) -std=c++11 -x c++ -c coulomb_ho2d_openfci.cc
	clang++ $(WARNFLAGS) -x c++ -c coulomb_ho2d_openfci.cc
	clang++ $(WARNFLAGS) -std=c++03 -x c++ -c coulomb_ho2d_openfci.cc
	clang++ $(WARNFLAGS) -std=c++11 -x c++ -c coulomb_ho2d_openfci.cc
	rm coulomb_ho2d_openfci.o

	cppcheck coulomb_ho2d_compat.c
	gcc $(WARNFLAGS) -x c -c coulomb_ho2d_compat.c
	g++ $(WARNFLAGS) -x c -c coulomb_ho2d_compat.c
	g++ $(WARNFLAGS) -std=c89 -x c -c coulomb_ho2d_compat.c
	g++ $(WARNFLAGS) -std=c99 -x c -c coulomb_ho2d_compat.c
	g++ $(WARNFLAGS) -x c++ -c coulomb_ho2d_compat.c
	g++ $(WARNFLAGS) -std=c++03 -x c++ -c coulomb_ho2d_compat.c
	g++ $(WARNFLAGS) -std=c++11 -x c++ -c coulomb_ho2d_compat.c
	clang $(WARNFLAGS) -x c -c coulomb_ho2d_compat.c
	clang++ $(WARNFLAGS) -x c -c coulomb_ho2d_compat.c
	clang++ $(WARNFLAGS) -std=c89 -x c -c coulomb_ho2d_compat.c
	clang++ $(WARNFLAGS) -std=c99 -x c -c coulomb_ho2d_compat.c
	clang++ $(WARNFLAGS) -x c++ -c coulomb_ho2d_compat.c
	clang++ $(WARNFLAGS) -std=c++03 -x c++ -c coulomb_ho2d_compat.c
	clang++ $(WARNFLAGS) -std=c++11 -x c++ -c coulomb_ho2d_compat.c
	rm coulomb_ho2d_compat.o

clean:
	rm -fr $(DOCDIR) $(OUTDIR) $(INTDIR)

doc:
	mkdir -p $(DOCDIR)
	doxygen

doc-upload: doc $(DOCDIR)/.git/config
	cd $(DOCDIR) \
	  && git add -A \
	  && git commit --amend -q -m Autogenerated \
	  && git push -f origin master:gh-pages

$(DOCDIR)/.git/config:
	mkdir -p $(DOCDIR)
	cd $(DOCDIR) \
	  && git init \
	  && git config user.name Bot \
	  && git config user.email "<>" \
	  && git commit -m _ --allow-empty \
	  && git remote add origin $(REMOTE)

$(OUTDIR)/libcoulombho2d.a: $(OUTDIR)/libcoulombho2d_$(IMPL).a
	cp -f $(OUTDIR)/libcoulombho2d_$(IMPL).a $@

$(OUTDIR)/libcoulombho2d.so: $(OUTDIR)/libcoulombho2d_$(IMPL).so
	ln -fs libcoulombho2d_$(IMPL).so $@

$(OUTDIR)/libcoulombho2d_am.a: \
    $(INTDIR)/coulomb_ho2d_am.o \
    $(INTDIR)/coulomb_ho2d_compat.o
	mkdir -p $(OUTDIR)
	$(AR) $(ARFLAGS) $@ \
	      $(INTDIR)/coulomb_ho2d_am.o \
	      $(INTDIR)/coulomb_ho2d_compat.o

$(OUTDIR)/libcoulombho2d_am.so: \
    $(OUTDIR)/libcoulombho2d_am.so.$(VERSION)
	ln -fs libcoulombho2d_am.so.$(VERSION) $@

$(OUTDIR)/libcoulombho2d_am.so.$(VERSION): \
    $(INTDIR)/coulomb_ho2d_am.o \
    $(INTDIR)/coulomb_ho2d_compat.o
	mkdir -p $(OUTDIR)
	$(CC) -shared -Wl,-soname,libcoulombho2d_am.so.$(VERSION) -o $@ \
	      $(INTDIR)/coulomb_ho2d_am.o \
	      $(INTDIR)/coulomb_ho2d_compat.o

$(OUTDIR)/libcoulombho2d_openfci.a: \
    $(INTDIR)/libopenfci.a \
    $(INTDIR)/coulomb_ho2d_openfci.o \
    $(INTDIR)/coulomb_ho2d_compat.o
	mkdir -p $(OUTDIR)
	mkdir -p $(INTDIR)/libopenfci
	cd $(INTDIR)/libopenfci && $(AR) -x ../libopenfci.a
	$(AR) $(ARFLAGS) $@ \
	      $(INTDIR)/libopenfci/*.o \
	      $(INTDIR)/coulomb_ho2d_openfci.o \
	      $(INTDIR)/coulomb_ho2d_compat.o

$(OUTDIR)/libcoulombho2d_openfci.so: \
    $(OUTDIR)/libcoulombho2d_openfci.so.$(VERSION)
	ln -fs libcoulombho2d_openfci.so.$(VERSION) $@

$(OUTDIR)/libcoulombho2d_openfci.so.$(VERSION): \
    $(INTDIR)/libopenfci.a \
    $(INTDIR)/coulomb_ho2d_openfci.o \
    $(INTDIR)/coulomb_ho2d_compat.o
	mkdir -p $(OUTDIR)
	mkdir -p $(INTDIR)/libopenfci
	cd $(INTDIR)/libopenfci && $(AR) -x ../libopenfci.a
	$(CC) -shared -Wl,-soname,libcoulombho2d_openfci.so.$(VERSION) -o $@ \
	      $(INTDIR)/libopenfci/*.o \
	      $(INTDIR)/coulomb_ho2d_openfci.o \
	      $(INTDIR)/coulomb_ho2d_compat.o \
	      $(LLAPACK) -lstdc++

$(INTDIR)/coulomb_ho2d_am.o: coulomb_ho2d_am.c
	mkdir -p $(INTDIR)
	$(CC) $(CFLAGS) -o $@ -c coulomb_ho2d_am.c

$(INTDIR)/coulomb_ho2d_openfci.o: \
    coulomb_ho2d_openfci.cc \
    $(INTDIR)/$(OPENFCI)/src/quantumdot/QdotInteraction.hpp
	mkdir -p $(INTDIR)
	$(CXX) $(CXXFLAGS) -o $@ \
	       -I$(INTDIR)/$(OPENFCI)/src \
	       -I$(INTDIR)/$(OPENFCI)/src/manybody \
	       -Wno-sign-conversion -c coulomb_ho2d_openfci.cc

$(INTDIR)/coulomb_ho2d_compat.o: coulomb_ho2d_compat.c
	mkdir -p $(INTDIR)
	$(CC) $(CFLAGS) -o $@ -c coulomb_ho2d_compat.c

# download LAPACK Plus Plus
$(INTDIR)/$(OPENFCI)/src/manybody/lpp/lapack.hh:
	mkdir -p $(INTDIR)/$(OPENFCI)/src/manybody
	./download $(LPP_URL) | tar -xzf - \
	    -C $(INTDIR)/$(OPENFCI)/src/manybody lpp

# download OpenFCI
$(INTDIR)/$(OPENFCI)/src/quantumdot/QdotInteraction.hpp: \
    $(INTDIR)/$(OPENFCI)/src/manybody/lpp/lapack.hh
	mkdir -p $(INTDIR)
	./download $(OPENFCI_URL) | tar -xzf - -C $(INTDIR)
	cp -f $(OPENFCI).patch $(INTDIR)/$(OPENFCI)
	cd $(INTDIR)/$(OPENFCI) && patch <$(OPENFCI).patch -lsN -p0 -r- -s

# compile OpenFCI (only the parts that are needed)
$(INTDIR)/libopenfci.a: $(INTDIR)/$(OPENFCI)/src/quantumdot/QdotInteraction.hpp
	cp -f $(OPENFCI).mk $(INTDIR)/$(OPENFCI)/src/Makefile
	cd $(INTDIR)/$(OPENFCI)/src && $(MAKE)
