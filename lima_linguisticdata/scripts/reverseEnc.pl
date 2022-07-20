#!/usr/bin/perl

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;
use warnings;
use open qw(:std :utf8);

while (<>)
{
  if (/[kl]="([^"]*)"/)
  {
    my $rev = reverse $1;
    s/$1/$rev/;
  }
  print;
}
