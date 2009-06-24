EXTRA_DIST=${wildcard *.xcu} ${wildcard *.xml} 

tweakconfig : $(BUILDDIR)/bin/tweakconfig.oxt

$(BUILDDIR)/bin/tweakconfig.oxt: *.xcu *.xml
	mkdir -p $(BUILDDIR)/bin/tweakconfig && cp $(srcdir)/*.xcu $(srcdir)/*.xml $(BUILDDIR)/bin/tweakconfig/
	sed -i '/WritePath/ a <value>file://$(top_srcdir)/test</value>' $(BUILDDIR)/bin/tweakconfig/Paths.xcu
	zip $@ $(BUILDDIR)/bin/tweakconfig/*.xcu $(BUILDDIR)/bin/tweakconfig/*.xml 

all: tweakconfig

clean-local:
	rm -f $(BUILDDIR)/bin/tweakconfig.oxt
	rm -rf $(BUILDDIR)/bin/tweakconfig
