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
 *   Copyright (C) 2008 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_COREFSOLVINGNORMALIZEDXMLLOGGER_H
#define LIMA_LINGUISTICPROCESSING_COREFSOLVINGNORMALIZEDXMLLOGGER_H

#include "CorefSolvingExport.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace Coreferences
{

#define COREFSOLVINGNORMALIZEDXMLLOGGER_CLASSID "CorefSolvingNormalizedLogger"



class LIMA_COREFSOLVING_EXPORT CorefSolvingNormalizedXmlLogger : public AbstractLinguisticLogger
{
public:
  CorefSolvingNormalizedXmlLogger();

  virtual ~CorefSolvingNormalizedXmlLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const;

private:
  //bool m_compactFormat; // compact format is the same as the RecognizerResultLogger
  MediaId m_language;
  std::string m_graph;
};

} // Coreferences
} // LinguisticProcessing
} // Lima

#endif
