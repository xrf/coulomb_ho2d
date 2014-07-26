ARFLAGS=-cru
INCLDIRS=-Imanybody -Iquantumdot
INCLUDES=--include=stdlib.h --include=climits
CXXFLAGS=$(INCLDIRS) $(INCLUDES) -fPIC -O2

all: ../../libopenfci.a

../../libopenfci.a: \
    manybody/gauss_tools.o \
    quantumdot/QdotInteraction.o \
    quantumdot/RadialPotential.o
	$(AR) $(ARFLAGS) $@ \
	      manybody/gauss_tools.o \
	      quantumdot/QdotInteraction.o \
	      quantumdot/RadialPotential.o

.SUFFIXES: .cc

.cc:
	$(CXX) $(CXXFLAGS) -o $@ -c $<
