server_in_files = GNOME_OpenOfficeOrg.server.in.in

serverdir = $(libdir)/bonobo/servers
server_DATA = $(server_in_files:.server.in.in=.server)

%.server.in: %.server.in.in
	sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" $< > $@

@INTLTOOL_SERVER_RULE@

rdbdir = $(libdir)/ooo-bonobo
rdb_DATA = ooo-bonobo.rdb

ooo-bonobo.rdb:
	$(REGMERGE) $@.tmp / $(DKREGISTRYNAME)
	$(REGCOMP) -register -r $@.tmp -c $(OOODEV_LIBDIR)/connector.uno$(SHAREDLIBEXT)
	$(REGCOMP) -register -r $@.tmp -c $(OOODEV_LIBDIR)/remotebridge.uno$(SHAREDLIBEXT)
	$(REGCOMP) -register -r $@.tmp -c $(OOODEV_LIBDIR)/bridgefac.uno$(SHAREDLIBEXT)
	$(REGCOMP) -register -r $@.tmp -c $(OOODEV_LIBDIR)/uuresolver.uno$(SHARELIBEXT)
	mv $@.tmp $@

ini_in_files = ooo-bonobo-uno-bootstraprc.in
inidir = $(datadir)/ooo-bonobo
ini_DATA = $(ini_in_files:rc.in=rc)

%rc: %rc.in
	sed -e "s|\@RDBDIR\@|$(rdbdir)|" $< > $@

uidir = $(datadir)/gnome-2.0/ui
ui_DATA = ooo-bonobo-control-ui.xml

libexec_SCRIPTS = ooo-bonobo

ooo-bonobo: ooo-bonobo.in
	sed -e "s|\@LIBEXECDIR\@|$(libexecdir)|" \
	    -e "s|\@OOO_LIBDIR\@|$(OOO_INSTALLDIR)/program|" $< > $@

BUILT_SOURCES=$(ini_DATA) $(rdb_DATA) $(server_DATA) $(libexec_SCRIPTS)
CLEANFILES=$(BUILT_SOURCES)

EXTRA_DIST=$(server_in_files) $(ini_in_files) $(ui_DATA) ooo-bonobo.in
