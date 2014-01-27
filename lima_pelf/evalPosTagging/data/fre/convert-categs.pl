#!/usr/bin/perl -s
#
# Perl Program created by Romaric Besancon on Tue Mar 16 2010
# Version : $Id$ 

# Help mode
if ($main::h || $main::help) {
    print <<EOF;
usage convert-categs.pl categ_file corpus
EOF
    exit;
}

use strict;
my $categs=shift @ARGV;
my %Categs;
&readCategs($categs,\%Categs);

while (<>) {
    chomp;
    next if m/^\w*$/;
    my ($word,$cat)=split(/\t/);
    if (exists $Categs{$cat}) {
        print "$word\t$Categs{$cat}\n";
    }
    else {
        print STDERR "Error: cannot find category '$cat' (for word '$word')\n";
        # leave line unchanged
        print "$word\t$cat\n";
    }
}

sub readCategs {
    my ($file,$categs)=@_;
    open(FIN,$file) || die "cannot open $file";
    while(<FIN>) {
        chomp;
        my ($symb,$cat)=split(/\t/);
        $$categs{$symb}=$cat;
    }
    close(FIN);
}
