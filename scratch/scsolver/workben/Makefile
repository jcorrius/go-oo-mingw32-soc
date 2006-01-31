
OBJDIR=./addon_pkg/obj
CPP_INCLUDES=-I../source/inc
CPP=/usr/bin/g++
CPPFLAGS=-DDEBUG -Wall -Os -mtune=pentium3

simplex_test.o: simplex_test.cxx
	$(CPP) $(CPPFLAGS) $(CPP_INCLUDES) \
	-o $@ -c $<

simplex_test: simplex_test.o
	$(CPP) -o $@ simplex_test.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/lpbase.o \
	$(OBJDIR)/lpmodel.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/lpsimplex.o

matrix_test.o: matrix_test.cxx
	$(CPP) $(CPPFLAGS) $(CPP_INCLUDES) \
	-o $@ -c $<

matrix_test: matrix_test.o
	$(CPP) -o $@ matrix_test.o \
	$(OBJDIR)/matrix.o \
	$(OBJDIR)/global.o