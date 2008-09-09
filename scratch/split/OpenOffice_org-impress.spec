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

Name:           OpenOffice_org-impress
Version:        3.0.0.2
Release:        1
%define         ooo_build_version 3.0.0.2b
%define         piece      impress
BuildRequires:  ImageMagick ant ant-apache-regexp bison boost-devel ccache cups-devel curl-devel db-devel flac-devel flex gnome-vfs2-devel gperf gtk2-devel icu java-devel kdelibs3-devel krb5 libexif mono-devel neon-devel openldap2-devel pam-devel perl-Archive-Zip perl-Compress-Zlib python-devel recode unixODBC-devel unzip xml-commons-apis xorg-x11 zip
BuildRequires:  OpenOffice_org-libs-core-devel = %version
# artwork and l10n is needed to build the sdext (license files)
BuildRequires:  OpenOffice_org-artwork-devel = %version
# poppler is needed to build the PDF import extension
# the needed version 0.8.0 has been avaiable only since openSUSE-11.0
%if %suse_version >= 1100
BuildRequires:  libpoppler-devel
%endif
%define	        ooo_prefix	%_libdir
%define	        ooo_home	ooo3
%define	        ooo_build_tag	ooo300-m4
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

%package devel
Requires:       %name = %version
Requires:       OpenOffice_org-libs-core-devel = %version
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

%files devel
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%ooo_prefix/%ooo_home/solver/*
%exclude %ooo_prefix/%ooo_home/solver/noarch

%files l10n-prebuilt
%defattr(-,root,root)
%ooo_prefix/%ooo_home/solver/noarch

%changelog
