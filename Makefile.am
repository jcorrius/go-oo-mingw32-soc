# if BONOBO
#  MAYBE_BONOBO = bonobo
# endif

SUBDIRS = po desktop bin patches man doc fonts stamp src www $(MAYBE_BONOBO)

include $(top_srcdir)/Makefile.shared

EXTRA_DIST = \
	emacs.el		\
	MAINTAINERS		\
	HACKING			\
	download.in		\
	Makefile.shared		\
	intltool-merge.in	\
	intltool-update.in	\
	intltool-extract.in	\
	ChangeLog-to-1.3.0

ACLOCAL_AMFLAGS = -I m4

clean-local:
	test -z "$(OOBUILDDIR)" || rm -rf $(OOBUILDDIR)
	for icons in $(OOO_ICONS_VERS) ; do \
	    test "$(BUILDDIR)/$$icons" = "/" || rm -rf "$(BUILDDIR)/$$icons" ; \
	done

maintainer-clean-local:
	test -z "$(BUILDDIR)" || rm -rf $(BUILDDIR)
