#!/usr/bin/perl -s

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

#
# Perl Program created by besancon on Mon Nov  8 2004
# Version : $Id$


sub usage {
    print <<EOF;
usage test-positions.pl [-noAnalysis] [-dataDir=<dir>] [-language=<lang>] [files]
  if files are indicated, test only on these, otherwise test on *.xml
  in data dir

  -noAnalysis  : do not launch analysis, use existing result
  -datadir=..  : indicate the directory containing data \(default is 'data'\)
  -language=.. : indicate the language \(default is 'fre'\)
  -h | -help  : display this help message
EOF
}

#use autodie qw(:all);
use strict;


# Help mode
if ($main::h || $main::help) {
    usage();
    exit 0;
}

print STDERR "PATH: $ENV{PATH}\n";
print STDERR "LD_LIBRARY_PATH: $ENV{LD_LIBRARY_PATH}\n";

# default value for data dir
$main::datadir="data" unless $main::datadir;
$main::language="fre" unless $main::language;

my (@files,$file);

if (@ARGV==0) {
    opendir(DIR,$main::datadir) || die("Cannot open dir $main::datadir");
    @files=map {$_=$main::datadir."/".$_;} grep { /.xml$/; } readdir(DIR);
    closedir(DIR);
}
else {
    @files=@ARGV;
}

my $nbFiles=scalar @files;
if ($nbFiles == 0) {
    print STDERR "No input files provided. Abort\n";
    usage();
    exit 1;
}

print STDERR "test-positions.pl\n";
print STDERR "Input files:\n ".join(", ",@files)."\n";


my (%RefPositions);
my $nbErrorsFile=0;

if (! $main::noAnalysis) {
    my @args = ("analyzeXml", "--language=$main::language", join(" ",@files));
    system(join(" ",@args)) == 0
        or die "system @args failed: $?";
}

foreach $file (@files) {
    if (! -e "$file.pos.ref") {
        print STDERR "no reference position file for $file: ignored\n";
        next;
    }
    undef %RefPositions;
    next unless &readRefPositions("$file.pos.ref",\%RefPositions);

    if ($main::noAnalysis) {
        if (! -e "$file.mult") {
            print STDERR "no existing result for $file: ignored\n";
            next;
        }
    }

    my $nbErrors=0;
    my $nbTested=0;
    my $nbTest=keys %RefPositions;
    open(FBOW,"readMultFile --output-format=xml $file.mult |")
      or die "Can't execute readMultFile pipe!";
;
    while (<FBOW>) {
        if (m%<bowToken id="[^\"]*" lemma="([^\"]*)" category="[^\"]*" position="([^\"]*)" length="([^\"]*)"/>%) {
            my $lemma=$1;
            my $pos=$2;
            my $len=$3;
            $nbTested++;
            if (! exists ${$RefPositions{$lemma}}{$pos}) {
                print STDERR "$file:position of \"$lemma\" is $pos, should be ".
                    join(",",sort { $a <=> $b }
                         keys %{$RefPositions{$lemma}})."\n";
                $nbErrors++;
            }
        }
    }
    close(FBOW);
    if ($nbErrors || $nbTest != $nbTested) {
        print STDERR "$file: FAILED.\t$nbErrors errors found . $nbTested tested positions . $nbTest reference positions\n";
        $nbErrorsFile++;
    }
    else {
        print "$file: OK.\tValidated $nbTested/$nbTest positions of bowToken\n";;
    }
}
print "Errors in $nbErrorsFile file out of ".(scalar @files)."\n";
if ($nbErrorsFile) {
    exit 1;
}
exit 0;


sub readRefPositions {
    my ($file,$RefPositions)=@_;
    local ($_);
    if (! open(FREF,$file)) {
        print STDERR "cannot open file $file\n";
        return 0;
    }
    while (<FREF>) {
        chop;
        my @fields = split;
        ${$$RefPositions{$fields[0]}}{$fields[1]}=1;
    }
    close(FREF);
    return 1;
}

