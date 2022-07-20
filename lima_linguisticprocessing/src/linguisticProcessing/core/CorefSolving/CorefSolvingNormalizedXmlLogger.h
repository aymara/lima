// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  //bool m_compactFormat; // compact format is the same as the RecognizerResultLogger
  MediaId m_language;
  std::string m_graph;
};

} // Coreferences
} // LinguisticProcessing
} // Lima

#endif
