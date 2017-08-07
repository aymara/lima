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
#ifndef FSA_ABSTRACT_LEXICON_ID_ACCESSOR_H
#define FSA_ABSTRACT_LEXICON_ID_ACCESSOR_H

#include "common/LimaCommon.h"
#include "common/Data/LimaString.h"

namespace Lima {
namespace Common {

class LIMA_FSAACCESS_EXPORT AbstractLexiconIdAccessor
{
  public:
    AbstractLexiconIdAccessor() {}
    virtual ~AbstractLexiconIdAccessor() {}
     
    // implementation of operation inherited from interface IndexElementIdGenerator
    // rely on abstract operation implemented by concrete IdGenerator (fromMap or from fromFsaAccess)
    virtual uint64_t getId(const Lima::LimaString& word) = 0;
    virtual uint64_t getId(const std::vector<uint64_t>& structure) = 0;
    virtual uint64_t getSize() const = 0;
    virtual uint64_t getSimpleTermSize() const = 0;
    static const uint64_t NOT_A_LEXICON_ID;
    static const uint64_t MAX_TOKEN_ID;
    static const uint64_t MAX_DEP_ID;
    static const uint64_t BUFFER_SIZE;
};

} // namespace Common
} // namespace Lima

#endif   //FSA_ABSTRACT_LEXICON_ID_ACCESSOR_H
