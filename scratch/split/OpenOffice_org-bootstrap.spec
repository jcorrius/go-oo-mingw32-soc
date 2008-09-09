#
# spec file for package OpenOffice_org (Version 3.0.0)
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


Name:           OpenOffice_org-bootstrap
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
%if %suse_version > 1100
# libexpat-devel is explicitely needed since openSUSE-11.1
BuildRequires:  libexpat-devel
%endif
# poppler is needed to build the PDF import extension
# the needed version 0.8.0 has been avaiable only since openSUSE-11.0
%if %suse_version >= 1100
BuildRequires:  libpoppler-devel
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
# FIXME: part of the language-dependant stuff is still built here and
# put into the -devel package
# test_build_langs:   	   0 = only en-US
#			   1 = en-US, de, and cs
#			   2 = all supported languages 
%define		test_build_langs 	1
# test_build_binfilters:   0 = no
#			   1 = yes
%define		test_build_binfilters 	1
# test_build_SDK:  	   0 = no
#			   1 = yes (if possible, see below the definition of ooo_build_sdk)
%define		test_build_sdk 		1
# build_module_in_parallel 0 = no
#			   1 = yes (the number is defined be the number of cpus)
#			   2,3,4... = yes (force the number of modules to be built in parallel)
%define		build_module_in_parallel	1
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
%define	      use_gij		no
# java target bytecode; should be compatible with the gcj
%if %suse_version > 1030
%define       java_target_version	1.5
%else
%define       java_target_version	1.4
%endif
# we would like to support also a non-conflicting OOo2 package
%define	      ooo_bin_suffix	no
# 
%define	      ooo_prefix	%_libdir
%define	      ooo_home		ooo3
%define	      ooo_base_dir	basis3.0
%define	      ooo_build_dir	build
%define	      ooo_build_version 3.0.0.2b
%define	      ooo_build_tag     ooo300-m4
License:        Artistic License; BSD 3-Clause; GPL v2 or later; LaTeX Public License (LPPL); LGPL v2.1 or later; LGPL v3 only; MOZILLA PUBLIC LICENSE (MPL/NPL); X11/MIT
Group:          Productivity/Office/Suite
Version:        3.0.0.2
Release:        1
AutoReqProv:    on
Summary:        OOo bootstrap
Url:            http://www.openoffice.org/
#Url-help:    ftp://ftp.services.openoffice.org/pub/OpenOffice.org/contrib/helpcontent/
Source0:        ooo-build-%ooo_build_version.tar.gz
Source1:        %ooo_build_tag-bootstrap.tar.bz2
Source2:        setup_native_packinfo-ooo300-m4.tar.bz2
# necessary for SDK, http://udk.openoffice.org/common/man/spec/transparentofficecomponents.html
# FIXME: can be built using mingw32 C++ compiler
Source3:        unowinreg.dll
#
# patches against ooo-build
# Patch0:       ooo-build-2.4-enable-foo.diff	# example
Patch0:         ooo-build-pack-noarch.diff
Patch1:         ooo-build-helpcontent-en-us-only.diff
Patch2:         ooo-build-help-in-usr-share.diff
Patch3:         ooo-build.diff
Patch4:         ooo-build-1.diff
Patch5:         ooo-build-2.diff
#
# patches against the upstream sources, must be -p0
# Patch1000:    bug-123456-fix-foo.diff		# example
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
OOo bootstrap




%prep
%if %prepare_build == 0
%setup -D -T -q -n ooo-build-%ooo_build_version
%else
%setup -q -n ooo-build-%ooo_build_version
#
# apply patches against ooo-build
# %patch0	# example
#
install -m 644 %{S:1} %{S:2} %{S:3} src/
# apply patches against ooo-build
# %patch0	# example
%patch0
%patch1
%patch2
%patch3
%patch4
%patch5
chmod 755 bin/piece/file-list-*
chmod 755 bin/piece/install-l10n*
chmod 755 bin/piece/sort-l10n
chmod 755 bin/piece/link-to-ooo-home
chmod 755 bin/piece/merge-file-lists
chmod 755 bin/piece/post-inst-*
chmod 755 bin/piece/save-noarch
%endif

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
%if %suse_version > 1030 && %suse_version <= 1100
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
./configure $PARALLEL_BUILD \
	--with-split \
	--with-piece=bootstrap \
	--libdir=%{_libdir} \
	--prefix=%{_prefix} \
	--mandir=%{_mandir} \
	--sysconfdir=/etc \
	--with-docdir=%{_docdir}/%{name} \
	--with-binsuffix=%ooo_bin_suffix \
	--with-installed-ooo-dirname=%ooo_home \
	--with-distro="$with_distro" \
%if %suse_version <= 1030
	--with-intro-bitmaps="%{S:140} %{S:142}" \
	--with-about-bitmaps="%{S:141} %{S:143}" \
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
%if %ooo_build_sdk == yes
	--enable-odk \
%endif
%if %suse_version < 920 && %sles_version <= 8
	--disable-openldap \
	--disable-gnome-vfs \
	--disable-gtk \
%endif
%ifnarch %ix86 x86_64
	--disable-mono \
%endif
%if %suse_version <= 1020
	--disable-mono \
%endif
	--disable-access \
	--disable-post-install-scripts \
	--disable-strip \
	--enable-openxml \
	--disable-report-builder \
	--enable-build-noarch \
	--enable-split-app-modules \
	--enable-split-opt-features
%endif
# FIXME: try to finish build even with the strange random build crash in officecfg
make || make || make || make


%install
%ifarch x86_64 ppc
%if %suse_version <= 1020
# FIXME: move this to ooo-build
export JAVA_HOME=/usr
%endif
%endif
#
make DESTDIR=$RPM_BUILD_ROOT install

%clean
rm -rf $RPM_BUILD_ROOT

%files
%defattr(-,root,root)
%dir %ooo_prefix/%ooo_home
%dir %ooo_prefix/%ooo_home/solver
%dir %ooo_prefix/%ooo_home/solenv
%dir %ooo_prefix/%ooo_home/ooo-build
%ooo_prefix/%ooo_home/solver/*
%ooo_prefix/%ooo_home/solenv/*
%ooo_prefix/%ooo_home/ooo-build/*


%changelog
* Fri Aug 15 2008 pmladek@suse.cz
- updated to the milestone ooo300-m4 (3.0-betaX)
- updated ooo-build to version 3.0.0 (still beta):
  * support rotated ellipses in WPG files
  * Kohei's solver is back available
  * two new 3D transitions
  * more on the layout stuff
  * fix KDE fpicker to work with 3-layer OOo (bnc#413475)
  * update splash for new OSL_PIPE protocol
  * dash array parsing in SVG import
  * honour custom shape capabilities
  * missing text in GTK dialogs
  * messed font transform on EPS export
  * incorrect interpretation of color components
  * slideshow crasher with the GTK plugin (i#91496)
  * more on the localized arg separator stuff (n#358558, n#373479)
  * datapilot dialog and table insertion failure (bnc#408934)
  * support changeable separators in formula expression
  * chart subtitle export to XLS (bnc#411855, i#92357)
  * filtering by page fields with dates (i#90022)
  * cell with mixed font content (bnc#409448)
  * XLSX import crasher (bnc#413456)
  * filter on date issue (bnc#414303)
  * Excel formula parser fix (bnc#407807)
  * regenerate password hashes when necessary
  * cell format toggling improvement (i#86377)
  * leaving "Format code" blank crasher (bnc#416837)
  * external defined names in Calc (i#3740, i#4385, bnc#355685)
  * update libwpg to version 0.1.3
  * fixed to find help and icons in /usr/share
  * set paths to system dictionaries
  * VBA bits:
  * more on user forms and even handling
  * "IF Not r Is Nothing Then" issue (bnc#407805)
  * Replace doesn't replace globally (bnc#411203)
  * simple unowrapper for the GraphicObject
  * Painterface and PrintArea method fixes
  * some keywords can be used as variable
  * "Sub Test( Name as String )" support
  * Range.Find doesn't work as expected
- updated libwpg to version 0.1.3
- removed obsolete lp_solve sources
- added Kohei's Calc solver sources, snapshot 2008-08-13
- added dependency on the exact version of the icon themes
- added hack to hide dependecy on the internal redland (bnc#411538)
- updated branding for OOo-3.0 and older distributions (bnc#409144)
- enabled mono again
* Fri Jul 18 2008 pmladek@suse.cz
- updated to the milestone dev300-m25 (3.0-alphax)
- updated ooo-build to version 2.99.25:
  * more on SVG import
  * lots improvements of the layout stuff
  * more on the WebDAV locking (bnc#403724)
  * more on automatic font shrinking in text boxes in Impress
  * section break in Word8 documents (bnc#405071, i#91395)
  * skip data in hidden cells when rendering charts (bnc#404190, i#81209)
  * default to bigger handles in Draw/Impress
  * enabled the new 3D transitions again
  * VBA bits:
  * more on the user forms and event handling
  * lcl_makeXRangeFromSheetCellRanges and no cellranges
  * support Worksheet.EnableSelection (bnc#405312)
  * support Worksheet.HPageBreaks (bnc#405312)
  * support Pagesetup.Orientation (bnc#405310)
  * support ListBox.removeItem (bnc#405306)
  * support Application.Union (bnc#405313)
  * support Worksheet.Index (bnc#403974)
  * support range.find (bnc#405304)
  * add SpinButton (bnc#405308)
- updated spec for the more split sources
- temporary disabled the mono support until it builds again
- temporary used internal boost for openSUSE <= 10.3 until it builds
  with the system one again
- used java-devel instead of gcj even for 10.2-x86_64 and sles10-x86_64
- removed obsolete BuildRequires hacks for SL 10.0
* Wed Jul 02 2008 pmladek@suse.cz
- updated to the milestone dev300-m21 (3.0-alphax)
- updated ooo-build to version 2.99.21:
  * Excel export encryption support (i#60305, i#71468, i#84766)
  * WP file-formats password protection
  * CJK character unit is back again
  * slideshow media control (i#91250)
  * allow to insert SVGs into documents
  * shrink font automatically in text boxes in Impress
  * Macro Nudge, 10x nudge function (i#91249, bnc#188199)
  * don't lstat() that much while creating absolute URLs
  * lots improvements in the SVG import filter
  * lots fixes in the data pilot (i#83250, i#88473, i#88531)
  * lots improvements of the layout stuff
  * EMF+ import fixes (bnc#361534)
  * better support for various formula syntax
  * FREQUENCY function regression (i#88477)
  * better highlighting of current row
  * bin check for incompatible ODF version
  * no source border when doing cut [Jon]
  * opening a tab delimited CSV file (bnc#376473)
  * tab color of selected and unselected sheets (bnc#386029)
  * allow special characters in sheet names (i#6087)
  * INDIRECT and Text as numbers calculation error (bnc#391330)
  * center content when toggling merge cells (bnc#398243, lp#236378)
  * column/row breaks and fit to x pages (bnc#404232, bnc#404563)
  * rename the obsolete SFX_NOTIFY to Notify (i#89804)
  * better fix for the rounding problems (i#86775)
  * cutting cell and moving cursor (bnc#388770)
  * page format in print preview (bnc#386052)
  * warn when inserting a linked graphic (bnc#348149)
  * deselected inplace OLE objects (i#89283, bnc#384506)
  * easy moving of Draw/Impress text frames (bnc#384563)
  * honour XDG_CONFIG_HOME environment variable (i#91247)
  * "Animate attached Shape" check box (bnc#380036, i#69364)
  * hidden slides in custom slideshow (bnc#355638, i#90145)
  * hyperlinks with numeric slide names (bnc#355674, i#55224)
  * handle multipart object records (bnc#385338)
  * export to MS Word Doc crasher (bnc#391197)
  * documents with form fields crasher (bnc#390953)
  * better conformance to DIS26300 (ODF) (bnc#396280)
  * object positioning in tables (bnc#367341, i#18732)
  * Word document set to Read Only (i#90306)
  * title style cannot set alignment (i#23221)
  * WebDAV locking timeout (bnc#403724)
  * fixed crash in RandR (bnc#398244, i#90809)
  * open document with trailing space in folder name (bnc#277603)
  * do not lost pictures added by drag and drop (bnc#348149)
  * allow to change broken java configuration (i#83140, bnc#393719)
  * bash completion for writer and master documents (bnc#257299)
  * mozilla plug-in crashes on x86_64 (i#82545)
  * shrinked french localization (bnc#340679, i#89475)
  * loading and storing SMB hyperlinks (bnc#382718)
  * hyperlinks without scheme (bnc#376389, i#91105)
  * avoid unnecessary dns lookup (bnc#389257)
  * Chinese forbidden characters
  * VBA bits:
  * lots improvements of user forms and even handling
  * workbook/worksheet events and event handle class
  * support multiple array indices (i#75443)
  * Worksheet_SelectionChange event (bnc#388049)
  * delegate security check to the basic manager
  * PageSetup interface and PrintArea method
  * brackets and array syntax handling
  * hyperlink service and interface
  * non-working mediawiki extension
  * switch on VBA import by default
  * myFunction Not bBool (bnc#397325)
  * empty types comparison (bnc#397438)
  * support Range.ShowDetail, Range.MergeArea
  * osx button names not imported (bnc#359933)
  * packaging of extensions that we build it
- added ant-apache-regexp to BuildRequires
- switched to mozilla-xulrunner190 for openSUSE > 11.0
- changed ooo-home from ooo-2.0 to ooo3
- started to require 3.x branding
- split en-US localization and help content into separate packages; moved
  them to the l10n-group1 source package
- added extra extsw source with extensions
- updated for the new three layer structure (basis subdir dir)
- installed the KDE templates also for KDE4 on openSUSE > 10.1
- added symlink for acor_fr-CA.dat until we have a more generic solution for
  the autocorrection fallback (bnc#364523)
* Tue Jun 24 2008 pmladek@suse.cz
- updated to the milestone ooh680-m17 == 2.4.1rc2 == 2.4.1-final
- updated ooo-build to version 2.4.1.6:
  * fixed crash in RandR (bnc#398244, i#90809)
  * fixed doc protection problem (bnc#400884)
  * center content when toggling merge cells (bnc#398243, lp#236378)
  * round corners rectangle drawing (bnc#361534)
  * mozilla plug-in crashes on x86_64 (i#82545)
  * fixed crash in svg importer
  * export to MS Word Doc crasher (bnc#391197)
  * hidden slides in custom slideshow (bnc#355638, i#90145)
  * save background image of slide master (i#87071)
  * unopkg --share when running OOo (i#86080)
  * better conformance to DIS26300 (ODF) (bnc#396280)
  * cannot create new forms (i#90011)
  * Find and Replace with formatting inserts unwanted "x" (i#87092)
  * crash when calling the mono test case (i#90076)
  * hyperlinks with numeric slide names (bnc#355674)
  * more improvements in the SVG import
  * allow to insert SVGs into documents
  * loading and storing SMB hyperlinks (bnc#382718)
  * DOC import to show "hidden text" (i#53885)
  * easy moving of Draw/Impress text frames (bnc#384563)
  * lots improvements in the SVG import
  * really enable the fix for xinerama & multiscreen (bnc#359955)
  * VBA stuff:
  * Worksheet change event support (bnc#390082, bnc#391880)
  * PageSetup object and PrintArea method (bnc#390080)
- updated the prebuilt cli_types.dll for older distributions
* Thu Jun 05 2008 pmladek@suse.cz
- updated ooo-build to version 2.4.0.14:
  * export to MS Word Doc crasher (bnc#391197)
  * save background image of slide master (i#87071)
* Thu May 22 2008 pmladek@suse.cz
- updated ooo-build to version 2.4.0.13:
  * allow to change broken java configuration (i#83140, bnc#393719)
  * registry store crasher; found when compiling on Solaris
  * documents with form fields crasher (bnc#390953)
  * INDIRECT and Text as numbers calculation error (bnc#391330)
  * unable to mix fonts in a single cell (bnc#391818, i#89598)
  * avoid unnecessary dns lookup (bnc#389257)
  * shrinked french localization (bnc#340679, i#89475)
  * really disable the registration dialog (bnc#391477)
  * bash completion for writer and master documents (bnc#257299)
  * split configuration for openSUSE-11.0
  * improvements of the SVG import
  * symbol font problem (bnc#383812)
  * cutting cell and moving cursor (bnc#388770)
  * deselected inplace OLE objects (i#89283, bnc#384506)
- updated the with_distro option for the split configuration
- enabled the mono subpackage again [bnc#357054]
- enabled preinstalling the mono GAC
- used Requires instead of PreReq in the -mono subpackage because
  mono-core is not longer needed in the %%post script
* Fri May 09 2008 pmladek@suse.cz
- updated ooo-build to ooo-build-2.4.0.10:
  * linked graphics in SVG import
  * tab color of selected and unselected sheets (bnc#386029)
  * warn when inserting a linked graphic (bnc#348149)
  * bin check for document generated by never OOo version
  * more fixes of the datapilot drill-down stuff (i#88531)
  * default character set in CSV import dialog (bnc#376473)
  * better fix for xinerama & multiscreen (bnc#359955)
  * better fix for "Animate attached Shape" checkbox
  * allow to run the mediawiki extension (bnc#361061, i#85804)
  * crash during printing (i#88818)
  * VBA bits:
  * Worksheet_SelectionChange event (bnc#388049)
* Thu Apr 24 2008 pmladek@suse.cz
- updated ooo-build to ooo-build-2.4.0.8:
  * "Animate attached Shape" checkbox (bnc#380036, i#69364)
  * FREQUENCY function regression (i#88477)
  * datapilot drill-down regressions (i#83250, i#88473)
  * OpenGL stuff with ATI regression
  * more layout dialog stuff fixes and improvements
  * stability improvements (i#88002, i#88005)
  * unable to add patch for templates (i#85931)
  * random spellcheck dialog behavior (bnc#377345, i#88120)
  * disabled menu items in quickstarter by mistake (i#88382)
  * problems with xinerama & multiscreen (bnc#359955)
  * crasher when attempt to save read-only document (i#5226)
  * Arabic/Hindi numbers instead of decimal numbers (i#86811)
  * scroll speed when extending cell range (bnc#375909, i#71362)
  * default character set in CSV import dialog (bnc#376473)
  * font attributes at entire cell level (bnc#374580)
  * crasher with non closed polygons (bnc#361059)
  * gradients in SVG import filter
  * VBA bits:
  * regression with Range.AutoOutline
  * crasher when opening dialogs (bnc#379844)
  * Application.Goto returns BASIC runtime error '1' (bnc#309981)
  * new configure option --with-additional-sections for L3 team
  * use --with-system-hunspell, --with-system-icu, --with-system-libwpd,
      --with-system-libwpg, --with-system-libwps for openSUSE-11.0
- added back the extra localizations sources; were removed by mistake
  (bnc#335643)
- started to generate Java 1.5 bytecode on openSUSE-11.0
- fixed build dependency in the oox module
- better support for L3 team:
  * use autoreconf instead of autoconf to work even with ooo-build generated
  on newer system
  * sample comments where and how to put extra patches
* Mon Apr 14 2008 pmladek@suse.cz
- updated ooo-build to ooo-build-2.4.0.6:
  * disable problematic measurement unit stuff (bnc#376788)
  * more on the OpenGL stuff
  * modified graphic elements were not visible (i#86163)
  * non-working oofromtemplate (i#60696, bnc#375529)
  * more on the layout dialog stuff
  * update and sort mime-types
  * Categories=Office;Math; in math.desktop
  * VBA bits:
  * revert the support worksheet_deactivate events stuff
- fixed dependency of the branding-upstream package; it should get installed
  when both the branding and the OpenOffice_org package are requested
- fixed random spellcheck dialog behavior (bnc#377345, i#88120)
- added one more fix for the openGL transitions on ATI (bnc#373076)
- enabled system libwpd, libwps, libwpg, hunspell for openSUSE >= 11.0
- added libwpd-devel libwpg-devel libwps-devel hunspell-devel to BuildRequires
  for openSUSE >= 11.0
- created a hacky icon-themes-prebuilt package to speed up the build of the
  real architecture independent icon-themes package
- added "hu ja" to the min language selection to support the split i18n-group2
  package
* Fri Mar 28 2008 pmladek@suse.cz
- updated to the milestone ooh680-m12 (2.4rc6 == 2.4-final)
- updated ooo-build to ooo-build-2.4.0.5:
  * better support for various formula syntax
  * left-mouse click and protected cell crasher [n#371204]
  * cell background painting improvements [n#361360, i#3907]
  * wrong result of interactive functions, e.g MIN [n#372255]
  * better floating-point rounding error fix [n#310706, i#86775]
  * more on the localized arg separator stuff [n#358558, n#373479]
  * detect the symlinked mozilla wrapper correctly [n#368532, i#87258]
  * interference between R1C1 and localized arg separator stuff [n#372494]
  * visual feedback for copied cell range [n#367489]
  * keys that quits the paste on Enter mode [n#372446]
  * output range format in the datapilot regression
  * drag&drop with plus/minus signs in Navigator [i#64886, n#129410]
  * unopkg wrapper and association with .oxt [n#352574]
  * VBA bits:
  * support worksheet_deactivate events
- updated branding packaging to the openSUSE >= 11.0 style:
  * created branding-upstream subpackage
  * moved openSUSE and SLED10 branding to extra packages
  * kept compatibility when building for older distros
* Thu Mar 13 2008 pmladek@suse.cz
- updated to the milestone ooh680-m11 (2.4rc5) [fate#303312]
- updated ooo-build to ooo-build-2.4.0.3:
  * improvements of the OpenGL slide transitions
  * paste on Enter in Calc crash
  * highlighting of pasted cell regression [i#86894]
  * more on navigating in the selected cells [n#364351]
  * skip overlapped cells when navigating [n#362674, i#86943]
  * bin dangerous floating-point rounding error fix [n#310706]
  * keyboard configuration migrations during update [n#367160]
  * background of highlighted cell regression [n#361360, i#3907]
  * better icons for toggle sheet grid lines command [n#358550]
  * prefer odf-converter over the internal OOX filters [n#348471]
  * no autocorrection when "!" has been typed [i#86580]
  * text layout enhancement for better CJK support
  * several char unit issues [n#356101]
  * fallback to original 2D transitions
  * more Hungarian localization fixes
  * unlock WebDAV stream when OOo crashes
  * send documents via GroupWise [n#368044, i#86886]
  * send filenames with spaces via sylpheed-claws [n#362773, i#86887]
  * VBA bits:
  * better support for passing UNO Integer params to Range.Cells
  * bin problematic NULL behavior compatibility with MSO fix
  * improvements of the XInterior API [n#365557]
  * fix for Autofit
  * fix enumeration of Range.Rows()
  * MSO Userform controls fix [i#87007]
- removed obsolete postinstall script for the -mono subpackage
- used weak dependency (Recommends) also for the myspell-american dictionary
* Thu Mar 06 2008 pmladek@suse.cz
- updated to the milestone ooh680-m9 (2.4rc3) [fate#303312]
- updated ooo-build to ooo-build-2.4.0.2:
  * more on the SVG import
  * more on the localized arg separator [bnc#358558]
  * more on the paste on Enter behavior in Calc [bnc#358545]
  * better solution for the dynamic range resize [bnc#352662]
  * reduce the extra margins around cell text [bnc#361358]
  * pasting with the middle button in Calc [bnc#282767]
  * navigating in the selected cells [bnc#364351]
  * floating-point rounding error [bnc#310706]
  * input fields crashes [bnc#257496]
  * transparent bitmaps handling in cairocanvas
  * prefill username in WebDAV authentication dialog [bnc#363363]
  * build with xulrunner-1.9
  * various build fixes
* Mon Mar 03 2008 pmladek@suse.cz
- updated ooo-build to ooo-build-2.4.0.1:
  * update XML filter stuff to the state that passed QA and will get
  integrated upstream
  * CAPS lock correction also for Writer/Draw/Impress [bnc#364350]
  * displaying pictures with alpha channel in Cairo canvas
  * VBA bits:
  * XInterior API
  + updated oox and writerfilter to the snapshot 2008-02-29
  * include lots fixes and improvements
  * corresponds to the QA-ed XML filter stuff
* Thu Feb 28 2008 pmladek@suse.cz
- updated to the milestone ooh680-m8 (2.4rc2)
- updated ooo-build to ooo-build-2.4.0:
  * redirect extensions site via go-oo.org
  * lots improvements of the SVG import
  * OpenGL slide transitions stuff fixes
  * WebDAV locking fixes
  * Berkeley db exception handling
  * reference to a blank cell error
  * togglable cell number format toolbar icons [bnc#358548]
  * automatic correction of accidental CAPS LOCK use [bnc#364350]
  * missing icons for toggling of cell grid lines per sheet [bnc#358550]
  * extra margins around cell text so that it's easier to read [bnc#361358]
  * don't apply patterns when we're only moving the cursor [bnc#359690]
  * parse "june-2007" as "June 1 2007" in en-US locales [bnc#358750]
  * handling objects with 0x8000 bit set in EMF+ import
  * additional paragraph imported from ww8 [bnc#364533]
  * correctly display linespacing < 100%% [bnc#364534]
  * VBA bits:
  * more Application.Goto macro fixes [bnc#349283]
  * string '+' numbers do not compute [bnc#359943]
  * update CommandBar APIs [bnc#353242]
- updated extra translation sources: be-BY, bg, cs, da, et, fi, ga, km, mk,
  sl, vi
- removed obsolete extra translation sources: hu, sh-YU, sr-CS
- fixed build dependencies:
  * used java-devel instead of the obsolete java2-devel-packages
  * added update-alternatives on SL 10.0
- fixed dependencies:
  * added weak dependency on evolution-data-server-32bit [bnc#245649]
  * used weak dependency on jre-32 on ppc as well
- fixed build:
  * do not build all help localizations in the devel package
  * build on PPC with gcc-4.3
  * temporary disabled automatic correction of accidental CAPS LOCK fix;
  made problems in STABLE/FACTORY
* Thu Feb 21 2008 pmladek@suse.cz
- updated ooo-build to ooo-build-2.3.99.5:
  * improvements in the OpenGL slide transitions stuff
  * improvements in the datapilot dynamic range stuff
  * improvements in the t602 import stuff
  * improvements in the text grid stuff
  * fast tokenizer in OOX import filter fixes
  * allow to save even unmodified documents [bnc#347423]
  * allow toggling of cell grid lines per sheet [bnc#358550]
  * pasting over source cell crasher [bnc#358545]
  * cell background over the gridlines [bnc#361360]
  * compatibility for handling of "string numbers" with Excel [i#5658]
  * keep read/write access over smb:// and WebDAV after reload [bnc#264140]
  * show correct size for documents accessed through gnome-vfs [bnc##264585]
  * serve dav://, davs://, webdav://, and webdavs:// URLs in the WebDAV UCP
  [bnc#346713]
  * some more fixes in the WebDAV stuff [bnc#363363, bnc#363093, bnc#357048,
  i#29152]
  * Hungarian localization fixes
  * build with neon-0.28
  * VBA bits:
  * allow to set a numeric value as Range.Formula
  * SpecialCells treatment of xlCellTypeLastCell
- fixed to create the right /usr/bin/soffice symlink in %%post
* Thu Feb 14 2008 pmladek@suse.cz
- updated to the milestone ooh680-m7 (2.4rc1) [fate#303312]
- updated ooo-build to ooo-build-2.3.99.4:
  * lots fixes in the datapilot stuff [bnc#182126, bnc#182006, bnc#181981]
  * lots fixes of the WebDAV locking [bnc#346711]
  * oosplash hangup [bnc#357659]
  * make the formula arg separator optional [bnc#358558]
  * MATCH should perform case insensitive search in Calc
  * update SCSolver dialogs to support longer localized texts
  * VBA stuff:
  * more fixes of DateSerial behavior [bnc#353254]
  * TextBox ( shape ) object selection
  * mono .dlls installation; preinstall to GAC [bnc#357348]
- updated libwpd to version 0.8.14:
  * two infinite loops with broken documents
- removed obsolete postinstall script for the -mono subpackage
* Thu Feb 07 2008 pmladek@suse.cz
- updated to the milestone ooh680-m6 (2.4-betaX) [fate#303312]
- updated ooo-build to ooo-build-2.3.99.3:
  * paste on Enter in Calc [bnc#358545]
  * allow to disable field shading
  * allow to change checkbox status in data fields via mouse [bnc#248354]
  * comma as the separator in Calc functions [bnc#358558]
  * allow to save even unmodified documents [bnc#347423]
  * find command in Impress crasher [bnc#355302]
  * regression in the export into WW8
  * letter wizard crasher [bnc#358043]
  * empty datapilot table crasher
  * VBA bits:
  * APIs for CommandBar and CommandBarControl [bnc#353242]
* Thu Feb 07 2008 pmladek@suse.cz
- enabled system icu in STABLE again
* Fri Feb 01 2008 pmladek@suse.cz
- updated to the milestone ooh680-m5 (2.4-betaX)
- updated ooo-build to ooo-build-2.3.99.2:
  * new OpenGL slide transitions
  * enabled EMF+ import and  rendering again [bnc#232232]
  * crasher when loading an Excel document [bnc#355304]
  * crasher when finding strings in Impress [bnc#347355]
  * pivot charts import [bnc#355300]
  * locking for WebDAV UCP [bnc#346711]
  * WW-like input-fields and checkboxes [bnc#248354]
  * font config hinting stuff update
  * localized default names for autoshape and other objects
  * header/footer export into WW8 [bnc#178981]
  * default formatting in PPT export [bnc#285959]
  * saving as HTML in various encodings fix
  * conversion of date/time strings in Word documents [bnc#344306]
  * import sheet names properly from Lotus 1-2-3 document [bnc#347379]
  * graph labelling of x-axis after importing an .xls [bnc#257079]
  * use of Dynamic Range Names in Validation Lists [bnc#274387]
  * "Print Preview" to follow "Print Only Selected Sheets" option [bnc#335684]
  * import/export protection options from and to an Excel
  * more shortcuts to Impress and GTK file dialog
  * locking on CIFS [bnc#333001]
  * fontconfig stuff improvements
  * undo of deleted columns
  * cut&paste in datapilot
  * MATCH function enhancement
  * FREQUENCY function behavior in Calc
  * dynamic resizing of a filtered range for the autofilter [bnc#352662]
  * SUMIF's 3rd parameter handling to be compatible with MS Excel [bnc#350716]
  * crasher with out-of-bound cell ranges
  * VBA bits:
  * conversion between bytearray and string [bnc#353254]
  * non-triggered "before close" event (Noel)
  * support for more worksheet events [bnc#339306]
  * DateSerial behavior [bnc#353254]
  * DateValue return value [bnc#353260]
  * fixed to store simple VBA objects in Word documents [bnc#339946]
  * Application.Goto macro fixes [bnc#349283]
  * better support for Application.Caller macro [bnc#339941]
  * and more
  * desktop files to follow the standard
  * support for gcj-4.2.1, gcj-4.2.2 and gcc-4.3
  * use system STL for archs like x86-64 as the default
  * removed dependency on nas, sndfile, portaudio
  * less verbose build messages
  * support for post-ooo-build-release hotfixes
  * many more fixes, improvements, and speedups
- updated oox to snapshot 2008-01-29
- updated libwps to version 0.1.2
- updated libwpg to version 0.1.2
- updated libwpg to version 0.1.2
- updated the prebuilt cli_types.dll
- moved witerfilter stuff to extra tarball, snapshot 2008-01-29
- removed the obsolete %%datadir/xml from the sdk filelist
- removed obsolete xorg-x11-Xvfb from BuildRequires
- removed unused glitz sources
- removed unused startup-notification-devel libsndfile-devel portaudio-devel
  from BuildRequires
- added mono-core to PreReq of -mono subpackage; runs gacutil in %%post
- "sr-CS" was internally renamed to "sr" and "sh-YU" to "sh"
- renamed option --with-icecream-max-jobs to --with-max-jobs
- enabled again compilation of SDK and stlport
- temporary disabled mono in STABLE until bnc#357054 was fixed
* Tue Dec 11 2007 pmladek@suse.cz
- updated ooo-build to 2.3.1.1:
  * disabled too incomplete SVG Import (Rene)
  * oosplash and relative paths and non-UTF-8 locales
  * crash on =erf(0.65)
  * string hashing table fix
  * add easier way to add hotfixes
- temporary disabled the new EMF+ support; still not enough stable
- added mono-core to PreReq of -mono subpackage; runs gacutil in %%post
* Mon Dec 03 2007 pmladek@suse.cz
- updated to milestone oog680-m9 (OOo-2.3.1rc1 == final)
- updated ooo-build to 2.3.1:
  * target oog680-m9
  * oosplash argument handling
  * Spanish translation of Y Axis
- updated the prebuilt cli_types.dll for openSUSE < 10.3 builds
* Wed Nov 21 2007 pmladek@suse.cz
- updated to milestone oog680-m8 (OOo-2.3.1.betaX)
- updated ooo-build to 2.3.0.6:
  * EMF+ import and  rendering
  * crash with some GTK themes
  * Flat XML type detection
  * open files from read-only SMB share [n#330145]
  * broken print preview [n#191005, n#257422]
  * detect removed JRE [n#336242]
  * crash with CMYK JPEGs [n#272574]
  * crash with broken TIFF pictures
  * do not lost file name when changing dir in KDE filepicker
  * MIME type and bash completion for flat ODF and WPX file types
  * keep background when saving in proprietary doc format
  * slide show displays vertical line [n#340142]
  * edit fields text displaying [n#305205]
  * MIN/MAX functions and empty cells
  * infinite loop on column insert in empty document
  * better fix for swapping delete and backspace keys in Calc
  * object positions when rows/columns are resized
  * support named ranges in INDIRECT and INDIRECT_XL functions [n#274387]
  * default numbering in X-axis of X-Y chart [n#257079]
  * Math object doesn't anchor to cell
  * allow to copy-paste on merged cells
  * lots fixes and improvements for Calc Solver
  * VBA bits:
  * sheet refs with single cell references
  * Application.Goto returns BASIC runtime error '1' [n#309981]
  * static keyword for functions and subroutines are not supported [n#306717]
  * uninitialised pointer ScXMLImport::pShapeHyperlinkTokenMap [n#336869]
  * local variables in static procedures
  * erroneous use of getCurrentDocument
  * choosing macro to run in IDE
  * import dialog failure
  * crash in TreeControl
  * "Is" behaviour
  * "Replace" method
  * "StrConv" function
  * missing functions in Window class:PointsToScreenPixelX
  * "before close" event is not triggered when file saved as ODS [n#339306]
  * saving embedded word document objects containing macros [n#339946]
  * UNO controls can't be made 'really' invisible
  * support new C++ test client
  * moved stuff to CWS npower8
  * build with gcc-4.3
  * bin unused glitz sources download
  * correct packaging of GNOME and KDE integration [n#333815]
- updated cairo to version 1.4.10 (used only for older products)
- added MIME types definition for WPX file types
- removed obsolete glitz sources
- used weak dependency (Recommends) on JRE in stead of the hard one (Requires);
  needed because of the disk space problems on single CD products
- temporary disabled SDK in STABLE/FACTORY, did not compile with gcc-4.3
- temporary disabled stlport, did not compile with gcc-4.3
- temporary used internal icu because a patched version was needed
* Thu Oct 18 2007 pmladek@suse.cz
- updated ooo-build to 2.3.0.5:
  * allow to access SMB share via KDE fpicker
  * copying across formulas from one merged cell to another
  * allow insertion of cell range that cuts across merged areas
  * Writer/WEB: Export of HTTP-EQUIV is loosing timer parameter [i#81250]
  * activating a math formula takes very long time [i#81789]
  * crash when deleting some simple text [i#82678]
  * ooqstart reworked to remove dependency on glib
  * do not accept all rows to be header rows in a WW8 document
  * more Hungarian translation for CELL and INFO functions [i#80299]
  * process relative SMB paths correctly [n#261623]
  * generate xhtml that works in IE
  * OpenDocument Presentation Flat XML type detection
  * Turkey and Russian key names
  * 64-bit problem in Excel filter
  * Calc functions via the UNO FunctionAccess service crasher [n#328496]
  * build with neon-0.24
  * VBA stuff:
  * lots fixes for shapes and shape ranges [n#294302]
  * workbook_beforeclose is not aware of codename [n#334240]
  * SaveCopyAs Property still does not work on Windows [n#288274]
  * ActiveWindow.DisplayVerticalBar implementation [n#292877]
  * Application.DisplayFormulaBar implementation [n#309965]
- updated libwpd to version 0.8.12
- added gjdoc to BuildRequires for 10.2-x86_64
- set AutoReqProv off for the tricky devel package; it provided all
  the libraries, so it was possible to install 32-bit OpenOffice_org
  with 64-bit OpenOffice-writer and OpenOffice_org-devel
- added the Supplement tag to the app subpackages to fix update from older
  distributions [n#328171]
- enabled system neon again on 10.0/10.1/SLED10
* Fri Sep 21 2007 pmladek@suse.cz
- milestone oog680-m5 has been marked OOo-2.3 final
* Thu Sep 13 2007 pmladek@suse.cz
- updated to milestone oog680-m5 (OOo-2.3.rc3) [n#309238]
- updated ooo-build to 2.3.0.1.2:
  + target oog680-m5
  + non-working Flat XML document type detection, simple typo
  + XBitmap::getSize broken in Cairo canvas [n#309764]
  + problem in clipping in Cairo canvas backend [n#309765]
  + Hungarian translation for CELL and INFO functions [n#274424]
  + updated oox to 2007-09-05; includes some important fixes
  + build dependency in reportdesign
  + fix ant home when building on 10.2
- added a workaround to find tools.jar; affects only openSUSE-10.2
* Mon Sep 03 2007 pmladek@suse.cz
- updated to milestone oog680-m3 (OOo-2.3.rc1)
- updated ooo-build to 2.3.0.1:
  * target oog680-m3
  * myspell hashify ported for hunspell
  * minor fixes in the VBA stuff
  * better check for the minimal password length [i#21923]
  * DoubleNumericField crasher [i#80796]
  * prefer Tango over the old Industrial icon theme [n#304615]
- fixed installation to move the whole English help to /usr/share (even the
  help included in subpackages)
* Fri Aug 31 2007 pmladek@suse.cz
- updated to milestone oog680-m2 (another OOo-2.3.beta, close rc1)
- updated ooo-build to oog680-m2:
  * target oog680-m2
  * search templates on more locations
  * lots optimizations of the text search operations
  * lots fixes in the layout stuff
  * lots fixes and improvements in the OOX import filters
  * export to MS format for autoshape macro bindings/associations [n#304739]
  * load condensed paragraphs from Word Perfect documents
  * Writer.append not compatible with PrintWriter.append
  * associate sheet and codename when insert a sheet
  * crash when deleting rows via autofilter [n#199224]
  * display bitmap images in presentation mode correctly [n#266082]
  * read the real help path from registry everywhere
  * display help correctly when is it symlinked
  * URLs with invalid characters [n#246629]
  * GDK hang
  * no Help->registration dialog by default
  * read the right GNOME color for menus [n#187693]
  * search /usr/lib64 for 64-bit JREs
  * updated list of gij search paths
  * fix testtool to work again [n#301439]
  * finally removed -fsigned-char on PPC [n#169875]
  * branched SUSE-10.3 configuration
- updated libwpd to version 0.8.11
  * crashes with tables where cells span more then 0x7f rows
  * incorrect reading of font information
- added the tarball with OOX import filters; all these files were added by
  patches in the previous versions of ooo-build; it was ugly...
- fixed dependency on java; any 64-bit JRE is enough on x86_64
- moved also English help to /usr/share
- configured ooo-build to use the 10.3-specific configuration
* Mon Aug 20 2007 pmladek@suse.cz
- fixed build dependencies of the desktop module
* Fri Aug 17 2007 pmladek@suse.cz
- updated to milestone oog680-m1 (another OOo-2.3.beta, close rc1)
- updated ooo-build to oog680-m1:
  * target oog680-m1
  * Excel A1 style format in hyperlink destination [n#294522]
  * some artifacts with the new optimized icons [n#232280]
  * better Chinese font substitutions [n#295561, n#295568]
  * SDK examples file permissions [n#297549]
  * gij detection improvement [n#299635]
  * do not longer prefer gij on x86_64; the hack is obsolete [n#219982]
  * backported some upcoming fixes from upstream
- fixed crash when saving in Microsoft Word 97/2000/XP file format [n#298593]
- stripped the stuff for the temporary devel package during the install phase
  to do not bloat the debuginfo package [n#300679]
* Fri Aug 10 2007 pmladek@suse.cz
- updated to milestone src680-m225 (another OOo-2.3.beta)
- updated ooo-build to src680-m225:
  * target m225
  * import/export support for Excel 97-2003 filters improvement
  * black splash screen in the quickstarter [n#296502]
  * search icons also in /usr/share [n#296502]
  * VBA stuff:
  * various badness in the collection implementations [n#293873]
  * compiler error [n#294302]
  * ANT_HOME is /usr/share/ant on openSUSE-10.3
  * use internal neon on 10.0 and 10.1 because the newer version is needed
  * use explicitly --with-system-mozilla=mozilla on 10.0
- updated libwpg to version 0.1.0; first official release; includes few
  important fixes in compare with the last snapshot
- added portaudio-devel libicu-devel to BuildRequires only for > openSUSE-10.2
- removed the unneeded dependency of the devel package on the main package;
  it fixed the dependency loop between the devel and the icon-themes subpackage
* Mon Aug 06 2007 pmladek@suse.cz
- updated the prebuilt cli_types.dll to fix build on ppc
* Fri Aug 03 2007 kendy@suse.cz
- updated to milestone src680-m224 (another OOo-2.3.beta)
- updated ooo-build to src680-m224:
  * target src680-m224
  * gcc 4.2-related fix in Writer
  * VBA update
  * CJK compatibility tab page
  * various build and dependency fixes
  * various other fixes
  * WebDAV save as
  * XML detection
  * corrupt hyperlinks on import
  * handling of generic protocols
* Fri Jul 27 2007 pmladek@suse.cz
- updated to milestone src680-m222 (another OOo-2.3.beta)
- updated ooo-build to src680-m222:
  * target src680-m222
  * expand AWT to add metric field support
  * type detection over protocols with gnome-vfs
  * VBA stuff:
  * keep codenames if they exist in sync
  * allow workbook and worksheet objects to be standalone services
  * MSGBOX macro from Excel spreadsheet not working
  * PPC build fixes
  * cli_ure and mono bridge build fix
  * various build and dependency fixes
  * enabled system icu, portaudio and xalan for openSUSE-10.3
- updated libwpg to 0.1.0~cvs20070726
- updated the prebuilt cli_types.dll
- prebuilt stuff for the the new i18n packages: br, en-ZA, ga, gl, he, ka, nr,
  sh-YU, ss, tg, th, uk, ve
- split icons into separate noarch subpackage -icon-themes [n#284980]
- removed dependencies on the release number; will rather update the package
  version when needed [fate#301975]
- added icu libicu-devel portaudio-devel to BuildRequires
- used the same ooo-build config for all architectures
* Fri Jul 20 2007 pmladek@suse.cz
- updated to milestone src680-m221 (another OOo-2.3.alpha)
- updated ooo-build to src680-m221:
  * target src680-m221
  * partial implementation of SVG import
  * partial implementation of DOCX, PPTX and XLSX import filters
  * startup speed improvements
  * use prime hash table sizes
  * lots fixes and improvements in the VBA stuff
  * lots files in the WPS import
  * button drop-offs
  * don not change current working directory in ooqstart
  * KDE file picker and file names with spaces
  * KDE file picker with non-UTF-8 locales
  * GNOME VFS and broken symlinks
  * process relative SMB paths correctly [n#261623]
  * embedded objects having old invalid MIME types
  * install-dict improvements
  * PPC build with -fPIC
  * binfilters split [n#284980]
- updated libwpg to 0.1.0~cvs20070712
- updated libwpd to 0.8.10
- added libsvg, version 0.1.4
- added back system -source tarball for <= SL 10.1; the newer version of the
  internal neon is required on these older distributions
- split the thesaurus and hyphen dictionaries into separate packages:
  OpenOffice_org-hyphen and OpenOffice_org-thesaurus-<lang> [n#284980]
- moved OpenOffice_org-galleries to the openclipart source package [n#290807]
- fixed Requires of the filters subpackage; it should require all the
  app modules that the filters are for
- removed obsolete db-java from BuildRequires; CXXHelpLinker is used now
- removed obsolete openclipart from BuildRequires
- added gperf to BuildRequires; needed by ooxml module at the build time
- used commercial java for building even on x86_64 and ppc; provoked by
  the gij memory problems [n#290807]
- used Requires instead of Recommends on SL 10.0
- used internal neon on < SL 10.1 because a newer version is required
- renamed rpmlintrc to OpenOffice_org-rpmlintrc
- fixed build dependency in the svx module
* Fri Jun 15 2007 pmladek@suse.cz
- updated to milestone src680-m211 (2.3.alpha)
- updated ooo-build to src680-m211:
  * target src680-m211
  * accelerate intern by not using stl
  * don't burn 3%% of startup doing CRC's we don't need
  * lots fixes and improvements in the scsolver stuff
  * lots fixes and improvements in the inline arrays stuff
  * lots fixes in WPG import
  * lots fixes in SVG import
  * import and export Excel's sheet protection options, and use it to
  constrain cell cursor movement [n#274482]
  * fix Calc 'PostIt' notes
  * Delete Rows/Delete Columns should remember the content
  * toolbar customization crasher
  * traversal over objects in Draw on 64bit
  * emf files reading improvement [n#232232]
  * misdrawing on 64bit
  * remove a shape from macro crashes OOo drawing document
  * translate some localized keywords in Calc
  * initial support for separate helpcontent build
  * allow to split more optional packages
  * disable climaker on ppc
- updated lp_solve to version 5.5.0.10
- updated libwpg to version 0.1.0~cvs20070608.tar.gz
- removed obsolete tango icons sources
- removed hacks for building on NLD9 and SL 9.3
- started package rework [fate#302110]
  * build the noarch stuff in separate package OpenOffice_org-i18n; created
  the -devel package for this purpose
  * split the application modules into subpackages: -base, -calc, -draw,
      -impress, -math, and -writer
  * split more optional features into separate packages: -filters, -mailmerge,
      -pyuno, -testtool
* Mon Jun 04 2007 pmladek@suse.cz
- updated to milestone oof680-m18 (2.2.1rc3)
- updated ooo-build to 2.2.0.2:
  * WPG import filter for Draw (Fridrich)
  * allow to read CMYK JPEGs (Florian)
  * extensions building and deploying (Noel)
  * printing from impress with "Notes" (Radek)
  * more fixes for climaker and bridge test (Radek)
- updated extra translation sources: African, Belorussian, Bulgarian, Czech,
  Danish, Estonian, Finnish, Greek, Hungarian, KHmer, Latvian, Macedonia,
  Norwegian Bokmaal, Norwegian Nynorsk, Slovak, Slovene, Vietnamese, Welsh
- added libwpg sources, version 0.1.0~cvs20070507
- temporary disabled rpmlint checks for duplicated files and devel files in
  non-devel packages; they should be solved by the upcoming package split
* Mon May 21 2007 pmladek@suse.cz
- removed duplicate X-SuSE-translate from base.desktop
* Thu May 17 2007 pmladek@suse.cz
- updated to milestone oof680-m16 (2.2.1rc1)
- updated ooo-build to 2.2.0.1:
  * target oof680-m16
  * libpaper support
  * printing with Trim and Tile options [n#237134]
  * javaunohelper build dependency [i#77465]
- updated the prebuilt cli_types.dll for openSUSE < 10.3 builds
- fixed BuildRequires for the Build Service:
  * added j2sdk update-alternatives for SLED10 (temporary workaround)
  * added mozilla-devel mozilla-nss-devel mozilla-nss for SL 10.0
* Thu May 10 2007 pmladek@suse.cz
- updated to version 2.2
- updated ooo-build to 2.2.0.0:
  * target oof680-m14
  * T602 import filter
  * c# climaker implementation
  * more text grid enhancements for better CJK support
  * gvfs silly infinite recursion
  * bug in WordPerfect typedetection
  * fixes of the +/- stuff in the Navigator for headings [n#129410]
  * "autoshape" elements are not correctly imported from ppt [n#251602]
  * don't add system path to LD_LIBRARY_PATH
  * when in presentation mode, no interruptions should be allowed [n#169207]
  * unable to open file if name contains some national characters [i#59251]
  * database fields with leading 0 vanish from tables [n#239193]
  * desktop files support version 1.0 [n#265444]
  * find locked certs
  * allow some other types of signing certs
  * wrong Chinese Simplified/Traditional conversion table
  * some fixes from Fedora
  * more 64-bit fixes
  * lots VBA fixes and improvements
- added libwps-0.1.0~svn20070129 sources
- updated branding for openSUSE-10.3 [n#244914]
- finally enabled import filter for MS Works Wordprocessing Document (.wps)
- finally enabled parallel build of modules by default [n#244581]
- added gjdoc to BuildRequires for SLED10-x86_64
* Thu May 03 2007 pmladek@suse.cz
- updated ooo-build to 2.1.14:
  * document converter wizard did not work; some existing StarBasic macros
  depends on a bad behavior of arrays [n#269978]
  * crash while loading templates in impress [n#270544]
* Mon Apr 23 2007 pmladek@suse.cz
- updated ooo-build to 2.1.13:
  * lock even .xls when accessing over smb:// [n#264137]
* Thu Apr 19 2007 pmladek@suse.cz
- updated ooo-build to 2.1.12:
  * finally includes the sec fixes for CVE-2007-0239, CVE-2007-0238
  * adds some translations [n#222535]
  * GoalSeek VBA implementation [n#258761]
  * read of .png over smb:// [n#203737]
  * fix "Save As" to work on smb:// again [n#203737]
  * get nsplugin to work again
- removed obsolete extra sec. patches
* Mon Apr 02 2007 pmladek@suse.cz
- updated ooo-build to 2.1.10:
  * samba locking [n#203737]
  * crash in gnome-vfs directory reading; found during the samba locking
  fixing
  * VBA stuff: rows on active cell fails with row syntax [n#258824]
* Fri Mar 30 2007 pmladek@suse.cz
- added flex, bison, and unzip to BuildRequires
- removed two dates (around 28 November, 2006) from changes ; they were out
  of order and broke the build in the Build Service
* Thu Mar 29 2007 pmladek@suse.cz
- updated ooo-build to 2.1.9:
  * detects the icon theme in GNOME and KDE; sets the default icon theme
  in OOo accordingly [n#254855]
  * better CJK font setting [i#65097]
  * crash with 16x16 Crystal icons [i#75531]
  * correct readlink error handling in nsplugin
  * associate application/vnd.wordperfect MIME [n#256030]
  * Tango icon for GNOME quickstarter applet [n#171585]
  * support also upper-case suffixes in bash completion [n#245410]
  * RTF autoshape elements are not correctly imported - partial fix [n#251602]
  * VBA bits:
  * work with file names with spaces [n#258367]
  * setting cell content with range.formular1c1 [n#258381]
  * goto fails with references starting with 'r' or 'R' [n#258812]
  * configurable mono-*.pc file name [n#249381]
- mono-ooo-2.1.pc was renamed back to mono-ooo-2.0.pc [n#249381]
* Mon Mar 19 2007 pmladek@suse.cz
- added Tango icon theme; used by default in GNOME, n#254855
* Thu Mar 15 2007 pmladek@suse.cz
- updated ooo-build to 2.1.8:
  + qstart applet disappeared after the session restart [n#252242]
  + add "openSUSE" to the Novell dictionary [n#251444]
  + add missing translations of the "Zoom" dialog
  + really install hyph_hr_HR, hyph_nb_NO, hyph_nn_NO
* Wed Feb 28 2007 pmladek@suse.cz
- updated ooo-build to 2.1.7:
  * corruption when saving ppt files [n#229874]
  * WordPerfect type detection [i#74194]
  * missing en-GB strings [n#231678]
  * searching for JREs [n#203830, n#222708]
  * temporary prefer gij on x86_64 [n#219982]
  * search also gij32 [n#222708]
  * better Hungarian help font setting [n#244488]
  * VBA bits:
  * warn only about real macros [n#146994]
  * access to hardcoded paths [n#245152]
  * critical warnings on PPC [n#244339]
  * another bridges cleanup for 64-bit
- fixed security issue with hyper links, CVE-2007-0239, n#241636
- fixed critical vulnerability in OpenOffice StarCalc file format parser,
  CVE-2007-0238, n#241652
- removed obsolete stuff from /opt/gnome for openSUSE > 10.2 [n#246588]
- added extra localization source: Hungarian [n#243518]
- added support to simply use parallel build of modules; disable by default
  now [n#244581]
* Thu Feb 15 2007 pmladek@suse.cz
- updated ooo-build to 2.1.6:
  * crash when opening files up over sftp connection [n#243805]
  * prefixes in Hungarian translation [n#210797]
  * crashes when trying to open an exported mail [n#228839]
  * pictures disappeared when saved documents loaded from .docx [n#240776]
  * improved workaround for media:// files in KDE [n#238695]
  * removed problematic %%Title in the generated postscript [n#241015]
  * fix the pre/post-processing capability for signed documents
  * optionally associate more OpenXML file formats
  * bridges cleanup & float fix for 64-bit
  * VBA bits:
  * fix core with Range.Areas collection
  * fix erase for redim
  * improve 'like' implementation
  * problem with compare of objects
- add extra biblio sources to fix bibliographic encoding [n#155725]
- fixed java requires for x86_64 [n#245332]
* Fri Feb 02 2007 pmladek@suse.cz
- updated ooo-build to 2.1.3:
  * random crashes when loading .docx documents [n#240928]
  * import of control on userform with rowsource fails [i#73753]
  * make sure that 'AutoExtension' is not disabled [i#73876]
  * crash when loading some documents
  * too large toggles [i#73875]
  * various small fixes
  * some more stability fixes in libwpd [n#233981]
  * correct icon for main OpenOffice.org menu entry [n#236816]
  * VBA bits:
  * illegal access to undefined dimensions
  * correct implementation of Range.PrintOut method
  * add 'LIKE' implementation [i#73830]
  * update to more stable libwpd 0.8.8 [n#233981]
- updated branding for SLED10-SP1 [n#236687]
- added the needed libwpd-0.8.8 sources [n#233981]
- reverted/removed the wps import filter feature
* Tue Jan 23 2007 pmladek@suse.cz
- removed the system directory /usr/lib/xml from the file list
* Mon Jan 22 2007 pmladek@suse.cz
- updated ooo-build to 2.1.1:
  * bin problematic thread stack size optimization
  * improvements in the VBA API implementation
  * pre/post-processing capability shows progress
  * some warnings and dependency fixes
- updated the prebuilt cli*.dll [n#236353]
- enabled clipart again
* Thu Jan 11 2007 pmladek@suse.cz
- updated to version 2.1 (ooe680-m6 = 2.1GM) [fate#301001]
- updated ooo-build to 2.1
  * target OOE680_m6 == 2.1 GM
  * bash completion, MIME types for open XML Writer file format
  * ask user on logout/shutdown to save documents
  * pixel precise desktop icons
  * extensive optimization to gain speedup
  * lots VBA fixes and improvements
  * optional argument issues in Calc
  * Delete Rows/Columns should remember the content
  * crashes and performance problems during slideshow
  * key/mouse input ignored in slideshow during media playback
  * impress navigator icons transparency
  * problems with dbus connection
  * mozilla certificate dir detection
  * and more
- updated extra translation sources: Bulgarian, Czech, Danish, Estonian, Greek,
  Finnish, KHmer, Macedonia, Norwegian Bokmaal, Norwegian Nynorsk, Slovak,
  Slovene, Welsh
- added extra localization source: Vietnamese
- removed obsolete extra localization sources: British
- changed installation prefix from /usr/lib/ooo-2.0 to /usr/lib/ooo-2.1;
  the user configuration is still in ~/.ooo-2.0 to do not lost the old
  setting
- really added xalan-j2, xerces-j2, and xml-commons-apis to Requires;
  they were only in BuildRequires because of a typo
- enabled support for open XML Writer file format
- temporary disabled clipart; it crashes build on some machines
* Thu Nov 30 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.9:
  * some VBA fixes
  * better style of desktop icons
  * pre/post-processing capability for import/export fixes
  * localized all strings and removed debug outputs in scsolver
* Tue Nov 28 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.8:
  * pre- and post-processing capability for import and export
  * VBA error in Range::Cells
  * lots fixes in the scsolver stuff
  * do not exit the whole OOo by the screensaver poke
  * outline mode editing crasher
  * Chinese font Size name conversion
  * more 64-bit fixes
- reverted the db-java -> db43-java rename to make it buildable for
  older products
- hot fix: gij32 must be checked before gij to be found [#222708]
* Mon Nov 20 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.7:
  * wrong calculation in Calc, r1c1 stuff [i#71455, i#221421]
  * searching for JREs [n#203830, n#222708]
  * python-2.5 paths for 32-bit package on x86_64
- changed requires from jre to jre-32 on 32-bit architectures [n#222708]
- added back the trigger on python to set the paths; otherwise, the python
  components (mail merge) do not work with the 32-bit package on x86_64
- added the related myspell packages to Recommends for: Hindi, Gujarati,
  Sotho, Tsonga, Xhosa (used Recommends instead of Requires to do not break
  CD layout at this stage)
- added scalable-font-km to Requires of the km subpackage
- replaced db-java with db43-java in BuildRequires; the package was renamed
  [n#216880]
* Wed Nov 15 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.6:
  * incorrect use of & and &&, obvious typo [i#71388]
  * do not export corrupted metadata [i#71343]
  * get rid of hypocycloid error on workbook_open, another VBA fix
  * make setsdkenv usable in RPM spec files, affects only sdk [#215627]
  * better Hindi help fonts setting, so the help is readable by default
  * small fixes of the man page [#202573]
  * duplicity in a file list, thanks to aj for noticing it
- updated branding for openSUSE-10.2 [n#220411]
* Mon Nov 13 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.5:
  * prefer gij on x86_64 [#219982]
  * another fix in the fontconfig for font fallback [i#54603]
  * more VBA fixes for Excell sheets [#201635]
  * some fixes in the scsolver stuff
  * installation of the mono pkg-config file
- built against mozilla-xulrunner181-devel instead of
  mozilla-xulrunner180-devel [#218792]
- added gcc-java to Requires on x86_64 [#219982]
- added xalan-j2, xerces-j2, and xml-commons-apis to Requires; we started to
  use the system versions some time ago; it is java stuff, so the dependency
  was not added by default
- fixed installation of mono-ooo-2.0.pc to be in PKG_CONFIG_PATH on x86_64
* Tue Nov 07 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.3:
  * serious problems with condensed text portions printing [i#70759]
  * missing borders VBA range implementation
  * fix permissions of the documentation, for the new
  brp-check-executable-docs
* Sat Nov 04 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.2:
  * arch independent directory for templates
  * crash in Draw and Impress by pressing the Del key
  * problem with binding for controls with accents in VBA [n#214312]
  * CSV saved with .XLS extension should load in Calc
  * objects allocation in pyuno
  * more Slovak localization
  * hash item collections fix
  * Pivot Report without Format layout [n#190970]
  * conflicts in r1c1 stuff
  * some fixes from Fedora
  * build with new gcc-4.1.2
  * hacked hsqldb to work on 64-bit build
  * branched configuration for openSUSE-10.2
- enabled help localizations (are in good state now): Hindi, Slovene
- enabled sdk on all architectures [n#215227]
- fixed dependency on JRE to be required on all architectures
- used the branched configuration for openSUSE-10.2
- removed obsolete hack for setting PYTHONHOME on x86_64; it is not needed with
  the native 64-bit build
* Mon Oct 23 2006 pmladek@suse.cz
- fixed build with gcc (GCC) 4.1.2 20061018
- added /usr/share/templates to search path for templates; made sure that it was
  added also for the existing user configuration
* Thu Oct 19 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.4.1:
  * common dir for language independent templates
  * new "Labels" template category
  * temporary disabled some more dangerous speedup fixes
  * lots VBA fixes
  * lots fixes from Fedora
  * mozab driver path encoding problem on Win32
  * build with recent wizard crash fix
  * parallel build fix
  * install SDK only for en-US
- updated extra translation sources: Danish, Greek, Welsh
- updated thesaurus dictionaries: German, Polish
- updated hyphen dictionaries: Polish
- added thesaurus dictionaries: Norwegian Nynorsk, Portuguese
- added hyphen dictionaries: Croatian, Norwegian Nynorsk, Norwegian Bokmaal
- added some Slovene extras
- removed outdated extra translation sources: Polish
* Thu Oct 12 2006 pmladek@suse.cz
- updated to 2.0.4 (final)
- updated ooo-build to version 2.0.4:
  * target OOO_2_0_4
  * allow to build older bytecode with newer JDK
  * save more space/time/code
  * temporary disabled some dangerous speedup fixes
  * lots VBA fixes and improvements
  * lots 64bit fixes
  * input field dialog improvement
  * unopkg fails when Setup.xcu is missing
  * optional argument issues
  * use fontconfig for font fallback
  * mozilla certificates detection
  * some i18n support added
  * build with Xalan >= 2.7.x fix
  * bashism fixes
  * prebuilt mono dlls update
  * wazard crash
- updated extra translation sources: Danish, Greek, Welsh
- forced source="1.4" and target="1.4" for build with Sun java
- removed java-1_4_2-sun-devel from BuildRequires and added back the default
  java2-devel-packages
- synced features between x86_64 and ix86 build; synced BuildRequires
  accordingly
- enabled gstreamer also when build on SLED10
- updated the prebuilt cli*.dll [n#209342]
- added postinstall script to put the dlls into GAC
- disabled insufficient fix for the wizard crash
- added hack to start makefile several times to try to finish build even with
  the strange random build crash in officecfg
* Fri Sep 29 2006 pmladek@suse.cz
- added gcc-gij to BuildRequires for openSUSE-10.2 and higher to fix build
  on x86_64 and ppc
- removed obsolete gcc41-gij from BuildRequires
* Thu Sep 21 2006 pmladek@suse.cz
- updated to snapshot ood680-m4 (2.0.4-rc2)
- updated ooo-build to version ood680-m4:
  + target ood680-m4
  + more shrink apparent & real memory use
  + lots gstreamer fixes and improvements
  + better error handling in r1c1 stuff
  + lots VBA fixes and improvements
  + r1c1 and VBA
  + find the mozilla certificate dir
  + "Save As" crash regression fix
  + java.home property to gnujre searchpath
  + missing stuff for getopt/readdir_r from system glibc
  + support for evolution-data-server-1.8
  + temporary fix for gcc >= 4.1.2 20060913
  + correct build and installation of the mono stuff
- updated extra translation sources: African, Belorussian, Bulgarian, Czech,
  Danish, Estonian, Greek, Finnish, Latvian, Macedonia, Norwegian Bokmaal,
  Norwegian Nynorsk, Slovak, Slovene, Sotho, Tsonga, Welsh, Xhosa, Zulu
- temporary added java-1_4_2-sun-devel to BuildRequires for openSUSE-10.2
  to build the Java 1.4 compatible bytecode; it is needed until we make the
  target bytecode optional
- temporary added gcc41-gij to BuildRequires until the dependency from
  gcc-java is fixed
- temporary added hack to ignore python UNO component registration problems;
  it is needed until we fix it with python-2.5
- added mozilla-xulrunner to BuildRequires for older distributions to
  help to resolve dependencies
- added the new mono-ooo-2.0.pc to the mono subpackage; it did not make
  sense to do the mono-devel subpackage
- fixed build with xalan-2.7
* Fri Sep 01 2006 pmladek@suse.cz
- updated to snapshot ood680-m2 (2.0.4-rc-pre)
- updated ooo-build to version ood680-m2:
  * target ood680-m2
  * shrink apparent & real memory use
  * lots fixes and improvements in the VBA stuff
  * lots fixes and improvements in the r1c1 stuff
  * more fixes in bibliographic encoding for Chinese
  * getopt/readdir_r from system glibc
  * some fixes in the gstreamer stuff
  * input field dialog improvement
  * install soffice symlink needed by UNO components
  * build with large file support
  * better support for gcj-4.1
  * some more build fixes
- added some few temporary and hot fixes
  * disable all size shrink patches
  * save as dialog fix
  * evolution-data-server-2.8 soname
- added fastjar to BuildRequires on non-ix84 to fix build on ppc [#201707]
- removed the obsolete OOo* wrapper for newer builds [#202573]
- marked the new /usr/bin/soffice symlink as %%ghost; added %%post and %%postun
  scripts to update the symlink
- cleaned up the %%install and %%files sections
* Wed Aug 23 2006 pmladek@suse.cz
- updated to snapshot ood680-m1 (2.0.4-rc-pre)
- updated ooo-build to version ood680-m1:
  * target ood680-m1
  * implementation of layered accelerators
  * gstreamer backend for avmedia
  * new properties for VBA range
  * lots VBA fixes and improvements
  * lots fixes and improvements in ooqstart
  * cairo canvas image scaling issue
  * table sorting fix
  * sourceview font setting
  * new 'Natural' variant of sorting
  * print and print preview issue with sheet local names
  * number of systray fixes in the quickstart stuff
  * simple shrink of base FillInfo by using bitfields
  * better readable version in Help/About dialog
  * input method crash
  * bibliographic encoding for Chinese
  * set default varchar lenght to 100 in dbaccess
  * better using fontconfig for font fallback with Chinese
  * Simplified and Traditional Chinese translation in stc_char.dic
  * support for new gcj/gij
  * build with shell=bash by default
  * various build fixes
- removed obsolete tcsh from BuildRequires
- added gstreamer010-plugins-base-devel to BuildRequires needed by the
  gstreamer/avmedia feature
- moved branding images to package sources
- added prebuilt unowinreg.dll to avoid build dependency on mingwin; the dll
  is necessary to build portable java extensions by OOo-sdk
- improved spec file to skip build preparation stuff easily; it is helpful
  for testing
* Fri Jul 14 2006 pmladek@suse.cz
- updated to snapshot OOO_2_0_3
- updated ooo-build to version 2.0.3.0:
  * target OOO_2_0_3
  * buggy speed image stuff temporary disabled
  * allow to resize Cols/Rows even in read-only document
  * better support for CJK embolden, port from 2.0.2
  * range should stay selected after un-toggle
  * problem with the key [Alt F]
  * add Row addressing to A1 style in Calc
  * more VBA fixes
  * some build and install fixes for scsolver
  * build with gcc-3.3
* Wed Jun 28 2006 pmladek@suse.cz
- updated to snapshot ooc680-m7 == 2.0.3rc7
- updated ooo-build to version ooc680-m7:
  * new module for Mixed Integer Linear Programming
  * KDE backend for system-wide settings
  * Lotus Word Pro import filer as a xml filter implementation
  * plus / minus sign next to each heading in Navigator
  * linking speed up and many other changes
  * lots of new constants for VBA excel, font attributes
  * lots fixes and improvements in the VBA stuff
  * lots fixes and improvements in the SCSolver stuff
  * lots fixes and improvements in the CalcSolver stuff
  * lots impress related fixes
  * lots 64-bit fixes
  * better export of spreadsheets to HTML
  * ldap configuration fix
  * updated various bits to be in sync with CWS
  * better support for distro specific branding
  * more gcj/gij related fixes
  * build with gcc 4.1
  * and many more
- added mozilla-xulrunner180-devel into BuildRequires instead of gecko-sdk
  for SL > 10.1
- added sablot-devel to BuildRequires for x86_64
- added support to build on x86_64
- temporary disabled sdk on ppc and x86_64
- added lp_solve souurces, necessary for the new Mixed Integer Linear
  Programming feature
- removed the obsolete security patches
- removed obsolete --with-vendor=Novell configure switch
* Thu Jun 22 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.15:
  * clipping problem in case cached primitive is used [#187780]
  * reading of freed memory in slidesorter [#186621]
  * file lists on ppc with gcj
- added three security fixes:
  * check macros even on non-standard locations, CVE-2006-2198, #184372
  * disable java applets by default because they might break out of
  the sandbox and any secure fix is not easy, CVE-2006-2199, #184385
  * potential buffer overflow in the xml utf8 converter, CVE-2006-3117,
  [#185717]
- added SL 10.1 compat fixes (does not affect the SLED10 build):
  * disabled cairo by default [#152730]
  * disabled sdk stuff
* Thu Jun 15 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.14:
  * ~/.recently-used corruption [#184895]
  * use correct context in the VBA stuff [#184871]
  * missing string for the quickstarter check-box [#183717]
  * updates and fixes of translations
- added checks for macros even on non-standard locations, n#184372
- disabled java applets by default because they might break out of the sandbox
  and any secure fix is not easy, n#184385
* Thu Jun 01 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.12:
  * lots fixes in the VBA stuff [#178312, #179188, #175726, #178717, #105391]
  * problems with cropped text sprites [#180871]
  * problem with text animations [#173009]
  * printing from samba share [#169587]
  * slideshow rendered incorrectly on Xgl [#174507]
  * bibliographic encoding [#155725]
  * crasher dismissing menus [#170772]
  * top-level basic-shapes icon [#178384]
  * metrics of the font Open Symbol [#160267]
  * poke gnome screensaver during slideshow [#169207]
  * automatically wrap text inside shapes [#171052]
  * backport of some fixes for the certificates stuff [#157991]
  * gengal stuff clean up, saves 7MBs [i#65361]
- fixed to use the extended dependencies (Recommends, Suggests, Enhances)
  only on SL10.1 and higher
* Fri May 19 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.11:
  * context menu behavior fix
  * large pivot tables were unusable slow [#177261]
* Thu May 18 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.10:
  * more i18n support
  * better fake bold for big cjk fonts [#176701]
  * some new font attributes for the VBA stuff
  * some more fixes for the VBA stuff [#104857, #176055, #175726, #105125]
  * using a filter when browsing samba shares [#167900]
  * missing strlen check
  * sort-order of files in extra galleries
- added a compat symlink for writer icon to fix update from NLD9 [#173140]
- disabled openldap, gnome-vfs, and gtk support on SL9.1, except for NLD9
- added check if suseconfig_fonts macros were defined to fix build on SL9.2
* Thu May 04 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.9:
  * more i18n support
  * typo in gcj 32-bit run time paths
  * lots fixes in the VBA stuff
  * support to remove poor help localizations
- disabled poor help localizations with less than 50%% of localized strings:
  African, Belorussian, Bulgarian, Catalan, Croatian, Finnish, Greek, Hindi,
  Kinyarwanda, Latvian, Norwegian Bokmaal, Macedonia, Norwegian Nynorsk,
  Serbian, Slovak, Slovene, Sotho, Turkish, Tsonga, Vietnamese, Welsh, Xhosa
* Thu Apr 27 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.8:
  * support for OOo SDK packaging [#168810]
  * optimization for loading large pivot tables from xls
  * crash in fileview
  * layout hang
  * attempting to access current document fails in VBA
  * default Greek fonts definition [#166267]
  * small problems with preferred font order
  * missing minor version number in versionrc
  * synced atkbridge fixes with CWS
  * Writer/Web desktop category [#168663]
- added sdk and sdk-doc subpackages [#168810]
- disabled Writer/Web menu entry on SLED [#168663]
* Fri Apr 14 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.7:
  * better fix for crash when selecting email application [#154458]
  * avoids strange 2.6 specific hacks
* Thu Apr 13 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.6:
  * crash when selecting email application
  * allow right-click if focus is set to mouse in Metacity
  * prefer Calc over Gnumeric in KDE, set InitialPreference=6 in calc.desktop
  * enable Math MIME type setting; use NoDisplay=true instead of Hidden=true
  in math.desktop
  * crash with a11y [#165538]
  * -display and other options with ooqstart [#160296]
  * expression evaluation changed between 1.x and 2.0 [#164294]
  * disable use of fontconfig to enable/disable bitmaps again [#164868]
* Wed Apr 05 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.5:
  * use fontconfig to enable/disable bitmaps
  * pressing escape when context menu is displayed crasher
* Fri Mar 31 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.4:
  * import of VBA stream hangup
  * lots new constants for VBA excel
  * lots fixes in Atk bridge
  * i18n support within pasting to Calc
  * bin the obsolete LD_PRELOAD of libgcj [#153386]
- added some NLD9 specific fixes (affects only build on NLD9):
  * marked the compat desktop files GNOME only
  * substituted the bin suffix in the compat desktop files
  * enabled openclipart
  * packaged missing directories
* Mon Mar 27 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.3:
  * preload libgcj when gcj/gij JRE is selected [#153386]
* Thu Mar 23 2006 pmladek@suse.cz
- updated ooo-build to version 2.0.2.2:
  * more VBA fixes
  * markup the menu items correctly
  * branch SUSE-10.1.conf from SUSE.conf
  * update man page
  * bin obsolete hacks for build with gcj on ppc
- removed unused ooo_custom_images-13.tar.bz2, ooo_crystal_images-1.tar.gz,
  libwpd-0.8.3.tar.gz; were unused many weeks ;-(
- fixed to use SUSE-10.1.conf for SL 10.1 build
* Fri Mar 17 2006 kendy@suse.cz
- updated ooo-build to version 2.0.2.1:
  * more VBA fixes
  * better HTML validity
  * ODMA fixes
  * ooqstart fixes [#157583] [#144795]
  * multiselection in fpicker
  *'Ctrl' instead of 'Control' in menus
* Fri Mar 10 2006 pmladek@suse.cz
- updated to the final OOO_2_0_2 sources
- updated ooo-build to version 2.0.2:
  * target OOO_2_0_2
  * pure X11-libs based ooqstart
  * lots fixes in the VBA stuff
  * lots fixes in the Atk bridge
  * some safer permissions by default again
  * line spacing in OpenSymbol font
  * oofromtemplate do not longer open two windows [#152530]
  * use system allocator and boost
  * fix build on ppc with gcj [#115274]
- updated extra translation sources: Greek, Macedonian, Welsh
- added %%run_suseconfig_fonts to %%post, %%postun [#155404]
- added boost-devel devel to BuildRequirtes
- moved java2-devel-packages to ix86 specific BuildRequires
- added java-1_4_2-gcj-compat-devel gcc-java xalan-j2 xerces-j2
  xml-commons-apis to ppc specific BuildRequires
- enabled build with gcj on ppc [#115274]
- used "|| true" rather than "|| exit 0" in the various scriptlets to do not
  break their other parts
* Thu Mar 02 2006 pmladek@suse.cz
- updated to snapshot oob680-m5 == 2.0.2rc4
- updated ooo-build to version oob680.5.0:
  * target oob680-m4, oob680-m5 == 2.0.2rc4
  * lots fixes in the VBA stuff
  * textures handling
  * bitmap surface drawing
  * arrows misplacing after sheet recalculation
  * parallel build of cli_ure fix
  * build on ppc without java
  * new wrappers for smoketest, qadevOOo, qatesttool
- updated extra translation sources: Bulgarian, Estonian, Greek, Latvian,
  Slovak, Slovene
- removed wrong locale(en_US) provides from the main package; OOo should not
  be selected always with en_US locales
- removed hard dependency on libgcj, finally [#144113]
* Fri Feb 24 2006 pmladek@suse.cz
- updated to snapshot oob680-m3 == 2.0.2rc3
- updated ooo-build to version oob680.3.0:
  * target oob680-m2, oob680-m3
  * ask user to save documents on desktop logout/shutdown
  * drawing texts with different bit depths
  * don't fork date thousands of times during the build
  * lots fixes in the VBA stuff
  * some fixes in the GETPIVOTDATA function
  * send document as PDF attachments fix to work
  * run without quickstarter fix
  * fpicker crasher
  * word import filter speed improvement fix
  * bash completion for paths starting with ~
  * preserve SaveAs name when changing document format
  * associate some less typical MIME types
  * better support for build with gcj
  * Novell palette fix
  * extra configuration for build on ppc where java is disabled for now
- updated extra translation sources: Bulgarian, Estonian, Latvian, Macedonian,
  Norwegian Bokmaal, Norwegian Nynorsk, Slovak, Welsh
- added extra translation source: Polish
- added provides locale(parent:lang) to all lang subpackages
- fixed tarball with the desktop files for KDE desktop templates [#152869]
- added special version of xt, which is necessary for build with gcj
* Fri Feb 17 2006 pmladek@suse.cz
- updated to version 2.0.2, snapshot oob680-m1 == 2.0.2rc1
- updated ooo-build to version oob680.1.1:
  * target oob680-m1 [=2.0.2rc1]
  * lots fixes in the VBA stuff
  * 'like' Evo2 handling
  * link with -Wl,-zdynsort
  * lots fixes and improvements in the Calc solver
  * crash when pasting text into writer
  * disabled the registration menus by default
  * enabled some latest changes in the VBA stuff
  * bin problematic oopadmin wrapper
  * Novell corporate dictionary
  * some fixes for parallel make
  * build with system db 4.3 fix
- updated extra translation sources: Bulgarian, Czech, Danish, Estonian,
  Khmer, Latvian, Slovak, Slovene
- updated translations of the desktop files for KDE desktop templates
- removed/disabled the buggy mdbtools/access stuff [#130178]
- removed the problematic OOo-padmin wrapper
* Thu Feb 16 2006 stbinner@suse.de
- fix %%suse_update_desktop_file parameter order
* Fri Feb 10 2006 pmladek@suse.cz
- updated to version 2.0.0.99.156, snapshot src680-m156
- updated ooo-build to version src680.156.0:
  * target m155, m156
  * direct atk-bridge for GNOME accessibility
  * two new templates
  * lots fixes in the VBA stuff
  * some fixes in the Evo2 stuff
  * some fixes in Cairo stuff
  * reworked patch for d_type entry based speedup
  * testtool compatibility with our VBA stuff [#145906]
  * too wide "Menu/Insert/Picture/From File..." dialog
  * quickstarter argv[0] handling
  * Hebrew font configuration [#114495]
  * helpcontent configuration of some localizations
  * packaging of KDE Address Book
  * helpcontent configuration fix
  * new SL 10.1 branding [#149017]
- updated extra translation sources: Belorussian, Bulgarian, Czech, Danish,
  Estonian, Greek, Latvian, Norwegian Bokmaal, Norwegian Nynorsk, Slovak
- cleaned up BuildRequires:
  * removed gnutls-devel, libglade2-devel, libgnomeprintui-devel,
  libgnomeui-devel, libwnck-devel, mDNSResponder-devel, mono-basic,
  mono-data-sqlite, openldap2, openslp-devel
  * added gnome-vfs2-devel, gtk2-devel
- used a better dependency on gij, still not the final solution [#144113]
- enabled binfilters only in the spec file again now
* Fri Feb 03 2006 pmladek@suse.cz
- updated to version 2.0.0.99.154, snapshot src680-m154
- updated ooo-build to version src680.154.0:
  + target m154
  + lots fixes in the VBA stuff
  + thumbnails compatibility with windows
  + fixup & re-enable evo-2.4 support
  + handle media:/ files locally
  + allow to open files with relative paths by quickstarter
  + updated various bits to be in sync with CWS
  + cleaned up configuration of ooo-build vs. spec file
- updated extra translation sources: Bulgarian, Czech, Estonian, Greek,
  Macedonian, Slovak
- cleaned up spec file; moved most of the enable/disabled/with configure
  switches to ooo-build, so they are at one place; renamed some variables
  to be consistent
* Thu Jan 26 2006 pmladek@suse.cz
- updated to version 2.0.0.99.151, snapshot src680-m151
- updated ooo-build to version src680.151.0:
  * target m151
  * do not include the ldapbe2.uno.so service for remote ldap config
  * lots fixes in the VBA stuff
  * problem with templates and macros
  * enable gij-4.1 for all
  * disable visibility markup only for i18nshrink
  * hide mouse cursor during slideshow
  * make Calc solver menu translatable
  * recognize gij in jvmsetup
  * do not start writer if no mode is selected by quickstarter
  * 0x0 size canvas creating
  * DIN-regular font mapping fix
  * some fixes for GNOME splash
  * Cairo smooth curves disabled
  * fixed ootool to do not add directories "twice"
  * parallel build of images
- updated extra translation sources: Czech, Estonian, Latvian, Macedonian,
  Slovak
- added extra translation sources: Belorussian
- added a temporary hack to fix dependency on gij [#144113]
- fixed dependency on Indian fonts [#144135]
* Wed Jan 25 2006 mls@suse.de
- converted neededforbuild to BuildRequires
* Fri Jan 20 2006 pmladek@suse.cz
- updated to version 2.0.0.99.150, snapshot src680-m150
- updated ooo-build to version src680.150.1:
  * target m150
  * updated crystal icons to ver. 1 / 1st candidate for up-streaming
  * lots improvements and fixes in the VBA stuff
  * regression in formulae for getValue
  * create ooffice wrapper script too for quickstarter [#144285]
  * do not refcount the empty string
  * do not check for dir and file
  * do not stat dirs over and over
  * use d_type entry instead of using stat
  * more fixes for myspell hashify
  * create pipename in multiple places issue
  * fontconfig always uses UTF-8
  * some fixes in Kohei's Calc solver
  * some hunspell build fixes
  * some installer errors fixes
  * config files in file lists fix [#137975]
  * lots double warnings fixes
  * updated NLD10 branding
- update KDE icons to version 1 (1st candidate for up-streaming)
- updated extra translation sources: Bulgarian, Czech, Estonian, Greek, Slovak
- added 1 more localization (subpackages): Khmer (km) [#143483]
- added extra translation sources: Khmer
- added OpenOffice_org-hunspell to Obsoletes
- fixed dependency on fonts and myspell dictionaries in the subpackage mk, and
  sr-CS
- fixed OOo-wrapper to call ooffice instead of the obsolete ooo-wrapper
  [#144285]
- fixed build with openclipart on older distributions
* Mon Jan 16 2006 pmladek@suse.cz
- fixed build with openclipart-0.18 (this version started to use another path
  and a symlink)
* Fri Jan 13 2006 pmladek@suse.cz
- updated to version 2.0.0.99.149, snapshot src680-m149
- updated ooo-build to version src680.149.0:
  * target m149
  * Kohei's Calc solver
  * Desktop Entry Specification conforming desktop files [#10543]
  * more splash speedup
  * quick-starter improvements
  * simple wrappers for quick-starter
  * crash on 16bit depth displays with Cairo
  *  myspell hashify for Win32
  *  problems with gallery stuff
  * linguistic simple dictionary stuff improvements
  * missing GETPIVOTDATA function in Calc
  * some mono-related fixes
  * more robust build-galleries script
  * improved dictionaries documentation installation
  * enabled build with sytem db
- updated extra translation sources: Arabic, British, Bulgarian, Czech,
  Danish, Estonian, Greek, Norwegian Bokmaal, Norwegian Nynorsk, Slovak,
  Slovene, Welsh
- updated hyphen dictionaries: Dutch, Greek, Latvian, Polish
- updated thesaurus dictionaries: American, German, Polish
- added 7 more localizations (subpackages): Hindi (hi-IN), Lithuanian (lt),
  Macedonian (mk), Kinyarwanda (rw), Serbian (sr-CS), Sotho (st),
  Tsonga (ts)
- added extra translation sources: Lithuanian, Macedonian, Serbian, Sotho,
  Tsonga
- added thesaurus dictionaries: Czech, Spanish
- added db-devel db-java to neededforbuild
- added desktop files for NLD9 build
- removed obsolete extra translation sources: Croatian, Hungarian
- removed obsolete tarball with SUSE specific desktop files
- removed obsolete hunspell sources and hunspell subpackage [#141750]
- enabled build of quiskstarter
- fixed installation of desktop files
* Mon Jan 09 2006 pmladek@suse.cz
- fixed some localizations of the About dialog
* Sat Jan 07 2006 pmladek@suse.cz
- updated to version 2.0.0.99.148, snapshot src680-m148
- updated ooo-build to version src680.148.0:
  * target m148
  * lots improvements and fixes in the VBA stuff
  * splash speedup
  * quickstart improvements
  * fixed undefined usage of pointers
  * added missing glipth for default bullet
  * more 64bit fixes
  * more fixes for gcc41
  * some Cairo improvements
  * build with system mdbtools fix
  * build with external stlport fix
  * build with system mozilla fix
  * build with XULRunner allowed
  * KDE only build fix
  * fixed build of extra galleries
  * fixed stripping
- added templates that can be used to create a new file on KDE desktop
- added InitialPreference=6 to draw.desktop to be preferred over Karbon14
* Thu Dec 22 2005 pmladek@suse.cz
- updated to version 2.0.0.99.145, snapshot src680-m145 (2.0.1.rc4)
- updated ooo-build to version src680.145.0:
  * target m145
  * lots improvements and fixes in the VBA stuff
  * icons theme status storing
  * switching back from HighContrast icon theme
  * series sum fix in Calc
  * return value of empty cell
  * disabled debug messages from myspell
  * fixed pointer usage in icu [#139376]
- added nld subpackage for NLD branding
- added better support for faster test builds
- added additional system libraries sources, used and packaged only on NLD9
* Mon Dec 12 2005 pmladek@suse.cz
- updated to version 2.0.0.99.143, snapshot src680-m143 (2.0.1.rc2)
- updated ooo-build to version src680.143.0:
  * bug fixes:
  * lots improvements in the VBA stuff
  * some fixes for R1C1 address support in Spreadsheet
  * build against newer OpenLDAPs
  * hack to build >=m142 with mono enabled
  * updated patches for gcc-4.1, added patches for binfilter
  * updated configure.in header for new autoconf stuff
  * updated patches for 64-bit support
  * updated patches for icon switching
  * installation of mailmerge.py with system Python
- updated extra translation sources: Bulgarian, Czech, Danish, Estonian, Greek,
  Finnish, Slovak, Slovene
- fixed installation of desktop files and fixed the file list for NLD9
- enabled binfilters again
* Tue Nov 29 2005 pmladek@suse.cz
- updated to version 2.0.0.99.140, snapshot src680-m140
- updated ooo-build to version src680.140.0:
  * target m140
  * cairo updated to version 1.0.2
  * improved built-in quick-starter applet / systray thing
  * lots VBA fixes
  * Word and Excel filters save the VT_CF thumbnail property
  * some optimization/speedup
  * lots VBA fixes
  * prioritize Hebrew culmus fonts
  * obsolete oohtml wrapper symlink
  * missing Tilde & back-tick keys
  * ctrl-back-tick toggle formula view
  * 'Use hardware acceleration' check box moved to the right column
  * 'Document converter' basic error
  * desktop files moved to /usr/share/applications
  * more fixes in the support for pyuno on x86_64
  * LD_LIBRARY_PATH problems
  * Evo and columnNames selection
  * set the toolbox height according to the icon theme
  * resize toolbox according to the selected theme
  * print-related dispatch API
  * accuracy of ERF/ERFC for large x value improvement
- updated extra translation sources: African, Bulgarian, Czech, Danish,
  Estonian, Greek, Finnish, Norwegian Bokmaal, Norwegian Nynorsk, Slovak,
  Slovene, Welsh, Xhosa, Zulu
- added extra translation sources: Arabic
- temporary disabled binfilters even for full build
- fixed installation of menu entries for NLD9
- added OOo2 packages to provides and obsoletes, it was necessary for NLD9
- removed myspell-british from requires of the main package; en-GB subpackage
  is the right place
- added myspell-estonian to requires of the et subpackage
- added support for mimimalized test build
* Tue Oct 25 2005 pmladek@suse.cz
- updated ooo-build to version 2.0.0.1:
  * error while using wizard document converter [#113235]
  * odt import of 0.5pt table borders
  * LD_LIBRARY_PATH problem [#118188]
  * fixes in the support for pyuno on x86_64
  * updated man page
* Thu Oct 20 2005 pmladek@suse.cz
- updated to version 2.0.0
- updated ooo-build to version 2.0.0:
  * target 2.0.0
  * updated artwork
* Mon Oct 17 2005 pmladek@suse.cz
- updated to version 2.0.rc3
- updated ooo-build to version 2.0.rc3:
  * ooo680-m* support
  * VBA update
  * better message when there is no help
  * fixed layout of Options...->View
  * more parallel build of ooo680-m*
  * fixed crash on load of .doc
- updated extra translation sources: Bulgarian, Czech, Danish, Estonian,
  Hungarian, Slovak
- removed the outdated Dutch extra translation source
- updated internal cairo to version 1.0.2 (used just for older distributions)
- updated libwpd to version 0.8.3
- used mozilla-devel-packages intead of mozilla and mozilla-devel in
  neededforbuild
- fixed to compile with mozilla-xulrunner on SL 10.1 and above
- updated NLD branding
- removed unneeded exporting DIR and LIBDIR for QT and KDE
* Fri Oct 07 2005 lmichnovic@suse.cz
- added exporting DIR and LIBDIR for QT and KDE into specfile
* Wed Oct 05 2005 lmichnovic@suse.cz
- added flac, flac-devel into needforbuild (required by the new libsndfile)
* Tue Oct 04 2005 lmichnovic@suse.cz
- added MimeType application/vnd.ms-powerpoint into adequate desktop file (bug #118393)
* Thu Sep 22 2005 pmladek@suse.cz
- updated to version 1.9.129
- updated ooo-build to version 1.9.129:
  * target m129
  * updated KDE Address Book integration
  * VBA form control event handling and more features
  * drawing with cairo-1.0.0 fix
  * check buttons rendering problems
  * spin-button event timeout problems
  * lots 64-bit fixes
  * user config dir name
  *  undefined token pool states in QPro stuff
  * 'backspace' deletes slides like 'delete' in slide sorter
  * really set file type by extension in GTK+ fpicker
  * turn on impress ruler by default
  * run crash report only when it is built
  * some fixes in configure
  * start Xvfb with -ac when building galleries
  * do not copy system stdlibs
- fixed to really disable java on ppc [#115274]
- added initial support to build with gcj
- added text/x-comma-separated-values and text/x-csv mimes to calc.desktop
  [#80362]
* Mon Sep 05 2005 pmladek@suse.cz
- updated ooo-build to version 1.9.125.1.2:
  * disabled some unreviewed patches [#114992]
  * check buttons rendering problem [#80447]
* Thu Sep 01 2005 pmladek@suse.cz
- updated ooo-build to version 1.9.125.1.1:
  * disable cairo by default [#106644]
  * proper fix for bullets exported to MS [#10555]
  * crash with GTK+ fpicker [#114644]
  * crashing while inserting slides [#114062]
  * ooconfig improvements and fixes
  * man page fixes
  * show text correctly with cairo-1.0.0
  * unnecessary duplicities in sub-packages
  * do not install mono cli_*.dll files twice
  * do not run crash_report when it is not built
  * set file type by extension in GTK+ fpicker
  * allow to apply the extra localizations sources everywhere
- fixed a typo in Slovak translation, thanks to lmichnovic@suse.cz
* Mon Aug 29 2005 pmladek@suse.cz
- updated ooo-build to version 1.9.125.1:
  * VBA crash with resize property [#105450]
  * set file type by extension in GTK+ fpicker
  * avoid version in Hunspell service name
  * avoid some heinous GTK+ brokenness
  * create some dirs and files with secure permissions [#106004]
  * updated branding for 10.0
  * add simple utility ooconfig to easy change some setting
- updated internal cairo to version 1.0.0 (used just for older distributions)
* Tue Aug 23 2005 pmladek@suse.cz
- updated to version 1.9.125
- updated ooo-build to version 1.9.125:
  * target m125
  * updated internal cairo to 0.9.2
  * backported lots fixes that were approved for 2.0.1
  * case sensitive autoword completion
  * proper ms access tab page for creating msaccess db
  * crash in evo2, fix for new API [#10446]
  * insensitive menu items [#105864]
  * file locking on NFS [#104655]
  * bullets exported to MS [#10555]
  * crash administering printers without (parsable) PPDs [#73034]
  * default file type by export to PDF in GTK file picker
  * textdoc.desktop support for text/rtf mime
  * mailmerge.py error with UTF-8 [#105244]
  * VBA stuff fixes
  * GNOME desktop integration fixes
  * bin obsolete --icons-set ooo-wrapper's option
  * allow grouping of the items under Templates and Document [#106603]
  * some powerpc build related fixes
  * m125 build error with gcj fix
  * new script to update PYTHONPATH on x86_64 [#105227]
- updated extra translation sources: Bulgarian, Czech, Danish, Estonian,
  Hungarian
- updated internal cairo to version 0.9.2 (used just for older distributions)
- removed obsolete internal libpixman sources
- removed obsolete branding sources
- added application/rtf to writer.desktop (used by GNOME)
- added application/vnd.sun.xml.base to base.desktop (prefered now)
- created hunspell subpackage, so the Hunspell spell checker is not enabled
  by default for all languages [#112770]
- added a trigger to update PYTHONPATH for pyuno on x86_64 [#105227]
* Thu Aug 18 2005 pmladek@suse.cz
- updated to version 1.9.124
- updated ooo-build to version 1.9.124:
  * target m124
  * 'save as rendered' option while exporting to .csv
  * allow to disable the VBA feature at runtime
  * some fixes of the cairo canvas stuff
  * default option in the templates and documents dialog fix
  * warning about usage of an undefined variable in install-dict
  * unnecessary error messages from hunspell
  * possibility to turn off strip
  * allow to use ARCH_FLAGS when building partaudio
- updated extra translation sources: Czech, Danish, Estonian, Slovak
- updated cairo source, version 0.9.0 (used just for older distributions)
- updated branding once more for 10.0 [#102355]
- fixed provides of the officebean subpackage
- disabled strip by the native installer
- fixed permissions of installed libraries and modules
- fixed a return in the VBA stuff
- fixed a crash with evo2 [#104462]
- fixed to get proper ms access tab page for creating msaccess db
* Mon Aug 15 2005 pmladek@suse.cz
- fixed some potentially dangerous warnings
- fixed to build portaudio with RPM_OPT_FLAGS
- updated branding for 10.0 [#102355]
* Wed Aug 10 2005 pmladek@suse.cz
- updated to version 1.9.123
- updated ooo-build to version 1.9.123:
  * target m122, m123
  * support for Hunspell
  * lots vba compatibility fixes and improvements
  * some updates in cairo canvas stuff
  * GNOME desktop integration fixes
  * compile mdbtools with -fPIC
- updated extra translation sources: African, Bulgarian, Czech, Danish,
  British, Estonian, Hungarian, Norwegian Bokmaal, Norwegian Nynorsk, Slovak,
  Xhosa, Zulu
- updated cairo source, version 0.6.0 (used just for older distributions)
- added subpackage officebean to remove the direct dependency on Sun Java
  from the main package [#47904]
- added necessary sources for Hunspell, enabled Hunspell [#81225]
- fixed provides and obsoletes for OOo2 package
* Thu Aug 04 2005 pmladek@suse.cz
- updated to version 1.9.121
- updated ooo-build to version 1.9.121:
  * target m120, m121
  * AA slide show cairo canvas
  * VBA compatibility
  * GNOME desktop integration
  * mdbtools based Access base backend
  * shrink myspell memory usage
  * misc IDL comments fixes
  * make csv/text import settings persistent
  * bash completion of filenames with spaces and directories
  * some more gcc4 related fixes
- updated extra translation sources: Bulgarian, Czech, Welsh, Danish, Estonian,
  Slovak
- added necessary sources for cairo and mdbtools features
* Fri Jul 29 2005 pmladek@suse.cz
- updated extra translation sources: Bulgarian, Czech, Danish, Estonian
- updated hyphen dictionaries: Ukrainian
- updated thesaurus dictionaries: American, German, Slovak
- added more subpackages with localizations: Gujarati, Punjabi, Xhosa, Zulu
- added extra translation sources: Xhosa, Zulu
- added hyphen dictionaries: Lithuanian, Romanian
- added thesaurus dictionaries: Bulgarian, French, Polish
- enabled Mono UNO bridge and created mono subpackage (for SL 10.0 and above)
- added mono-devel-packages to needeforbuild, added necessary extra sources
- fixed critical warnings in the mono stuff
- fixed installation of mono and new dictionaries
- added more triggers to configure some new dictionaries in the OOo2 package
* Fri Jul 22 2005 pmladek@suse.cz
- updated to version 1.9.118
- updated ooo-build to version 1.9.118:
  * target m118
  * allow to start formula with '+' or '-'
  * access vba api as uno component
  * corrected check boxes spacing
- added more subpackages with localizations: African, Belorussian, Bulgarian,
  Welsh, British, Croatian, Norwegian Bokmaal, Norwegian Nynorsk, Brazilian
  Portuguese, and Vietnamese
- added extra translation sources: African, Bulgarian, Welsh, British, Croatian,
  Norwegian Bokmaal, Norwegian Nynorsk, Brazilian Portuguese, and Vietnamese
- updated extra translation sources: Czech, Danish, Estonian, Slovak
- fixed to do not use the system libsndfile on SL 9.3 because the package
  libsndfile-32bit does not exist for 9.3-x86_64
* Fri Jul 15 2005 pmladek@suse.cz
- updated to version 1.9.116
- updated ooo-build to version 1.9.116:
  * target m116
  * new HiContrast icon theme
  * new mail merge email component
  * startup speedups
  * use system libsndfile
  * initial support for system libnss
  * more potentially dangerous warnings fixes
  * m117 build with gcc-3.2 and gcc-3.3 fix
  * misc. 64bit fixes
  * gcj/Java fixes
  * better check for broken qt/gtk theme
- updated extra translation sources: Czech, Danish, Estonian, Hungarian, Slovak
- added libsndfile and libsndfile-devel to neededforbuild
- removed problematic provides of upstream packages
- added binfilter and new lang sources
- cleaned up list of sources
- removed obsolete CONFIGURE_EXTRA_OPTIONS setting
- enabled binfilters
* Thu Jun 23 2005 pmladek@suse.cz
- updated to version 1.9.110
- updated ooo-build to version 1.9.110:
  * target m109,m110,m111
  * icon theme switching
  * COLUMN and ROW functions return series for arrays fix
  * more 64-bit fixes
  * ergonomics of export in fpicker
  * problems with hsqldb
  * better fallback when some plugins are missing
  * initialize fontconfig only once
  * lots nasty leaks fixes
  * symlink targets in install-dict
  * possibility of insertion of rows instead of overwriting disabled again
  * unused wizards removed
  * more gcc4 fixes
- updated extra translation sources: Czech, Danish, Estonian, Slovak
- added provides compatible with the upstream packages
- added triggers to configure dictionaries in the OOo2 package
* Thu Jun 09 2005 pmladek@suse.cz
- updated to version 1.9.108
- updated ooo-build to version 1.9.108:
  * target m106, m107, m108
  * expression optimization in calc
  * Quattro Pro 6.0 import filter enabled by default
  * lots mono, 64-bit and gcc4-related fixes
  * resources handling in fpickers fix
  * check if help is available
  * new type selection expander for fpickers
- updated extra translation sources: Czech, Danish, Estonian, Slovak
- fixed to build the module np_sdk with the system mozilla-nspr
- fixed to use the system memmove
* Thu May 26 2005 pmladek@suse.cz
- updated to version 1.9.104
- updated ooo-build to version 1.9.104:
  * target m104, m102
  * new --with-openclipart configure switch
  * new info function in calc
  * lots fixes in fpicker, KDE vclplug, Evo2, mono, vba, etc.
  * lots of gcc4 build fixes
  * do not longer use bug-buddy
  * fix to build with system nspr
  * do not mark PPD files as %%config
  * disable epm during build
  * do not strip installed binaries
- updated crystal icons to version 6
- updated extra translation sources: Czech, Danish, Estonian, Finnish,
  Hungarian, Slovak
- added German and Slovak thesaurus dictionaries
- enabled to build openclipart, for SL > 9.3
- added openclipart, xorg-x11-Xvfb, libusb, and resmgr to neededforbuild
- added galleries subpackage with extra galleries from openclipart, for SL > 9.3
- removed obsolete bug-buddy from requires
- removed obsolete --with-system-gcc configure switch
- removed obsolete hack that was needed to build with more localizations
- fixed missing forward declarations and some dangerous warnings for gcc4
* Thu May 05 2005 pmladek@suse.cz
- updated to version 1.9.100
- updated ooo-build to version 1.9.100:
  * target m100, m99
  * simple lwp import
  * gengal - gallery generator
  * fixed to build with java and gcc4
  * Indian font fixes
  * data pilot crasher
  * calc spaced out numbers
- removed so strong dependency (on the release number) between lang subpackages
  and the main package; dependency on the version number is enough
- enabled build with java in STABLE again
- fixed some new potentially dangerous warnings
* Tue Apr 19 2005 pmladek@suse.de
- updated to version 1.9.92
- updated ooo-build to version 1.9.92:
  * target m92
  * ordinal type iteration
  * filtering improvements
  * startup notification improvements
  * impress/scroll-wheel ergonomics
  * more compatible error values
  * GCJ fixes
  * updated evo driver
  * fix UNO/gcc3 breakage
  * Session Mgmt fix
  * fpicker fixes
  * font substitution fixes
  * better zoom combo-box
  * insert/note popup
  * double graphics cache limits
  * improved QPro bits
  * print error warnings
  * calc/sheet rename
  * excel/import crasher
  * evolution/defaults
  * binfilter unpack / build
  * improve Qt detection
  * enabled support for openldap
  * use wildcards when looking for libs to work on ppc [#74032]
  * fixed potential heap overflow when reading DOC file, CAN-2005-0941,
  [#76741]
- updated extras to version 2
- updated extra translation sources: Czech, Danish, Estonian, Finnish,
  Hungarian, Slovak
- added openldap-related packages to neededforbuild
- disabled java on ppc [#74031]
- added myspell-american, myspell-british to Requires
- removed the no longer supported OOo-setup wrapper [#76858]
- added some gcc4 related fixes, disabled java on STABLE for now
- removed -Wno-return-type for distributions never than SL 9.3
- removed obsolete hack to find /usr/lib/qt3/bin/moc
- added hack to enable build with more localizations
* Fri Mar 18 2005 pmladek@suse.cz
- update ooo-build to version 1.9.79.2.3:
  * more printing-related crashes [#73034]
  * pspfontcache updating fix [#72012]
* Thu Mar 17 2005 pmladek@suse.cz
- update ooo-build to version 1.9.79.2.2:
  * XHTML export fix correction [#66586]
  * more interoperability problems between 2.0 and 1.1 fixes
  * mail merge defaults to Evo fix [#66217]
  * multiple components defining ColRowSettings fix [#72869]
  * printing-related memory corruptions fixes [#66353,#73034]
  * man page update
- updated custom images to version 13
- added bug-buddy to Requires [#73622]
* Tue Mar 15 2005 pmladek@suse.cz
- update ooo-build to version 1.9.79.2.1:
  * libwpd filter fix [#72313]
  * spellchecker crasher fix [#71931]
  * XHTML export fix [#66586]
  * look of toolbar in several KDE styles fix
  * interoperability between 2.0 and 1.1.4 fix [#71105]
  * XIM input method fix
  * size of some fields in create table wizard fix [#66809]
  * URL handling in KDE fpicker fix
- hidden the oomath menu entry
- updated crystal images to version 5
* Fri Mar 11 2005 pmladek@suse.cz
- update ooo-build to version 1.9.79.2:
  * many backported CWSs
  * top left toolbar handle fix
  * linking with mozilla libs fix
  * GNOME vfs fixes
  * GTK fpicker fixes
  * allow saving xml files
  * no send email dialog during crash recovery
  * canvas polyline
  * do not show help button without related help
  * better font resolving in KDE vclplug
  * crash with double lock
  * wrong condition
  * Email attachment
  * show popup on print error
  * better window title
  * remove non-functional starformats from save dialogs
  * wizards crash fix
  * crash in impress fix
  * better fonts for help
  * allow to update default java CLASSPATH
- updated extra translation sources: Czech, Danish, Estonian, Finnish,
  Hungarian, Slovak
- updated custom images to version 12
- updated crystal images to version 4
- added StartupNotify=true to desktop files [#71964]
- used %%distro_conf and %%vendor_conf to do no conflict with the %%vendor tag
- removed duplicate definition of extra_localizations_sources.tar.bz2 source
- added triggers to add mysql-connector-java.jar to the default CLASSPATH
  if installed [#67267]
- added pixmaps also to the non-conflicting package
* Fri Mar 04 2005 pmladek@suse.cz
- updated ooo-build to version 1.9.79.1:
  * huge amount of backported CWS
  * various crash fixes
  * wizard evo fix
  * fpicker fixes [#60527,#67124,#66765]
  * menu highlighting, check mark color, toolbar toggle buttons
  * oofromtemplate fix to work [#66694]
  * more support for Oasis MIME types
  * default multimedia menu item out
  * unused printer properties out
  * no larger default writer bullet
  * email attachment
  * VFS authentication problems
  * save only modified documents
  * more system libs
  * do not pack standard libs
  * updated icons
- updated extra translation sources: Danish, Estonian, Hungarian, Slovak
- removed wrong thesaurus dictionaries; only the English one is available
  in the new format
- updated ooo custom images to version 11
- updated libwpd to stable version 0.8.0
- fixed bash completion to support OASIS file types, directories; be able to
  finalize suffix
- removed obsolete extra icons [#10543]
- remove GNOME desktop files [#10543, #66084]
- fixed update of Danish help
- added unixODBC-devel, neon, and  neon-devel to neededforbuild
- enabled build with icecream
* Thu Feb 24 2005 pmladek@suse.cz
- updated to version src680-m79
- updated ooo-build to version 1.9.79:
  * target m79
  * new SUSE branding
  * zoom combo box
  * GNOME VFS under KDE too
  * connection to evolution address book
  * install-dict keep valid user changes
  * size of check-boxes and radio buttons
  * gallery alpha problem
  * crash on exit with Keramik theme
  * speed up the startup of the KDE NWF a bit
  * random .applications file install
  * relative URLs [#66704]
  * various fpicker fixes
  * user config dir ~/.ooo-2.0-pre, NovellOnly now
- updated extra translation sources: Czech, Estonian, Hungarian, Slovak
- added extra translation sources : Danish
- added OOo* wrappers to keep the backward compatibility [#66483,#66484]
- installed shared mime info for the new OASIS MIME types; added
  shared-mime-info to neededforbuild
- updated desktop to know the new Oasis desktop files
- added new desktop file for oobase
- removed the ugly desktop file for oopadmin
- used new icons for calc, draw, impress, and writer; made by jimmac
* Fri Feb 18 2005 pmladek@suse.de
- updated ooo-build to version 1.9.78.2:
  * the GTK splash hint to the window remove
  * GNOME fpicker: easier selection, filter detection
  * fallback for the crystal icons
  * use DESKTOP_LAUNCH in 'Send Document as E-mail'
  * fpicker hanging on accessible desktop
  * allows to install more dictionaries
  * recode documentation of dicts to UTF-8
  * localizations updating from extra sources
  * bash completion for OOo wrappers
  * default writer bullet larger
- updated crystal images to version 3
- updated hyphen dictionaries: Spanish
- added hyphen dictionaries: American, Bulgarian, British, Danish, Estonian,
  German, Greek, Russian
- added Thesaurus dictionaries: Bulgarian, Polish, Spanish
- added sources to update localization: Czech, Estonian, Finish, Hungarian,
  Dutch, Slovak
- added recode to neededforbuild; necessary to recode documentation
  of dictionaries to UTF-8
* Fri Feb 11 2005 pmladek@suse.cz
- updated to version src680-m78
- updated ooo-build to version 1.9.78.1:
  * target 78
  * support for oobase
  * zoom combo box
  * +/- keysyms fix
  * make speed-status-bar more effective
  *  datapilot crash fix
  * insensitive toolbar icons looks far nicer
  *  make ESC to work in Calc print preview
  * web bullet export improving
  * autotriming of autofilter choices fix
  * datapilot remote database crash fix
  * search more localizations for system Mozilla
  * GNOME-VFS support
  * allow insertion of rows instead of overwriting
  * extra HTML export / navigation artwork
  * enable cups
- updated icons to custom_images-10 and crystal_images-2
- added extra HTML export and navigation artwork as extras-1
- cleaned up PreReq tags
- do not require scalable-font when built for older distributions
- moved GNOME desktop files to the gnome subpackage
* Thu Feb 03 2005 pmladek@suse.cz
- updated to version src680-m74
- updated ooo-build to version 1.9.74:
  * target m74
  * QPro infinite loop fix
  * center icons when up-sizing
  * excessive vertical toolbar bloat
  * internal libcurl installation fix
  * better versioning in Help->About dialog
  * new KDE NWF toolbars and menus
  * E-mail attachment file name append with "_1"
  * do not depend on evolution-data-server at buildtime
- removed obsolete evolution-data-server and evolution-data-server-devel
  from neededforbuild
- installed GNOME desktop files
- updated ooo_custom_images to version 7
- configured requested localizations via the new --with-lang option
* Fri Jan 28 2005 pmladek@suse.cz
- updated to version src680-m72
- updated ooo-build to version 1.9.72:
  * alpha and themable WM icons
  * ported some fixes for sc from OOo-1.1.3
  * QPro filter detection
  * support for lang subpackages, dictionaries
- fixed to install stuff that is not yet localized
- partly fixed GNOME desktop files
- temporary added fixed desktop files and extra icons for SUSE menu
- added ar, ca ,cs, da ,el, et, fi, hu, ja, ko, nl, pl, pt, ru, sk, sl, sv, tr
  subpackages
- removed obsolete triggers
- improved spec file to simply create non-conflicting packages for
  older products; it is just enough to change the Name tag
* Wed Jan 19 2005 pmladek@suse.cz
- updated to version src680-m70
- updated ooo-build to version 1.9.70:
  * fixes to build with src680-m70
  * improved icons
  * email as MS attachment
  * default optimal page wrap
  * calc / filter fixes
  * drop libart bits
  * build fixes
- renamed package to OpenOffice_org, set prefix to /usr/lib/ooo-2.0,
  removed the wrappers suffix 1.7, set the package version to 1.9.70
- added de, es, fr and it subpackages
- added extra hyphen and thesaurus dictionaries
- used ooo_custom_images instead of gnome icons sources
- added libsoup and libsoup-devel to neededforbuild and fixed build with
  evolution-data-server-1.2
* Fri Dec 17 2004 pmladek@suse.cz
- updated to version src680-m65
- updated ooo-build to version 1.7.4:
  * update the splash screen
  * improve GTK fpicker
  * build with system mozilla, libxml, zlib
  * many build and installation fixes for src680-m65
- added KDE and GNOME icons
- added mozilla and mozilla-devel to neededforbuild
* Mon Dec 13 2004 pmladek@suse.cz
- updated to version src680-m62
- updated ooo-build to version 1.7.2:
  * adds KDE and GNOME widgets, icons
  * build against system python
  * many build and installation fixes of src680-m62
- created kde and gnome subpackages
- added %%post and %%preun scripts to update reps. remove UNO cache
- unified spec file for NLD and SL
* Tue Nov 02 2004 pmladek@suse.cz
- package created, src680-m57, ooo-build-1.7.0.4
- single package providing English localization only
