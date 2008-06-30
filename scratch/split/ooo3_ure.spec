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

%define         piece ure
Name:           ooo3_%piece
BuildRequires:  ooo3_base-devel
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo-3
%define	      ooo_build_tag     dev300-m19
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
Version:        3
Release:        1
AutoReqProv:    on
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-ure.tar.bz2

%description
ure

%package devel
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description devel
devel package

%prep
%setup -q -n %ooo_build_tag

%build

#setup env.
OO_INSTDIR=%{_libdir}/%ooo_home/
OO_SOLVERDIR=%{_libdir}/%ooo_home/solver
OO_TOOLSDIR=%{_libdir}/%ooo_home/ooo-build/bin

source $OO_TOOLSDIR/piece/sys-setup
$OO_TOOLSDIR/piece/build-%piece

%install

source $OO_TOOLSDIR/piece/sys-setup
$OO_TOOLSDIR/piece/install-%piece

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
