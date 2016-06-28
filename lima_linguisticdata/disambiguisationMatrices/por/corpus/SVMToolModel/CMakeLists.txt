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
install(FILES 
    lima.A0.UNK  lima.A2      lima.A3.UNK  lima.AMBP  lima.M0.LR.MRG      lima.WIN
    config-minimale.SVMT  lima.A1      lima.A2.UNK  lima.A4      lima.DICT  lima.UNK.M0.LR.MRG
    lima.A0              lima.A1.UNK  lima.A3      lima.A4.UNK  lima.EXP   lima.UNKP
  COMPONENT por DESTINATION share/apps/lima/resources/Disambiguation/SVMToolModel-por)

# To learn a new model
# copy corpus file to work dir
# call  "sed -i -e's/ /_/g' -e's/   / /g' " on copied file
# copy config.svmt to work dir
# replace in config.svmt the value of TRAINSET by the full path of the copied corpus file and the value of SVMDIR by the path of svm_light
# call SVMTlearn config.svmt
# install generated lima.* files
