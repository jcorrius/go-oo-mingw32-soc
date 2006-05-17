TARGET=lpsolve.uno
XNAME=XLpAlgorithm

APPDIR=/opt/openoffice.org2.0/program
BASEDIR=../../..

SLVDIR=../../../../solver/680/unxlngi6.pro
IDLDIR=$(BASEDIR)/idl/org/openoffice/sc/solver

LPSOLVE_PATH=../lp_solve_5.5
LPSOLVE_STATIC=$(LPSOLVE_PATH)/lpsolve55/liblpsolve55.a

DEBUG=-DDEBUG
LDFLAGS= -lsal -lcppuhelpergcc3 -lcppu -lstlport_gcc
CXX=g++
CXXCPP=-I$(SLVDIR)/inc -I$(LPSOLVE_PATH) -I.
CXX_DEFINES=$(DEBUG) -DUNX -DGCC -DLINUX -DCPPU_ENV=gcc3 -DHAVE_GCC_VISIBILITY_FEATURE
CXX_SHARED_FLAGS=-fPIC -fno-common -export-dynamic -fvisibility=hidden

CPPFLAGS=-DDEBUG -Wall -Os -mtune=pentium3
BINDIR=$(SLVDIR)/bin

XCUFILES = ./ProtocolHandler.xcu

OBJFILES = \
	lpsolve_uno.o \
	testmodel.o

HEADERS = \
	testmodel.hxx \
	lpsolve_uno.hxx

IDLFILES = \
	$(IDLDIR)/Equality.idl \
	$(IDLDIR)/Goal.idl \
	$(IDLDIR)/Bound.idl \
	$(IDLDIR)/XLpAlgorithm.idl \
	$(IDLDIR)/XLpModel.idl \
	LpSolve.idl \
	TestModel.idl
	
URDFILES = \
	Equality.urd \
	Goal.urd \
	Bound.urd \
	XLpAlgorithm.urd \
	XLpModel.urd \
	LpSolve.urd \
	TestModel.urd

all: $(TARGET).zip

idlc:
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(BINDIR)/idlc -O. -I$(SLVDIR)/idl -I$(BASEDIR)/idl $(IDLFILES)

preprocess: idlc
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(BINDIR)/regmerge $(TARGET).rdb /UCR $(URDFILES) && \
	$(BINDIR)/cppumaker -BUCR -Torg.openoffice.sc.solver.XLpAlgorithm -Torg.openoffice.sc.solver.XLpModel $(BINDIR)/types.rdb $(TARGET).rdb

testmodel.o: testmodel.cxx preprocess $(HEADERS)
	$(CXX) $(CXX_DEFINES) $(CPPFLAGS) $(CXXCPP) $(CXX_SHARED_FLAGS) -o $@ -c testmodel.cxx

testmodel.uno.so: testmodel.o
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(CXX) -shared -Wl,-soname,$@ -o $@ -lc testmodel.o && \
	$(BINDIR)/regcomp -register -r $(TARGET).rdb -c $@
	
lpsolve_uno.o: lpsolve_uno.cxx preprocess $(HEADERS)
	$(CXX) $(CXX_DEFINES) $(CPPFLAGS) $(CXXCPP) $(CXX_SHARED_FLAGS) -o $@ -c lpsolve_uno.cxx

$(TARGET).so: lpsolve_uno.o
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(CXX) -shared -Wl,-soname,$@ -o $@ -lc lpsolve_uno.o $(LPSOLVE_STATIC) && \
	$(BINDIR)/regcomp -register -r $(TARGET).rdb -c $@

$(TARGET).zip: $(TARGET).so testmodel.uno.so
	zip -j $@ $(TARGET).so testmodel.uno.so $(TARGET).rdb

regview:
	$(BINDIR)/regview $(TARGET).rdb

clean:
	rm -f $(OBJFILES) $(TARGET).so testmodel.uno.so $(TARGET).zip $(TARGET).rdb \
		$(URDFILES) && \
	rm -rf com org

unopkg: $(TARGET).zip
	$(APPDIR)/unopkg add -f $<

runcalc:
	$(APPDIR)/scalc

demo: demo.cxx
	$(CXX) $(CXXCPP) -o $@ $< -llpsolve55 -L$(LPSOLVE_PATH)/lpsolve55

demo.run: demo
	export LD_LIBRARY_PATH=$(LPSOLVE_PATH)/lpsolve55 && \
	./demo
