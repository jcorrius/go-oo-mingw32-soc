TARGET=lpsolve.uno
XNAME=XLpAlgorithm

SLVDIR=../../../../solver/680/unxlngi6.pro

LPSOLVE_PATH=../lp_solve_5.5
LPSOLVE_STATIC=$(LPSOLVE_PATH)/lpsolve55/liblpsolve55.a

DEBUG=
LDFLAGS= -lsal -lcppuhelpergcc3 -lcppu -lstlport_gcc $(DEBUG)
CXX=g++
CXXCPP=-I$(SLVDIR)/inc -I$(LPSOLVE_PATH) -I.
CXX_DEFINES=-DUNX -DGCC -DLINUX -DCPPU_ENV=gcc3 -DHAVE_GCC_VISIBILITY_FEATURE
CXX_SHARED_FLAGS=-fPIC -fno-common -export-dynamic -fvisibility=hidden

CPPFLAGS=-DDEBUG -Wall -Os -mtune=pentium3
BINDIR=$(SLVDIR)/bin

XCUFILES = ./ProtocolHandler.xcu

OBJFILES = \
	lpsolve_uno.o

all: $(TARGET).zip

lpsolve_uno.o: lpsolve_uno.cxx preprocess
	$(CXX) $(CXX_DEFINES) $(CPPFLAGS) $(CXXCPP) $(CXX_SHARED_FLAGS) -o $@ -c lpsolve_uno.cxx

preprocess: $(XNAME).idl
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(BINDIR)/idlc -I$(SLVDIR)/idl $(XNAME).idl && \
	$(BINDIR)/regmerge $(TARGET).rdb /UCR $(XNAME).urd && \
	$(BINDIR)/cppumaker -BUCR -Torg.openoffice.sc.XLpAlgorithm $(BINDIR)/types.rdb $(TARGET).rdb

$(TARGET).so: lpsolve_uno.o
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(CXX) -shared -Wl,-soname,$@ -o $@ -lc lpsolve_uno.o $(LPSOLVE_STATIC) && \
	$(BINDIR)/regcomp -register -r $(TARGET).rdb -c $@

$(TARGET).zip: $(TARGET).so
	zip -j $@ $< $(TARGET).rdb $(XCUFILES)

regview:
	$(BINDIR)/regview $(TARGET).rdb

clean:
	rm -f $(OBJFILES) $(TARGET).so $(TARGET).zip $(TARGET).rdb $(XNAME).urd && \
	rm -rf com org
