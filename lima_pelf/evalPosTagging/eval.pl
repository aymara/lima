#!/usr/bin/perl -s

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

#
# Perl Program created by Romaric Besancon on Wed Mar 17 2010
# Version : $Id$ 

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
usage eval.pl [options]
EOF
    exit;
}

use strict;

# global variables
my $nAll=0;
my $okAll=0.0;
my $lineAll=0;
my $notAlignedAll=0;
my $ignoredAll=0;

my $file;
foreach $file (@ARGV) {
    &evalFile($file);
}

if (scalar @ARGV>1) {
    print "all\tlines\t$lineAll\n";
    print "all\tignored\t$ignoredAll\n";
    print "all\ttreated\t$nAll\n";
    print "all\tnot_aligned\t$notAlignedAll\n";
    print "all\tok\t$okAll\n";
    if ($nAll) {
        printf("all\tprecision\t%2.2f%%\n",$okAll/$nAll*100);
    }
    else {
        printf("all\tprecision\t0.0%%\n");
    }
}

sub evalFile {
    my ($file)=@_;
    if (!open(FIN,$file)) {
        print STDERR "Error: cannot open file $file\n";
        return;
    }
    my $n=0;
    my $ok=0.0;
    my $line=0;
    my $notAligned=0;
    my $ignored=0;
    while (<FIN>) {
        $line++;
        chomp;
        if (m%<CASE [^>]*><TOK tag=([^>]*)>([^<]*)<TOK tag=([^>]*)>(.*)%) {
            my ($tag1,$w1,$tag2,$w2)=($1,$2,$3,$4);
            if ($w1 ne $w2) {
                print STDERR "$file: Warning line $line: words not aligned ($w1/$w2)\n";
                $notAligned++;
            }
            else {
                if ($tag1 eq $tag2) {
                    $ok++;
                }
                elsif ($main::debug) {
                    print STDERR "$file: tag error: $w1 is tagged $tag1, should be $tag2\n";
                }
                $n++;
            }
        }
        else {
            print STDERR "$file: cannot parse line ($line): $_\n";
            $ignored++;
        }
    }
    
    print "$file\tlines\t$line\n";
    print "$file\tignored\t$ignored\n";
    print "$file\ttreated\t$n\n";
    print "$file\tnot_aligned\t$notAligned\n";
    print "$file\tok\t$ok\n";
    if ($n) {
        printf("$file\tprecision\t%2.2f%%\n",$ok/$n*100);
    }
    else {
        printf("$file\tprecision\t0.0\n");
    }

    $nAll+=$n;
    $lineAll+=$line;
    $ignoredAll+=$ignored;
    $notAlignedAll+=$notAligned;
    $okAll+=$ok;
}
