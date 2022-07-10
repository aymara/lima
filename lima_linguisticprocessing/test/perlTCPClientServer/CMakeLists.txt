# Copyright 2002-2013 CEA LIST
# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

########### install files ###############

install(FILES  analserverrc analserverrc-eng analserverrc-fre DESTINATION share/config/lima)
install(FILES  AnalysisClient.pl NonInteractiveAnalysisClient.pl AnalysisServer.pl DESTINATION bin)
