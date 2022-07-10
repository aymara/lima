#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

open (IN,"<:utf8",$ARGV[0]) or die("can't open file $ARGV[0]");
open (OUT,">:utf8",$ARGV[1]) or die("can't open file $ARGV[1]");

while (<IN>)
{
  chop();
  @fields = split("	");
  print OUT reverse(shift(@fields))."	",join("	",@fields)."\n";
}
