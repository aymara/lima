#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open (IN1,"<:utf8","$ARGV[0]") || die ("Impossible d'ouvrir $ARGV[0]");
open (IN2,"<:utf8","$ARGV[1]") || die ("Impossible d'ouvrir $ARGV[1]");
open (OUT1,">:utf8","$ARGV[2]") || die ("Impossible d'ouvrir $ARGV[2]");
open (OUT2,">:utf8","$ARGV[3]") || die ("Impossible d'ouvrir $ARGV[3]");

my $l1,$l2;
while (($l1=<IN1>) && ($l2=<IN2>)) {
  chop($l1);
  chop($l2);
  if (($l1 ne "") and ($l2 ne "")) {
    print OUT1 $l1."\n";
    print OUT2 $l2."\n";
  }
}
