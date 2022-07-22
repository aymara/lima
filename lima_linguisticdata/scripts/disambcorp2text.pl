#!/usr/bin/perl -w

# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;
use warnings;

while (<>)
{
	chomp;
	s/^s' /s'/;
	if (/'$/)
  {
		print;
  }
	else
	{
		print "$_\n";
	}
}

print "\n";
