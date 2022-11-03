// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      logger for checking chinese segmentation
  *
  * @file       disambiguationLogger.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  * @version    $Id$
  */

#ifndef LIMA_POSTAGGER_DISAMBIGUATIONLOGGER_H
#define LIMA_POSTAGGER_DISAMBIGUATIONLOGGER_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"

#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

#define DISAMBIGUATIONLOGGER_CLASSID "DisambiguationLogger"

class LIMA_POSTAGGER_EXPORT DisambiguationLogger : public AbstractLinguisticLogger
{

public:
  DisambiguationLogger();
  virtual ~DisambiguationLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

protected:

private:

  bool m_logAll;
  MediaId m_language;
  const Common::PropertyCode::PropertyManager* m_microManager;

};

} // PosTagger
} // namespace LinguisticProcessing
} // Lima


#endif
