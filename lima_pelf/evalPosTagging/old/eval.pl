#! /usr/bin/perl -s

$posTagger=$ARGV[0];
$lang=$ARGV[1];
$fileFilter=$ARGV[2];
if ($fileFilter eq "")
{
    $fileFilter="log";
}

# debut de comparaison

if (length($lang)!=3 || ($lang eq "CVS"))
{
    next;
}

$nbEntrees=0;
$nbEntreesNonAmbigues=0;
$nbEntreesNonAmbiguesCorrectes=0;


$nbEntreesAmbigues=0;
$nbEntreesAmbiguesCorrectes=0;
$nbCategoriesEntreesAmbigues=0;


$nbEntreesMalDecoupees=0;
$nbEntreesManquantes=0;

opendir(REFS,"references/$lang") || die ("impossible d ouvrir le repertoire references/$lang");
while ($file = readdir(REFS) )
{

if (!($file =~ /$fileFilter$/)) { next;}

open(REF,"references/$lang/$file") || die ("impossible d ouvrir le fichier references/$lang/$file");
open(EVAL,"$posTagger/$lang/$file") || die ("impossible d ouvrir le fichier $posTagger/$lang/$file");

&followRef;
&followEval;

while (($ligneRef ne "") && ($ligneEval ne ""))
{

  if (&RefEqEval) 
  {
      #print "equal\n";

    if (scalar(@infosEval)==3) 
    {
      #entree non ambigue
      #print "entr� non ambigue\n";
      $nbEntreesNonAmbigues++;
      if (&isvalid(2))
      {
        #print "correcte\n";
        $nbEntreesNonAmbiguesCorrectes++;
        chomp $ligneRef;
        print STDERR " $ligneRef\t\t$ligneEval";
      }
      else
      {
        chomp $ligneRef;
        print STDERR "-$ligneRef\t\t$ligneEval";
      }
    } 
    else 
    {
      #entree ambigue
      #$nbcat=scalar(@infosEval);
      #print "entr� ambigue ($nbcat)\n";
      $nbEntreesAmbigues++;
            $nbCategoriesEntreesAmbigues+=(scalar(@infosEval)-2);
      $found=0;
      for ($i=2;$i<scalar(@infosEval);$i++)
      {
          if (&isvalid($i))
        {
            # categorie trouvee
          #print "found $infosEval[$i] equals $infosRef[2]\n";
          $nbEntreesAmbiguesCorrectes++;
          $found=1;
          last;
        }
      }
      if (!$found) 
      {
        #print "categorie '$infosRef[2]' non trouv� dans ligne :\n$ligneEval";
        chomp $ligneRef;
        print STDERR "+$ligneRef\t\t$ligneEval";
      }
      else
      {
        chomp $ligneRef;
        print STDERR " $ligneRef\t\t$ligneEval";
      }
    }

    &followRef;
    &followEval;

  } 
  else 
  {

    if (&RefLtEval) 
    {
      #print "ref < eval\n";
      &followRef;
      $nbEntreesManquantes++;
    } 
    else 
    {
      #print "ref > eval\n";
      &followEval;
      $nbEntreesMalDecoupees++;
    }
  }
}
while ($ligneRef ne "") 
{
  &followRef;
  $nbEntreesManquantes++;
}
while ($ligneEval ne "") 
{
  &followEval;
  $nbEntreesMalDecoupees++;
}
$nbEntrees--;
# on oublie la derni�e incr�entation

}

#print OUT "$nbEntrees entr�s dans le fichier �alu�n";
#print OUT "$nbEntreesNonAmbigues entr�s non ambigues\n";
#print OUT "$nbEntreesNonAmbiguesCorrectes entr� non ambigues correctes\n";
#print OUT "$nbEntreesAmbigues entr�s ambigues\n";
#print OUT "$nbEntreesAmbiguesCorrectes entr�s ambigues contenant la bonne categorie\n";
#print OUT "$nbCategoriesEntreesAmbigues categories dans les entr�s ambigues\n";
#print OUT "\n";
#print OUT "$nbEntreesMalDecoupees entr�s mal d�oup�s (non attendues dans la r��ence)\n";
#print OUT "$nbEntreesManquantes entr�s manquantes (attendues dans la r��ence mais non trouv�s)\n";
#print OUT "\n";
#print OUT "$nbCategoriesAvantDesambiguisation categories avant d�ambiguisation\n";
print "$posTagger $lang $nbEntrees $nbEntreesNonAmbigues $nbEntreesNonAmbiguesCorrectes $nbEntreesAmbigues $nbEntreesAmbiguesCorrectes $nbCategoriesEntreesAmbigues $nbEntreesMalDecoupees $nbEntreesManquantes\n";


sub RefEqEval {
  return (($infosRef[0]==$infosEval[0]) && (length($infosRef[1])==length($infosEval[1])));
}

sub RefLtEval {
  if ($infosRef[0]<$infosEval[0]) { return 1; }
  if ($infosEval[0]<$infosRef[0]) { return 0; }
    return (length($infosRef[1])<length($infosEval[1]));
}

sub followRef {
    $ligneRef= <REF>;
  #print "read REF : $ligneRef";
    @infosRef = split(/\|/,$ligneRef);
  for ($i=1;$i<scalar(@infosRef);$i++)
  {
      chomp($infosRef[$i]);
      $infosRef[$i]=~ s/^\s*//;
      $infosRef[$i]=~ s/\s*$//;
  }
}

sub followEval {
    $ligneEval= <EVAL>;
  #print "read EVAL : $ligneEval";
    @infosEval = split(/\|/,$ligneEval);
  for ($i=1;$i<scalar(@infosEval);$i++)
  {
      chomp($infosEval[$i]);
      $infosEval[$i]=~ s/^\s*//;
      $infosEval[$i]=~ s/\s*$//;
  }
  $nbEntrees++;
}

sub isvalid {
    my ($i) = @_;
  for (my $j=2;$j<scalar(@infosRef);$j++)
  {
      if ($infosEval[$i] eq $infosRef[$j])
    {
        return 1;
    }
  }
  return 0;
}