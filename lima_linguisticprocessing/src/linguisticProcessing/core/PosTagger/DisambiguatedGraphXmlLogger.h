// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_POSTAGGERDISAMBIGUATEDGRAPHXMLLOGGER_H
#define LIMA_LINGUISTICPROCESSING_POSTAGGERDISAMBIGUATEDGRAPHXMLLOGGER_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{

/**
@author Benoit Mathieu
*/
class LIMA_POSTAGGER_EXPORT DisambiguatedGraphXmlLogger : public AbstractLinguisticLogger
{
public:
  DisambiguatedGraphXmlLogger();

  virtual ~DisambiguatedGraphXmlLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  MediaId m_language;
  const Common::PropertyCode::PropertyManager* m_macroManager;
  const Common::PropertyCode::PropertyManager* m_microManager;

};

}

}

}

#endif // LIMA_LINGUISTICPROCESSING_POSTAGGERDISAMBIGUATEDGRAPHXMLLOGGER_H
