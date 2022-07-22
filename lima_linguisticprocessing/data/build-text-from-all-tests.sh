# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# récupère tous les textes des tests pour les extractions d'EN
grep "key=\"text" test-fre.se-*.xml | cut --delim=$'"' -f 4 > test-fre-se.txt

grep "key=\"text" test-eng.se-*.xml | cut --delim=$'"' -f 4 > test-eng.se.txt
