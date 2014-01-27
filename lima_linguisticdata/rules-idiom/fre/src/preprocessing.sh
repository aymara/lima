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
# Shell script created by besancon on Fri Jun 24 2005 
# Version : $Id$ 
#


# gather reflexive verbs (to use only one rule)

# temporary move idioms-fre.txt to idioms-fre.txt.orig
# (reverse done in postprocessing)
/bin/mv idioms-fre.txt idioms-fre.txt.orig;

# important that name of additional rules created is after
# separableVerbs.rules in *.rules
./buildSeparableVerbList.pl -list=../separableVerbs.lst -rules=separableVerbsSecond.rules idioms-fre.txt.orig > idioms-fre.txt
