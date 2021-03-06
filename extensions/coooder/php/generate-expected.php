#!/usr/bin/php
<?php
/*
 *   OpenOffice.org extension for syntax highlighting
 *   Copyright (C) 2008  Cédric Bosdonnat cedricbosdo@openoffice.org
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Library General Public
 *   License as published by the Free Software Foundation; 
 *   version 2 of the License.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this library; if not, write to the Free
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

include("geshi/geshi.php");

// get the arguments

$sourceFile = $argv[1];
$destDir = $argv[2];

// Read the file
$source = file_get_contents($sourceFile);
$lang = basename($sourceFile, ".txt");

// Run Geshi
$geshi = new GeSHi($source, $lang);
$highlighted = $geshi->parse_code();

// Write to the ouput directory
file_put_contents("$destDir/$lang.html", $highlighted);
