#!/bin/sh

#   OpenOffice.org extension for syntax highlighting
#   Copyright (C) 2008  Cédric Bosdonnat cedricbosdo@openoffice.org
#
#   This library is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Library General Public
#   License as published by the Free Software Foundation; 
#   version 2 of the License.
#
#   This library is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Library General Public License for more details.
#
#   You should have received a copy of the GNU Library General Public
#   License along with this library; if not, write to the Free
#   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

OUTPUT_DIR=../config/org/openoffice/langs
GESHI_DIR=/media/stockage/perso/Web/mirror/ftpperso.free.fr/wordpress/wp-content/plugins/wp-syntax/geshi/geshi

mkdir $OUTPUT_DIR

for phpFile in `ls $GESHI_DIR/*.php`
do
    echo "Converting $phpFile"
    php ./from-geshi.php $phpFile $OUTPUT_DIR
done
