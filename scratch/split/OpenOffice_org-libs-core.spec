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

Name:           OpenOffice_org-libs-core
Version:        3.0.0.2
Release:        1
%define         ooo_build_version 3.0.0.2b
%define         piece      libs_core
BuildRequires:  OpenOffice_org-libs-gui-devel = %version
BuildRequires:  ImageMagick ant ant-apache-regexp bison boost-devel ccache cups-devel curl-devel db-devel flac-devel flex gnome-vfs2-devel gperf gtk2-devel icu java-devel kdelibs3-devel krb5 libexif mono-devel neon-devel openldap2-devel pam-devel perl-Archive-Zip perl-Compress-Zlib python-devel recode unixODBC-devel unzip xml-commons-apis xorg-x11 zip
%if %suse_version > 1100
# libexpat-devel is explicitely needed since openSUSE-11.1
BuildRequires:  libexpat-devel
%endif
# 10.3 features
%if %suse_version > 1020
BuildRequires:  libicu-devel
%endif
# CODE10 features
%if %suse_version > 1000
BuildRequires:  gstreamer010-plugins-base-devel sablot-devel xalan-j2 xerces-j2 xml-commons-apis
%endif
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo3
%define	      ooo_build_tag     ooo300-m4
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
AutoReqProv:    on
Requires:       OpenOffice_org-libs-gui = %version
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-%piece.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
ure

%package -n OpenOffice_org-gnome
License:        LGPL v3 only
Summary:        GNOME Extensions for OpenOffice.org
Group:          Productivity/Office/Suite
Provides:       ooo:/usr/lib/ooo-1.9/program/libevoab2.so
Provides:       OpenOffice_org17-gnome
Requires:       OpenOffice_org = %version
Obsoletes:      OpenOffice_org17-gnome
Provides:       OpenOffice_org2-gnome
Obsoletes:      OpenOffice_org2-gnome

%description -n OpenOffice_org-gnome
This package contains some GNOME extensions for OpenOffice.org.



%package -n OpenOffice_org-kde
License:        LGPL v3 only
Summary:        KDE Extensions for OpenOffice.org
Group:          Productivity/Office/Suite
Provides:       OpenOffice_org17-kde
Requires:       OpenOffice_org = %version
Obsoletes:      OpenOffice_org17-kde
Provides:       OpenOffice_org2-kde
Obsoletes:      OpenOffice_org2-kde

%description -n OpenOffice_org-kde
This package contains some KDE extensions for OpenOffice.org.

%package -n OpenOffice_org-mailmerge
License:        LGPL v3 only
Summary:        Mail Merge Functionality for OpenOffice.org
Group:          Productivity/Office/Suite
Provides:       OpenOffice_org:%ooo_prefix/%ooo_home/program/mailmerge.py
Requires:       OpenOffice_org-pyuno = %version
%if %suse_version > 1000
Supplements:    OpenOffice_org
%endif

%description -n OpenOffice_org-mailmerge
This module allows you to create form letters or send E-mail messages
to many recipients using OpenOffice.org office suite.


%package devel
Requires:       %name = %version
Requires:       OpenOffice_org-ure-devel = %version
Requires:       OpenOffice_org-libs-gui-devel = %version
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description devel
devel package

%package l10n-prebuilt
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description l10n-prebuilt
devel package



%prep
%setup -q -n %ooo_build_tag-%piece

%build
# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export VERBOSE=1
export EXTRA_BUILD_FLAGS=%{?jobs:-P%jobs}
export EXTRA_DMAKE_FLAGS="%{?jobs:-- -P%jobs}"
# FIXME: start it several times to avoid strange paralel build problem in basic
#        missing org/openoffice/vba/XErrObject.hpp
#        same problem happens also in linguistic
$OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag || $OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag || $OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag

%install

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export DESTDIR=$RPM_BUILD_ROOT

$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag
# prune redundant files the scp2 likes to make for us
rm -Rf \
    "$DESTDIR/$OO_INSTDIR/basis-link" \
    "$DESTDIR/$OO_INSTDIR/ure" \
    "$DESTDIR/$OO_INSTDIR/basis3.0/program/services.rdb"

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%post devel
/sbin/ldconfig

%postun devel
/sbin/ldconfig

%files -f files-%piece.txt
%defattr(-,root,root)
#%dir %ooo_prefix/%ooo_home/basis3.0
#%dir %ooo_prefix/%ooo_home/program
#%dir %ooo_prefix/%ooo_home/share
#%ooo_prefix/%ooo_home/basis3.0/*
#%ooo_prefix/%ooo_home/program/*
#%ooo_prefix/%ooo_home/share/*

%files devel
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%ooo_prefix/%ooo_home/solver/*
%exclude %ooo_prefix/%ooo_home/solver/noarch

%files -f files-gnome.txt -n OpenOffice_org-gnome
%defattr(-,root,root)

%files -f files-kde.txt -n OpenOffice_org-kde
%defattr(-,root,root)

%files -f files-mailmerge.txt -n OpenOffice_org-mailmerge
%defattr(-,root,root)

%files l10n-prebuilt
%defattr(-,root,root)
%ooo_prefix/%ooo_home/solver/noarch

%changelog
