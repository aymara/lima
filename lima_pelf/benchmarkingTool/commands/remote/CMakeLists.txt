# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

install(PROGRAMS analyzer-distribute.sh analyzer-evaluator-distribute.sh analyzer-evaluator-node.sh analyzer-node.sh evaluator-distribute.sh evaluator-easy.sh evaluator-node.sh install-dictionnary.sh install-idioms.sh install-ngrams.sh install-synrules.sh DESTINATION share/apps/pelf/commands/remote
  COMPONENT runtime)
