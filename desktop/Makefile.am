desktop_in_files = ${wildcard *.desktop.in.in}

all: $(desktop_in_files:.desktop.in.in=.desktop)

clean: 
	rm -f $(desktop_in_files:.desktop.in.in=.desktop.in) $(desktop_in_files:.desktop.in.in=.desktop)

%.desktop: %.desktop.in $(top_builddir)/intltool-merge $(wildcard $(top_srcdir)/po/*.po)
	$(top_builddir)/intltool-merge $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

EXTRA_DIST=$(desktop_in_files) \
	${wildcard *.png} \
	${wildcard openoffice*in} \
	openoffice.xml \
	openoffice.mime
