#! /usr/bin/perl -s

open(RESULTS,$ARGV[0]) || die ("impossible d ouvrir le fichier $ARGV[0]");
#first line is header
$ligne=<RESULTS>;
print  "   PosTagger    Langue Precision Decision Score PrecisionSup PrecisionInf AmbiguiteMoyenne entreeManquantes entreesSuperflues\n";

while ($ligne=<RESULTS>)
{
    chomp($ligne);
    @data = split(/\s/,$ligne);

	$postagger=$data[0];
	$lang=$data[1];
    $nbEntrees=$data[2];
	$nbEntreesNonAmbigues=$data[3];
	$nbEntreesNonAmbiguesCorrectes=$data[4];
	$nbEntreesAmbigues=$data[5];
	$nbEntreesAmbiguesCorrectes=$data[6];
	$nbCategoriesEntreesAmbigues=$data[7];
	$nbEntreesMalDecoupees=$data[8];
	$nbEntreesManquantes=$data[9];

    $precision=$nbEntreesNonAmbiguesCorrectes / $nbEntreesNonAmbigues;
	$decision=$nbEntreesNonAmbigues / ($nbEntreesNonAmbigues + $nbEntreesAmbigues);
	$score=($precision + $decision) / 2;
	$precisionSup= ($nbEntreesNonAmbiguesCorrectes + $nbEntreesAmbiguesCorrectes) / ($nbEntreesNonAmbigues + $nbEntreesAmbigues);
	$precisionInf= ($nbEntreesNonAmbiguesCorrectes) / ($nbEntreesNonAmbigues + $nbEntreesAmbigues);
    $ambiguiteMoyenne=($nbEntreesNonAmbigues+$nbCategoriesEntreesAmbigues) / ($nbEntreesNonAmbigues + $nbEntreesAmbigues);
    $entreeManquantes=$nbEntreesManquantes / ($nbEntreesNonAmbigues + $nbEntreesAmbigues + $nbEntreesManquantes);
	$entreeSuperflues=$nbEntreesMalDecoupees / $nbEntrees;

	printf("%15s %6s %9.2f %8.2f %5.2f %12.2f %12.2f %16.2f %16.2f %17.2f\n",$postagger,$lang,100*$precision,100*$decision,100*$score,100*$precisionSup,100*$precisionInf,$ambiguiteMoyenne,100*$entreeManquantes,100*$entreeSuperflues);

}
