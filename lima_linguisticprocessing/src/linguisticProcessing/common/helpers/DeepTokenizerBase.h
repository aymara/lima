// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_LINGUISTICPROCESSING_DeepTokenizerBase_H
#define LIMA_LINGUISTICPROCESSING_DeepTokenizerBase_H

#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

namespace Lima
{
namespace LinguisticProcessing
{

class LIMA_LP_HELPERS_EXPORT DeepTokenizerBase
{
public:
  DeepTokenizerBase() {}
  virtual ~DeepTokenizerBase() {}
  DeepTokenizerBase(const DeepTokenizerBase&) {}
  DeepTokenizerBase& operator=(const DeepTokenizerBase&) {return *this;}

  virtual void computeDefaultStatus(LinguisticAnalysisStructure::TStatus& curSettings);
  virtual void computeDefaultStatus(LinguisticAnalysisStructure::Token& token);
};

} // namespace LinguisticProcessing
} // namespace Lima

#endif
