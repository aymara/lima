#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
