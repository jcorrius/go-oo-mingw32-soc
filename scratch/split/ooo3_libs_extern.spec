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

%define         piece      libs_extern
Name:           ooo3_%piece
Requires:       ooo3_bootstrap
BuildRequires:  ooo3_bootstrap-devel
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo-3
%define	      ooo_build_tag     dev300-m21
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
Version:        3
Release:        5
AutoReqProv:    on
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-%piece.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
ure

%package devel
Requires:       ooo3_bootstrap-devel
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description devel
devel package

%prep
%setup -q -n %ooo_build_tag-%piece

%build

#setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_SOLVERDIR=$OO_INSTDIR/solver
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin

$OO_TOOLSDIR/../patches/apply.pl $OO_TOOLSDIR/../patches/dev300 --pieces --tag=%ooo_build_tag --distro=SUSE --distro=Piece `pwd` 
source $OO_TOOLSDIR/piece/sys-setup
$OO_TOOLSDIR/piece/build-generic

%install

#setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_SOLVERDIR=$OO_INSTDIR/solver
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export DESTDIR=$RPM_BUILD_ROOT

source $OO_TOOLSDIR/piece/sys-setup
$OO_TOOLSDIR/piece/install-generic

%clean
%post
%post devel

%files
%defattr(-,root,root)
# FIXME - should have the actual files in it ! ;-)
# except we have ~none here - just a devel package (?)
# is there anything we absolutely need here ?

%files devel
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%ooo_prefix/%ooo_home/solver/*

%changelog
