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
#!/usr/bin/perl -W

use strict;

my $lang = shift @ARGV;
my $newCode = shift @ARGV;

open BI, ">>$lang/matrices/bigramMatrix-$lang.dat"
  || die "Cannot open bigrams matrix file for appending
";
open TRI, ">>$lang/matrices/trigramMatrix-$lang.dat"
  || die "Cannot open trigrams matrix file for appending
";

open (CODES,"../analysisDictionary/$lang/code/codesList-$lang.dat")
  || die "Cannot open codes list file for reading
";

my $first = <CODES>;
while (substr($first,0,1) eq "#")
{
  $first = <CODES>;
}
chomp $first;
if ($first =~ /^([^#][^;]*)/)
{
  $first = $1;
}

while (<CODES>)
{ 
  chomp;
  if (/^([^#][^;]*)/)
  {
    my $second = $1;
    print BI "$newCode	$first	0.00001
";
    print BI "$first	$newCode	0.00001
"; 
  
    print TRI "$first	$first	$newCode	0.00001
";  
    print TRI "$first	$newCode	$first	0.00001
";  
    print TRI "$newCode	$first	$first	0.00001
";  
    
    print TRI "$first	$second	$newCode	0.00001
";  
    print TRI "$first	$newCode	$second	0.00001
";  
    print TRI "$newCode	$first	$second	0.00001
";  
    print TRI "$second	$first	$newCode	0.00001
";  
    print TRI "$second	$newCode	$first	0.00001
";  
    print TRI "$newCode	$second	$first	0.00001
";  
    
    $first = $second;
  }
}
print BI "$newCode	$first	0.00001
";
print BI "$first	$newCode	0.00001
"; 

print TRI "$first	$first	$newCode	0.00001
";  
print TRI "$first	$newCode	$first	0.00001
";  
print TRI "$newCode	$first	$first	0.00001
";  
    
close BI;
close TRI;
