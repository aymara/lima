#!/usr/bin/perl -w

# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;
use utf8;

binmode(STDIN, ":encoding(utf-8)");
binmode(STDOUT, ":encoding(utf-8)");

my $print_gold_tags = 0;
my $print_doc_start = 0;

foreach my $arg (@ARGV) {
  if ($arg eq "--print-gold-tags") {
    $print_gold_tags = 1;
  } elsif ($arg eq "--print-doc-start") {
    $print_doc_start = 1;
  }
}

while(<STDIN>) {
  chomp;
  my @gold = split;

  if(/^$/) {
    if ($print_doc_start) {
      print "-DOCSTART- O O\n\n";
    }
    next;
  }

  my $idx = 1;

  while (@gold) {
    my $gold = shift @gold;
    my @gfields = split /[_|]/, $gold;
    my $gword = shift @gfields;
    my $gtag = pop @gfields;

    $gword =~ s/[_]/\\_/g;
    $gword =~ s/^\s*$/\\_/g;

    if ($print_gold_tags) {
      print join("\t", ($idx, $gword, "_", $gtag, "_", "_", "_", "_", "_", "_")) . "\n";
    } else {
      print join("\t", ($idx, $gword, "_", "_", "_", "_", "_", "_", "_", "_")) . "\n";
    }

    $idx += 1;
  }

  print "\n";
}
