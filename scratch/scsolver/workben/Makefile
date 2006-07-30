SRCDIR=../source
NUMERICDIR=$(SRCDIR)/numeric
UIDIR=$(SRCDIR)/ui

#OBJDIR=./addon_pkg/obj
#CPP_INCLUDES=-I../source/inc
CPPFLAGS= \
	-DDEBUG -Wall -Os -mtune=pentium3 \
	-I$(SRCDIR)/inc

all: nlptest

matrix.o: $(NUMERICDIR)/matrix.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

nlptest.o: nlptest.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

global.o: $(UIDIR)/global.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

nlpmodel.o: $(NUMERICDIR)/nlpmodel.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

nlpqnewton.o: $(NUMERICDIR)/nlpqnewton.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

funcobj.o: $(NUMERICDIR)/funcobj.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

diff.o: $(NUMERICDIR)/diff.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

nlpbase.o: $(NUMERICDIR)/nlpbase.cxx
	$(CXX) $(CPPFLAGS) -o $@ -c $<

nlptest: matrix.o nlptest.o global.o nlpmodel.o nlpqnewton.o funcobj.o diff.o nlpbase.o
	$(CXX) -o $@ matrix.o nlptest.o global.o nlpmodel.o nlpqnewton.o funcobj.o diff.o nlpbase.o

simplex_test.o: simplex_test.cxx
	$(CXX) $(CPPFLAGS) $(CPP_INCLUDES) \
	-o $@ -c $<

simplex_test: simplex_test.o
	$(CXX) -o $@ simplex_test.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/lpbase.o \
	$(OBJDIR)/lpmodel.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/lpsimplex.o

matrix_test.o: matrix_test.cxx
	$(CXX) $(CPPFLAGS) $(CPP_INCLUDES) \
	-o $@ -c $<

matrix_test: matrix_test.o
	$(CXX) -o $@ matrix_test.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/global.o

mxref_test.o: mxref_test.cxx
	$(CXX) $(CPPFLAGS) $(CPP_INCLUDES) \
	-o $@ -c $<

mxref_test: mxref_test.o
	$(CXX) -o $@ mxref_test.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/global.o