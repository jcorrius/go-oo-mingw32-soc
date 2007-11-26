CWS_WORK_STAMP=koheidatapilot01
MILESTONE=m233
BASEDIR=$(HOME)/ooo/$(CWS_WORK_STAMP)-$(MILESTONE)
INSTBASEDIR=$(HOME)/ooo/install/$(CWS_WORK_STAMP)-$(MILESTONE)
OBJDIR=obj
DEFINES=-DUNX -DGCC -DLINUX -DCPPU_ENV=gcc3 -DHAVE_GCC_VISIBILITY_FEATURE -g -O0
CPPFLAGS= \
	$(DEFINES) \
	-I$(BASEDIR)/cppuhelper/inc \
	-I$(BASEDIR)/solver/680/unxlngi6.pro/inc \
	-I$(BASEDIR)/solver/680/unxlngi6.pro/inc/offuh \
	-I./inc

LDFLAGS= \
	-lcppuhelpergcc3 -lcppu -lsal -lstlport_gcc -luno_salhelpergcc3 -L$(INSTBASEDIR)/program

EXEC=dptest

OBJFILES= \
	$(OBJDIR)/main.o \
	$(OBJDIR)/global.o \
	$(OBJDIR)/dpcachetable.o \
	$(OBJDIR)/dptestbase.o \
	$(OBJDIR)/dpresulttester.o

all: pre $(EXEC)

pre:
	mkdir $(OBJDIR) 2> /dev/null || /bin/true

$(EXEC): $(OBJFILES)
	$(CXX) -o $@ $(OBJFILES) $(LDFLAGS)

$(OBJDIR)/main.o: source/main.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS)

$(OBJDIR)/global.o: source/global.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS)

$(OBJDIR)/dpcachetable.o: source/dpcachetable.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS)

$(OBJDIR)/dptestbase.o: source/dptestbase.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS)

$(OBJDIR)/dpresulttester.o: source/dpresulttester.cxx
	$(CXX) -o $@ -c $< $(CPPFLAGS)

clean:
	rm $(EXEC) $(OBJFILES)
