# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

# Deprecated legacy media (rule-based/statistical analysis), only built with
# WITH_LEGACY_PIPELINES=ON.
set(LIMA_LEGACY_LANGUAGES eng fre por)
# Media whose analysis uses the deeplima backend (tokenization, PoS tagging,
# lemmatization, dependency parsing) and get ModEx resources.
set(LIMA_UD_LANGUAGES ud ud-eng ud-fra)
set(LIMA_LANGUAGES ${LIMA_LEGACY_LANGUAGES} ${LIMA_UD_LANGUAGES})
