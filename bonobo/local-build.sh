#!/bin/sh

TAG='OOO_1_1_0'
CWD=`pwd`;
./autogen.sh --with-ooo-solver-dir=$CWD/../build/$TAG/solver/645/unxlngi4.pro --with-ooo-install-dir=/usr/lib/ooo-1.1.0 --prefix=/usr --sysconfdir=/etc
make
