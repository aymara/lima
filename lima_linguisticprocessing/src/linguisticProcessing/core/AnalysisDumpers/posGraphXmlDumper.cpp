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
  * @file       posGraphXmlDumper.cpp
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *             Jorge García Flores <jorge.garcia-flores@cea.fr>  
  *             Copyright (C) 2004 by CEA LIST
  * @author     Besancon Romaric (besanconr@zoe.cea.fr)
  * @date       March 28 2008
  *
  * @brief      dump just the content of the posgraph in XML format
  *
  * @change_log: 
  * 12/2007:   JGF: changed breadth_first_visit instead of breadth_first_search 
  * 03/2007:    JGF: correct duplicate vertex error in ::outputVertex
  * 01/2012:    GC: no more boost search algorithm
  */

#include "linguisticProcessing/common/BagOfWords/bowDocument.h"

#include "posGraphXmlDumper.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"


#include "common/Data/strwstrtools.h"
#include "common/time/timeUtilsController.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h" //rajout
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/AnalysisDumpers/BowGeneration.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
//TOTO include annotation.h
#include "linguisticProcessing/core/CorefSolving/coreferentAnnotation.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"

#include <fstream>
#include <queue>
//ajout include
#include <boost/regex.hpp>
#include <boost/config.hpp>

using namespace std;
//using namespace boost;
using namespace boost::tuples;

using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;

using namespace Lima::LinguisticProcessing::Compounds;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {


//***********************************************************************
// constructors
//***********************************************************************
SimpleFactory<MediaProcessUnit,posGraphXmlDumper> posGraphXmlDumperFactory(POSGRAPHXMLDUMPER_CLASSID);

posGraphXmlDumper::posGraphXmlDumper()
: MediaProcessUnit(),
      m_dumpFullTokens(true),
      m_handler()

{
}

posGraphXmlDumper::~posGraphXmlDumper()
{
}

void posGraphXmlDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DUMPERLOGINIT;
  LDEBUG << "posGraphXmlDumper init!";
  m_language=manager->getInitializationParameters().media;
  m_propertyCodeManager= &(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager());
  try
  {
    m_dumpFullTokens = (unitConfiguration.getParamsValueAtKey("dumpTokens") == "true");
  }
  catch (NoSuchParam& )
  {
    LWARN << "dumpTokens parameter not found, using default: "
        << (m_dumpFullTokens?"true":"false");
  }
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& )
  {
    m_graph=string("PosGraph");
  }
  try
  {
    m_handler=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )
  {
    DUMPERLOGINIT;
    LERROR << "posGraphXmlDumper::init: Missing parameter handler in posGraphXmlDumper configuration";
    throw InvalidConfiguration();
  }

  m_bowGenerator = new BowGenerator();
  m_bowGenerator->init(unitConfiguration, m_language);
  
}

LimaStatusCode posGraphXmlDumper::process(AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("posGraphXmlDumper");
  DUMPERLOGINIT;

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "posGraphXmlDumper::process: no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  LDEBUG << "handler will be: " << m_handler;
  AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
  if (handler==0)
  {
    LERROR << "posGraphXmlDumper::process: handler " << m_handler << " has not been given to the core client";
    return MISSING_DATA;
  }

  AnalysisGraph* graph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (graph==0) {
    graph=new AnalysisGraph(m_graph,m_language,true,true);
    analysis.setData(m_graph,graph);
  }

  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    syntacticData=new SyntacticAnalysis::SyntacticData(static_cast<AnalysisGraph*>(analysis.getData(m_graph)),0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }

  // Are sentences bounds right?
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    sb=new SegmentationData(m_graph);
    analysis.setData("SentenceBoundaries",sb);
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData=new AnnotationData();
    if (static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"))->populateAnnotationGraph(annotationData, "AnalysisGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }

  handler->startAnalysis();
  HandlerStreamBuf hsb(handler);
  std::ostream outputStream(&hsb);
  std::set< std::pair<size_t, size_t> > alreadyDumped;

  outputStream << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
  outputStream << "<!DOCTYPE lima_analysis_dump SYSTEM \"lima-xml-output.dtd\">" << std::endl;
  outputStream << "<lima_analysis_dump>" << std::endl;



   // ??OME2 SegmentationData::iterator sbItr=sb->begin();
   std::vector<Segment>::iterator sbItr=(sb->getSegments().begin());

   AnalysisGraph* anagraph = static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
   AnalysisGraph* posgraph = static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
   if (posgraph != 0)
  {
    std::vector< bool > alreadyDumpedTokens;
    std::map< LinguisticAnalysisStructure::Token*, uint64_t > fullTokens;
    LinguisticGraphVertexIt i, i_end;
    uint64_t id = 0;
    alreadyDumpedTokens.resize(num_vertices(*posgraph->getGraph()));
    for (boost::tie(i, i_end) = vertices(*posgraph->getGraph()); i != i_end; ++i)
    {
      alreadyDumpedTokens[id] = false;
      fullTokens[get(vertex_token, *posgraph->getGraph(), *i)] = id;
      id++;
    }
    outputStream << "  <PosGraph>" << std::endl;
    int sentenceId = 0;
    // ??OME2 while (sbItr!=sb->end())
    while (sbItr!=(sb->getSegments().end()))
    {
      LinguisticGraphVertex sentenceBegin=sbItr->getFirstVertex();
      LinguisticGraphVertex sentenceEnd=sbItr->getLastVertex();
      dumpLimaData(outputStream,
                    sentenceBegin,
                    sentenceEnd,
                    anagraph,
                    posgraph,
                    syntacticData,
                    annotationData,
                    "PosGraph",
                    true, alreadyDumpedTokens, fullTokens, ++sentenceId);

      sbItr++;
    }
    outputStream << "  </PosGraph>" << std::endl;
  }
  outputStream << "</lima_analysis_dump>" << std::endl;
  handler->endAnalysis();
  return SUCCESS_ID;
}


//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void posGraphXmlDumper::dumpLimaData(std::ostream& os,
                            const LinguisticGraphVertex begin,
                            const LinguisticGraphVertex end,
                            const AnalysisGraph* anagraph,
                            const AnalysisGraph* posgraph,
                            const SyntacticData* syntacticData,
                            const AnnotationData* annotationData,
                            const std::string& graphId,
                            bool bySentence,
                            std::vector< bool >& alreadyDumpedTokens,
                            std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                            int sentenceId) const
{

  DUMPERLOGINIT;

  LDEBUG << "posGraphXmlDumper::dumpLimaData parameters: ";
  LDEBUG << "begin = "<< begin;
  LDEBUG << "end = " << end ;
  LDEBUG << "posgraph fist vertex= " << posgraph->firstVertex() ;
  LDEBUG << "posgraph last vertex= " << posgraph->lastVertex() ;
  LDEBUG << "graphId= " << graphId ;
  LDEBUG << "bySentence= " << bySentence ;
//    just in case we want to check alreadt dumped tokens' array
//     for (uint64_t i=0; i<alreadyDumpedTokens.size(); i++)
//     if (alreadyDumpedTokens[i]) LDEBUG << "already_dumped_tokens[" << i << "]=" << alreadyDumpedTokens[i];



  LinguisticGraph* lanagraph = const_cast< LinguisticGraph* >(anagraph->getGraph());
  LinguisticGraph* lposgraph = const_cast< LinguisticGraph* >(posgraph->getGraph());
  if (bySentence)
  {
    os << "  <sentence id=\""<<sentenceId<<"\">" << std::endl;
  }
  else
  {
    os << "  <"<<graphId<<">" << std::endl;
  }
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  toVisit.push(begin);
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    outputVertex(v, *lanagraph, *lposgraph, syntacticData, annotationData, os, fullTokens, alreadyDumpedTokens, graphId);

    if (v == end) {
      continue;
    }

    for (boost::tie(outItr,outItrEnd)=out_edges(v,*lposgraph); outItr!=outItrEnd; outItr++)
    {
      LinguisticGraphVertex next=target(*outItr,*lposgraph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }
  }
  if (bySentence)
  {
    os << "  </sentence>" << std::endl;
  }
  else
  {
    os << "  </"<<graphId<<">" << std::endl;
  }
}

//***********************************************************************
// output functions
//***********************************************************************


LimaString posGraphXmlDumper::getPosition(const uint64_t position) const
{
  std::ostringstream pos;
  pos << position;
  return Common::Misc::utf8stdstring2limastring(pos.str());
}

void posGraphXmlDumper::outputVertex(const LinguisticGraphVertex v,
                                     const LinguisticGraph& lanagraph,
                                     const LinguisticGraph& lposgraph,
                                const SyntacticData* syntacticData,
                                const AnnotationData* annotationData,
                                std::ostream& xmlStream,
                                std::map< LinguisticAnalysisStructure::Token*, uint64_t >& fullTokens,
                                std::vector< bool >& alreadyDumpedTokens,
                                const std::string& graphId) const
{
 FsaStringsPool& sp=Common::MediaticData::MediaticData::changeable().stringsPool(m_language);
 Token* token = get(vertex_token, lposgraph, v);
 uint64_t tokenId = (*(fullTokens.find(token))).second;
 bool alreadyDumped = alreadyDumpedTokens[tokenId];

// without this condition, there's duplicate vertex in the XML output!!!
 if (!alreadyDumped)
 {
    DUMPERLOGINIT;
    LDEBUG << "posGraphXmlDumper::outputVertex " << v;
    if (v == syntacticData->iterator()->firstVertex() ||
        v == syntacticData->iterator()->lastVertex())
    {
        xmlStream << "    <vertex id=\"_" << v << "\" />" << std::endl;
        return;
    }
    if (token == 0)
    {
      DUMPERLOGINIT;
      LWARN << "No token (vertex_token) for vertex "  << v;
      xmlStream << "    <vertex id=\"_" << v << "\" />" << std::endl;
      return;
    }


    xmlStream << "    <vertex id=\"_" << v << "\"";
  // debugging to take out JGF
  //   DUMPERLOGINIT;
    const VertexChainIdProp& chains = get(vertex_chain_id, lposgraph,v);
    if (chains.size() > 0)
    {
        xmlStream << " chains=\"";
        VertexChainIdProp::const_iterator itChains, itChains_end;
        itChains = chains.begin(); itChains_end = chains.end();
        xmlStream << (*itChains); itChains++;
        for (; itChains != itChains_end; itChains++)
        {
            xmlStream << "," << (*itChains);
        }
        xmlStream << "\"";
    }
    xmlStream << " >" << std::endl;

    if (graphId != "AnalysisGraph")
    {
      const DependencyGraph* depGraph = syntacticData->dependencyGraph();
      DependencyGraphVertex depV = syntacticData->depVertexForTokenVertex(v);
      if (out_degree(depV, *depGraph) > 0)
      {
          xmlStream << "      <deps>" << std::endl;
          DependencyGraphOutEdgeIt depIt, depIt_end;
          boost::tie(depIt, depIt_end) = out_edges(depV, *depGraph);
          for (; depIt != depIt_end; depIt++)
          {
              DependencyGraphVertex depTargV = target(*depIt, *depGraph);
              LinguisticGraphVertex targV = syntacticData-> tokenVertexForDepVertex(depTargV);
  //               CEdgeDepChainIdPropertyMap chainsMap = get(edge_depchain_id, *depGraph);
              CEdgeDepRelTypePropertyMap relTypeMap = get(edge_deprel_type, *depGraph);
              xmlStream << "        <dep v=\"_" << targV;
  //               xmlStream << "\" c=\"" << chainsMap[*depIt];
              xmlStream << "\" t=\"" <<
                static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).
                getSyntacticRelationName(relTypeMap[*depIt]);
              xmlStream << "\" />" << std::endl;
          }
          xmlStream << "      </deps>" << std::endl;
      }
    }

  /* ADD here the output of coreference antecedents if any */
    bool hasAntecedent = false;

    std::set< AnnotationGraphVertex > matches = annotationData->matches("PosGraph",v,"annot");
    if (!matches.empty())
    {
      AnnotationGraphVertex av = *matches.begin();

      AnnotationGraphVertex referee = av;
      // go through the referents chain to output the initial referee
      while (annotationData->hasAnnotation(referee, Common::Misc::utf8stdstring2limastring("Coreferent")))
      {
        AnnotationGraphVertex newReferee = 0;
        AnnotationGraphOutEdgeIt it, it_end;
        boost::tie(it, it_end) = boost::out_edges(referee, annotationData->getGraph());
        for (; it != it_end; it++)
        {
          if (annotationData->hasAnnotation(target(*it, annotationData->getGraph()), Common::Misc::utf8stdstring2limastring("Coreferent")))
          {
            newReferee = target(*it, annotationData->getGraph());
            break;
          }
        }
        if (newReferee != 0 && newReferee != referee)
        {
          hasAntecedent = true;
          referee = newReferee;
        }
        else break;
      } //while
      if (hasAntecedent)
      {
        std::set< AnnotationGraphVertex > refereeMatches = annotationData->matches("annot",referee,"PosGraph");
        if (refereeMatches.empty())
        {
          DUMPERLOGINIT;
          LERROR << "posGraphXmlDumper::outputVertex:  No PoS graph vertex matches annotation graph vertex  " << referee <<  ". This should not happen.";
        }
        AnnotationGraphVertex refereeAv = *refereeMatches.begin();
        xmlStream << "      <antecedent id=\"_" << refereeAv <<   "\" />" << endl;
      }
    }

    MorphoSyntacticData* data = get(vertex_data, lposgraph, v);
    if (data == 0)
    {
      DUMPERLOGINIT;
      LWARN << "No morphosyntactic (vertex_data) data for vertex "  << v;
    }
    else
    {
      data->outputXml(xmlStream, *m_propertyCodeManager,sp);
    }
    if (m_dumpFullTokens && !alreadyDumped)
    {
      token->outputXml(xmlStream, *m_propertyCodeManager,sp);
    }
    else
    {
      xmlStream << "    <ref>" << tokenId << "</ref>" << std::endl;
    }
    alreadyDumpedTokens[tokenId] = true;
    xmlStream << "    </vertex>" << std::endl;

  // dump complex tokens this token is the head of

    std::set<LinguisticGraphVertex> visited;
    std::set< std::string > alreadyStored;

    std::set< AnnotationGraphVertex > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead");
    if (!cpdsHeads.empty())
    {
      std::set< AnnotationGraphVertex >::const_iterator cpdsHeadsIt, cpdsHeadsIt_end;
      cpdsHeadsIt = cpdsHeads.begin(); cpdsHeadsIt_end = cpdsHeads.end();
      for (; cpdsHeadsIt != cpdsHeadsIt_end; cpdsHeadsIt++)
      {
        AnnotationGraphVertex agv  = *cpdsHeadsIt;
        std::vector<std::pair< boost::shared_ptr< BoWRelation>, boost::shared_ptr< BoWToken > > > bowTokens =
        m_bowGenerator->buildTermFor(agv, agv, lanagraph, lposgraph, 0, syntacticData, annotationData, visited);
        for (auto bowItr=bowTokens.begin(); bowItr!=bowTokens.end(); bowItr++)
        {
          std::string elem = (*bowItr).second->getIdUTF8String();
          if (alreadyStored.find(elem) != alreadyStored.end())
          { // already stored
          //          LDEBUG << "BuildBoWTokenListVisitor: BoWToken already stored. Skipping it.";
          }
          else
          {
            boost::shared_ptr< BoWToken > compound = (*bowItr).second;
            LDEBUG << "Outputing compound: " << *compound;
//             std::string cat = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(compound->getCategory());

            QVector<LimaString> compounds;
            naturalCompoundTokenString(boost::dynamic_pointer_cast< Common::BagOfWords::BoWTerm >(compound).get(), compounds);
            Q_FOREACH(const LimaString& compoundString, compounds)
            {
//               qDebug() << "naturalCompoundTokenString :" << compoundString;
              xmlStream << "    <vertex id=\"_compound\">" << std::endl;
              xmlStream << "      <string>" << Common::Misc::transcodeToXmlEntities(compoundString) << "</string>" << std::endl;
              xmlStream << "      <position>" << compound->getPosition() << "</position>" << std::endl;
              xmlStream << "      <length>" << compound->getLength() << "</length>" << std::endl;
              MorphoSyntacticData* data = get(vertex_data, lposgraph, v);
              if (data == 0)
              {
                DUMPERLOGINIT;
                LWARN << "No morphosyntactic (vertex_data) data for vertex "  << v;
              }
              else
              {
                xmlStream << "      <data>" << std::endl;
                xmlStream << "      <compound>" << std::endl;
                LimaString form=compoundString;
                LimaString lemma=compoundString;
                LimaString norm=compoundString;
                xmlStream << "      <form infl=\"" << Common::Misc::transcodeToXmlEntities(form) << "\" ";
                xmlStream << "lemma=\"" << Common::Misc::transcodeToXmlEntities(lemma) << "\" ";
                xmlStream << "norm=\"" << Common::Misc::transcodeToXmlEntities(norm) << "\">" << std::endl;
              }
              const std::map<std::string,Common::PropertyCode::PropertyManager>& managers=m_propertyCodeManager->getPropertyManagers();
              xmlStream << "        <property>" << std::endl;
              for (std::map<std::string,Common::PropertyCode::PropertyManager>::const_iterator propItr=managers.begin(); propItr!=managers.end(); propItr++)
              {
                if (!propItr->second.getPropertyAccessor().empty(data->begin()->properties))
                {
                  xmlStream << "          <p prop=\"" << propItr->first << "\" val=\"" << propItr->second.getPropertySymbolicValue(data->begin()->properties) << "\"/>" << std::endl;
                }
              }
              xmlStream << "        </property>" << std::endl;
              xmlStream << "      </form>" << std::endl;
              xmlStream << "    </compound>" << std::endl;
              xmlStream << "    </data>" << std::endl;
              xmlStream << "    </vertex>" << std::endl;
            }
          }
  //           outputCompound();
          alreadyStored.insert(elem);
        }
      }
    }
  }
}

/**
 * @todo Limit compound size
 * @todo output subterms
 */
void posGraphXmlDumper::naturalCompoundTokenString(const Common::BagOfWords::BoWTerm* compound, QVector< Lima::LimaString >& strings) const
{
//   qDebug() << "posGraphXmlDumper::naturalCompoundTokenString IN" << compound->getOutputUTF8String();
  if (compound == 0)
  {
//     strings << Common::Misc::utf8stdstring2limastring(compound->getOutputUTF8String());
    return;
  }

#ifndef WIN32
#if __cplusplus >= 201103L || ( ( defined(__GXX_EXPERIMENTAL_CXX0X__) ) && ( not defined(BOOST_NO_LAMBDAS) ) )

  std::deque< BoWComplexToken::Part > parts = compound->getParts();

  QMap<int, QSet<LimaString> > subresults;


  std::function<QSet<LimaString>(QMap <int, QSet <Lima::LimaString > >,int,int)> recurseResult;
  recurseResult = [&recurseResult](QMap <int, QSet <Lima::LimaString > >subresults,int i, int head)
  {
    QSet<LimaString> recurseResultResult;
    if (i < subresults.size())
    {
//       qDebug() << "posGraphXmlDumper::naturalCompoundTokenString recurseResult i=" << i << " ; subresults size=" << subresults.size();
      QSet<LimaString> E = subresults.values()[i]; // clazy:exclude=container-anti-pattern
      QSet<LimaString> nextResult = recurseResult(subresults,i+1,head);
      Q_FOREACH(const LimaString& e, E)
      {
        if (/*i == head || */nextResult.isEmpty())
        {
//           qDebug() << "posGraphXmlDumper::naturalCompoundTokenString recurseResultResult" << i << e;
          recurseResultResult << e;
        }
        Q_FOREACH(const LimaString& nextResultString, nextResult)
        {
//           qDebug() << "posGraphXmlDumper::naturalCompoundTokenString recurseResultResult concat" << i << (e + " " + nextResultString);
          recurseResultResult << (e + " " + nextResultString);
        }
      }
    }
//     qDebug() << "posGraphXmlDumper::naturalCompoundTokenString recurseResult lambda "<<i<<" returns result of size" << recurseResultResult.size()<<recurseResultResult;
    return recurseResultResult;
  };
  
  
  std::function< QMap<int, QSet<LimaString> >(std::deque< BoWComplexToken::Part >&,int)> recurse;
  recurse = [&recurse,&recurseResult](std::deque< BoWComplexToken::Part >& parts,uint64_t head) -> QMap<int, QSet<LimaString> >
  {
//     qDebug() << "posGraphXmlDumper::naturalCompoundTokenString entering recurse lambda "<<parts.size()<<head;
    QMap<int, QSet<LimaString> > recurseresults;
    for (std::deque< BoWComplexToken::Part >::size_type i = 0; i < parts.size(); i++)
    {
      boost::shared_ptr< BoWToken > partToken = parts[i].getBoWToken();
      recurseresults.insert(partToken->getPosition(), QSet<LimaString>());
      const BoWComplexToken::Part& part = parts[i];
      LimaString relation;
      if (part.getBoWRelation() != 0)
      {
        relation = part.getBoWRelation()->getRealization();
      }
      if (!relation.isEmpty())
      {
        QSet< LimaString > relationSet;
        relationSet.insert(relation);
        recurseresults.insert(partToken->getPosition()-1,relationSet);
      }
      if (boost::dynamic_pointer_cast<Common::BagOfWords::BoWTerm>(partToken) != 0)
      {
        std::deque< BoWComplexToken::Part > parts = boost::dynamic_pointer_cast< Common::BagOfWords::BoWTerm >(partToken)->getParts();
        QMap<int, QSet<LimaString> > partTokenResults = recurse(parts,boost::dynamic_pointer_cast< Common::BagOfWords::BoWTerm >(partToken)->getHead());
//         naturalCompoundTokenString(dynamic_cast<const Common::BagOfWords::BoWTerm*>(partToken), partStrings);
        QSet<LimaString> partStrings = recurseResult(partTokenResults,0,head);
        // After building all terms for the parts, add the head
        // @TODO Add all terms built from the head token if complex
        partStrings.insert(parts[head].getBoWToken()->getLemma());
        
//         qDebug() << "posGraphXmlDumper::naturalCompoundTokenString recurse lambda after return from recurseResult with nb terms:" << partStrings.size();
        recurseresults.insert(partToken->getPosition(), QSet<LimaString>());
        Q_FOREACH(const QString& partString, partStrings)
        {
//           qDebug() << "posGraphXmlDumper::naturalCompoundTokenString subresults part term" << partToken->getPosition() << partString;
          recurseresults[partToken->getPosition()].insert(partString);
        }
      }
      else
      {
//         qDebug() << "posGraphXmlDumper::naturalCompoundTokenString subresults part simple" << partToken->getPosition() << partToken->getLemma();
        recurseresults[partToken->getPosition()].insert(partToken->getLemma());
      }
    }
//     qDebug() << "posGraphXmlDumper::naturalCompoundTokenString going out of recurse lambda with nb positions"<<recurseresults.size()<<recurseresults;
    return recurseresults;
  };
  subresults = recurse(parts,compound->getHead());

  QSet<LimaString> result = recurseResult(subresults,0,compound->getHead());
  Q_FOREACH(const LimaString& string, result)
  {
//     qDebug() << "posGraphXmlDumper::naturalCompoundTokenString final result:" << string;
    strings << string;
  }
  
#endif
#endif
  return;
}


void posGraphXmlDumper::outputEdge(const LinguisticGraphEdge e,
                              const LinguisticGraph& graph,
                              std::ostream& xmlStream) const
{
  xmlStream << "    <edge src=\"" << source(e, graph)
          << "\" targ=\"" << target(e, graph) << "\" />" << std::endl;
}


} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
