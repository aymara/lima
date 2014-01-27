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

  virtual void startEntry(StringsPoolIndex form);
  virtual void endEntry();

  virtual void foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm);
  virtual void deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm);
  virtual void endLingInfos();

  virtual void foundConcatenated();
  virtual void deleteConcatenated();
  virtual void foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form);
  virtual void endComponent();
  virtual void endConcatenated();

  virtual void foundAccentedForm(StringsPoolIndex form);
  virtual void deleteAccentedForm(StringsPoolIndex form);
  virtual void endAccentedForm();

  virtual void foundProperties(LinguisticCode lings);

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
