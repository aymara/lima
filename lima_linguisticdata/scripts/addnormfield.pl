#!/usr/bin/perl -s

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

my @data;
while (<>)
{
  chop();
  next if (/^$/ || /^#/);
  @data=split(/	/);
  if ($#data < 3)
  {
    print "$data[0]	$data[1]		$data[2]\n";
  }
  else
  {
    print "$data[0]	$data[1]	$data[2]	$data[3]\n";
  }
}
