#! /usr/bin/perl -s

#récupération des categories;
open(CODE,"$ARGV[0]") || die ("impossible d ouvrir le fichier $ARGV[0]");

while ($ligne = <CODE>)
{
    chomp($ligne);
    if ($ligne=~ /<m>/)
	{
         $Lcateg=$ligne;
         $Lcateg=~ s/^.+<n>//;
         $Lcateg=~ s/<\/n>.*$//;
        
         $ligne=~ s/^.+<m>//;
         $ligne=~ s/<\/m>.*$//;
         

        $conv{$ligne} = $Lcateg;
        print "read $Lcateg -> $ligne\n";

    }
}

# vérification de toutes les categories
open(REF,"$ARGV[1]") || die ("impossible d ouvrir le fichier $ARGV[1]");
open(OUTPUT,">$ARGV[2]") || die ("impossible d ouvrir le fichier $ARGV[2]");

$i=0;
while ($ligne=<REF>)
{
    $i++;
    @infos=split(/\|/,$ligne);
    print OUTPUT "$infos[0]|$infos[1]";
    for ($i=2;$i<scalar(@infos);$i++)
    {
	chomp($infos[$i]);
	$infos[$i]=~ s/^\s*//;
        $infos[$i]=~ s/\s*$//;
	if ($conv{$infos[$i]} eq "")
	{
	   print "ligne $i : categorie $infos[$i] is invalid !\n";
        } else {
           print OUTPUT " | $conv{$infos[$i]}";
        }
    }
    print OUTPUT "\n"
}
