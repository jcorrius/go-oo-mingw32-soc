INCLUDES =								    \
	-DINIFILE=\"$(datadir)/ooo-bonobo/ooo-bonobo-uno-bootstraprc\"	    \
	-DDATADIR=\""$(datadir)"\"					    \
	-DRDBFILE=\""$(libdir)/ooo-bonobo/ooo-bonobo.rdb"\"		    \
	-DGNOMELOCALEDIR=\""$(datadir)/locale"\"			    \
	$(OOO_BONOBO_CFLAGS)							

LDADD =	\
	$(OOO_BONOBO_LIBS)

libexec_PROGRAMS = ooo-bonobo.bin

noinst_PROGRAMS = test

test_SOURCES =			\
	remote-uno-helper.cxx	\
	remote-uno-helper.h	\
	star-frame-widget.cxx	\
	star-frame-widget.h	\
	test.cxx

ooo_bonobo_bin_SOURCES =		\
	ooo-bonobo-control.cxx	\
	ooo-bonobo-control.h	\
	remote-uno-helper.cxx	\
	remote-uno-helper.h	\
	services.h		\
	star-frame-widget.cxx	\
	star-frame-widget.h	\
	string-macros.h		\
	main.cxx
