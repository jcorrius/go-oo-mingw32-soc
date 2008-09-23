# if BONOBO
#  MAYBE_BONOBO = bonobo
# endif

SUBDIRS = po desktop distro-configs bin etc patches man doc src stamp templates www # $(MAYBE_BONOBO)

include $(top_srcdir)/Makefile.shared

EXTRA_DIST = \
	emacs.el		\
	MAINTAINERS		\
	download.in		\
	Makefile.shared		\
	intltool-merge.in	\
	intltool-update.in	\
	intltool-extract.in	\
	ChangeLog-to-1.3.0	\
	ChangeLog-to-1.9.0	\
	ChangeLog-to-2.0.0	\
	ChangeLog-to-2.2

clean-local:
	test -z "$(OOBUILDDIR)" || rm -rf $(OOBUILDDIR)
	for icons in $(OOO_ICONS_VERS) ; do \
	    test "$(BUILDDIR)/$$icons" = "/" || rm -rf "$(BUILDDIR)/$$icons" ; \
	done
	test -z "$(BUILDDIR)/gcc-3.4.1" || rm -rf $(BUILDDIR)/gcc-3.4.1
	test -z "$(BUILDDIR)/bin" || rm -rf $(BUILDDIR)/bin/*
	test -z "$(BUILDDIR)/include/c++" || rm -rf $(BUILDDIR)/include/c++
	test -z "$(BUILDDIR)/lib" || rpm -rf $(BUILDDIR)/lib/*
	test -z "$(BUILDDIR)/libexec" || rpm -rf $(BUILDDIR)/libexec/*

maintainer-clean-local:
	test -z "$(BUILDDIR)" || rm -rf $(BUILDDIR)
