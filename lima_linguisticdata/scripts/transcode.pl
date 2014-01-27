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
usage transcode.pl [-help] code-xxx.xml symbolicCode-xxx.xml
where xxx is the language trigram;
EOF
    exit;
}

require $ENV{'LINGUISTIC_DATA_ROOT'}."/scripts/dictionarycode.pl";

use strict;

die "need two files (see -help)" unless @ARGV>1;

&readNumericCodes(shift @ARGV);
&readSymbolicCodes(shift @ARGV);

if ($main::dictionary) { 
    #default is numericToSymbolic
    $main::numericToSymbolic=1 unless 
        $main::SymbolicToNumeric ||
        $main::SymbolicToText ||
        $main::NumericToText;
    
    while (<>) {
        if (m%^(.*<p>)([0-9]*)(</p>.*)$%) {
            my ($pre,$code,$post)=($1,$2,$3);
            my $newCode=&transcode($code);
            $newCode=~s/\-/-/g; # remove \ before -
            print $pre.$newCode.$post."\n";
        }
        else {
            print;
        }
    }
}
else {
    while (<>) {
        chop;
        print "$_;".&transcode($_).";\n";
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
