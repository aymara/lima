# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

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
  DESTINATION share/apps/lima/scripts
  COMPONENT runtime)

install(FILES
    conf/config-minimale-eng.SVMT
    conf/config-minimale-fre.SVMT
    conf/config-minimale-por.SVMT
  PERMISSIONS OWNER_READ  OWNER_WRITE   GROUP_READ WORLD_READ
  DESTINATION share/config/lima
  COMPONENT runtime)
