# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

python3 ask_wikidata.py -f $1 \
    | jq '.results.bindings[].item.value' \
    | grep -Eo 'Q[0-9]+' \
    | sort -V | uniq
