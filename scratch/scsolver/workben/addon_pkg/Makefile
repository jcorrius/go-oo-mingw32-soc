BUILDDIR=obj
SRCDIR=src
TARGET=scsolver.uno.zip

all: build
	mv $(BUILDDIR)/$(TARGET) ./

build: preprocess
	cd obj && make && cd ..

preprocess:
	mkdir $(BUILDDIR) >& /dev/null || /bin/true
	cp Makefile_build $(BUILDDIR)/Makefile

clean:
	rm -f *~ $(TARGET)
	
distclean: clean
	rm -rf $(BUILDDIR) $(SRCDIR)