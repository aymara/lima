// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2026 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_CONLLU_MWT_DICT_H
#define DEEPLIMA_CONLLU_MWT_DICT_H

#include <ostream>
#include <string>
#include <vector>

namespace deeplima
{
namespace CoNLLU
{

// Extract a multiword-token (MWT) expansion dictionary from CoNLL-U files.
//
// For every "N-M" range line, the surface form maps to the forms of the
// following (M-N+1) real-word lines (e.g. French "du" -> ["de", "le"]).
// Surfaces seen with several distinct expansions are resolved by frequency
// (the most frequent expansion wins); discarded alternatives are logged to
// std::cerr. The output has one line per surface:
//
//   surface <TAB> count <TAB> word1 <TAB> word2 [<TAB> ...]
//
// where `count` is the frequency of the chosen expansion (usable downstream to
// drop rare/noisy entries). Returns the number of distinct surfaces written.
size_t extract_mwt_dict(const std::vector<std::string>& conllu_files, std::ostream& out);

} // namespace CoNLLU
} // namespace deeplima

#endif
