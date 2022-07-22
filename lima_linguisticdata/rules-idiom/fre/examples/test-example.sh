# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

export exampleBaseName="idiom-example-";

# local config and resources
if [[ ! -e conf ]]; then 
    mkdir conf; 
    for f in $LIMA_CONF/*; do ln -s $f conf; done
fi
if [[ ! -e resources ]]; then 
    mkdir resources; 
    for f in $LIMA_RESOURCES/*; do ln -s $f resources; done
fi
if [[ ! -e resources/idiom-examples ]]; then 
    mkdir resources/idiom-examples
fi

sed -e 's/lima-lp-tva-fre.xml/lima-lp-tva-fre-example.xml/' $LIMA_CONF/lima-lp-tva.xml > conf/lima-lp-tva-example.xml

for example in $*; do
    export exampleFile=${exampleBaseName}$example.bin;
    if [[ -a $exampleFile ]]; then
        cp $exampleFile resources/idiom-examples
        
        perl -pe 's%LinguisticProcessings/fre/idiomaticExpressions-fre.rules%idiom-examples/$ENV{"exampleFile"}%' $LIMA_CONF/lima-lp-tva-fre.xml > conf/lima-lp-tva-fre-example.xml;
        echo "running test on $exampleFile";
        tva --language=fre --resources-dir=resources --config-dir=conf --lp-config-file=lima-lp-tva-example.xml idiom-example-fre-test.xml >& tva-$example.log;
        egrep "(TestReport|total)" tva-$example.log
    else
        echo "cannot find compiled example rules $exampleFile";
    fi;
done
