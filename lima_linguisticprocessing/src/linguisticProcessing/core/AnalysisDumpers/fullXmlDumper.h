// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file       fullXmlDumper.h
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>

  *             Copyright (C) 2004 by CEA LIST
  * @author     Besancon Romaric (besanconr@zoe.cea.fr)
  * @date       Mon Apr  26 2004
  * Project     s2lp
  *
  * @brief      dump the full content of the analysis graph in XML format
  *
  *
  */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_FULLXMLDUMPER_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_FULLXMLDUMPER_H

#include "AnalysisDumpersExport.h"

#include "common/MediaProcessors/MediaProcessUnit.h"
#include "StopList.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/AnalysisDumpers/AnnotationGraphXmlDumper.h"

#include <boost/tuple/tuple.hpp>
#include <boost/graph/properties.hpp>

#include <map>
#include <iostream>
#include <set>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define FULLXMLDUMPER_CLASSID "FullXmlDumper"

/**
 * @brief Dumps all the content of the analysis on an XML stream. The exact
 * state should be rebuildable from the dump
 * @todo ensure to dump all the nodes one time and only one time,
 * sentence by sentence for the main stream
*/
class LIMA_ANALYSISDUMPERS_EXPORT FullXmlDumper : public MediaProcessUnit
{
public:

  FullXmlDumper();

  virtual ~FullXmlDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;


protected:
    AnnotationGraphXmlDumper* m_annotXmlDumperPtr;

    void dumpLimaData(std::ostream& os,
                    const LinguisticGraphVertex begin,
                    const LinguisticGraphVertex end,
                    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                    const SyntacticAnalysis::SyntacticData* syntacticData,
                    const std::string& graphId,
                    bool bySentence,
                    std::vector< bool >& alreadyDumpedTokens,
                    std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens) const;
    /**
    * a visitor for going through the graph between two points
    * and storing a BoWToken for every vertex that are not part
    * of a complex token
    */
    class DumpGraphVisitor : public boost::default_bfs_visitor
    {
        const FullXmlDumper& m_dumper;
        std::ostream& m_os;
        LinguisticGraphVertex m_lastVertex;
        const SyntacticAnalysis::SyntacticData* m_syntacticData;
        MediaId m_language;
        std::map< LinguisticAnalysisStructure::Token*, uint64_t >& m_fullTokens;
        std::vector< bool >& m_alreadyDumpedFullTokens;
        const std::string& m_graphId;

        public:
        DumpGraphVisitor(const FullXmlDumper& dumper,
                        std::ostream& outputStream,
                        const LinguisticGraphVertex& end,
                        const SyntacticAnalysis::SyntacticData* syntacticData,
                        std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                        std::vector< bool >& alreadyDumpedFullTokens,
                        MediaId language,
                        const std::string& graphId):
              m_dumper(dumper),
              m_os(outputStream),
              m_lastVertex(end),
              m_syntacticData(syntacticData),
              m_language(language),
              m_fullTokens(fullTokens),
              m_alreadyDumpedFullTokens(alreadyDumpedFullTokens),
              m_graphId(graphId)
        {
        }

        void discover_vertex(LinguisticGraphVertex v, const LinguisticGraph& g);
        void examine_edge(LinguisticGraphEdge e, const LinguisticGraph& g);
        void examine_vertex(LinguisticGraphVertex v, const LinguisticGraph& g);
        class EndOfSearch : public std::runtime_error
        {
        public:
            EndOfSearch():std::runtime_error("") {}
        };
    };

    LimaString getPosition(const uint64_t position) const;

    void outputVertex(const LinguisticGraphVertex v,
                      const LinguisticGraph& graph,
                      const SyntacticAnalysis::SyntacticData* syntacticData,
                      std::ostream& xmlStream,
                      std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                      std::vector< bool >& alreadyDumpedFullTokens,
                      const std::string& graphId) const;

    void outputEdge(const LinguisticGraphEdge e,
                    const LinguisticGraph& graph,
                    std::ostream& xmlStream) const;

    MediaId m_language;
    const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;

    bool m_dumpFullTokens;
    std::string m_graph;
    std::string m_handler;
};

} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_FULLXMLDUMPER_H
