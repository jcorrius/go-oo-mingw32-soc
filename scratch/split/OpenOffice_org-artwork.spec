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

Name:           OpenOffice_org-artwork
Version:        3.0.0.2
Release:        1
%define         ooo_build_version 3.0.0.2b
%define         piece      artwork
BuildRequires:  OpenOffice_org-bootstrap = %version
BuildRequires:  ImageMagick ant ant-apache-regexp bison boost-devel ccache cups-devel curl-devel db-devel flac-devel flex gnome-vfs2-devel gperf gtk2-devel icu java-devel kdelibs3-devel krb5 libexif mono-devel neon-devel openldap2-devel pam-devel perl-Archive-Zip perl-Compress-Zlib python-devel recode unixODBC-devel unzip xml-commons-apis xorg-x11 zip
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo3
%define	      ooo_build_tag     ooo300-m4
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
AutoReqProv:    on
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-%piece.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
ure

%package devel
Requires:       OpenOffice_org-bootstrap = %version
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
$OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag

%install

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export DESTDIR=$RPM_BUILD_ROOT

$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag

%clean
rm -rf $RPM_BUILD_ROOT

%post devel
/sbin/ldconfig

%postun devel
/sbin/ldconfig

# FIXME - should have the actual files in the main package ! ;-)
# except we have ~none here - just a devel package (?)
# is there anything we absolutely need here ?

%files devel
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%ooo_prefix/%ooo_home/solver/*

%changelog
