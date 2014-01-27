#!/usr/bin/perl -s

#   Copyright 2002-2013 CEA LIST
#    
#   This file is part of LIMA.
#
#   LIMA is free software: you can redistribute it and/or modify
#   it under the terms of the GNU Affero General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   LIMA is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU Affero General Public License for more details.
#
#   You should have received a copy of the GNU Affero General Public License
#   along with LIMA.  If not, see <http://www.gnu.org/licenses/>
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
