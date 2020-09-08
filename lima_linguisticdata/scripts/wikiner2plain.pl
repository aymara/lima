#!/usr/bin/perl -w

use strict;
use utf8;

binmode(STDIN, ":encoding(utf-8)");
binmode(STDOUT, ":encoding(utf-8)");

while(<STDIN>) {
  chomp;
  my @gold = split;

  if(/^$/){
    #print "-DOCSTART- O O\n\n";
    next;
  }

  my @words;
  while (@gold) {
    my $gold = shift @gold;
    my @gfields = split /[_|]/, $gold;
    my $gword = shift @gfields;
    my $gtag = pop @gfields;

    push @words, $gword;
  }

  my $sent = join(" ", @words);
  while ($sent =~ s/([a-z]+)\s(\'s\s)/$1$2/gi) { }
  while ($sent =~ s/([a-z]s)\s([\']\s)/$1$2/gi) { }
  while ($sent =~ s/\s([,\.\?\!\:\;\)])/$1/g) { }
  while ($sent =~ s/([\(])\s/$1/g) { }

  print "$sent\n";
}
