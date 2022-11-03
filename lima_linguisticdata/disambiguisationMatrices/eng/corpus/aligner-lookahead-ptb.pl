# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

use strict;

binmode(STDOUT, ":utf8");

$#ARGV == 2 or die "Not enough args
";


open (ONE, "<:encoding(UTF-8)", "$ARGV[0]") or die "Cannot open $ARGV[0]
";
open (TWO, "<:encoding(UTF-8)", "$ARGV[1]") or die "Cannot open $ARGV[1]
";

open (OUT, ">:encoding(UTF-8)", "$ARGV[2]") or die "Cannot open output.pos for appending
";

# the size of the lookahead
my $LOOKAHEAD=30;

my @leftTokLookahead;
my @rightTokLookahead;
my @leftTagLookahead;
my @rightTagLookahead;
my @leftPrintLookahead;
my @rightPrintLookahead;

sub init();
sub forward();
sub forwardLeft();
sub forwardRight();
sub readOneLeft();
sub readOneRight();
sub testCase0();
sub testCaseI();
sub testCaseII();
sub testCaseIII();
sub testCaseIV();
sub testCaseV();
sub testCaseVI();
sub testCaseVII();
sub printLookaheads();

init();
my $lineNumOne = 0;
my $lineNumTwo = 0;

while ($#leftTokLookahead>=0 && $#rightTokLookahead>=0)
{
    if ($leftTagLookahead[0] eq "UNK")
    {
      $leftTagLookahead[0] = $rightTagLookahead[0];
    }
  printLookaheads();
  if (testCase0())
  {
    print "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
    print OUT "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
    forward();
  }
  elsif (my $testCaseIResult = testCaseI())
  {
    print "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
    print OUT "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
    for (my $i = 0; $i < $testCaseIResult; $i++)
    {
      forwardLeft();
    }
    forward();
  }
  elsif (my $testCaseIIResult = testCaseII())
  {
    for (my $i = 0; $i < $testCaseIIResult; $i++)
    {
      print "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      print OUT "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      forwardLeft();
    }
  }
  elsif (my $testCaseIIIResult = testCaseIII())
  {
    if ($rightPrintLookahead[0] eq "http")
    {
      for (my $i = 0; $i < $testCaseIIIResult; $i++)
      {
        forwardRight();
      }
      print "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      print OUT "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      forward();
      if ( ($rightPrintLookahead[0] ne $leftPrintLookahead[0])
        && ("$rightPrintLookahead[0]" =~ /\Q$leftPrintLookahead[0]$/) )
      {
        forward();
      }
    }
    else
    {
      for (my $i = 0; $i < $testCaseIIIResult; $i++)
      {
        print "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
        print OUT "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
        forwardRight();
      }
      print "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
      print OUT "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
      forward();
    }
  }
  elsif (my @testCaseVResult = @{testCaseV()})
  {
    my ($testCaseVResultLeft, $testCaseVResultRight) = @testCaseVResult;
    for (my $i = 0; $i <$testCaseVResultLeft; $i++)
    {
      print "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      print OUT "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      forwardLeft();
    }
    for (my $i = 0; $i <$testCaseVResultRight; $i++)
    {
      forwardRight();
    }
  }
  elsif (my @testCaseIVResult = @{testCaseIV()})
  {
    foreach my $testCaseIVResult (reverse(@testCaseIVResult))
    {
      splice @leftTokLookahead, $testCaseIVResult, 1;
      splice @leftTagLookahead, $testCaseIVResult, 1;
      splice @leftPrintLookahead, $testCaseIVResult, 1;
      readOneLeft();
    }
    print "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
    print OUT "$rightPrintLookahead[0]	$rightTagLookahead[0]
";
    forwardRight();
  }
  elsif (testCaseVI())
  {
      print "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      print OUT "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      forwardLeft();
  }
  elsif (testCaseVII())
  {
      print "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      print OUT "$leftPrintLookahead[0]	$leftTagLookahead[0]
";
      forward();
  }
  else
  {
    print "Alignement error on lines $lineNumOne / $lineNumTwo
";
    die "Alignement error on lines $lineNumOne / $lineNumTwo
";
  }
}

close ONE;
close TWO;
close OUT;

sub init()
{
  for (my $i= 0; $i<$LOOKAHEAD; $i++)
  {
    readOneLeft();
    readOneRight();
  }
}

sub forwardLeft()
{
  shift @leftTokLookahead;
  shift @leftTagLookahead;
  shift @leftPrintLookahead;
  if ($#leftTokLookahead < $LOOKAHEAD)
  {
    readOneLeft();
  }
}

sub forwardRight()
{
  shift @rightTokLookahead;
  shift @rightTagLookahead;
  shift @rightPrintLookahead;
  if ($#rightTokLookahead < $LOOKAHEAD)
  {
    readOneRight();
  }
}

sub forward()
{
  # remove first element in each tab
  forwardLeft();
  forwardRight();
}

sub readOneLeft()
{
  if (not eof(ONE))
  {
    my $lineone = <ONE>;
    chomp $lineone;
    if (length($lineone)==0)
    {
      return;
    }
    $lineNumOne++;
    my ($tokone,$tagone) = split "	", $lineone;
    my $printone = $tokone;
    $tokone =~ s/\s//g;
#     $tokone =~ s/(.+)'s$/$1/;
    $tokone =~ s/-//g;
    push(@leftTokLookahead,$tokone);
    push(@leftTagLookahead,$tagone);
    push(@leftPrintLookahead,$printone);
  }
}

sub readOneRight()
{
  if (not eof(TWO))
  {
    my ($toktwo,$tagtwo);
    my $printtwo;
    do
    {
      my $linetwo = <TWO>;
      chomp $linetwo;
      if (length($linetwo)==0)
      {
        return;
      }
      $lineNumTwo++;
      ($toktwo,$tagtwo) = split "	", $linetwo;
      $printtwo = $toktwo;
      $toktwo =~ s/\s//g;
      $toktwo =~ s/(.+)'s$/$1/;
      $toktwo =~ s/^'s$//;
      $toktwo =~ s/-//g;
    } while ((not eof(TWO)) && (length($toktwo)==0) && (length($tagtwo)==0));
    push(@rightTokLookahead,$toktwo);
    push(@rightTagLookahead,$tagtwo,);
    push(@rightPrintLookahead,$printtwo);
  }
}

sub printLookaheads()
{
  print "		---- $lineNumOne	$lineNumTwo
";
  for (my $i = 0; $i < $LOOKAHEAD; $i++)
  {
    print "		>>>> ";
    if ($i <= $#leftTokLookahead)
    {
      print "$leftTokLookahead[$i]";
    }
    print "	";
    if ($i <= $#rightTokLookahead)
    {
      print "$rightTokLookahead[$i]";
    }
    print "
";
    if ($i > 5)
    {
      print "		>>>> ...	...
";
      return;
    }
  }
}

# tokens are equal or left one is possessive with 's and right one is not and is not followed by tokens "'" and "s"
# or left one is i.e./RB and right one is ./SENT (LIMA pb with i.e.)
# ot tokens are equal except for dash characters
sub testCase0()
{
  if ($leftTokLookahead[0] eq $rightTokLookahead[0]) {return 1;}
  my $left = $leftTokLookahead[0];
  $left =~ s/'s$//;
  
  if ($leftTokLookahead[0] eq "i.e." && $leftTagLookahead[0] eq "RB" && $rightTokLookahead[0] eq "." && $rightTagLookahead[0] eq "SENT") {return 1;}
  
  if ($leftTokLookahead[0] eq "#" && $rightTokLookahead[0] eq "") {return 1;}

  if ($left eq $rightTokLookahead[0] && !($rightTokLookahead[1] eq"'" && $rightTokLookahead[2]eq"s"))
  {
    return 1;
  }

  my $right = $rightTokLookahead[0];
  return ($left eq $right);
}

# several tokens on the left, grouped in one on the right
sub testCaseI()
{
  my $result = 0;
  my $right = $rightTokLookahead[0];
  my $left = $leftTokLookahead[0];
  $left =~ s/(.+)'s?$/$1/;
  my $left2 = $left;
  $left2 =~ s/-$//;
  if ($right =~ s/^\Q$left// || $right =~ s/^\Q$left2// )
  {
    my $i = 1;
    while (length($right) > 0 && $i < $LOOKAHEAD)
    {
      $left = $leftTokLookahead[$i];
      if ($left eq "'s" && !($rightTokLookahead[1] eq"'" && $rightTokLookahead[2]eq"s"))
      {
        print "      testCaseI returns $result
";
        return $result;
      }
      $left =~ s/'s?$//;
      
      if ( (length($left) == 0) 
        || ($right =~ s/^\Q$left//) )
      {
        $result++;
      }
      else
      {
        last;
      }
      $i++;
    }
    die "testCaseI Lookahead exceeded
" if $i >= $LOOKAHEAD;
  }
  if (length($right) != 0)
  {
    $result = 0;
  }
  print "      testCaseI returns $result
";
  return $result;
}

# tokens are missing on the right
sub testCaseII()
{
  my $result = 0;
  if ( ("x$rightTokLookahead[0]" !~ /^x\Q$leftTokLookahead[0]/)
    &&  ("x$leftTokLookahead[0]" !~ /^x\Q$rightTokLookahead[0]/) )
  {
#     print "      inif '$leftTokLookahead[0]' '$rightTokLookahead[0]'
";
    my $i = 1;
    while ($i < $LOOKAHEAD && $i < $#leftTokLookahead)
    {
#       print "      infor $i
";
      if ( ("x$rightTokLookahead[0]" !~ /^x\Q$leftTokLookahead[$i]/)
        &&  ("x$leftTokLookahead[$i]" !~ /^x\Q$rightTokLookahead[0]/) )
      {
        $result++;
      }
      elsif ( ("x$rightTokLookahead[0]" =~ /^x\Q$leftTokLookahead[$i]/)
            || ("x$leftTokLookahead[$i]" =~ /^x\Q$rightTokLookahead[0]/) )
      {
        $result++;
        last;
      }
      $i++;
    }
    return 0 if $i >= $LOOKAHEAD;
#     die "testCaseII Lookahead exceeded
" if $i >= $LOOKAHEAD;
  }
  else
  {
#     print "      inelse '$leftTokLookahead[0]' '$rightTokLookahead[0]'
";
  }
  print "      testCaseII returns $result
";
  return $result;
}

# one token on the left splitted in several tokens on the right
sub testCaseIII()
{
  my $result = 0;
  my $limit = $LOOKAHEAD;
  my $left = $leftTokLookahead[0];
  if ($left =~ s/^\Q$rightTokLookahead[0]//)
  {
    $left =~ s/-//g;
    if ($rightTokLookahead[0] eq "http")
    {
      $limit = 200;
    }
    my $i = 1;
    while ($i < $limit)
    {
      my $right = $rightTokLookahead[$i];
      $right =~ s/-//g;
      if ($left =~ s/^'s\Q$right// || $left =~ s/^\Q$right//)
      {
        $result++;
      }
      else
      {
        if ($left =~ s/^'s\Q$right//)
        {
          $result++;
          $i++;
          print "      testCaseIII next on $left, $i and $rightTokLookahead[$i]
";
          next;
        }
        
        last;
      }
      $i++;
      if ($i >= $LOOKAHEAD)
      {
        readOneLeft();
        readOneRight();
      }
    }
    die "testCaseIII Lookahead exceeded
" if $i >= $limit;
#     print "testCaseIII  remove $left from $rightTokLookahead[$i]
";
#     $rightTokLookahead[$i] =~ s/^\Q$left//;
  }
  if (length($left) == 0 || $left eq "'s" ) 
  {
    print "      testCaseIII returns $result
";
    return $result;
  }
  print "      testCaseIII returns 0
";
  return 0;
}

# several non contiguous tokens on the left grouped into one on the right
sub testCaseIV()
{
  my @result;
  my $right = $rightTokLookahead[0];
  if ($right =~ s/^\Q$leftTokLookahead[0]//)
  {
    push @result, 0;
    my $i = 1;
    while ($i < $LOOKAHEAD && $i <= $#leftTokLookahead)
    {
      if ($right =~ s/^\Q$leftTokLookahead[$i]//)
      {
        push @result, $i;
      }
      elsif (length($right) == 0)
      {
#         print "      testCaseIV returns $#result: @result
";
        return \@result;
      }
      $i++;
    }
    my @empty;
    print "testCaseIV Lookahead exceeded
" if $i >= $LOOKAHEAD;
    return \@empty
  }
#   print "      testCaseIV returns $#result: @result
";
  return \@result;
}

# different tokenizations: (A, BCD) vs (AB,C,D)
sub testCaseV()
{
  my $left=$leftTokLookahead[0];
  if (length($left) > 1)
  {
    $left =~ s/-//g;
  }
  my $right=$rightTokLookahead[0];
  if (length($right) > 1)
  {
    $right =~ s/-//g;
  }
  my $i = 1;
  my $j = 1;
  while ($i < $LOOKAHEAD && $j < $LOOKAHEAD)
  {
    print "      testCaseV $left $i $right $j
";
    if ($left eq $right)
    {
      print "      testCaseV returns $i, $j
";
      my @result;
      push @result, $i;
      push @result, $j;
      return \@result;
    }
    elsif ("$right" =~ /^\Q$left/)
    {
      $left .= $leftTokLookahead[$i];
      if ($left =~ /-$/ && $rightTokLookahead[$j] eq "-")
      {
        $j++
      }
      $left =~ s/-//g;
      $i++;
    }
    elsif ("$left" =~ /^\Q$right/)
    {
      print "      testCaseV rightTokLookahead[$j] $rightTokLookahead[$j]
";
      $right .= $rightTokLookahead[$j];
      $right =~ s/-//g;
      $j++;
    }
    else
    {
      print "      testCaseV returns []
";
      my @result;
#       push @result, 0;
#       push @result, 0;
      return \@result;
    }
  }
  die "testCaseV Lookahead exceeded
" if $i >= $LOOKAHEAD || $j >= $LOOKAHEAD;
}

# first left is dash and second left and first right are compatible: (- A...) (A'...)
sub testCaseVI()
{
  my $result = ( ($leftTokLookahead[0] eq "-")
      && (("$rightTokLookahead[0]" =~ /^\Q$leftTokLookahead[1]/)
         || ("$leftTokLookahead[1]" =~ /^\Q$rightTokLookahead[0]/)) );
#   print "      testCaseVI returns ".($result?"true":"false")."
";
  return $result;
}

# first token on left and right are compatible and next ones are equal: (x A...) (xy A...)
sub testCaseVII()
{
  my $result = ( (("$rightTokLookahead[0]" =~ /^\Q$leftTokLookahead[0]/)
         || ("$leftTokLookahead[0]" =~ /^\Q$rightTokLookahead[0]/))
      && ($rightTokLookahead[1] eq $leftTokLookahead[1])
          );
#   print "      testCaseVII returns ".($result?"true":"false")."
";
  return $result;
}
