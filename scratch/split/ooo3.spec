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
%define         piece      postprocess
Name:           ooo3
Requires:       ooo3_libs_core ooo3_ure ooo3_libs_gui ooo3_l10n
BuildRequires:  ooo3_bootstrap ooo3_base ooo3_bootstrap ooo3_calc ooo3_components ooo3_impress ooo3_l10n ooo3_libs_core ooo3_libs_extern ooo3_libs_gui ooo3_ure ooo3_writer ooo3_filters
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
main ooo3 package.

%prep
%setup -q -n %ooo_build_tag-%piece

%build

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export VERBOSE=1
export EXTRA_BUILD_FLAGS=%{?jobs:-P%jobs}
export EXTRA_DMAKE_FLAGS="%{?jobs:-- -P%jobs}"
$OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag %distro

%install

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export DESTDIR=$RPM_BUILD_ROOT
export OO_ALLOW_PROFILES=1
$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag %distro

# Now do magic stuff to register the services - fully hacky (sadly).
export OO_BUILD_SERVICES=1
$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag %distro
# urgh - unbelievably nasty:
cp -a OpenOffice/gid_Starregistry_Services_Rdb_servicesrdb/*/services.rdb \
    $DESTDIR$OO_INSTDIR/basis3.0/program/services.rdb
cp -a OpenOffice/gid_Starregistry_Services_Rdb_Ure_servicesrdb/*/services.rdb \
    $DESTDIR$OO_INSTDIR/ure/share/misc/services.rdb
# no need for solver bits at this late stage
rm -Rf "$DESTDIR/$OO_INSTDIR/solver"

%clean
%post

%files
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/basis3.0
%dir %ooo_prefix/%ooo_home/program
%dir %ooo_prefix/%ooo_home/share
%dir %ooo_prefix/%ooo_home/ure
%ooo_prefix/%ooo_home/basis3.0/*
%ooo_prefix/%ooo_home/program/*
%ooo_prefix/%ooo_home/share/*
%ooo_prefix/%ooo_home/ure/*
%ooo_prefix/%ooo_home/basis-link

%changelog
