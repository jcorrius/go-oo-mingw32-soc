# if BONOBO
#  MAYBE_BONOBO = bonobo
# endif

SUBDIRS = po desktop distro-configs bin etc patches man doc src stamp templates extensions # $(MAYBE_BONOBO)

include $(top_srcdir)/Makefile.shared

EXTRA_DIST = \
	emacs.el		\
	MAINTAINERS		\
	download.in		\
	Makefile.shared		\
	intltool-merge.in	\
	intltool-update.in	\
	intltool-extract.in

dist-hook:
	git log --date=short --pretty="format:@%cd  %an  <%ae>  [%H]%n%n%s%n%n%e%b" | sed -e "s|^\([^@]\)|\t\1|" -e "s|^@||" >$(distdir)/ChangeLog

clean-local:
	test -z "$(OOBUILDDIR)" || rm -rf $(OOBUILDDIR)
	for icons in $(OOO_ICONS_VERS) ; do \
	    test "$(BUILDDIR)/$$icons" = "/" || rm -rf "$(BUILDDIR)/$$icons" ; \
	done

maintainer-clean-local:
	test -z "$(BUILDDIR)" || rm -rf $(BUILDDIR)
