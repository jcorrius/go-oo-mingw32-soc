INCLUDES =								    \
	-DINIFILE=\"$(datadir)/ooo-bonobo/ooo-bonobo-uno-bootstrap.ini\"    \
	$(OOO_BONOBO_CFLAGS)

LDADD =	\
	$(OOO_BONOBO_LIBS)

libexec_PROGRAMS = ooo-bonobo

noinst_PROGRAMS = test

test_SOURCES =			\
	star-frame-widget.cxx	\
	star-frame-widget.h	\
	test.cxx

ooo_bonobo_SOURCES =		\
	star-frame-widget.cxx	\
	star-frame-widget.h	\
	main.cxx