#! /usr/bin/perl -s

#récupération des categories;
open(REF,"$ARGV[0]") || die ("impossible d ouvrir le fichier $ARGV[0]");


# vérification de toutes les categories
open(OUT,">$ARGV[1]") || die ("impossible d ouvrir le fichier $ARGV[1]");

while ($lref=<REF>)
{
    @infosref=split(/\|/,$lref);
    $infosref[0]=~ s/^\s*//;
    $infosref[0]=~ s/\s*$//;
    $infosref[1]=~ s/^\s*//;
    $infosref[1]=~ s/\s*$//;
    print OUT "$infosref[0] | $infosref[1] ";
    for ($i=2;$i<scalar(@infosref);$i++)
    {
      print OUT "|$infosref[$i]";
    }
}

close(REF);
close(OUT);