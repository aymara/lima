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

set(SCRIPTS_PATH ${CMAKE_SOURCE_DIR}/../lima_linguisticdata/scripts)
configure_file(tfcv.py tfcv.py @ONLY)

########### install files ###############
install(FILES 
    aligner.pl
    eval.pl
    launch_eval.sh
    build-dico.sh
    lima-pelf-functions.sh
    MFT.pl
    micro2macro.sh
    problemesAlignement.sh
    reBuildRawCorpus.sh
    words.sh
    ${CMAKE_CURRENT_BINARY_DIR}/tfcv.py
    ExploitResults/src/detailed-res.sh
    ExploitResults/src/confusionMatrix.sh
    ExploitResults/src/erreursFrequentes.pl
    ExploitResults/src/matriceConfusion.pl
    ExploitResults/src/MyImagePlot.r
    ExploitResults/src/synthese.tex
    ExploitResults/src/toPDF.sh
  PERMISSIONS OWNER_READ  OWNER_WRITE   OWNER_EXECUTE   GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
  DESTINATION share/apps/lima/scripts)

install(FILES
    conf/config-minimale-eng.SVMT
    conf/config-minimale-eng.ud.SVMT
    conf/config-minimale-fre.SVMT
    conf/config-minimale-por.SVMT
  PERMISSIONS OWNER_READ  OWNER_WRITE   GROUP_READ WORLD_READ
  DESTINATION share/config/lima)
