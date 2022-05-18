// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICT_ABSTRACTDICTIONARYENTRYHANDLER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICT_ABSTRACTDICTIONARYENTRYHANDLER_H

#include "AnalysisDictExport.h"
#include "common/misc/fsaStringsPool.h"
 
namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {

class LIMA_ANALYSISDICT_EXPORT AbstractDictionaryEntryHandler
{

public:
  virtual ~AbstractDictionaryEntryHandler() {}
  
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
};

} // AnalysisDict
} // LinguisticProcessing
} // Lima

#endif
