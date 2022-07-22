#!/usr/bin/perl -s

# Copyright 2002-2019 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
    #if (/(.*):(ABS_)?IDIOM\$([^:]*)(.*)/) {
    # category can contain a ":" but normalized form should not
    if (/(.*):(ABS_)?IDIOM\$(.*)(:.*)/) {
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
