# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
find examples of a given trigram in the corpus:
tags must be in Grace format or in S2 format (L_..) with option -conv

usage find_trigram_in_corpus.pl [-conv=..] tag1 tag2 tag3 corpus_file
 -conv=.. : file indicating the conversion between tags given in 
            arguments and tags in the corpus
 -context=n : indicates the number of words to print before
              the trigram \(default is 0\)

each of the tags can be replaced by "ANY" to match any tag
EOF
    exit;
}

use strict;

die "need four arguments (see -help)" unless @ARGV>=3;

my $tag1=shift @ARGV;
my $tag2=shift @ARGV;
my $tag3=shift @ARGV;

my (%tag1,%tag2,%tag3);

my (%Convert);
if ($main::conv) {
    &readConversionFile($main::conv,\%Convert);
    %tag1=%{$Convert{$tag1}} if $tag1 ne "ANY";
    %tag2=%{$Convert{$tag2}} if $tag2 ne "ANY";
    %tag3=%{$Convert{$tag3}} if $tag3 ne "ANY";
}
else {
    $tag1{$tag1}=1 if $tag1 ne "ANY";
    $tag2{$tag2}=1 if $tag2 ne "ANY";
    $tag3{$tag3}=1 if $tag3 ne "ANY";
}
print STDERR "looking for trigram [$tag1 $tag2 $tag3]
";

my ($tagPre,$tagPre2,$wordPre,$wordPre2);
my (@contextBefore,@contextAfter);

while (<>) {
    next if (/^</);
    chop;
    my ($word,$tag)=split;
    if ((exists $tag1{$tagPre2} || $tag1 eq "ANY") &&
        (exists $tag2{$tagPre}  || $tag2 eq "ANY") &&
        (exists $tag3{$tag}     || $tag3 eq "ANY") ) {
        if ($main::context) {
            print join(" ",@contextBefore)."	";
        }
        print "$wordPre2 $wordPre $word
";
    }
    if ($main::context) {
        push @contextBefore, $wordPre2;
        shift @contextBefore if (@contextBefore>$main::context);
    }

    $tagPre2=$tagPre;
    $tagPre=$tag;
    $wordPre2=$wordPre;
    $wordPre=$word;
}

sub readConversionFile {
    my ($file,$convert) = @_;
    open(FIN,$file) || die "cannot open file $file";
    while (<FIN>) {
        next if (/^\#/);
        chop;
        split;
        $$convert{$_[0]}{$_[1]}=1;
        $$convert{$_[1]}{$_[0]}=1; # store both (simpler)
    }
}
