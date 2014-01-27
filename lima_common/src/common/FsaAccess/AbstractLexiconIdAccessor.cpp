/*
    Copyright 2002-2013 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
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
