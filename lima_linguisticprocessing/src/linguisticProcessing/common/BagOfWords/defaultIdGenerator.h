// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  DefaultIdGenerator(AbstractLexiconIdGeneratorInformer* informer, 
                     const uint64_t lastId=1);
  ~DefaultIdGenerator();

  uint64_t getId(const LimaString& /*unused word*/) override;

  uint64_t getId(  const std::vector<uint64_t>& /*unused structure*/ ) override;

  uint64_t getSize() const override;

  uint64_t getSimpleTermSize() const override;

private:
  DefaultIdGeneratorPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
