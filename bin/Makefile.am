script_files = \
	build-ooo \
	scale-icons \
	nothing \
	openoffice-xlate-lang \
	package-lang \
	package-ooo \
	ootouch \
	linkoo \
	install-dict.in \
	ooo-wrapper.in \
	latest-patchset

EXTRA_DIST = $(script_files)

all:
	./build-ooo ${TAG} --checkout

install:
	chmod +x ooo-wrapper
	install -D ooo-wrapper $(DESTDIR)${bindir}/ooo-wrapper${BINSUFFIX}
	for app in calc draw impress math writer ffice; do \
	    ln -sf ooo-wrapper${BINSUFFIX} $(DESTDIR)${bindir}/oo${app}${BINSUFFIX}; \
	done
	./package-ooo ${TAG} --clean

