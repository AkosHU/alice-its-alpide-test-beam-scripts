ROOTCONFIG   = root-config

ROOTCFLAGS   = $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  = $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     = $(shell $(ROOTCONFIG) --libs)
ROOTGLIBS    = $(shell $(ROOTCONFIG) --glibs)
ROOTLIBDIR   = $(shell $(ROOTCONFIG) --libdir)
ROOTINCDIR   = $(shell $(ROOTCONFIG) --incdir)
#ROOTLIBS     += -lMinuit2
#ROOTGLIBS    += -lMinuit2

all: trackResolutionCalculation.exe

trackResolutionCalculation.o: trackResolutionCalculation.cc
	g++ -c trackResolutionCalculation.cc -I$(ROOTINCDIR) $(ROOTCFLAGS)
trackResolutionCalculation.exe: trackResolutionCalculation.o
	g++ trackResolutionCalculation.o -o trackResolutionCalculation.exe -O3 -L$(ROOTLIBDIR) $(ROOTLIBS) $(ROOTLDFLAGS)
