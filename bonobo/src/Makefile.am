INCLUDES =								    \
	-DINIFILE=\"$(datadir)/ooo-bonobo/ooo-bonobo-uno-bootstrap.ini\"    \
	-DDATADIR=\""$(datadir)"\"					    \
	-DRDBFILE=\""$(libdir)/ooo-bonobo/ooo-bonobo.rdb"\"		    \
	$(OOO_BONOBO_CFLAGS)							

LDADD =	\
	$(OOO_BONOBO_LIBS)

libexec_PROGRAMS = ooo-bonobo

noinst_PROGRAMS = test

test_SOURCES =			\
	remote-uno-helper.cxx	\
	remote-uno-helper.h	\
	star-frame-widget.cxx	\
	star-frame-widget.h	\
	test.cxx

ooo_bonobo_SOURCES =		\
	ooo-bonobo-control.cxx	\
	ooo-bonobo-control.h	\
	remote-uno-helper.cxx	\
	remote-uno-helper.h	\
	star-frame-widget.cxx	\
	star-frame-widget.h	\
	main.cxx