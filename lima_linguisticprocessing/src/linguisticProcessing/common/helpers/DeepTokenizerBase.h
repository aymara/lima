/*
    Copyright 2002-2021 CEA LIST

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
