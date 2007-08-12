#!/bin/sh

# Create configure script from configure.ac.
aclocal || exit 1
autoconf || exit 1

