desktopdir = $(datadir)/applications
desktop_in_files = ${wildcard *.desktop.in} 
desktop_DATA = $(desktop_in_files:.desktop.in=.desktop)
@INTLTOOL_DESKTOP_RULE@

EXTRA_DIST=${wildcard *.desktop} ${wildcard *.png}
