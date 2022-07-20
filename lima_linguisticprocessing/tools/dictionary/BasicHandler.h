// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef BASICHANDLER_H
#define BASICHANDLER_H

#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntryHandler.h"
#include <ostream>

namespace Lima
{

/**
@author Benoit Mathieu
*/
class BasicHandler : public LinguisticProcessing::AnalysisDict::AbstractDictionaryEntryHandler
{
public:
  BasicHandler(
    std::ostream* out);

  virtual ~BasicHandler();

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

};

}

#endif
