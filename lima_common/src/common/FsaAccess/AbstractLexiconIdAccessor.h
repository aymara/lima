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
