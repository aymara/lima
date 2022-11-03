// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file       easyXmlDumper.h
  * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

  *             Copyright (C) 2004 by CEA LIST
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  * @date       Mon Oct 07 2008
  * Project     s2lp
  *
  * @brief      dump the content of the analysis graph in Easy XML format
  *
  */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_EASYXMLDUMPER_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_EASYXMLDUMPER_H

#include "EasyXmlDumperExport.h"
#include "ConstituantAndRelationExtractor.h"
#include "EasyDumper.h"

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/AnalysisDumpers/StopList.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"

#include <map>
#include <iostream>
#include <set>
#include <algorithm>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

#define EASYXMLDUMPER_CLASSID "EasyXmlDumper"

/**
 * @brief Dumps all the content of the analysis on an XML stream. The exact
 * state should be rebuildable from the dump
 * @todo ensure to dump all the nodes one time and only one time,
 * sentence by sentence for the main stream
*/
class LIMA_EASYXMLDUMPER_EXPORT EasyXmlDumper : public MediaProcessUnit
{

public:

  EasyXmlDumper();

  virtual ~EasyXmlDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  std::vector<std::string> m_sentIds;

protected:

  void dumpLimaData(std::ostream& os,
                     const LinguisticGraphVertex& begin,
                     const LinguisticGraphVertex& end,
                     const LinguisticAnalysisStructure::AnalysisGraph& anaGraph,
                     const LinguisticAnalysisStructure::AnalysisGraph& posGraph,
                     const Common::AnnotationGraphs::AnnotationData& annotationData,
                     const SyntacticAnalysis::SyntacticData& syntacticData,
                     const std::string& graphId,
                     std::vector< bool >& alreadyDumpedTokens,
                     std::map< LinguisticAnalysisStructure::Token*, uint64_t >& easyTokens,
                     std::string sentIdPrefix) const;

  MediaId m_language;
  const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;

  std::string m_graph;

private:

  std::map<std::string,std::string> m_typeMapping;
  std::map<std::string,std::string> m_srcTag;
  std::map<std::string,std::string> m_tgtTag;
  std::string m_handler;
};

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_EASYXMLDUMPER_H
