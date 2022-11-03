# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

for d in {1..3} ; do ./reformat.sh $LIMA_ROOT/lima/lima_pelf/evalPosTagging/results.eng.viterbi/$d/10pc.brut.out ; done > oanc.pos.lima
