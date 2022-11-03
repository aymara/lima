// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "ParagraphBoundariesFinder.h"
#include "SegmentationData.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <QtCore/QRegularExpression>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace LinguisticAnalysisStructure {

SimpleFactory<MediaProcessUnit,ParagraphBoundariesFinder> paragraphBoundariesFinderFactory(PARAGRAPHBOUNDARIESFINDER_CLASSID);

ParagraphBoundariesFinder::ParagraphBoundariesFinder():
MediaProcessUnit(),
m_graph("PosGraph"),
m_paragraphSeparator(),
m_dataName("ParagraphBoundaries")
{
  m_paragraphSeparator=Common::Misc::utf8stdstring2limastring("\n\n");
}


ParagraphBoundariesFinder::~ParagraphBoundariesFinder()
{}

void ParagraphBoundariesFinder::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* /*manager*/)

{
  /** @addtogroup ProcessUnitConfiguration
   * - <b>&lt;group name="..." class="ParagraphBoundariesFinder"&gt;</b>
   */  
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // optional: keep default
  }

  try
  {
    m_dataName=unitConfiguration.getParamsValueAtKey("data");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // optional: keep default
  }

  
  try
  {
    m_paragraphSeparator=Common::Misc::utf8stdstring2limastring(unitConfiguration.getParamsValueAtKey("paragraphSeparator"));
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // optional: keep default
  }
}

LimaStatusCode ParagraphBoundariesFinder::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  SENTBOUNDLOGINIT;
  LINFO << "start finding paragraph founds";
  
  // find paragraphs in text (positions of double carriage returns),
  // then find corresponding vertices in graph

  auto graph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData(m_graph));
  if (graph==0) {
    LERROR << "no graph '" << m_graph << "' available !";
    return MISSING_DATA;
  }
  SegmentationData* boundaries=new SegmentationData(m_graph);
  analysis.setData(m_dataName,boundaries);

  auto text = std::dynamic_pointer_cast<LimaStringText>(analysis.getData("Text"));
  
  std::vector<uint64_t> paragraphPositions;
  int currentPos=0;
  int i=text->indexOf(m_paragraphSeparator,currentPos);
  while (i!=-1) {
    paragraphPositions.push_back((uint64_t)i);
    // goto next char that is not a carriage return
    currentPos=text->indexOf(QRegularExpression(QString(QLatin1String("[^%1]")).arg(m_paragraphSeparator)),i+1);
    i=text->indexOf(m_paragraphSeparator,currentPos);
  }

  if (paragraphPositions.empty()) {
    LWARN << "no paragraph found";
    return SUCCESS_ID;
  }

  // find vertices related to positions in graph
  uint64_t parNum=0;
  std::deque<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;

  LinguisticGraphVertex beginParagraph=graph->firstVertex();

  toVisit.push_back(graph->firstVertex());
  visited.insert(graph->firstVertex());

  while (!toVisit.empty())
  {
    LinguisticGraphVertex currentVertex=toVisit.front();
    toVisit.pop_front();

    if (currentVertex == graph->lastVertex()) { // end of the graph
      continue;  // may be other nodes to test in queue
    }

    if (currentVertex != graph->firstVertex()) {
      Token* t = get(vertex_token,*(graph->getGraph()),currentVertex);
      uint64_t position=t->position();
      if (position >= (paragraphPositions[parNum]+1)) {
        // end vertex is previous vertex
        LinguisticGraphInEdgeIt inEdge,inEdge_end;
        boost::tie(inEdge,inEdge_end)=in_edges(currentVertex,*(graph->getGraph()));
        LinguisticGraphVertex prevVertex=source(*inEdge,*(graph->getGraph()));
        if (beginParagraph!=prevVertex) {
          boundaries->add(Segment("paragraph", beginParagraph, prevVertex, graph.get()));
        }
        beginParagraph=prevVertex;
        //boundaries->add(Segment("paragraph",beginParagraph,currentVertex,graph));
        //beginParagraph=currentVertex;
        parNum++;
        if (parNum >= paragraphPositions.size()) {
          break;
        }
      }
    }

    // store following nodes to test
    LinguisticGraphOutEdgeIt outEdge,outEdge_end;
    boost::tie(outEdge,outEdge_end)=out_edges(currentVertex,*(graph->getGraph()));

    for (; outEdge!=outEdge_end; outEdge++) {
      LinguisticGraphVertex next=target(*outEdge,*(graph->getGraph()));
      if (visited.find(next)==visited.end()) {
        toVisit.push_back(next);
        visited.insert(next);
      }
    }
  }
  
  // add last segment as a paragraph
  if (beginParagraph!=graph->lastVertex()) {
    boundaries->add(Segment("paragraph", beginParagraph, graph->lastVertex(), graph.get()));
  }
  
  TimeUtils::logElapsedTime("ParagraphBoundariesFinder");
  return SUCCESS_ID;
}


}

}

}
