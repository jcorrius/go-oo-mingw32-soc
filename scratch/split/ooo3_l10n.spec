#
# spec file for package OpenOffice_org (Version 2.4.1.7)
#
# Copyright (c) 2008 SUSE LINUX Products GmbH, Nuernberg, Germany.
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild

%define         distro     SUSE
%define         piece      l10n
Name:           ooo3_%piece
Requires:       ooo3_libs_core
BuildRequires:  ooo3_libs_core-devel
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo-3
%define	      ooo_build_tag     ooo300-m1
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
Version:        3
Release:        8
AutoReqProv:    on
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-%piece.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
ure

%package devel
Requires:       ooo3_ure-devel ooo3_libs_extern-devel
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description devel
devel package

%prep
%setup -q -n %ooo_build_tag-%piece

%build

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export VERBOSE=1
# don't build help 8x since - well - it needs a shed load of memory.
export EXTRA_BUILD_FLAGS=%{?jobs:-P2}
export EXTRA_DMAKE_FLAGS="%{?jobs:-- -P2}"
$OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag %distro

%install

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export DESTDIR=$RPM_BUILD_ROOT

$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag %distro
# prune redundant files the scp2 likes to make for us
rm -Rf \
    "$DESTDIR/$OO_INSTDIR/basis-link" \
    "$DESTDIR/$OO_INSTDIR/ure"

%clean
%post
%post devel

%files
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/basis3.0
%dir %ooo_prefix/%ooo_home/LICENSE
%dir %ooo_prefix/%ooo_home/LICENSE.html
%dir %ooo_prefix/%ooo_home/README
%dir %ooo_prefix/%ooo_home/README.html
%dir %ooo_prefix/%ooo_home/readmes
%dir %ooo_prefix/%ooo_home/licenses
%dir %ooo_prefix/%ooo_home/share
%dir %ooo_prefix/%ooo_home/THIRDPARTYLICENSEREADME.html
%ooo_prefix/%ooo_home/basis3.0/*
%ooo_prefix/%ooo_home/readmes/*
%ooo_prefix/%ooo_home/licenses/*
%ooo_prefix/%ooo_home/share/*

%files devel
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%ooo_prefix/%ooo_home/solver/*

%changelog
