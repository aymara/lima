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
# /bin/cp ${CMAKE_CURRENT_SOURCE_DIR}/corpus/corpus_fr.txt corpus_fr_merge.txt
# /bin/cat ${CMAKE_CURRENT_SOURCE_DIR}/corpus/corpus_fr_ajout.txt >> corpus_fr_merge.txt
# /bin/awk -F"	" '{print $$2}' corpus_fr_merge.txt > succession_categs.txt

cp $1 $3
cat $2 >> $3
awk -F"	" '{print $2}' $3 > succession_categs.txt
