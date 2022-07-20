#!/usr/bin/perl -W

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;

# 
# Perl script written by Gael de Chalendar on Tue Aug 02 2004
# code taken from dictionarycode.pl by Romaric Besancon
# Version : $Id$ 

$#ARGV eq 1 || die "Must have exactly two arguments";

my ($filenameCodes,$lang) = @ARGV;
my ($categoryName,$valueCodeName);

open(CODESLIST,">codesList-$lang.dat")
  || die "Cannot create codes list file: codesList-$lang.dat\n";
print CODESLIST "# Warning: generated file; do NOT edit !\n";
print CODESLIST "NONE_1;0\n";
open(MACROCODESLIST,">macroCodesList-$lang.dat")
  || die "Cannot create macro codes list file: macroCodesList-$lang.dat\n";
print MACROCODESLIST "# Warning: generated file; do NOT edit !\n";
print MACROCODESLIST "NONE_1\n";

open(CATEGSLIST,">categoriesNames.dat")
  || die "Cannot create categories names file: categoriesNames-$lang.dat\n";
print CATEGSLIST "NONE\n";
  
# xxxCode.xml is generally in UTF16 
# 2004/08/08 - gael - not anymore in utf16
open(FILECODES,"$filenameCodes") 
  || die "cannot open code file $filenameCodes";

while(<FILECODES>) 
{
  if (m%<cat_name>([^<]*)</cat_name>%) 
  {
    $categoryName=$1;
    print CATEGSLIST "$categoryName\n";
  }
  if (m%<n>([^<]*)</n>%) 
  {
    $valueCodeName=$1;
    print CODESLIST "$valueCodeName\n";
    if ($categoryName eq "MACRO")
    {
      print MACROCODESLIST "$valueCodeName\n";
    }
  }
}

close(FILECODES);
close(CODESLIST);
close(MACROCODESLIST);
close(CATEGSLIST);
