#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
