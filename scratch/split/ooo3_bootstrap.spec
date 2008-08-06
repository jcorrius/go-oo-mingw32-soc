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

%define         distro     SUSE
%define         piece bootstrap
Name:           ooo3_bootstrap
BuildRequires:  ImageMagick ant bison boost-devel ccache cups-devel curl-devel db-devel flac-devel flex gnome-vfs2-devel gperf gtk2-devel icu kdelibs3-devel krb5 libexif mono-devel neon-devel openldap2-devel pam-devel perl-Archive-Zip perl-Compress-Zlib python-devel recode unixODBC-devel unzip xml-commons-apis xorg-x11 zip
%ifarch %ix86
BuildRequires:  java-devel
%else
%if %suse_version > 1020
# We have switched to the commercial java because of n#290807
BuildRequires:  java-devel
%else
BuildRequires:  gcc-java java-1_4_2-gcj-compat-devel
%if %suse_version > 1010
# openSUSE 10.2 gcj related features
BuildRequires:  fastjar gcc-gij
%endif
%endif
%endif
%if %suse_version == 1000
# there were missing dependencies in xerces-j2 and more java packages on SL 10.0
BuildRequires:  update-alternatives
%endif
# mozilla stuff
%if %suse_version > 1010
BuildRequires:  mozilla-xulrunner181-devel
%else
%if %suse_version > 1000
# for SL 10.1
BuildRequires:  gecko-sdk mozilla-xulrunner
%else
# for BuildService on SL 10.0
BuildRequires:  mozilla-devel mozilla-nss mozilla-nss-devel
%endif
%endif
# 10.3 features
%if %suse_version > 1020
BuildRequires:  libicu-devel
%endif
# 11.0 features
%if %suse_version > 1030
BuildRequires:  hunspell-devel libwpd-devel libwpg-devel libwps-devel
%endif
# CODE10 features
%if %suse_version > 1000
BuildRequires:  gstreamer010-plugins-base-devel sablot-devel xalan-j2 xerces-j2 xml-commons-apis
%endif
# hack to build SDK on 10.2-x86_64 and SLED10-x86_64
%if %suse_version == 1020 || %sles_version == 10
%ifarch x86_64
BuildRequires:  gjdoc
%endif
%endif
# FIXME: part of the language-dependant stuff is still built here and
# put into the -devel package
# test_build_langs:   	   0 = only en-US
#			   1 = en-US, de, and cs
#			   2 = all supported languages 
%define		test_build_langs 	2
# test_build_binfilters:   0 = no
#			   1 = yes
%define		test_build_binfilters 	1
# test_build_SDK:  	   0 = no
#			   1 = yes (if possible, see below the definition of ooo_build_sdk)
%define		test_build_sdk 		1
# build_module_in_parallel 0 = no
#			   1 = yes (the number is defined be the number of cpus)
#			   2,3,4... = yes (force the number of modules to be built in parallel)
%define		build_module_in_parallel	0
# prepare_build:	   0 = no
#			   1 = yes (just build, install and create packages; without %%prep section and configure)
%define		prepare_build 		1
#
%if %test_build_langs == 0
%define       ooo_langs		"en-US"
%else
%if %test_build_langs == 1
%define       ooo_langs		"en-US cs de hu ja"
%else
%define       ooo_langs		"en-US af ar be-BY bg br ca cs cy da de el en-GB en-ZA es et fi fr ga gl gu-IN he hi-IN hu hr it ja ka km ko lt mk nb nl nn nr pa-IN pl pt pt-BR ru rw sh sk sl sr ss st sv tg th tr ts uk ve vi xh zh-TW zh-CN zu"
%endif
%endif
%define       ooo_poor_help_localizations "af bg be-BY br ca cy el fi ga he hr ka lt mk nb nn nr rw sh sr sk ss st tg th tr th ts uk ve vi xh zu"
# sdk stuff enable only on SL 10.2, SLED10 and higher
# force no if test_build_sdk is disabled
%if %test_build_sdk == 0
%define		ooo_build_sdk 		no
%else
%if %suse_version > 1010 || %sles_version == 10
%define		ooo_build_sdk 		yes
%else
%define		ooo_build_sdk 		no
%endif
%endif
# java stuff
%define	      with_java		yes
%ifarch %ix86
%define	      use_gij		no
%else
# x86_64 ppc
%if %suse_version > 1020
%define	      use_gij		no
%else
%define	      use_gij		yes
%endif
%endif
# java target bytecode; should be compatible with the gcj
%if %suse_version > 1030
%define       java_target_version	1.5
%else
%define       java_target_version	1.4
%endif
%define	      ooo_bin_suffix	no
# 
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo-3
%define	      ooo_build_dir	build
%define	      ooo_build_version 2.99.25
%define	      ooo_build_tag     dev300-m28
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
Version:        3
Release:        8
AutoReqProv:    on
PreReq:         coreutils /usr/bin/update-mime-database
PreReq:         %{?suseconfig_fonts_prereq:%suseconfig_fonts_prereq}
Summary:        A Free Office Suite (Framework)
Url:            http://www.openoffice.org/
#Url-help:    ftp://ftp.services.openoffice.org/pub/OpenOffice.org/contrib/helpcontent/
Source0:        ooo-build-%ooo_build_version.tar.gz
Source1:        %ooo_build_tag-bootstrap.tar.bz2
Source2:        setup_native_packinfo-m25.tar.bz2
#
# patches against the upstream sources, must be -p0
# Patch1000:    bug-123456-fix-foo.diff		# example
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
bootstrap !

%package devel
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Summary:        A Free Office Suite (Framework)
Group:          Productivity/Office/Suite
# FIXME: need until the devel package duplicates all the libraries
AutoReqProv:    off

%description devel
devel package


%prep
%setup -q -n ooo-build-%ooo_build_version
#
# apply patches against ooo-build
# %patch0	# example
install -m 644 %{S:1} %{S:2} src/

%build
%if %prepare_build != 0
#
# Parallel build settings ...
PARALLEL_BUILD=
if test -n "%{?jobs:%jobs}" -a -e "/opt/icecream/bin/gcc" -a -e "/opt/icecream/bin/g++" ; then
  PARALLEL_BUILD="--with-gcc-speedup=icecream
                  --with-max-jobs=%jobs"
fi
# parallel build of modules; it makes sense on machines with more CPUs
if test "%build_module_in_parallel" = "1" ; then
    PARALLEL_BUILD="$PARALLEL_BUILD --with-num-cpus=`grep ^processor /proc/cpuinfo | wc -l`"
fi
if test "%build_module_in_parallel" -gt "1" ; then
    PARALLEL_BUILD="$PARALLEL_BUILD --with-num-cpus=%build_module_in_parallel"
fi
#
# Distro settings ...
%if %suse_version <= 1030
    suse_major_ver=$((%suse_version / 100))
    suse_minor_ver=$((%suse_version / 10 - $suse_major_ver * 10))
    with_distro="SUSE-$suse_major_ver.$suse_minor_ver"
%else
%if %suse_version > 1030 && %suse_version <= 1110
    with_distro="SUSE-11.0"
%else
    with_distro="SUSE"
%endif
%endif
#
# run autoreconf to make sure that everything works correctly on current system
# also the autoconf stuff might have been patched
autoreconf -fi
%endif
# make sure that JAVA_HOME is set correctly
source /etc/profile.d/alljava.sh
# use RPM_OPT_FLAGS, ...
export ARCH_FLAGS="`echo $RPM_OPT_FLAGS|sed -e 's/\-O[0-9]//g'` -fno-strict-aliasing"
# FIXME: genccode from the system icu is in /usr/sbin
export PATH="$PATH:/usr/sbin"
# FIXME: Sun Java is borken on 10.2-x86_64, see https://bugzilla.novell.com/show_bug.cgi?id=219982
#        javac from java-1_4_2-gcj-compat-devel uses ecj that is not able to compile OOo
#        gij requires JAVA_HOME=/usr
#        ant requires tools.jar from /usr/lib64/jvm/java/lib
%ifarch x86_64
%if %suse_version == 1020
export CLASSPATH=$JAVA_HOME/lib/tools.jar
%endif
%endif
%if %prepare_build != 0
# FIXME: disabled mono for now (piece-wise build) - lots of cli breakage !
./configure $PARALLEL_BUILD \
	--with-split \
	--with-piece=bootstrap \
	--disable-mono \
	--libdir=%{_libdir} \
	--prefix=%{_prefix} \
	--mandir=%{_mandir} \
	--sysconfdir=/etc \
	--with-docdir=%{_docdir}/%{name} \
	--with-binsuffix=%ooo_bin_suffix \
	--with-installed-ooo-dirname=%ooo_home \
	--with-distro="$with_distro" \
%if %suse_version <= 1030
	--with-intro-bitmaps="%{S:40} %{S:42}" \
	--with-about-bitmaps="%{S:41} %{S:43}" \
%endif
	--with-tag=%ooo_build_tag \
	--with-lang=%ooo_langs \
	--with-poor-help-localizations=%ooo_poor_help_localizations \
	--with-mono-gac-root=%{_prefix}/lib \
%if %test_build_binfilters != 0
	--enable-binfilter \
%endif
%if %with_java == no
	--with-java=no \
	--with-jdk-home="" \
%else
%if %use_gij == yes
	--with-java=gij \
	--with-jdk-home=/usr \
%else
	--with-java=java \
	--with-jdk-home=$JAVA_HOME \
	--with-java-target-version=%java_target_version \
%endif
%endif
%if %suse_version < 920 && %sles_version <= 8
	--disable-openldap \
	--disable-gnome-vfs \
	--disable-gtk \
%endif
	--disable-access \
	--disable-post-install-scripts \
	--disable-strip \
	--enable-openxml \
	--enable-split-app-modules \
	--enable-split-opt-features
%endif
# FIXME: will be obsolete after we are able to build res files separately
export HELPCONTENT_EN_US_ONLY=YES
#
# FIXME: try to finish build even with the strange random build crash in officecfg
make

%install
%ifarch x86_64 ppc
%if %suse_version <= 1020
# FIXME: move this to ooo-build
export JAVA_HOME=/usr
%endif
%endif
# FIXME: will be obsolete after we are able to build res files separately
export HELPCONTENT_EN_US_ONLY=YES
export NOARCH_HELP=YES
make DESTDIR=$RPM_BUILD_ROOT install

# FIXME: strip binaries in the devel package now to do not bloat the debuginfo package, n#300679
# it is only temporary and quite unusable devel package anyway now
%clean
rm -rf $RPM_BUILD_ROOT

%post
%preun
%postun

%files
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home/solver
%dir %ooo_prefix/%ooo_home/solenv
%dir %ooo_prefix/%ooo_home/ooo-build
%ooo_prefix/%ooo_home/solver/*
%ooo_prefix/%ooo_home/solenv/*
%ooo_prefix/%ooo_home/ooo-build/*

%changelog
