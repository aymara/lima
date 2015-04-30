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
#!/bin/bash
#
# Shell script created by besancon on Tue Jul  5 2005
# Version : $Id$
#

# create a package of necessary resources for new-phenix

export languages="fre eng spa ger ara chi"
export tmpDir=tmp.$$

mkdir $tmpDir;
cd $tmpDir;

export checkout="cvs export -r HEAD";

# checkout dictionary code definitions
$checkout linguisticData/analysisDictionary/build_code.sh;
for lang in $languages; do
    $checkout linguisticData/analysisDictionary/$lang/code;
    cd linguisticData/analysisDictionary
    ./build_code.sh $lang
    cd ../..
done;

# checkout necessary files
$checkout linguisticData/rules-idiom/compile-user-idiom.sh;
$checkout linguisticData/rules-idiom/reformat_idioms.pl;
$checkout linguisticData/rules-idiom/fre;
$checkout linguisticData/rules-idiom/eng;
$checkout linguisticData/scripts/transcodeIdiomatic.pl;
$checkout linguisticData/scripts/dictionarycode.pl;
$checkout linguisticData/scripts/create_matrices_from_corpus.sh;
$checkout linguisticData/scripts/disamb_matrices_extract.pl;
$checkout linguisticData/scripts/disamb_matrices_normalize.pl;
$checkout linguisticData/scripts/disamb_matrices_retag.pl;
$checkout linguisticData/disambiguisationMatrices/fre;
$checkout linguisticData/disambiguisationMatrices/eng;
$checkout linguisticData/disambiguisationMatrices/spa;
$checkout linguisticData/disambiguisationMatrices/ger;
$checkout linguisticData/disambiguisationMatrices/ara;
$checkout linguisticData/rules-namedentities/Common;
$checkout linguisticData/rules-namedentities/ara;
$checkout linguisticData/rules-namedentities/eng;
$checkout linguisticData/rules-namedentities/fre;
$checkout linguisticData/rules-namedentities/spa;
$checkout linguisticData/rules-namedentities/ger;
$checkout linguisticData/syntacticAnalysis/categoriesClassesDeclaration-eng.txt;
$checkout linguisticData/syntacticAnalysis/categoriesClassesDeclaration-fre.txt;
$checkout linguisticData/syntacticAnalysis/chainsMatrix-ara.bin;
$checkout linguisticData/syntacticAnalysis/chainsMatrix-eng.bin;
$checkout linguisticData/syntacticAnalysis/chainsMatrix-fre.bin;
$checkout linguisticData/syntacticAnalysis/chainsMatrix-ger.bin;
$checkout linguisticData/syntacticAnalysis/chainsMatrix-spa.bin;
$checkout linguisticData/syntacticAnalysis/Makefile;
$checkout linguisticData/syntacticAnalysis/rules-eng-heterodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-fre-heterodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-l2r-ara-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-l2r-eng-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-fre-homodeps-pass1.txt;
$checkout linguisticData/syntacticAnalysis/rules-fre-pleonasticPronouns.txt;
$checkout linguisticData/syntacticAnalysis/rules-l2r-ger-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-l2r-spa-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-r2l-ara-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-r2l-eng-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-fre-homodeps-pass2.txt;
$checkout linguisticData/syntacticAnalysis/rules-r2l-ger-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/rules-fre-homodeps-pass0.txt;
$checkout linguisticData/syntacticAnalysis/rules-r2l-spa-homodeps.txt;
$checkout linguisticData/syntacticAnalysis/simplification-first-rules-fre.txt;
$checkout linguisticData/syntacticAnalysis/simplification-last-rules-fre.txt;
$checkout linguisticData/syntacticAnalysis/simplification-rules-eng.txt;
$checkout linguisticData/syntacticAnalysis/simplification-rules-fre.txt;
$checkout linguisticData/Reformulation/synonymes-eng.lic2m.utf8;
$checkout linguisticData/Reformulation/synonymes-fre.lic2m.utf8;
$checkout linguisticData/Reformulation/synonymes-spa.lic2m.utf8;
$checkout linguisticData/Reformulation/antonymes-spa.lic2m.utf8;
$checkout linguisticData/Reformulation/eng-ara.lic2m.utf8;
$checkout linguisticData/Reformulation/eng-ara.alma.utf8;
$checkout linguisticData/Reformulation/eng-ara.user.utf8;
$checkout linguisticData/Reformulation/pays-multilingue.utf8;
$checkout linguisticData/Reformulation/build.sh;
$checkout linguisticData/Reformulation/buildMono.sh;
$checkout linguisticData/Reformulation/pays2refo.pl;
$checkout linguisticData/Reformulation/extractentries.sh;
$checkout linguisticData/Reformulation/extractentries.pl;
$checkout linguisticData/Reformulation/extractentriesMono.pl;
$checkout linguisticData/Reformulation/normalizedico.pl;
$checkout linguisticData/Reformulation/normalizedicoMono.pl;
$checkout linguisticData/Reformulation/deploy.sh;
$checkout linguisticData/Reformulation/deployMono.sh;


cd linguisticData

#reorganize files in package
/bin/cp rules-idiom/compile-user-idiom.sh scripts
/bin/cp rules-idiom/reformat_idioms.pl scripts
/bin/cp disambiguisationMatrices/fre/corpus/corpus_fr.txt disambiguisationMatrices
/bin/cp disambiguisationMatrices/fre/code_symbolic2lic2m.txt disambiguisationMatrices

#create additional files resources.env and run-test-idiom.sh
echo "export LINGUISTIC_DATA_ROOT=\$PWD" > resources.env
echo "export PATH=\$LINGUISTIC_DATA_ROOT/scripts:\$PATH" >> resources.env

echo "source resources.env" > run-test-idiom.sh
echo "pushd rules-idiom/fre/examples" >> run-test-idiom.sh
echo "compile-user-idiom.sh fre idiom-example-1.txt" >> run-test-idiom.sh
echo "compile-user-idiom.sh fre idiom-example-2.txt" >> run-test-idiom.sh
echo "compile-user-idiom.sh fre idiom-example-3.txt" >> run-test-idiom.sh
echo "./test-example.sh 1 2 3" >> run-test-idiom.sh
chmod u+x run-test-idiom.sh

echo "source resources.env" > compile-matrices.sh
echo "pushd disambiguisationMatrices" >> compile-matrices.sh
echo "create_matrices_from_corpus.sh corpus_fr.txt code_symbolic2lic2m.txt" >> compile-matrices.sh
echo "popd" >> compile-matrices.sh
chmod u+x compile-matrices.sh

echo "pushd Reformulation" > build-eng-ara-user-dico.sh
echo "./build.sh eng ara eng-ara.user.utf8 user" >> build-eng-ara-user-dico.sh
echo "popd" >> build-eng-ara-user-dico.sh
chmod u+x build-eng-ara-user-dico.sh

cd ..

#create package
export packageName=resources-newphenix-`date +%Y%m%d`.tgz
tar zcvf ../$packageName *
cd ..
/bin/rm -r $tmpDir
