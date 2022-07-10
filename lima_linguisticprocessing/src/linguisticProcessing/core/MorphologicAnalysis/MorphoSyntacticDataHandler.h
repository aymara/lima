// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
