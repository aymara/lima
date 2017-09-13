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
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_MORPHOSYNTACTICDATAHANDLER_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_MORPHOSYNTACTICDATAHANDLER_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntryHandler.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

/**
@author Benoit Mathieu
*/
class LIMA_MORPHOLOGICANALYSIS_EXPORT MorphoSyntacticDataHandler : public AnalysisDict::AbstractDictionaryEntryHandler
{
public:

  MorphoSyntacticDataHandler(
    LinguisticAnalysisStructure::MorphoSyntacticData& output,
    LinguisticAnalysisStructure::MorphoSyntacticType type);

  virtual ~MorphoSyntacticDataHandler();

  virtual void startEntry(StringsPoolIndex form) override;
  virtual void endEntry() override;
  
  virtual void foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm) override;
  virtual void endLingInfos() override;

  virtual void foundAccentedForm(StringsPoolIndex form) override;
  virtual void endAccentedForm() override;

  virtual void foundProperties(LinguisticCode lings) override;
  
  virtual void foundConcatenated() override;
  virtual void endConcatenated() override;

private:
  LinguisticAnalysisStructure::MorphoSyntacticData& m_output;

  LinguisticAnalysisStructure::LinguisticElement m_current;
  
  bool m_inConcat;
  
};

}

}

}

#endif
