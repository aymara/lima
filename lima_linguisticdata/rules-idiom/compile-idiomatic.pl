#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
#!/usr/bin/perl -s
#
# Perl program created by Besancon Romaric on lun ao 30 2004
# Version : $Id$

use strict;

# is in the PATH
my $compile="compile-rules";

if ($main::encoding) {
    $compile.=" --encoding=".$main::encoding;
}

my @dirs;

if (@ARGV == 0) {
    @dirs=split `find . -maxdepth 1 -type d | grep -v CVS | grep -v \"^\.$\"`;
}
else {
    @dirs=@ARGV;
}

my $dir;
foreach $dir (@dirs) {
  my $lang=`basename $dir`;
  chop $lang;
  my $file="idiomaticExpressions-$lang.rules";
  my $rulesfile="idiomaticExpressions-$lang.bin";
  my $trigsfile="idiomaticExpressionsTriggers-$lang.xml";
  my $tmpfile="tmpfile";
  my $tmpfileTriggers="tmpfile-triggers";
  my (%CodesList,%MacroCodes);

  my $codePath="../analysisDictionary/$lang/code";

  open(FOUT,">$lang/$tmpfile") || die "cannot open temporary file $tmpfile";
  print "compiling $file
";
  system("cd $lang;$compile --language=$lang --output=$rulesfile $file");
}
