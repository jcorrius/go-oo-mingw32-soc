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

Name:           OpenOffice_org-components
Version:        3.0.0.2
Release:        1
%define         ooo_build_version 3.0.0.2b
%define         piece      components
BuildRequires:  OpenOffice_org-artwork-devel = %version
BuildRequires:  OpenOffice_org-libs-core-devel = %version
BuildRequires:  ImageMagick ant ant-apache-regexp bison boost-devel ccache cups-devel curl-devel db-devel flac-devel flex gnome-vfs2-devel gperf gtk2-devel icu java-devel kdelibs3-devel krb5 libexif mono-devel neon-devel openldap2-devel pam-devel perl-Archive-Zip perl-Compress-Zlib python-devel recode unixODBC-devel unzip xml-commons-apis xorg-x11 zip
# mozilla stuff
%if %suse_version > 1100
BuildRequires:  mozilla-xulrunner190-devel
%endif 
%if %suse_version > 1010 && %suse_version <= 1100
BuildRequires:  mozilla-xulrunner181-devel
%endif
%if %suse_version > 1000 && %suse_version <= 1010
BuildRequires:  gecko-sdk mozilla-xulrunner
%endif
# 11.0 features
%if %suse_version > 1030
BuildRequires:  hunspell-devel libwpd-devel libwpg-devel libwps-devel
%endif
# 10.3 features
%if %suse_version > 1020
BuildRequires:  libicu-devel
%endif
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo3
%define	      ooo_build_tag     ooo300-m4
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


%package -n OpenOffice_org-officebean
License:        LGPL v3 only
Summary:        OfficeBean Java Bean component for OpenOffice.org
Group:          Productivity/Office/Suite
Provides:       OpenOffice_org:/usr/lib/ooo-2.0/program/libofficebean.so
Requires:       OpenOffice_org = %version
Provides:       OpenOffice_org2-officebean
Obsoletes:      OpenOffice_org2-officebean

%description -n OpenOffice_org-officebean
With the OfficeBean, a developer can easily write Java applications,
harnessing the power of OpenOffice.org. It encapsulates a connection to
a locally running OpenOffice.org process, and hides the complexity of
establishing and maintaining that connection from the developer.

It also allows embedding of OpenOffice.org documents within the Java
environment. It provides an interface the developer can use to obtain
Java AWT windows into which the backend OpenOffice.org process draws
its visual representation. These windows are then plugged into the UI
hierarchy of the hosting Java application. The embedded document is
controlled from the Java environment, since the OfficeBean allows
developers to access the complete OpenOffice.org API from their Java
environment giving them full control over the embedded document, its
appearance and behavior.

%package -n OpenOffice_org-testtool
License:        LGPL v3 only
Summary:        OpenOffice.org Test Tool
Group:          Productivity/Office/Suite
Provides:       OpenOffice_org:%ooo_prefix/%ooo_home/program/testtool.bin
Requires:       OpenOffice_org = %version

%description -n OpenOffice_org-testtool
A Tool needed for automatic testing of OpenOffice.org office suite.


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
# prune redundant files the scp2 likes to make for us
rm -Rf \
    "$DESTDIR/$OO_INSTDIR/basis-link" \
    "$DESTDIR/$OO_INSTDIR/ure" \
    "$DESTDIR/$OO_INSTDIR/solver/bin/packinfo*.txt"

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

%files -f files-officebean.txt -n OpenOffice_org-officebean
%defattr(-,root,root)

%files -f files-testtool.txt -n OpenOffice_org-testtool
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
