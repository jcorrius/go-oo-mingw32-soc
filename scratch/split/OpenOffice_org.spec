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

Name:           OpenOffice_org
Version:        3.0.0.2
Release:        1
%define	      ooo_build_version 3.0.0.2b
%define         piece      postprocess
BuildRequires:  perl-Archive-Zip zip unzip
BuildRequires:  OpenOffice_org-artwork-devel = %version
BuildRequires:  OpenOffice_org-bootstrap = %version
BuildRequires:  OpenOffice_org-base-devel = %version
BuildRequires:  OpenOffice_org-calc-devel = %version
BuildRequires:  OpenOffice_org-components-devel = %version
BuildRequires:  OpenOffice_org-impress-devel = %version
#BuildRequires:  OpenOffice_org-l10n-devel = %version
BuildRequires:  OpenOffice_org-libs-core-devel = %version
BuildRequires:  OpenOffice_org-libs-extern-devel = %version
BuildRequires:  OpenOffice_org-libs-gui-devel = %version
BuildRequires:  OpenOffice_org-ure-devel = %version
BuildRequires:  OpenOffice_org-writer-devel = %version
BuildRequires:  OpenOffice_org-filters-devel = %version
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo3
%define	      ooo_build_tag     ooo300-m4
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
AutoReqProv:    on
Requires:       OpenOffice_org-ure = %version
Requires:       OpenOffice_org-libs-core = %version
Requires:       OpenOffice_org-libs-gui = %version
Requires:       OpenOffice_org-l10n-extras = %version
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
Source:         %ooo_build_tag-%piece.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
main ooo3 package.

%package icon-themes-prebuilt
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
AutoReqProv:    off

%description icon-themes-prebuilt
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
export OO_ALLOW_PROFILES=1
$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag

# Now do magic stuff to register the services - fully hacky (sadly).
#export OO_BUILD_SERVICES=1
#$OO_TOOLSDIR/piece/install-generic %piece %ooo_build_tag
# urgh - unbelievably nasty:
#cp -a OpenOffice/gid_Starregistry_Services_Rdb_servicesrdb/*/services.rdb \
#    $DESTDIR$OO_INSTDIR/basis3.0/program/services.rdb
#cp -a OpenOffice/gid_Starregistry_Services_Rdb_Ure_servicesrdb/*/services.rdb \
#    $DESTDIR$OO_INSTDIR/ure/share/misc/services.rdb
# no need for solver bits at this late stage
#rm -Rf "$DESTDIR/$OO_INSTDIR/solver"

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files -f files-%piece.txt
%defattr(-,root,root)
#%dir %ooo_prefix/%ooo_home/basis3.0
#%dir %ooo_prefix/%ooo_home/program
#%dir %ooo_prefix/%ooo_home/share
#%dir %ooo_prefix/%ooo_home/ure
#%ooo_prefix/%ooo_home/basis3.0/*
#%ooo_prefix/%ooo_home/program/*
#%ooo_prefix/%ooo_home/share/*
#%ooo_prefix/%ooo_home/ure/*
#%ooo_prefix/%ooo_home/basis-link

%files -f files-icon-themes.txt icon-themes-prebuilt
%defattr(-,root,root)

%changelog
