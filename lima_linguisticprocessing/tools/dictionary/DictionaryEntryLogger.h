// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef DICTIONARYENTRYLOGGER_H
#define DICTIONARYENTRYLOGGER_H

#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntryHandler.h"
#include "common/misc/fsaStringsPool.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include <ostream>

namespace Lima
{

/**
@author Benoit Mathieu
*/
class DictionaryEntryLogger : public LinguisticProcessing::AnalysisDict::AbstractDictionaryEntryHandler
{
public:
  DictionaryEntryLogger(
    std::ostream* out,
    const FsaStringsPool* sp,
    const Common::PropertyCode::PropertyCodeManager* manager);

  virtual ~DictionaryEntryLogger();

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

  std::ostream* m_out;
  const FsaStringsPool* m_sp;
  const Common::PropertyCode::PropertyCodeManager* m_manager;

};

}

#endif
