EXTRA_DIST=${wildcard *.po} \
	${wildcard *.sdf} \
	ooo-build.pot \
	README \
	ChangeLog \
	POTFILES.in

ooo_build_po_files = ${wildcard ooo-build-*.po}

update-sdf: $(ooo_build_po_files:ooo-build-%.po=ooo-build-%.sdf)

ooo-build.pot: ooo-build.sdf /usr/bin/oo2po
	oo2po -P --multifile=onefile ooo-build.sdf $@

ooo-build-*.po: ooo-build.pot /usr/bin/pot2po
	pot2po -t $@ -i ooo-build.pot -o $@.new
	mv $@.new $@

# ../bin/po2sdf is a crazy hack until
# http://bugs.locamotion.org/show_bug.cgi?id=487" is fixed"
ooo-build-%.sdf: ooo-build-%.po ooo-build.sdf /usr/bin/po2oo ../bin/po2sdf
	../bin/po2sdf $* ooo-build.sdf $< $@

/usr/bin/oo2po /usr/bin/pot2po /usr/bin/po2po:
	echo "Error: $@ does not exit" ; \
	echo "       You need to install translate-toolkit, see" ; \
	echo "       http://translate.sourceforge.net/wiki/start" ; \
	echo "       The package for openSUSE it named python-translate-toolkit" ; \
	echo "       and can be found at" ; \
	echo "       http://download.opensuse.org/repositories/OpenOffice.org:/EXTRAS/" ; \
	exit 1
