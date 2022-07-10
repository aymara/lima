# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# gather reflexive verbs (to use only one rule)

# temporary move idioms-fre.txt to idioms-fre.txt.orig
# (reverse done in postprocessing)
/bin/mv idioms-fre.txt idioms-fre.txt.orig;

# important that name of additional rules created is after
# separableVerbs.rules in *.rules
./buildSeparableVerbList.pl -list=../separableVerbs.lst -rules=separableVerbsSecond.rules idioms-fre.txt.orig > idioms-fre.txt
