if BONOBO
  MAYBE_BONOBO = bonobo
endif

SUBDIRS = po desktop bin etc patches man doc fonts stamp src www $(MAYBE_BONOBO)

include $(top_srcdir)/Makefile.shared

EXTRA_DIST = \
	emacs.el		\
	MAINTAINERS		\
	HACKING			\
	download.in		\
	intltool-merge.in	\
	intltool-update.in	\
	intltool-extract.in

ACLOCAL_AMFLAGS = -I m4
