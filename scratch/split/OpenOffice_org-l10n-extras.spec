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

Name:           OpenOffice_org-l10n-extras
Version:        3.0.0.2
Release:        1
%define         ooo_build_version 3.0.0.2b
%define         piece      l10n
BuildRequires:  java-devel zip unzip
BuildRequires:  OpenOffice_org-artwork-devel = %version
BuildRequires:  OpenOffice_org-libs-core-devel = %version
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo3
%define	      ooo_build_tag     ooo300-m4
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
AutoReqProv:    on
PreReq:	        OpenOffice_org-ure = %version
Requires:       OpenOffice_org-libs-core = %version
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-%piece.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildArch:      noarch

%description
ure

%package devel
PreReq:		OpenOffice_org-ure = %version
Requires:       %name = %version
Requires:       OpenOffice_org-libs-core-devel = %version
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description devel
devel package

%package prebuilt
PreReq:		OpenOffice_org-ure = %version
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description prebuilt
devel package


%prep
%setup -q -n %ooo_build_tag-%piece
# FIXME: move helpcontent2 to separate source tarball
rm -rf helpcontent2

%build

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export VERBOSE=1
# don't build help 8x since - well - it needs a shed load of memory.
export EXTRA_BUILD_FLAGS=%{?jobs:-P2}
export EXTRA_DMAKE_FLAGS="%{?jobs:-- -P2}"
$OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag

%install

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export DESTDIR=$RPM_BUILD_ROOT

$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag
# prune redundant files the scp2 likes to make for us
rm -Rf \
    "$DESTDIR/$OO_INSTDIR/basis-link" \
    "$DESTDIR/$OO_INSTDIR/ure"

%clean
rm -rf $RPM_BUILD_ROOT

# main
%posttrans
%_datadir/%ooo_home/link-to-ooo-home %_datadir/%ooo_home/files-%piece.txt || true

%preun
test "$1" = "0" && %_datadir/%ooo_home/link-to-ooo-home --unlink %_datadir/%ooo_home/files-%piece.txt || true

# prebuilt
%posttrans prebuilt
%_datadir/%ooo_home/link-to-ooo-home %_datadir/%ooo_home/files-%piece-prebuilt.txt || true

%preun prebuilt
test "$1" = "0" && %_datadir/%ooo_home/link-to-ooo-home --unlink %_datadir/%ooo_home/files-%piece-prebuilt.txt || true


# devel
%posttrans devel
%_datadir/%ooo_home/link-to-ooo-home %_datadir/%ooo_home/files-%piece-devel.txt || true

%preun devel
test "$1" = "0" && %_datadir/%ooo_home/link-to-ooo-home --unlink %_datadir/%ooo_home/files-%piece-devel.txt || true


%files -f files-%piece.txt
%defattr(-,root,root)
#%dir %ooo_prefix/%ooo_home/basis3.0
#%dir %ooo_prefix/%ooo_home/LICENSE
#%dir %ooo_prefix/%ooo_home/LICENSE.html
#%dir %ooo_prefix/%ooo_home/README
#%dir %ooo_prefix/%ooo_home/README.html
#%dir %ooo_prefix/%ooo_home/readmes
#%dir %ooo_prefix/%ooo_home/licenses
#%dir %ooo_prefix/%ooo_home/share
#%dir %ooo_prefix/%ooo_home/THIRDPARTYLICENSEREADME.html
#%ooo_prefix/%ooo_home/basis3.0/*
#%ooo_prefix/%ooo_home/readmes/*
#%ooo_prefix/%ooo_home/licenses/*
#%ooo_prefix/%ooo_home/share/*


%files -f files-%piece-prebuilt.txt prebuilt
%defattr(-,root,root)
%dir %_datadir/%ooo_home
%dir %_datadir/%ooo_home/solver

%files -f files-%piece-devel.txt devel
%defattr(-,root,root)

%changelog
