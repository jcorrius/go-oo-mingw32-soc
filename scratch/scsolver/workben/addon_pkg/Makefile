TARGET=scsolver.uno

BASEDIR=../..
SRCDIR=$(BASEDIR)/source
SLVDIR=../../../solver/680/unxlngi6.pro
IDLDIR=$(BASEDIR)/idl/org/openoffice/sc/solver

DEBUG=
LDFLAGS= -lsal -lcppuhelpergcc3 -lcppu -lstlport_gcc $(DEBUG)
CXXCPP=-I$(SLVDIR)/inc -I$(SRCDIR)/inc -I.
CXX_DEFINES=-DUNX -DGCC -DLINUX -DCPPU_ENV=gcc3 -DHAVE_GCC_VISIBILITY_FEATURE
CXX_SHARED_FLAGS=-fPIC -fno-common -export-dynamic -fvisibility=hidden

#LIBLPSOLVE=$(SLVDIR)/lib/liblpsolve55.a
#CXX_SHARED_FLAGS=-fPIC -fno-common -export-dynamic

LIBLPSOLVE=../external/lp_solve_5.5/lpsolve55/liblpsolve55.a

CPPFLAGS=-DDEBUG -Wall -Os -mtune=pentium3

BINDIR=$(SLVDIR)/bin
OBJDIR=./obj

HEADER= \
	$(SRCDIR)/inc/tool/timer.hxx \
	$(SRCDIR)/inc/tool/global.hxx \
	$(SRCDIR)/inc/optiondlg.hxx \
	$(SRCDIR)/inc/basedlg.hxx \
	$(SRCDIR)/inc/type.hxx \
	$(SRCDIR)/inc/unoglobal.hxx \
	$(SRCDIR)/inc/xcalc.hxx \
	$(SRCDIR)/inc/option.hxx \
	$(SRCDIR)/inc/baselistener.hxx \
	$(SRCDIR)/inc/dialog.hxx \
	$(SRCDIR)/inc/lpbuilder.hxx \
	$(SRCDIR)/inc/solver.hxx \
	$(SRCDIR)/inc/listener.hxx \
	$(SRCDIR)/inc/numeric/diff.hxx \
	$(SRCDIR)/inc/numeric/lpuno.hxx \
	$(SRCDIR)/inc/numeric/type.hxx \
	$(SRCDIR)/inc/numeric/exception.hxx \
	$(SRCDIR)/inc/numeric/nlpnewton.hxx \
	$(SRCDIR)/inc/numeric/matrix.hxx \
	$(SRCDIR)/inc/numeric/funcobj.hxx \
	$(SRCDIR)/inc/numeric/lpsolve.hxx \
	$(SRCDIR)/inc/numeric/lpbase.hxx \
	$(SRCDIR)/inc/numeric/lpmodel.hxx \
	$(SRCDIR)/inc/numeric/cellfuncobj.hxx \
	$(SRCDIR)/inc/numeric/nlpmodel.hxx \
	$(SRCDIR)/inc/numeric/lpsimplex.hxx \
	$(SRCDIR)/inc/numeric/polyeqnsolver.hxx \
	$(SRCDIR)/inc/numeric/nlpbase.hxx \
	$(SRCDIR)/inc/unohelper.hxx \
	$(SRCDIR)/inc/msgdlg.hxx \
	$(SRCDIR)/inc/solvemodel.hxx \
	$(SRCDIR)/inc/nlpbuilder.hxx

OBJFILES= \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/lpbase.o \
	$(OBJDIR)/lpmodel.o \
	$(OBJDIR)/lpsimplex.o \
	$(OBJDIR)/lpsolve.o \
	$(OBJDIR)/nlpqnewton.o \
	$(OBJDIR)/nlpbase.o \
	$(OBJDIR)/nlpmodel.o \
	$(OBJDIR)/diff.o \
	$(OBJDIR)/funcobj.o \
	$(OBJDIR)/service.o \
	$(OBJDIR)/basedlg.o \
	$(OBJDIR)/baselistener.o \
	$(OBJDIR)/dialog.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/listener.o \
	$(OBJDIR)/lpbuilder.o \
	$(OBJDIR)/msgdlg.o \
	$(OBJDIR)/solvemodel.o \
	$(OBJDIR)/unoglobal.o \
	$(OBJDIR)/unohelper.o \
	$(OBJDIR)/xcalc.o	

XCUFILES = Addons.xcu ProtocolHandler.xcu

all: $(OBJFILES)

#all: $(TARGET).zip

idlc:
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(BINDIR)/idlc -O. -I$(SLVDIR)/idl -I$(BASEDIR)/idl $(IDLFILES)

preprocess: idlc
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(BINDIR)/regmerge $(TARGET).rdb /UCR $(URDFILES) && \
	$(BINDIR)/cppumaker -BUCR -Torg.openoffice.sc.solver.XLpAlgorithm -Torg.openoffice.sc.solver.XLpModel $(BINDIR)/types.rdb $(TARGET).rdb

#----------------------------------------------------------------------------
# Numeric

NUMDIR=$(SRCDIR)/numeric

$(OBJDIR)/funcobj.o: $(HEADER) $(NUMDIR)/funcobj.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/funcobj.cxx

$(OBJDIR)/matrix.o: $(HEADER) $(NUMDIR)/matrix.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/matrix.cxx

$(OBJDIR)/diff.o: $(HEADER) $(NUMDIR)/diff.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/diff.cxx

$(OBJDIR)/lpbase.o: $(HEADER) $(NUMDIR)/lpbase.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/lpbase.cxx
	
$(OBJDIR)/lpmodel.o: $(HEADER) $(NUMDIR)/lpmodel.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/lpmodel.cxx

$(OBJDIR)/lpsimplex.o: $(HEADER) $(NUMDIR)/lpsimplex.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/lpsimplex.cxx

$(OBJDIR)/lpsolve.o: preprocess $(HEADER) $(NUMDIR)/lpsolve.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/lpsolve.cxx

$(OBJDIR)/nlpmodel.o: $(HEADER) $(NUMDIR)/nlpmodel.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/nlpmodel.cxx

$(OBJDIR)/nlpbase.o: $(HEADER) $(NUMDIR)/nlpbase.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/nlpbase.cxx

$(OBJDIR)/nlpqnewton.o: $(HEADER) $(NUMDIR)/nlpqnewton.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/nlpqnewton.cxx


#----------------------------------------------------------------------------
# Service

SRVDIR=$(SRCDIR)/service

$(OBJDIR)/service.o: $(HEADER) $(SRVDIR)/service.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(SRVDIR)/service.cxx


#----------------------------------------------------------------------------
# UI

UIDIR=$(SRCDIR)/ui

$(OBJDIR)/basedlg.o: $(HEADER) $(UIDIR)/basedlg.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/basedlg.cxx

$(OBJDIR)/baselistener.o: $(HEADER) $(UIDIR)/baselistener.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/baselistener.cxx

$(OBJDIR)/dialog.o: $(HEADER) $(UIDIR)/dialog.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/dialog.cxx

$(OBJDIR)/global.o: $(HEADER) $(UIDIR)/global.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/global.cxx

$(OBJDIR)/listener.o: $(HEADER) $(UIDIR)/listener.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/listener.cxx

$(OBJDIR)/lpbuilder.o: $(HEADER) $(UIDIR)/lpbuilder.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/lpbuilder.cxx

$(OBJDIR)/msgdlg.o: $(HEADER) $(UIDIR)/msgdlg.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/msgdlg.cxx

$(OBJDIR)/solvemodel.o: $(HEADER) $(UIDIR)/solvemodel.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/solvemodel.cxx

$(OBJDIR)/unoglobal.o: $(HEADER) $(UIDIR)/unoglobal.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/unoglobal.cxx

$(OBJDIR)/unohelper.o: $(HEADER) $(UIDIR)/unohelper.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/unohelper.cxx

$(OBJDIR)/xcalc.o: $(HEADER) $(UIDIR)/xcalc.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/xcalc.cxx


#----------------------------------------------------------------------------
# Misc

$(TARGET).zip: $(TARGET).so
	zip -j $@ $< $(TARGET).rdb $(XCUFILES)

$(TARGET).so: $(OBJFILES)
	export LD_LIBRARY_PATH=$(SLVDIR)/lib && \
	$(CXX) -shared -Wl,-soname,$@ -o $@ -lc $(OBJFILES) $(LIBLPSOLVE) && \
 	$(BINDIR)/regcomp -register -r $(TARGET).rdb -c $@

regview:
	$(BINDIR)/regview $(TARGET).rdb

clean:
	rm -f $(OBJFILES)
	
distclean: clean
	rm -f $(TARGET).* *~ 

