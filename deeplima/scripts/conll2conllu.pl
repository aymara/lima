#!/usr/bin/perl -w

# Copyright 2021 CEA LIST
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

my $idx = 1;
my $lines_printed = 0;
my $separator_printed = 0;

while(<STDIN>) {
  chomp;
  if ($_ =~ /^\s*$/ || $_ =~ /DOCSTART/) {
    if ($lines_printed > 0 && $separator_printed == 0) {
      print("\n");
      $separator_printed = 1;
    }
    $idx = 1;
    next;
  }

  my ($gword, $a, $b, $gtag) = split(/\s+/, $_); 

  $gword =~ s/[_]/\\_/g;
  $gword =~ s/^\s*$/\\_/g;

  print join("\t", ($idx, $gword, "_", $gtag, "_", "_", "_", "_", "_", "_")) . "\n";
  $lines_printed += 1;
  $separator_printed = 0;

  $idx += 1;
}

