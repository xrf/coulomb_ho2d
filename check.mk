.POSIX:
BASEDIR=.
OUTDIR=$(BASEDIR)/dist
INTDIR=$(OUTDIR)/tmp
CXXFLAGS=-pedantic -Wall -std=c++03 -O3 -I$(BASEDIR)
LDFLAGS=-L$(INTDIR) -L$(OUTDIR)
LIBS=-lm -lrt -ldl -lcoulombho2d

.PHONY: test

test: $(INTDIR)/test
	LD_LIBRARY_PATH=$(OUTDIR):"$$LD_LIBRARY_PATH" \
	    $(INTDIR)/test

$(INTDIR)/test: $(OUTDIR)/libcoulombho2d.a src/check.cc
	mkdir -p $(INTDIR)
	[ -f coulomb.mk ] && $(MAKE) -f coulomb.mk so-ref || :
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ src/check.cc $(LIBS)
