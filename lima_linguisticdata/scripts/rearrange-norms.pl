#!/usr/bin/perl

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

while (<>) {
  chop();
  my @data = split ("	");
  my $tmp=$data[3];
  $data[3]=$data[4];
  $data[4]=$tmp;
  print join("	",@data)."\n";
}
