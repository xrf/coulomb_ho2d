.POSIX:
BASEDIR=..
OUTDIR=$(BASEDIR)/dist
INTDIR=$(OUTDIR)/tmp
CXXFLAGS=-pedantic -Wall -std=c++03 -O3 -I$(BASEDIR)
# Lapack is only needed for OpenFCI version
LLAPACK=-llapack
LDFLAGS=-L$(INTDIR) -L$(OUTDIR)
LIBS=-lm -lrt -ldl -lcoulombho2d $(LLAPACK)

.PHONY: test

test: $(INTDIR)/test
	LD_LIBRARY_PATH=../dist:"$$LD_LIBRARY_PATH" \
	    $(INTDIR)/test

$(INTDIR)/test: $(OUTDIR)/libcoulombho2d.a coulomb_ho2d.cpp
	mkdir -p $(INTDIR)
	[ -f ../coulomb.mk ] && cd .. && $(MAKE) -f coulomb.mk so-ref || :
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ coulomb_ho2d.cpp $(LIBS)
