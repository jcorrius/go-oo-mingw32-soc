script_files = \
	build-ooo \
	font-munge \
	install-dict.in \
	latest-patchset \
	linkoo \
	nothing \
	ooo-wrapper.in \
	ootouch \
	openoffice-xlate-lang \
	owner \
	package-lang \
	package-ooo \
	relocate \
	scale-icons

EXTRA_DIST = $(script_files)

all:
	./build-ooo ${TAG} --checkout

install:
	./package-ooo ${TAG}
