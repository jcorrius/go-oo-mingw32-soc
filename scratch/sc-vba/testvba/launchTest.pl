#*************************************************************************
#
#   OpenOffice.org - a multi-platform office productivity suite
#
#   $RCSfile: createExtPackage.pl,v $
#
#   $Revision: 1.2 $
#
#   last change: $Author: rt $ $Date: 2007/04/25 16:15:07 $
#
#   The Contents of this file are made available subject to
#   the terms of GNU Lesser General Public License Version 2.1.
#
#
#     GNU Lesser General Public License Version 2.1
#     =============================================
#     Copyright 2005 by Sun Microsystems, Inc.
#     901 San Antonio Road, Palo Alto, CA 94303, USA
#
#     This library is free software; you can redistribute it and/or
#     modify it under the terms of the GNU Lesser General Public
#     License version 2.1, as published by the Free Software Foundation.
#
#     This library is distributed in the hope that it will be useful,
#     but WITHOUT ANY WARRANTY; without even the implied warranty of
#     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#     Lesser General Public License for more details.
#
#     You should have received a copy of the GNU Lesser General Public
#     License along with this library; if not, write to the Free Software
#     Foundation, Inc., 59 Temple Place, Suite 330, Boston,
#     MA  02111-1307  USA
#
#*************************************************************************

use File::Basename;

my $test_class = shift || die 'must provide a ext name';

die "can't access TestClass $test_class/TestVBA.class" unless -f "$test_class/TestVBA.class"; 
die "can't access officepath \$OFFICEPATH" unless -d $ENV{OFFICEPATH}; 
die "can't access testdocument \$TESTDOCUMENT" unless -d $ENV{TESTDOCUMENT}; 
die "testdocument not of the correct structure $ENV{TESTDOCUMENT}/logs/excel" unless -d "$ENV{TESTDOCUMENT}/logs/excel"; 
die "can't access output dir \$OUTPUTDIR" unless -d $ENV{OUTPUTDIR}; 


my $officeclasspath = "$ENV{OFFICEPATH}/program/classes/"; 
my $classpath = "$officeclasspath/jurt.jar:$officeclasspath/unoil.jar:$officeclasspath/jut.jar:$officeclasspath/juh.jar:$officeclasspath/java_uno.jar:$officeclasspath/ridl.jar:$test_class:$ENV{CLASSPATH}";
$ENV{CLASSPATH}=$classpath;
print "classpath $ENV{CLASSPATH}\n";
my $status = system("java -classpath $ENV{CLASSPATH} TestVBA $ENV{TESTDOCUMENT} $ENV{OUTPUTDIR}" );
