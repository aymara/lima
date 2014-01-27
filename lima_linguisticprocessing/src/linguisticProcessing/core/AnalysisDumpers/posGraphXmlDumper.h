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
  *
  * @file       posGraphXmlDumper.h
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Jorge García Flores <jorge.garcia-flores@cea.fr>
  *             Copyright (C) 2004 by CEA LIST
  * @author     Besancon Romaric (besanconr@zoe.cea.fr)
  * @date       March 28 2008
  * Project     s2lp
  *
  * @brief      dump just the content of the PosGraph in XML format
  *
  *
  */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_POSGRAPHXMLDUMPER_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_POSGRAPHXMLDUMPER_H


#include "AnalysisDumpersExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "StopList.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"

#include <boost/tuple/tuple.hpp>
#include <boost/graph/properties.hpp>

#include <map>
#include <iostream>
#include <set>


namespace Lima {
namespace Common {
  namespace AnnotationGraphs {
    class AnnotationData;
  }
  namespace BagOfWords {
    class BoWTerm;
  }
}
namespace LinguisticProcessing {
  namespace Compounds {
    class BowGenerator;
  }
namespace AnalysisDumpers {

#define POSGRAPHXMLDUMPER_CLASSID "posGraphXmlDumper"


class LIMA_ANALYSISDUMPERS_EXPORT posGraphXmlDumper : public MediaProcessUnit
{

   MediaId m_language;
   LinguisticGraphEdge e;
  LinguisticGraphEdge m_lastEdge;
  


  
public:
  posGraphXmlDumper();

  virtual ~posGraphXmlDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;


protected:
    void dumpLimaData(std::ostream& os,
                    const LinguisticGraphVertex begin,
                    const LinguisticGraphVertex end,
                    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                    const LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                    const SyntacticAnalysis::SyntacticData* syntacticData,
                    const Common::AnnotationGraphs::AnnotationData* annotationData,
                    const std::string& graphId,
                    bool bySentence,
                    std::vector< bool >& alreadyDumpedTokens,
                    std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                    int sentenceId) const;

    LimaString getPosition(const uint64_t position) const;

    void outputVertex(const LinguisticGraphVertex v,
                      const LinguisticGraph& lanagraph,
                      const LinguisticGraph& lposgraph,
                      const SyntacticAnalysis::SyntacticData* syntacticData,
                      const Common::AnnotationGraphs::AnnotationData* annotationData,
                      std::ostream& xmlStream,
                      std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                      std::vector< bool >& alreadyDumpedFullTokens,
                      const std::string& graphId) const;

    void outputEdge(const LinguisticGraphEdge e,
                    const LinguisticGraph& graph,
                    std::ostream& xmlStream) const;

    void naturalCompoundTokenString(const Common::BagOfWords::BoWTerm* compound, QVector<LimaString>& result) const;
        
    const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;

    bool m_dumpFullTokens;
    std::string m_graph;
    std::string m_handler;

    LinguisticProcessing::Compounds::BowGenerator* m_bowGenerator;
};

} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif 
