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
