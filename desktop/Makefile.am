SUBDIRS=16x16 22x22 24x24 32x32 48x48 scalable mimetypes

desktop_in_files = ${wildcard *.desktop.in.in}

old_gnome_files = \
	openoffice.applications.in \
	openoffice-extra.keys.in \
	openoffice.keys.in

all: $(desktop_in_files:.desktop.in.in=.desktop) $(old_gnome_files:.in=)

clean-local: 
	rm -f $(desktop_in_files:.desktop.in.in=.desktop.in) \
	      $(desktop_in_files:.desktop.in.in=.desktop) \
	      $(old_gnome_files:.in=)

%.desktop.in: %.desktop.in.in $(top_builddir)/intltool-merge $(wildcard $(top_srcdir)/po/*.po)
	$(top_builddir)/intltool-merge $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache

%: %.in
	sed -e "s|\@OOO_BINSUFFIX\@|@OOO_BINSUFFIX@|" $< > $@

EXTRA_DIST=$(desktop_in_files) \
	$(old_gnome_files) \
	${wildcard *.png} \
	openoffice.xml \
	openoffice.mime
