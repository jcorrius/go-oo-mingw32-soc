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
	latest-patchset \
	font-munge

EXTRA_DIST = $(script_files)

all:
	./build-ooo ${TAG} --checkout

install:
	./package-ooo ${TAG} --clean

foo:
	./echo.sh