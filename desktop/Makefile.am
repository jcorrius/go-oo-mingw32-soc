desktopdir = $(datadir)/applications
desktop_in_files = ${wildcard *.desktop.in} 
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)

%.desktop: %.desktop.in $(top_builddir)/intltool-merge $(wildcard $(top_srcdir)/po/*.po)
	$(top_builddir)/intltool-merge $(top_srcdir)/po $< $@ -d -u -c $(top_builddir)/po/.intltool-merge-cache


EXTRA_DIST=${wildcard *.desktop} ${wildcard *.png}

