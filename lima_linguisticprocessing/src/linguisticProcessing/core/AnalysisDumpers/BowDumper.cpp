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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "BowDumper.h"
// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "common/time/timeUtilsController.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/genericDocumentProperties.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/AnalysisDumpers/BowGeneration.h"
#include "common/Handler/AbstractAnalysisHandler.h"
#include "common/MediaProcessors/MediaAnalysisDumper.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
// #include "linguisticProcessing/core/LinguisticProcessors/DumperStream.h"
#include "common/MediaProcessors/DumperStream.h"

#include <boost/graph/properties.hpp>

#include <fstream>
#include <deque>
#include <queue>
#include <iostream>

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::Compounds;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
// using namespace Lima::LinguisticProcessing::Compounds;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace std;
namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<MediaProcessUnit,BowDumper> bowDumperFactory(BOWDUMPER_CLASSID);

typedef boost::color_traits<boost::default_color_type> Color;

BowDumper::BowDumper():
   AbstractTextualAnalysisDumper(),
//     MediaProcessUnit(),
    m_bowGenerator(new Compounds::BowGenerator()),
    m_handler(),
    m_graph()
{
}

BowDumper::~BowDumper()
{
  delete m_bowGenerator;
}

void BowDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
//   DUMPERLOGINIT;
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  
  MediaId language = manager->getInitializationParameters().media;
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& )
  {
    m_graph=std::string("PosGraph");
  }
  try
  {
    m_handler=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )
  {
    DUMPERLOGINIT;
    LERROR << "Missing parameter handler in BowDumper configuration";
    throw InvalidConfiguration();
  }
  
  m_bowGenerator->init(unitConfiguration, language);
}

LimaStatusCode BowDumper::process(
  AnalysisContent& analysis) const
{
  TimeUtilsController timer("BowDumper");
  DUMPERLOGINIT;

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  LDEBUG << "BowDumper handler will be: " << m_handler;
//   MediaId langid = static_cast<const  Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getMedia();

  AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));

  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
  if (handler==0)
  {
    LERROR << "BowDumper::process: handler " << m_handler << " has not been given to the core client";
    return MISSING_DATA;
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anagraph==0)
  {
    LERROR << "BowDumper::process: no graph 'AnaGraph' available !";
    return MISSING_DATA;
  }
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "BowDumper::process: no graph 'PosGraph' available !";
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "BowDumper::process: no annotation graph available !";
    return MISSING_DATA;
  }
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
/*  if (syntacticData==0)
  {
    LERROR << "BowDumper::process: no SyntacticData ! abort";
    return MISSING_DATA;
  }*/
  if (syntacticData==0)
  {
    syntacticData=new SyntacticAnalysis::SyntacticData(static_cast<AnalysisGraph*>(analysis.getData(m_graph)),0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }


  // build BoWText from the result of the analysis
  BoWText bowText;
  bowText.lang=metadata->getMetaData("Lang");
  buildBoWText(annotationData, syntacticData, bowText,analysis,anagraph,posgraph);

  // pass bowText to the handler
  /*  {
      BoWBinaryWriter writer;
      handler->startAnalysis();
      HandlerStreamBuf hsb(handler);
      std::ostream out(&hsb);
      writer.writeBoWText(out,bowText);
      out.flush();
      handler->endAnalysis();
    }*/
  {
      BoWBinaryWriter writer;
      DumperStream* dstream=initialize(analysis);
      writer.writeBoWText(dstream->out(),bowText);
      delete dstream;
  }
  /**
  * @TODO Handling of bowtext
  **
  {
    LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
    if (metadata == 0) {
        LERROR << "BowDumper::process: no LinguisticMetaData ! abort";
        return MISSING_DATA;
    }
    std::string outputFile=metadata->getMetaData("FileName") + ".bin";
    std::ofstream fout(outputFile.c_str());
    BoWFileHeader header(BOWFILE_TEXT);
    header.write(fout);
    bowText.write(fout);
    fout.close();
  }
  */
  ///
  return SUCCESS_ID;
}

void BowDumper::buildBoWText(
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const SyntacticData* syntacticData,
    BoWText& bowText,
                              AnalysisContent& analysis,
                              AnalysisGraph* anagraph,
                              AnalysisGraph* posgraph) const
{
  DUMPERLOGINIT;

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));

  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LWARN << "no SentenceBounds";
  }

  if (sb==0)
  {
    // no sentence bounds : there can be specific entities,
    // but no compounds (syntactic analysis depend on sentence bounds)
    // dump whole text at once
    addVerticesToBoWText(
        annotationData,
                          anagraph,
                          posgraph,
                          syntacticData,
        anagraph->firstVertex(),
        anagraph->lastVertex(),
        metadata->getStartOffset(),
        bowText);

  }
  else
  {
    // ??OME2 for (SegmentationData::const_iterator boundItr=sb->begin();
    //boundItr!=sb->end();
    for (std::vector<Segment>::const_iterator boundItr=(sb->getSegments()).begin();
      boundItr!=(sb->getSegments()).end();
    boundItr++)
    {    
      LinguisticGraphVertex sentenceBegin=boundItr->getFirstVertex();
      LinguisticGraphVertex sentenceEnd=boundItr->getLastVertex();

      LDEBUG << "dump sentence between " << sentenceBegin
      << " and " << sentenceEnd;

      LDEBUG << "dump simple terms for this sentence";
      addVerticesToBoWText(annotationData,
                           anagraph,
                           posgraph,
                           syntacticData,
                           sentenceBegin,
                           sentenceEnd,
                           metadata->getStartOffset(),
                           bowText);

    }
  }
}

void BowDumper::addVerticesToBoWText(
  const Common::AnnotationGraphs::AnnotationData* annotationData,
                                      AnalysisGraph* anagraph,
                                      AnalysisGraph* posgraph,
                                      const SyntacticData* syntacticData,
  const LinguisticGraphVertex begin,
  const LinguisticGraphVertex end,
  const uint64_t offset,
  BoWText& bowText) const
{

  DUMPERLOGINIT;
  LDEBUG << "BowDumper: ========================================";
  LDEBUG << "BowDumper: adding to BoWText from vertex "  << begin << " to vertex " << end;

  const LinguisticGraph& beforePoSGraph=*(anagraph->getGraph());
  const LinguisticGraph& graph=*(posgraph->getGraph());
  
  // go through the graph, add BoWTokens that are not in complex terms
  // Don't use visitor to avoid throwing exceptions

  LinguisticGraphVertex firstVx = anagraph->firstVertex();
  LinguisticGraphVertex lastVx = anagraph->lastVertex();

  std::set< std::string > alreadyStored;
  std::set<LinguisticGraphVertex> visited;
  //std::set<uint32_t> alreadyStoredVertices; compatibilite 32 64 bits
  std::set<LinguisticGraphVertex> alreadyStoredVertices;

  std::queue<LinguisticGraphVertex> toVisit;
  toVisit.push(begin);

  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  while (!toVisit.empty())
  {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (v == end) {
      continue;
    }

    for (boost::tie(outItr,outItrEnd)=out_edges(v,graph);
         outItr!=outItrEnd;
         outItr++)
    {
      LinguisticGraphVertex next=target(*outItr,graph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }

    if (v != firstVx && v != lastVx)
    {
      /// @todo replace v in LDEBUGs below by matching annotation vertices
//       LDEBUG << "BowDumper: hasAnnotation("<<v<<", CompoundTokenAnnotation): "
//         << annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation"));
//       LDEBUG << "BowDumper: hasAnnotation("<<v<<", SpecificEntity): "
//         << annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity"));
      //std::set< uint64_t > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead"); compatibilite 32 64 bits
      std::set< AnnotationGraphVertex > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead");
      if (!cpdsHeads.empty())
      {
        std::set< AnnotationGraphVertex >::const_iterator cpdsHeadsIt, cpdsHeadsIt_end;
        cpdsHeadsIt = cpdsHeads.begin(); cpdsHeadsIt_end = cpdsHeads.end();
        for (; cpdsHeadsIt != cpdsHeadsIt_end; cpdsHeadsIt++)
        {
          AnnotationGraphVertex agv  = *cpdsHeadsIt;
          std::vector<std::pair<BoWRelation*, BoWToken*> > bowTokens = m_bowGenerator->buildTermFor(agv, agv, beforePoSGraph, graph, offset, syntacticData, annotationData, visited);
          for (std::vector<std::pair<BoWRelation*, BoWToken*> >::const_iterator bowItr=bowTokens.begin();
              bowItr!=bowTokens.end();
              bowItr++)
          {
            std::string elem = (*bowItr).second->getIdUTF8String();
            if (alreadyStored.find(elem) != alreadyStored.end())
            { // already stored
              //          LDEBUG << "BuildBoWTokenListVisitor: BoWToken already stored. Skipping it.";
              delete (*bowItr).first;
              delete (*bowItr).second;
            }
            else
            {             
              bowText.push_back((*bowItr).second); // copy pointer
              std::set<uint64_t> bowTokenVertices = (*bowItr).second->getVertices();
              //std::set<LinguisticGraphVertex> bowTokenVertices = (*bowItr)->getVertices();
              alreadyStoredVertices.insert(bowTokenVertices.begin(), bowTokenVertices.end());
              alreadyStored.insert(elem);
              
              std::ostringstream oss;
              //std::set<uint32_t>::const_iterator asvit, asvit_end;
              std::set<LinguisticGraphVertex>::const_iterator asvit, asvit_end;
              asvit = alreadyStoredVertices.begin(); asvit_end = alreadyStoredVertices.end();
              for (; asvit != asvit_end; asvit++)
              {
                oss << *asvit << ", ";
              }
            LDEBUG << "BowDumper: for " << v << " alreadyStoredVertices are: " << oss.str();
            }
          }
        }
      }
      else if (alreadyStoredVertices.find(v) == alreadyStoredVertices.end())
      {
        std::vector<std::pair<BoWRelation*, BoWToken*> > bowTokens=
          m_bowGenerator->createBoWTokens(v, beforePoSGraph, graph, offset, annotationData, visited);

        for (std::vector<std::pair<BoWRelation*, BoWToken*> >::const_iterator bowItr=bowTokens.begin();
            bowItr!=bowTokens.end();
            bowItr++)
        {
          std::string elem = (*bowItr).second->getIdUTF8String();
          if (alreadyStored.find(elem) != alreadyStored.end())
          { // already stored
            //          LDEBUG << "BuildBoWTokenListVisitor: BoWToken already stored. Skipping it.";
            delete (*bowItr).first;
            delete (*bowItr).second;
          }
          else
          {
            bowText.push_back((*bowItr).second); // copy pointer
            std::set<uint64_t> bowTokenVertices = (*bowItr).second->getVertices();
            alreadyStoredVertices.insert(bowTokenVertices.begin(), bowTokenVertices.end());
            alreadyStored.insert(elem);
          
            std::ostringstream oss;
            //std::set<uint32_t>::const_iterator asvit, asvit_end;
            std::set<LinguisticGraphVertex>::const_iterator asvit, asvit_end;
            asvit = alreadyStoredVertices.begin(); asvit_end = alreadyStoredVertices.end();
            for (; asvit != asvit_end; asvit++)
            {
              oss << *asvit << ", ";
            }
          LDEBUG << "BowDumper: for " << v << " alreadyStoredVertices are: " << oss.str();
          }
        }
      }
      else
      {
        LDEBUG << "BowDumper: " << v << " is already stored.";
      }
    }
  }
}

} // AnalysisDumper

} // LinguisticProcessing

} // Lima
