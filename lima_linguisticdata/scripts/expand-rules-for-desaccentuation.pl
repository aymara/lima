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

open (CONV,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (RULES,"<:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[0]");
open (OUT,">:utf8","$ARGV[2]") || die ("Impossible d'ouvrir $ARGV[0]");

my %conv;
my %defconv;
my $trigs=0;

while (<CONV>)
{
    chop();
    my @data=split(/\s/);
    if ($data[0] ne $data[1]) {
        $conv{$data[0]} = $data[1];
        # print "$data[0] => $data[1] \n";
    }
}

close(CONV);

my $inClass = 0;
while (<RULES>) {
    if ($inClass)
    {
        if (/\)/) {
            # print STDERR "inclass becomes false with $_";
            $inClass = 0;
        }
        print OUT $_;
        next;
    }
    if (/^set/ or /^use/ or /^#/ or /^\+/ or /^\@[A-Za-z0-9]+=\([^)]*\)/) {
        print OUT $_;
        next;
    }
    # to jump multi-lines class definitions
    #elsif ('@AuxiliaireEtre=(être$V-VERBE_AUXILIAIRE_INDICATIF,' =~ /^\@[A-Za-z0-9]+=\([^)]*/) {
    elsif (/^\@[A-Za-z0-9]+=\([^\)]*,$/) {
        #print STDERR "inclass becomes true with $_";
        $inClass = 1;
        print OUT $_;
        next;
    }
    if (! /\w/) {
        print OUT $_;
        next;
    }
    @part=split(/:/);
    if (@part==1) {
        print OUT $_;
        next;
    }
    # traitement du declencheur
    {
        my $w=$part[0];
        $w =~ s/^\[//;
        $w =~ s/\]$//;
        if ($part[0] =~ /[\@\$\*]/) {
            next;
        } elsif ($part[0] =~ /(\&?)([^\&\$\@\(\)0-9]+)/) {
            my $w=$2;
            # print "handle $w in $part[0]\n";
            if (defined($conv{$w})) {
                # print "replace with $conv{$w}\n";
                if (not defined($defconv{$w})) {
                    print OUT "\@trig$trigs=($w,$conv{$w})\n";
                    $defconv{$w}="\@trig$trigs";
                    $trigs++;
                }
                $part[0] = $1.$defconv{$w};
            }
        }
    }
    
    
    for (my $i=1;$i<3;$i++)
    {
        @words=split(/\s/,$part[$i]);
        for(my $j=0;$j<scalar(@words);$j++) {
            if ($words[$j] =~ /\@\w+/) {
                next;
            } elsif ($words[$j] =~ /\$\w+/) {
                next;
            } elsif ($words[$j] =~ /([^\&\$\@\(\)0-9]+)/) {
                if (defined($conv{$1})) {
                    my $src=$1;
                    my $w1=$words[$j];
                    my $w2=$words[$j];
                $w2 =~ s/$src/$conv{$src}/g;
                    $words[$j]="($w1|$w2)";
                }
            }
        }
        $part[$i]=join(" ",@words);
    }
    print OUT join(":",@part);
}
