BASEDIR=..
OUTDIR=$(BASEDIR)/dist/
INTDIR=$(OUTDIR)/tmp
CXXFLAGS=-pedantic -Wall -std=c++11 -O3 -ffast-math -I$(BASEDIR)
LDFLAGS=-L$(INTDIR) -L$(OUTDIR)
LIBS=-lm -lrt -lcoulombho2d

# Reference code that is used to verify the matrix elements.
REF=coulomb_ref.c

test: $(INTDIR)/coulomb_ho2d
	$(INTDIR)/coulomb_ho2d | tee $(INTDIR)/coulomb_ho2d.log

$(INTDIR)/coulomb_ho2d: $(OUTDIR)/libcoulombho2d.a coulomb_ho2d.cpp
	[ -f $(REF) ] 	 						  \
	  && $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(INTDIR)/coulomb_ho2d      \
	            $(REF)  coulomb_ho2d.cpp $(LIBS)  			  \
	  || $(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(INTDIR)/coulomb_ho2d      \
	            -DNOREF coulomb_ho2d.cpp $(LIBS)

.PHONY: test
