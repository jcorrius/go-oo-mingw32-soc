TARGET=scsolver.uno

SCSOLVER_LANG=en-US
SDKDIR=$(HOME)/ooo/install/SRC680_m224_sdk
OOODIR=$(HOME)/ooo/install/SRC680_m224

SCSOLVERDIR=../..
SRCDIR=$(SCSOLVERDIR)/source
OBJDIR=./obj
BINDIR=$(SDKDIR)/linux/bin

DEBUG=
LDFLAGS= -lsal -lcppuhelpergcc3 -lcppu -lstlport_gcc $(DEBUG) -L$(OOODIR)
CXXCPP=-I$(SDKDIR)/include -I$(SRCDIR)/inc -I. -I$(OBJDIR)
CXX_DEFINES= \
	-DUNX -DGCC -DLINUX -DCPPU_ENV=gcc3 -DHAVE_GCC_VISIBILITY_FEATURE \
	-DSCSOLVER_UNO_COMPONENT

CXX_SHARED_FLAGS=-fPIC -fno-common -export-dynamic -fvisibility=hidden
LIBLPSOLVE=./lpsolve/lpsolve55/liblpsolve55.a
CPPFLAGS=-DDEBUG -Wall -Wno-non-virtual-dtor -Wno-strict-aliasing -Os -mtune=pentium3

SRVDIR=$(SRCDIR)/service
UIDIR=$(SRCDIR)/ui
TOOLDIR=$(SRCDIR)/tool

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
	$(OBJDIR)/optiondlg.o \
	$(OBJDIR)/basedlg.o \
	$(OBJDIR)/unoglobal.o \
	$(OBJDIR)/xcalc.o \
	$(OBJDIR)/option.o \
	$(OBJDIR)/baselistener.o \
	$(OBJDIR)/dialog.o \
	$(OBJDIR)/lpbuilder.o \
	$(OBJDIR)/listener.o \
	$(OBJDIR)/unohelper.o \
	$(OBJDIR)/msgdlg.o \
	$(OBJDIR)/solvemodel.o \
	$(OBJDIR)/nlpbuilder.o \
	$(OBJDIR)/timer.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/service.o

NUMERIC_OBJFILES = \
	$(OBJDIR)/diff.o \
	$(OBJDIR)/exception.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/funcobj.o \
	$(OBJDIR)/lpsolve.o \
	$(OBJDIR)/lpbase.o \
	$(OBJDIR)/lpmodel.o \
	$(OBJDIR)/cellfuncobj.o \
	$(OBJDIR)/nlpmodel.o \
	$(OBJDIR)/nlpbase.o \
	$(OBJDIR)/nlpqnewton.o

MISC_OBJFILES = \
	$(OBJDIR)/resource.o

XCUFILES = Addons.xcu ProtocolHandler.xcu

all: pre $(OBJDIR)/unoheaders $(TARGET).zip

pre:
	mkdir $(OBJDIR) 2> /dev/null || /bin/true

#----------------------------------------------------------------------------
# numeric

NUMDIR=$(SRCDIR)/numeric

$(OBJDIR)/numeric.a: pre $(NUMERIC_OBJFILES)
	$(AR) r $@ $(NUMERIC_OBJFILES)

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

$(OBJDIR)/cellfuncobj.o: $(HEADER) $(NUMDIR)/cellfuncobj.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/cellfuncobj.cxx

$(OBJDIR)/lpsimplex.o: $(HEADER) $(NUMDIR)/lpsimplex.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/lpsimplex.cxx

$(OBJDIR)/lpsolve.o: $(HEADER) $(NUMDIR)/lpsolve.cxx
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

$(OBJDIR)/exception.o: $(HEADER) $(NUMDIR)/exception.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(NUMDIR)/exception.cxx

$(OBJDIR)/scsolver.a: pre $(OBJFILES)
	$(AR) r $@ $(OBJFILES)

#----------------------------------------------------------------------------
# scsolver

$(OBJDIR)/service.o: $(HEADER) $(SRVDIR)/service.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(SRVDIR)/service.cxx

$(OBJDIR)/basedlg.o: $(HEADER) $(UIDIR)/basedlg.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/basedlg.cxx

$(OBJDIR)/baselistener.o: $(HEADER) $(UIDIR)/baselistener.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/baselistener.cxx

$(OBJDIR)/dialog.o: $(HEADER) $(UIDIR)/dialog.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/dialog.cxx

$(OBJDIR)/optiondlg.o: $(HEADER) $(UIDIR)/optiondlg.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/optiondlg.cxx

$(OBJDIR)/option.o: $(HEADER) $(UIDIR)/option.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/option.cxx

$(OBJDIR)/global.o: $(HEADER) $(TOOLDIR)/global.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(TOOLDIR)/global.cxx

$(OBJDIR)/listener.o: $(HEADER) $(UIDIR)/listener.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/listener.cxx

$(OBJDIR)/lpbuilder.o: $(HEADER) $(UIDIR)/lpbuilder.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/lpbuilder.cxx

$(OBJDIR)/nlpbuilder.o: $(HEADER) $(UIDIR)/nlpbuilder.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(UIDIR)/nlpbuilder.cxx

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

$(OBJDIR)/timer.o: $(HEADER) $(TOOLDIR)/timer.cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c $(TOOLDIR)/timer.cxx

#----------------------------------------------------------------------------
# addon only

$(OBJDIR)/resource.o: res/resource.hxx res/$(SCSOLVER_LANG).cxx
	$(CXX) $(CPPFLAGS) $(CXXCPP) $(CXX_DEFINES) $(CPP_LIBS) \
	-o $@ $(CXX_SHARED_FLAGS) -c res/$(SCSOLVER_LANG).cxx

#----------------------------------------------------------------------------
# misc

$(OBJDIR)/unoheaders:
	export LD_LIBRARY_PATH=$(OOODIR)/program && \
	$(SDKDIR)/linux/bin/cppumaker -BUCR $(OOODIR)/program/types.rdb -O$(OBJDIR) && \
	touch $@

$(TARGET).zip: pre $(OBJDIR)/$(TARGET).so
	zip -j $@ $(OBJDIR)/$(TARGET).so $(OBJDIR)/$(TARGET).rdb $(XCUFILES)

$(OBJDIR)/$(TARGET).so: $(NUMERIC_OBJFILES) $(OBJFILES) $(MISC_OBJFILES)
	export LD_LIBRARY_PATH=$(OOODIR)/program && \
	$(CXX) -shared -Wl,-soname,$@ -o $@ -lc $(NUMERIC_OBJFILES) $(OBJFILES) $(MISC_OBJFILES) && \
 	$(BINDIR)/regcomp -register -r $(OBJDIR)/$(TARGET).rdb -c $@

regview:
	$(BINDIR)/regview $(TARGET).rdb

clean:
	rm -rf $(OBJDIR)

distclean: clean
	rm -f $(TARGET).zip

