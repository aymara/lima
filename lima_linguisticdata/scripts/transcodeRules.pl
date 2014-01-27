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
usage transcode.pl [-help] symbolicCodeFile xxxCode.xml
EOF
    exit;
}

require $ENV{'LINGUISTIC_DATA_ROOT'}."/scripts/dictionarycode.pl";

use strict;

die "need two files (see -help)" unless @ARGV>1;

&readNumericCodes(shift @ARGV);
&readSymbolicCodes(shift @ARGV);

while (<>) {
    if (/(.*)\$([^:]*)(:[^:]*)$/) {
        print "$1\$".&transcode($2).$3;
    }
    else {
        print;
    }
}

sub transcode {
    my ($code) = @_;
    my ($output);

    if ($main::numericToSymbolic) {
        $output=&NumericToSymbolic($code);
    }
    elsif ($main::numericToText) {
        $output=&NumericToText($code);
    }
    elsif ($main::symbolicToText) {
        $output=&SymbolicToText($code);
    }
    else {
        $output=&SymbolicToNumeric($code);
    }

    return $output;
}
