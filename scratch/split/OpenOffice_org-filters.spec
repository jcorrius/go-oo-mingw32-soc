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

Name:           OpenOffice_org-filters
Version:        3.0.0.2
Release:        1
%define         ooo_build_version 3.0.0.2b
%define         piece      filters
BuildRequires:  ImageMagick ant ant-apache-regexp bison boost-devel ccache cups-devel curl-devel db-devel flac-devel flex gnome-vfs2-devel gperf gtk2-devel icu java-devel kdelibs3-devel krb5 libexif mono-devel neon-devel openldap2-devel pam-devel perl-Archive-Zip perl-Compress-Zlib python-devel recode unixODBC-devel unzip xml-commons-apis xorg-x11 zip
BuildRequires:  OpenOffice_org-libs-core-devel = %version
%if %suse_version > 1100
# libexpat-devel is explicitely needed since openSUSE-11.1
BuildRequires:  libexpat-devel
%endif
# 10.3 features
%if %suse_version > 1020
BuildRequires:  libicu-devel
%endif
# 11.0 features
%if %suse_version > 1030
BuildRequires:  hunspell-devel libwpd-devel libwpg-devel libwps-devel
%endif
%define	        ooo_prefix	%_libdir
%define	        ooo_home		ooo3
%define	        ooo_build_tag     ooo300-m4
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
AutoReqProv:    on
Requires:       OpenOffice_org-libs-core = %version
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-%piece.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
ure

%package optional
License:        LGPL v3 only
Summary:        Additional Import and Export Filters for OpenOffice.org
Group:          Productivity/Office/Suite
Provides:       OpenOffice_org:%ooo_prefix/%ooo_home/share/xslt/docbook/DocBookTemplate.stw
Requires:       OpenOffice_org-calc = %version
Requires:       OpenOffice_org-draw = %version
Requires:       OpenOffice_org-math = %version
Requires:       OpenOffice_org-writer = %version
Requires:       OpenOffice_org-impress = %version
%if %suse_version > 1000
Supplements:    OpenOffice_org
%endif

%description optional
This package includes some additional import and export filters for
OpenOffice.org:

- AportisDoc (Palm)

- Pocket Excel

- Pocket Word

- DocBook

- XHTML


%package l10n-prebuilt
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description l10n-prebuilt
devel package

%package devel
Requires:       %name = %version
Requires:       OpenOffice_org-ure-devel = %version
Requires:       OpenOffice_org-libs-extern-devel = %version
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
export EXTRA_BUILD_FLAGS=%{?jobs:-P%jobs}
export EXTRA_DMAKE_FLAGS="%{?jobs:-- -P%jobs}"
$OO_TOOLSDIR/piece/build-generic %piece %ooo_build_tag

%install

# setup env.
export OO_INSTDIR=%{_libdir}/%ooo_home
export OO_TOOLSDIR=$OO_INSTDIR/ooo-build/bin
export DESTDIR=$RPM_BUILD_ROOT

$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag

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
#%ooo_prefix/%ooo_home/basis3.0/*

%files -f files-%piece-optional.txt optional
%defattr(-,root,root)

%files devel
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%ooo_prefix/%ooo_home/solver/*
%exclude %ooo_prefix/%ooo_home/solver/noarch

%files l10n-prebuilt
%defattr(-,root,root)
%ooo_prefix/%ooo_home/solver/noarch

%changelog
