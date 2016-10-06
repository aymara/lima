#!/usr/bin/perl -s

# Perl Program created by Besancon Romaric on Mon Dec  2 2002
# Version : $Id: reformat_idioms.pl 5584 2006-03-21 15:45:23Z gael $

# Help mode
if ($main::h || $main::help)
{
    print <<EOF;
usage reformat_idioms.pl [-help] file
    -categoriesConvert: specify the category conversion file \(default is categories_convert\)
    -noCatConvert: no conversion of categories \(already in Grace format\)
EOF
    exit;
}

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
#
use strict;

my $charsep=':';

my $headSymbol='&';

my %Pos=('pp'=>'$PRON',
	 'D'=>'$ADV',
	 'V'=>'$V',
	 'N'=>'$NC',
	 'S'=>'$NC',
	 'A'=>'$ADJ',
	 'J'=>'$ADJ',
	 'C'=>'$CONJ',
	 'P'=>'$PREP'
	 );

my %LingProp;

my ($file)=@ARGV;
my $path=`dirname $file`;
chomp $path;

my $LingPropFile="$path/categories_convert";
$LingPropFile=$main::categoriesConvert if $main::categoriesConvert;

&read_LingProp($LingPropFile,\%LingProp) unless $main::noCatConvert;

# foreach (sort keys %LingProp) {
#     print STDERR $_.";".$LingProp{$_}."\n";
# }

#header of rules file

print "set encoding=utf8\n";
print "using modex lima-analysis.xml\n";
print "using groups LinguisticProcessing\n";
print "set defaultAction=>CreateIdiomaticAlternative()\n\n";


my $line=0;
while (<>) 
{
    chomp; # Remove end of line/carriage return character
    s/#.*//;  # Remove end of line comment
    next if ($_ =~ /^\s*$/); # ignore empty lines
    $line++;
    my ($null1,$null2,$contextual,$trigger,$automaton,$partOfSpeech,$lemma)
      = split(';');
    # split automaton in right and left part
    my $quotemetaTrigger=quotemeta($trigger); # to escape [ ] characters
    my ($left,$right);

    # in the following regular expression: .*? asks for the shortest match 
    # instead of the usual longest one; this is to force trigger to be the left 
    # most occurrence
    if ($automaton =~ /(.*?)(^| )$quotemetaTrigger( |$)(.*)/) 
    {
      $left=$1; 
      $right=$4;
    }
    else 
    {
      print STDERR "Warning : cannot find trigger [".$trigger.
          "] in expression [$automaton], line $line\n";
      next;
    }
    
    $left =~ s/ $//;  # skip last space
    $right =~ s/^ //; # skip first space

    # reformatting to new syntax
    $trigger = &reformat_pos($trigger,$partOfSpeech);
    $left = &reformat_pos($left,$partOfSpeech);
    $left = &reformat_parenth($left);
    $right = &reformat_pos($right,$partOfSpeech);
    $right = &reformat_parenth($right);

    # reformatting of the part-of-speech
    #$partOfSpeech =~ s/ /_/g;
    my $newCat;
    if ($main::noCatConvert) {
        $newCat=$partOfSpeech;
    }
    elsif (! exists $LingProp{$partOfSpeech}) 
    {
        print STDERR "Error on line $line: cannot find category \"".
            $partOfSpeech."\"\n"; 
        next;
    }
    else {
        $newCat=$LingProp{$partOfSpeech};
    }
    $partOfSpeech = "IDIOM\$".$newCat;
    if ($contextual eq "A") 
    {
        $partOfSpeech="ABS_".$partOfSpeech;
    }
    
    # last "de" of a preposition can be "de","des","du","d'"
    if ($partOfSpeech eq "preposition") 
    {
      $right =~ s/(de|des|du|d\')$/(de|des|du|d\')/;
      $right =~ s/(à|au|aux)$/(à|au|aux)/;
    }
    # last "de" of a preposition before a verbe can be "de","d'"
    if ($partOfSpeech eq "preposition_verbale") 
    {
      $right =~ s/(de|d\')$/(de|d\')/;
    }

    #set default normalized form if does not exist
    if (!$lemma) {
        $lemma=$automaton;
        $lemma=~s/\[[^\]*]\]//;
        $lemma=~s/\&//;
        $lemma=~s/\([^\)*]\)//;
        $lemma=~s/  +/ /;
    }

    # output
    print $trigger.$charsep.$left.$charsep.$right.
    $charsep.$partOfSpeech.$charsep.$lemma."\n";
}

sub reformat_pos 
{
    my ($pos);
    local ($_); 
    ($_,$pos)=@_;
    s/\[([^\]]*)\]([^ ]*)/$2$Pos{$1}/g;
    
#     if ($1 eq 'V' && $pos =~/^verbe/) {
# 	$_ = $headSymbol.$_;
#     }

    return $_;
}

sub reformat_parenth 
{
    local ($_)=@_;
    s/\(\<([0-9]*)\)/[*{0-$1}]/g; # (<4) => [*{0-4}]
    s/\(([^\)]*)\)/[$Pos{$1}?]/g; # (D)  => [$ADV]
    return $_;
}

sub read_LingProp
{
    my ($LingPropFile,$LingProp) = @_;
    if (! open(FILE,$LingPropFile)) {
        print STDERR "cannot open file $LingPropFile\n";
        die unless $main::noCatConvert;
        return;
    }
    while (<FILE>)
    {
      chomp;
      if (/(.*);(.*)/)
      {
          $$LingProp{$1} = $2;
      }
      elsif (/(.*[^	 ])[	 ]+([^	 ]+)[	 ]*$/)
      {
          $$LingProp{$1} = $2;
      }
      else {
        print STDERR "Error line:$_\n";
      }
    }
    close(FILE);
}
