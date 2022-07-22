// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 20036 by  CEA                                           *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  abstractLexiconIdAccessor.h  -  description                            *
 *  AbstractLexiconIdAccessor = interface class                            *
 *  find implementations in LexiconIdAccessor, defaultIdAccessor           *
 ***************************************************************************/

#include "common/LimaCommon.h"
#include "AbstractLexiconIdAccessor.h"

namespace Lima {
namespace Common {

const uint64_t AbstractLexiconIdAccessor::NOT_A_LEXICON_ID=0;
const uint64_t AbstractLexiconIdAccessor::MAX_TOKEN_ID=100000000;  // 100 millions de termes simples
const uint64_t AbstractLexiconIdAccessor::MAX_DEP_ID=2000000000;   // 2 milliards de d�endance
// reste 2 milliards de conjonction de d�endance
// taille des transferts
const uint64_t AbstractLexiconIdAccessor::BUFFER_SIZE=1024*32;

} // namespace Common
} // namespace Lima
