
SUBDIRS = bin desktop etc patches www

EXTRA_DIST = MAINTAINERS HACKING

all:
	cd bin; ./build-ooo ${TAG} --checkout

install:
	cd bin; ./package-ooo ${TAG} --clean

