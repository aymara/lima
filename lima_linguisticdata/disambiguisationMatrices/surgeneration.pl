# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
