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
	$(REGCOMP) -register -r $@.tmp -c $(OOO_libdir)/connector.uno$(SHAREDLIBEXT)
	$(REGCOMP) -register -r $@.tmp -c $(OOO_libdir)/remotebridge.uno$(SHAREDLIBEXT)
	$(REGCOMP) -register -r $@.tmp -c $(OOO_libdir)/bridgefac.uno$(SHAREDLIBEXT)
	$(REGCOMP) -register -r $@.tmp -c $(OOO_libdir)/uuresolver.uno$(SHARELIBEXT)
	mv $@.tmp $@

ini_in_files = ooo-bonobo-uno-bootstrap.ini.in
inidir = $(datadir)/ooo-bonobo
ini_DATA = $(ini_in_files:.ini.in=.ini)

%.ini: %.ini.in
	sed -e "s|\@RDBDIR\@|$(rdbdir)|" $< > $@

BUILT_SOURCES=$(ini_DATA) $(rdb_DATA) $(server_DATA)
CLEANFILES=$(BUILT_SOURCES)

EXTRA_DIST=$(server_in_files) $(ini_in_files)
