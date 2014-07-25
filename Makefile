BASEDIR=.
OUTDIR=$(BASEDIR)/dist/
DOCDIR=$(OUTDIR)/doc/
INTDIR=$(OUTDIR)/tmp
ARFLAGS=-cru
WARNFLAGS=-Wall -Wsign-conversion -pedantic
ALLFLAGS=$(WARNFLAGS) -ffast-math -O3 -DNDEBUG
CFLAGS=$(ALLFLAGS) -std=c99
CXXFLAGS=$(ALLFLAGS) -std=c++11 # TODO switch to c++03 later on
REMOTE=git@github-xrf:xrf/coulomb_ho2d.git

# OpenFCI (dependency of the OpenFCI-based interface)
OPENFCI_VER=0.6
OPENFCI=openfci-$(OPENFCI_VER)
OPENFCI_URL=http://folk.uio.no/simenkva/openfci/$(OPENFCI).tar.gz

# LAPACK Plus Plus (dependency of OpenFCI)
LPP_VER=0.2.b
LPP_URL2=lpp-$(LPP_VER)/lpp.$(LPP_VER).tgz/download
LPP_URL=http://sourceforge.net/projects/lpp/files/lpp/$(LPP_URL2)

lib: lib-am

all: lib-am lib-openfci test

lib-am: $(OUTDIR)/libcoulombho2d_am.a

use-am: $(OUTDIR)/libcoulombho2d_am.a
	cp -f $(OUTDIR)/libcoulombho2d_am.a $(OUTDIR)/libcoulombho2d.a

lib-openfci: $(OUTDIR)/libcoulombho2d_openfci.a

use-openfci: $(OUTDIR)/libcoulombho2d_openfci.a
	cp -f $(OUTDIR)/libcoulombho2d_openfci.a $(OUTDIR)/libcoulombho2d.a

$(OUTDIR)/libcoulombho2d_am.a: \
    $(INTDIR)/coulomb_ho2d_am.o \
    $(INTDIR)/coulomb_ho2d_compat.o
	mkdir -p $(OUTDIR)
	$(AR) $(ARFLAGS) $@ \
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

test:
	cd test && $(MAKE)

test-compilers:
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

doc: $(DOCDIR)/.git/config
	doxygen
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
	  && git commit -m Initial --allow-empty \
	  && git remote add origin $(REMOTE)

clean:
	rm -fr $(OUTDIR) $(INTDIR) $(DOCDIR)

.PHONY: clean test test-compilers
