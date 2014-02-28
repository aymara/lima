#!/usr/bin/perl -s
#
# Perl Program created by Romaric Besancon on Thu Mar 18 2010
# Version : $Id$ 

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
usage aligner.pl [options] ref test
EOF
    exit;
}

use strict;
my $tmpfile="/tmp/aligner.tmp";

die "need two args (see -help)" unless @ARGV>=2;
my ($refFile,$testFile)=@ARGV;

my $PELF_BIN_PATH = "$ENV{'LIMA_DIST'}/share/apps/lima/scripts/";
system("$PELF_BIN_PATH/words.sh $refFile > $tmpfile.ref.words");
system("$PELF_BIN_PATH/words.sh $testFile > $tmpfile.test.words");

my (@IgnoredRefPos,@IgnoredTestPos);
open(FDIFF,"diff $tmpfile.ref.words $tmpfile.test.words |");
my (@refWords,@testWords);
my ($refPos,$op,$testPos);
while(<FDIFF>) {
    chomp;
    if (/^---$/) {      
    }
    elsif (/^> (.*)/) { # test
        push @testWords,$1;
    }
    elsif (/^< (.*)/) { # ref
        push @refWords,$1;
        
    }
    elsif (m%(.*)([adc])(.*)%) {
        if (@testWords && @refWords) {
            print STDERR "alignment error: ".
                $refPos."/".$testPos."\t".
                join("/",@refWords)."\t".
                join("/",@testWords)."\n";
        }
        ($refPos,$op,$testPos)=($1,$2,$3);
        if ($op eq 'c') { # change: both are affected
            &updatePosTable(\@IgnoredRefPos,$refPos);
            &updatePosTable(\@IgnoredTestPos,$testPos);
        }
        elsif ($op eq 'a') { # add: ignore only test
            &updatePosTable(\@IgnoredTestPos,$testPos);
        }
        elsif ($op eq 'd') { # delete: ignore only ref
            &updatePosTable(\@IgnoredRefPos,$refPos);
        }
        undef @testWords;
        undef @refWords;
    }
    else {
        print STDERR "Diff: cannot parse line: $_\n";
    }
}
# last one
if (@testWords && @refWords) {
    print STDERR "alignment error: ".
        $refPos."/".$testPos."\t".
        join("/",@refWords)."\t".
        join("/",@testWords)."\n";
}
close(FDIFF);
print STDERR "ignored(ref)=".join(",",@IgnoredRefPos)."\n";
print STDERR "ignored(test)=".join(",",@IgnoredTestPos)."\n";

open(FREF,$refFile) || die "cannot open $refFile";
open(FTEST,$testFile) || die "cannot open $testFile";
my $done=0;
my ($nRef,$nTest)=(1,1); # line numbers
my ($iRef,$iTest)=(0,0); # indices in the ignoredPosTable
my ($lineRef,$lineTest);
my $nCase=0;
while (! $done) {
    #print STDERR "-- $nRef, $iRef, $IgnoredRefPos[$iRef]\n";
    #print STDERR "-- $nTest, $iTest, $IgnoredTestPos[$iTest]\n";
    while ($nRef==$IgnoredRefPos[$iRef]) {
        $lineRef=<FREF>;
        $nRef++;
        $iRef++;
    }
    $lineRef=<FREF>;
    $nRef++;
    while ($nTest==$IgnoredTestPos[$iTest]) {
        $lineTest=<FTEST>;
        $nTest++;
        $iTest++;
    }
    $lineTest=<FTEST>;
    $nTest++;

    if ($lineRef eq "" && $lineTest eq "") {
        $done=1;
        last;
    }

    chomp $lineRef;
    chomp $lineTest;

    $nCase++;
    my ($refWord,$refTag)=split("\t",$lineRef);
    my ($testWord,$testTag)=split("\t",$lineTest);
    printf("<CASE no=%06s><TOK tag=%s>%s<TOK tag=%s>%s\n",
           $nCase,$refTag,$refWord,$testTag,$testWord);
}

sub updatePosTable {
    my ($posTable,$pos)=@_;
    if ($pos=~/^([0-9]*),([0-9]*)/) {
        my ($posBegin,$posEnd)=($1,$2);
        push @{$posTable}, ($posBegin..$posEnd);
    }
    elsif ($pos=~/^([0-9]*)/) {
        push @{$posTable},$pos;
    }
    else {
        print STDERR "Diff: cannot parse position $pos\n";
    }
}
