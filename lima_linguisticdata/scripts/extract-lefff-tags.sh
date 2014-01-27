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
