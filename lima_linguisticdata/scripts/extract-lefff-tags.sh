# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

awk -F '	' '{ print $3$4; }' lefff-ext-3.2.txt | \
sed \
-e 's/^\(adv\|coo\|csu\|ponctw\|prep\).*/\1/' \
-e 's/^\(caimp\|ce\)\[.*//' \
-e 's/\(^v.*\)pred=".*Obj:.*"/\1@trans/' \
-e 's/ //g' \
-e 's/pred=".*"//' \
-e 's/\(lightverb\|diathesis\|chunk_type\|pre_det\|detach\|detach_neg\|clivee\|case\|refl\|clneg\)=[a-z_|+\-]*//' \
-e 's/,cat=[a-z0-9]*//' \
-e 's/@pron,@se_moyen/@pron/' \
-e 's/@se_moyen/@pron/' \
-e 's/@être_possible//' \
-e 's/@\(Suj.*\|Obj.*\|Ctrl.*\|ACompSubj\|Att.*\|Comp.*\|DeCompInd\|imperative\|avoir\|être_possible\|être\|fêtre\|favoir\|active\|passive\|pseudo-\(y\|le\)\|pers\|report_verb\|e\),//g' \
-e 's/@e\]/]/' \
-e 's/@\(trans\|impers\),@pron/@pron/' \
-e 's/@pron,@pseudo-en/@pron/' \
-e 's/@trans,@impers/@impers/' \
-e 's/@impers,@pseudo-en/@impers/' \
-e 's/\[@pseudo-en/[@pron/' \
-e 's/@trans,@pron/@pron/' \
-e 's/@pron_possible/@pron/' \
-e 's/,_possible//' \
-e 's/cat=[a-z0-9]*//' \
-e 's/adj\[@\(impers\|s\|pron\),\{0,1\}/adj[/' \
-e 's/\(adj\[.*\)@\(impers\|pron\|pers\)/\1/g' \
-e 's/detach=\+//' \
-e 's/detach_neg=\+//' \
-e 's/clgen=[+]//' \
-e 's/clloc=[+]//' \
-e 's/det=[+]//' \
-e 's/adv_kind=[a-zè]*//' \
-e 's/,[+]//' \
-e 's/,,/,/g' \
-e 's/\[,/[/g' \
-e 's/aux.*\[/v[@aux,/' \
-e 's/^cf.*/cf/' \
-e 's/,\]/]/' \
-e 's/\[\]//' \
-e 's/^:..$//' \
-e 's/^..:$//' \
-e 's/np\[.*\(@\(f\]\|fp\]\|fs\]\|m\]\|mp\]\|ms\]\|p\]\|s\]\)\).*/np[\1/' \
-e 's/np\[@hum\]/np/' \
-e 's/np\[@loc\]/np/' \
-e 's/nc\[.*\(@\(f\]\|fp\]\|fs\]\|m\]\|mp\]\|ms\]\|p\]\|s\]\)\).*/nc[\1/' \
-e 's/nc\[@time\]/nc/' \
-e 's/nc\[@weekday\]/nc/' \
-e 's/nc\[@year\]/nc/' \
| uniq | sort | uniq > tags.txt
