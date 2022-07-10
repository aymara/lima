// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_PAIRDATAHANDLER_H
#define LIMA_LINGUISTICPROCESSING_MORPHOLOGICANALYSIS_PAIRDATAHANDLER_H

#include "MorphologicAnalysisExport.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntryHandler.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

class LIMA_MORPHOLOGICANALYSIS_EXPORT SequenceEntryHandler : public AnalysisDict::AbstractDictionaryEntryHandler
{

public:

  SequenceEntryHandler() {};
  virtual ~SequenceEntryHandler() {};

  inline void addHandler(AbstractDictionaryEntryHandler* h);

  virtual void startEntry(StringsPoolIndex form) override;
  virtual void endEntry() override;

  virtual void foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm) override;
  virtual void deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm) override;
  virtual void endLingInfos() override;

  virtual void foundConcatenated() override;
  virtual void deleteConcatenated() override;
  virtual void foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form) override;
  virtual void endComponent() override;
  virtual void endConcatenated() override;

  virtual void foundAccentedForm(StringsPoolIndex form) override;
  virtual void deleteAccentedForm(StringsPoolIndex form) override;
  virtual void endAccentedForm() override;

  virtual void foundProperties(LinguisticCode lings) override;

private:

  std::vector<AbstractDictionaryEntryHandler*> m_handlers;

};

inline void SequenceEntryHandler::addHandler(AbstractDictionaryEntryHandler* h)
{
  m_handlers.push_back(h);
}





} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima

#endif
