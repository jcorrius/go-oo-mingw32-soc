#
# spec file for package OpenOffice_org-icon-themes (Version 3.0.0.2)
#
# Copyright (c) 2008 SUSE LINUX Products GmbH, Nuernberg, Germany.
#
# All modifications and additions to the file contributed by third parties
# remain the property of their copyright owners, unless otherwise agreed
# upon. The license for this file, and modifications and additions to the
# file, is the same license as for the pristine package itself (unless the
# license for the pristine package is not an Open Source License, in which
# case the license is the MIT License). An "Open Source License" is a
# license that conforms to the Open Source Definition (Version 1.9)
# published by the Open Source Initiative.

# Please submit bugfixes or comments via http://bugs.opensuse.org/
#

# norootforbuild


Name:           OpenOffice_org-icon-themes
Version:        3.0.0.2
Release:        5
%define	      ooo_build_version 3.0.0.2b
BuildRequires:  OpenOffice_org-icon-themes-prebuilt = %version
%define	      ooo_basis_dir	basis3.0
%define	      ooo_home		ooo3
License:        LGPL v2.1 only; LGPL v3 only
Group:          Productivity/Office/Suite
AutoReqProv:    on
Provides:       OpenOffice_org:%_libdir/%ooo_home/share/config/images.zip
Summary:        Icon Themes for OpenOffice.org
Url:            http://www.openoffice.org/
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
BuildArch:      noarch

%description
This package provides various icon themes for the OpenOffice.org office
suite.



%prep

%build

%install
install -m 755 -d $RPM_BUILD_ROOT%_datadir/%ooo_home/%ooo_basis_dir/share/config
for file in images_crystal.zip images_hicontrast.zip \
            images_industrial.zip images_tango.zip \
	    images.zip ; do
    install -m644 %_libdir/%ooo_home/%ooo_basis_dir/share/config/$file \
                  $RPM_BUILD_ROOT%_datadir/%ooo_home/%ooo_basis_dir/share/config
done

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%_datadir/%ooo_home

%changelog
* Fri Aug 29 2008 pmladek@suse.cz
- added build dependency on exact version of
  OpenOffice_org-icon-themes-prebuilt to avoid inconsistent state [bnc#421559]
* Wed Aug 27 2008 pmladek@suse.cz
- updated to version 3.0.0.2
* Thu Aug 21 2008 pmladek@suse.cz
- updated to version 3.0.0.1
* Fri Aug 15 2008 pmladek@suse.cz
- updated to version 3.0.0
* Fri Jul 18 2008 pmladek@suse.cz
- updated to version 2.99.25
* Thu Jul 03 2008 pmladek@suse.cz
- updated to version 2.99.21
- updated for the new three layer structure (basis subdir dir)
* Tue Jun 24 2008 pmladek@suse.cz
- updated to version 2.4.1.7
* Thu Jun 05 2008 pmladek@suse.cz
- updated ooo-build to version 2.4.0.14
* Thu May 22 2008 pmladek@suse.cz
- updated ooo-build to version 2.4.0.13
* Fri May 09 2008 pmladek@suse.cz
- updated ooo-build to ooo-build-2.4.0.10
* Thu Apr 24 2008 pmladek@suse.cz
- updated ooo-build to ooo-build-2.4.0.8
* Mon Apr 14 2008 pmladek@suse.cz
- split from the OpenOffice_org-i18n source package to get it faster
