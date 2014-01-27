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
/************************************************************************
 *
 * @file       defaultIdGenerator.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     BagOfWords
 * 
 * @brief default index element id generator: for each index element,
 * give an increasing id (last id given +1). First id given is 1 (0 is
 * reserved).
 * 
 * 
 ***********************************************************************/

#ifndef DEFAULTIDGENERATOR_H
#define DEFAULTIDGENERATOR_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/FsaAccess/AbstractLexiconIdGenerator.h"
#include <assert.h>

namespace Lima {
namespace Common {
namespace BagOfWords {

class LIMA_BOW_EXPORT DefaultIdGenerator : public AbstractLexiconIdGenerator
{
 public:
  DefaultIdGenerator(AbstractLexiconIdGeneratorInformer* informer, const uint64_t firstId=1);
  ~DefaultIdGenerator();
  
  uint64_t getId(const LimaString& /*unused word*/) const {
    assert(false);
    return m_lastId; 
  }
  uint64_t getId(const LimaString& /*unused word*/) {
    m_simpleTermCount++;
    return m_lastId++; 
  }
  uint64_t getId(const std::vector<uint64_t>& /*unused structure*/) const {
    assert(false);
    return m_lastId;
  }
  uint64_t getId(  const std::vector<uint64_t>& /*unused structure*/ ) {
    m_compoundTermCount++;
    return m_lastId++;
  }
  uint64_t getSize() const {
    return m_simpleTermCount + m_compoundTermCount;
  }
  uint64_t getSimpleTermSize() const {
    return m_simpleTermCount;
  }

 private:
  uint64_t m_firstId;
  uint64_t m_lastId;
  uint64_t m_simpleTermCount;
  uint64_t m_compoundTermCount;
};

} // end namespace
} // end namespace
} // end namespace

#endif
