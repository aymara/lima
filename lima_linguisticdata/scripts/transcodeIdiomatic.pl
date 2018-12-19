#!/usr/bin/perl -s
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
#
# Perl Program created by Besancon Romaric on Wed Mar 10 2004
# Version : $Id$

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
transcode symbolic codes to numeric
usage transcode.pl /path/to/convertSymbolicCodes code-xxx.xml symbolicCode-xxx.xml rules
EOF
    exit;
}

use strict;

die "need convertSymbolicCodes path and two files (see -help)" unless @ARGV>2;

my %conv;
my $convertSymbolicCodes=shift @ARGV;
my $codeFile=shift @ARGV;
my $rulesFile=pop @ARGV;

&getCodeConversion($codeFile,@ARGV);

open(FIN,$rulesFile) || die "cannot open $rulesFile";
while (<FIN>) {
    if (/(.*):(ABS_)?IDIOM\$([^:]*)(.*)/) {
        print "$1:$2IDIOM\$".$conv{$3}.$4."\n";
    }
    else {
        print;
    }
}
close(FIN);

sub getCodeConversion {
    my ($codeFile,@symbCodeFiles)=@_;
    if (!open(FCONV,"$convertSymbolicCodes --code=$codeFile ".
	      join(" ",@symbCodeFiles)." |")) {
	print STDERR "cannot get code conversion: $!\n";
	exit(1);
    }								    
    while(<FCONV>) {
	/(.*);(.*);/;
	$conv{$1}=$2;
    }
    close(FCONV);
}
