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

class DefaultIdGeneratorPrivate;
class LIMA_BOW_EXPORT DefaultIdGenerator : public AbstractLexiconIdGenerator
{
 public:
  DefaultIdGenerator(AbstractLexiconIdGeneratorInformer* informer, const uint64_t lastId=1);
  ~DefaultIdGenerator();

  uint64_t getId(const LimaString& /*unused word*/);

  uint64_t getId(  const std::vector<uint64_t>& /*unused structure*/ );

  uint64_t getSize() const;

  uint64_t getSimpleTermSize() const;

private:
  DefaultIdGeneratorPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
