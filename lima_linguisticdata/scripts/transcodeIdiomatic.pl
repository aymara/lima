#!/usr/bin/perl -s
#   Copyright 2002-2019 CEA LIST
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

my @args;
my @remaining_args;
foreach my $arg (@ARGV) {
    if ($arg =~ /^\-\-/) {
        push @args, $arg;
    } else {
        push @remaining_args, $arg;
    }
}

my $argline = join(" ", @args);

die "need convertSymbolicCodes path and two files (see -help)" unless (@remaining_args > 2);

my %conv;
my $convertSymbolicCodes = shift @remaining_args;
my $codeFile = shift @remaining_args;
my $rulesFile = pop @remaining_args;

&getCodeConversion($codeFile, @remaining_args);

my @conv_keys = keys %conv;
if ($#conv_keys < 0) {
    die "ERROR: transcodeIdiomatic.pl: can't load conv keys\n";
}

open(FIN,$rulesFile) || die "cannot open $rulesFile";
while (<FIN>) {
    if (/(.*):(ABS_)?IDIOM\$([^:]*)(.*)/) {
        if (! exists($conv{$3})) {
            #print STDERR "transcodeIdiomatic.pl: can't find conv for \"$3\"\n";
        }
        print "$1:$2IDIOM\$".$conv{$3}.$4."\n";
    } else {
        print;
    }
}
close(FIN);

sub getCodeConversion {
    my ($codeFile, @symbCodeFiles) = @_;
    if ( !open(FCONV,"$convertSymbolicCodes $argline --code=$codeFile " .  join(" ", @symbCodeFiles) . " |") ) {
        print STDERR "cannot get code conversion: $!\n";
        exit(1);
    }

    while(<FCONV>) {
        /(.*);(.*);/;
        $conv{$1} = $2;
    }

    close(FCONV);
}
