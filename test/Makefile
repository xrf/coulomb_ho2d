BASEDIR=..
OUTDIR=$(BASEDIR)/dist
INTDIR=$(OUTDIR)/tmp
CXXFLAGS=-pedantic -Wall -std=c++11 -ffast-math -O3 -I$(BASEDIR)
LLAPACK=-llapack                        # only needed for OpenFCI version
LDFLAGS=-L$(INTDIR) -L$(OUTDIR)
LIBS=-lm -lrt -ldl -lcoulombho2d $(LLAPACK)

test: $(INTDIR)/test
	LD_LIBRARY_PATH=../dist:"$$LD_LIBRARY_PATH" \
	    $(INTDIR)/test | tee $(INTDIR)/test.log

$(INTDIR)/test: $(OUTDIR)/libcoulombho2d.a coulomb_ho2d.cpp
	mkdir -p $(INTDIR)
	[ -f ../coulomb.mk ] && cd .. && $(MAKE) -f coulomb.mk so-ref || :
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ coulomb_ho2d.cpp $(LIBS)

.PHONY: test
