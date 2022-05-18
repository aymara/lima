// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @file       SentenceBoundariesXmlLogger.h
 * @author     Romaric Besancon <romaric.besancon@cea.fr>
 *             Copyright (c) 2006 by CEA
 * @brief      logger for the sentence boundaries (XML format)
 *
 */

#ifndef LIMA_LINGUISTICPROCESSING_SENTENCEBOUNDARIESXMLLOGGER_H
#define LIMA_LINGUISTICPROCESSING_SENTENCEBOUNDARIESXMLLOGGER_H

#include "TextSegmentationExport.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{

#define SENTENCEBOUNDARIESXMLLOGGER_CLASSID "SentenceBoundariesXmlLogger"

class LIMA_TEXTSEGMENTATION_EXPORT SentenceBoundariesXmlLogger : public AbstractTextualAnalysisDumper
{

public:

  SentenceBoundariesXmlLogger();
  virtual ~SentenceBoundariesXmlLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:
  std::string m_graphId;
  std::string m_boundaries;
};


} // LinguisticAnalysisStructure
} // LinguisticProcessing
} // Lima

#endif
