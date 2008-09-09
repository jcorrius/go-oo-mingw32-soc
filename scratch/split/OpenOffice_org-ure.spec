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

Name:           OpenOffice_org-ure
Version:        3.0.0.2
Release:        1
%define         ooo_build_version 3.0.0.2b
%define         piece ure
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

%ifarch %ix86 x86_64
#

%package -n OpenOffice_org-mono
License:        LGPL v3 only
Summary:        Mono UNO Bridge for OpenOffice.org
Group:          Productivity/Office/Suite
Requires:       mono-core
Requires:       %name = %version

%description -n OpenOffice_org-mono
The Mono/UNO binding allows a Mono application to access the complete
set of APIs exposed by OpenOffice.org via UNO. A trivial example
use-case might be a small mono application to create a spreadsheet,
populate it with data, charts, formatting, then save that in Excel
format.

Currently the use of Mono for add-ins & scripting inside OO.o itself is
not supported.



%endif

%package -n OpenOffice_org-pyuno
License:        LGPL v3 only
Summary:        Python UNO Bridge for OpenOffice.org
Group:          Productivity/Office/Suite
Provides:       OpenOffice_org:%ooo_prefix/%ooo_home/program/pyuno.so
Requires:       %name = %version

%description -n OpenOffice_org-pyuno
The Python-UNO bridge allows to use the standard OpenOffice.org API
from the well known Python scripting language. It can be used to
develop UNO components in python, thus python UNO components may be run
within the OpenOffice.org process and can be called from Java, C++ or
the built in StarBasic scripting language. You can create and invoke
scripts with the office scripting framework (OOo 2.0 and later) with
it. For example, it is used for the mail merge functionality.

You can find the more information at
http://udk.openoffice.org/python/python-bridge.html

%package devel
Requires:       %name = %version
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
#%dir %ooo_prefix/%ooo_home/ure
#%ooo_prefix/%ooo_home/basis3.0/*
#%ooo_prefix/%ooo_home/ure/*

%ifarch %ix86 x86_64

%files -f files-mono.txt -n OpenOffice_org-mono
%defattr(-,root,root)
#FIXME: %ooo_prefix/pkgconfig/mono-%ooo_home.pc
%endif

%files -f files-pyuno.txt -n OpenOffice_org-pyuno
%defattr(-,root,root)

%files devel
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%ooo_prefix/%ooo_home/solver/*

%changelog
