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
#include "DepTripletLogger.h"

#include "common/time/traceUtils.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowComplexToken.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/AnalysisDumpers/BowGeneration.h"

#include <fstream>
#include <deque>
#include <queue>
#include <algorithm>


using namespace std;
using namespace boost;
using namespace boost::tuples;

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::AnnotationGraphs;

using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Compounds;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SyntacticAnalysis
{

SimpleFactory<MediaProcessUnit,DepTripletLogger> DepTripletLoggerFactory(DEPTRIPLETLOGGER_CLASSID);

DepTripletLogger::DepTripletLogger() :
    AbstractLinguisticLogger(".out")
{
  m_bowGenerator = new BowGenerator();
}


DepTripletLogger::~DepTripletLogger()
{
  delete m_bowGenerator;
}

void DepTripletLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  m_language=manager->getInitializationParameters().media;
  std::deque<std::string> relation_names;

  try
  {
    relation_names=unitConfiguration.getListsValueAtKey("selectedDependency");
  }
  catch (NoSuchList& )
  {
    // empty display
  }
  for( std::deque<std::string>::iterator src = relation_names.begin() ; src != relation_names.end() ; src++ )
  {
    m_relation_names.insert(*src);
  }


  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));

  m_bowGenerator->init(unitConfiguration, m_language);
}

LimaStatusCode DepTripletLogger::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  SALOGINIT;
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anagraph==0)
  {
    LERROR << "no AnalysisGraph ! abort";
    return MISSING_DATA;
  }
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "no PosGraph ! abort";
    return MISSING_DATA;
  }
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    syntacticData=new SyntacticData(posgraph,0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }

  std::ofstream outputStream;
  if (!openLogFile(outputStream,metadata->getMetaData("FileName")))
  {
    SALOGINIT;
    LERROR << "Can't open log file ";
    return CANNOT_OPEN_FILE_ERROR;
  }

  VxToTermsMap compoundsHeads = getCompoundsHeads(annotationData,
    syntacticData, analysis,posgraph, *(anagraph->getGraph()), 
    *(posgraph->getGraph()));
  dumpDependencyRelations(outputStream, *(anagraph->getGraph()), 
                          *(posgraph->getGraph()), syntacticData, 
                          annotationData, compoundsHeads);

  //  delete syntacticData;

  TimeUtils::logElapsedTime("DepTripletLogger");
  return SUCCESS_ID;
}


void DepTripletLogger::dumpDependencyRelations(std::ostream& outputStream,
                                              const LinguisticGraph& anagraph,
                                              const LinguisticGraph& posgraph,
                                              const SyntacticData* syntData,
                        const AnnotationData* annotationData,
                        const VxToTermsMap& compoundsHeads) const
{
  SALOGINIT;
  LDEBUG << "Address of syntactic data used to dump: " << syntData;
  const DependencyGraph* depGraph = syntData-> dependencyGraph();
  LDEBUG << "Address of dependency graph used to dump: " << depGraph;


  const LinguisticGraph* graph = syntData->graph();
  CVertexDataPropertyMap dataMap = get(vertex_data, *graph);
//   const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  DependencyGraphEdgeIt it, it_end;
  boost::tie(it, it_end) = edges(*depGraph);
  for (; it != it_end; it++)
  {
    LDEBUG << "Dumping dependency edge " << source(*it, *depGraph) << " -> " << target(*it, *depGraph);
    try
    {
      LDEBUG << "DepTripletLogger::dumpDependencyRelations";
      CEdgeDepRelTypePropertyMap typeMap = get(edge_deprel_type, *depGraph);
      Common::MediaticData::SyntacticRelationId type = typeMap[*it];
      LDEBUG << "DepTripletLogger::dumpDependencyRelations relation = "
             << static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(type);
      std::set<std::string>::const_iterator relationPos =
          m_relation_names.find(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(type));
      if( relationPos != m_relation_names.end() )
      {
        LDEBUG << "Src  : Dep vertex= " << source(*it, *depGraph);
        LinguisticGraphVertex src = syntData->tokenVertexForDepVertex(source(*it, *depGraph));
        LDEBUG << "Src  : Morph vertex= " << src;
        LDEBUG << "Targ : Dep vertex= " << target(*it, *depGraph) ;
        LinguisticGraphVertex dest = syntData->tokenVertexForDepVertex(target(*it, *depGraph));
        LDEBUG << "Targ : Morph vertex= " << dest;

        std::set<StringsPoolIndex> srcLemmas=dataMap[src]->allLemma();
        std::set<StringsPoolIndex> destLemmas=dataMap[dest]->allLemma();

/*        for (set<StringsPoolIndex>::const_iterator srcItr=srcLemmas.begin();
             srcItr!=srcLemmas.end();
             srcItr++)
        {
          for (set<StringsPoolIndex>::const_iterator destItr=destLemmas.begin();
               destItr!=destLemmas.end();
               destItr++)
          {
            outputStream << sp[*srcItr] << "#" << *relationPos << "#" << sp[*destItr] << std::endl;
          }
        }*/
        dumpDepWithCompounds(outputStream, anagraph, posgraph, syntData, 
                             annotationData, src, dest,
                             *relationPos, compoundsHeads);
      }
      else
      {
        LDEBUG << "DepTripletLogger::dumpDependencyRelations: dump nothing..";
      }
    }
    catch (const std::range_error& )
    {
    }
    catch (...)
    {
      LDEBUG << "DepTripletLogger::dumpDependencyRelations: catch others.....";
      throw;
    }
  }
}


void DepTripletLogger::dumpDepWithCompounds(
      std::ostream& outputStream,
      const LinguisticGraph& anagraph,
      const LinguisticGraph& posgraph,
      const SyntacticData* syntData,
      const AnnotationData* annotationData,
      LinguisticGraphVertex src,
      LinguisticGraphVertex dest,
      const std::string& rel,
      const VxToTermsMap& compoundsHeads) const
{
  LIMA_UNUSED(syntData);
//   CVertexDataPropertyMap dataMap = get(vertex_data, posgraph);

  std::set< LinguisticGraphVertex > visited;
  std::vector< std::pair<Common::BagOfWords::BoWRelation*,Common::BagOfWords::AbstractBoWElement*> > srcTokens =
    m_bowGenerator->createAbstractBoWElement(src, anagraph, posgraph, 0, annotationData, visited);
  std::vector< std::pair<Common::BagOfWords::BoWRelation*,Common::BagOfWords::AbstractBoWElement*> >  destTokens =
    m_bowGenerator->createAbstractBoWElement(dest, anagraph, posgraph, 0, annotationData, visited);

  std::map<std::string, std::set<LinguisticGraphVertex> > srcs, dests;

  for (std::vector< std::pair<Common::BagOfWords::BoWRelation*,Common::BagOfWords::AbstractBoWElement*> > ::const_iterator srcItr=srcTokens.begin();
       srcItr!=srcTokens.end();
       srcItr++)
  {
    std::set<LinguisticGraphVertex> theSet;
//     std::cerr << "collecting vertices for " << (*srcItr)->getOutputUTF8String() << std::endl;
//     std::cerr << "  inserting " << src << std::endl;
    theSet.insert(src);
    srcs.insert(std::make_pair((*srcItr).second->getOutputUTF8String(),theSet));
    delete (*srcItr).first;
    delete (*srcItr).second;
  }
  if (compoundsHeads.find(src) != compoundsHeads.end())
  {
    std::set<const BoWTerm*>::const_iterator it, it_end;
    it = (*(compoundsHeads.find(src))).second.begin();
    it_end = (*(compoundsHeads.find(src))).second.end();
    for (; it != it_end; it++)
    {
      std::ostringstream oss;
      oss << (*it)->getOutputUTF8String();
      std::set<LinguisticGraphVertex> theSet;
//       std::cerr << "collectiong vertices for " << **it << std::endl;
      collectVertices(theSet, *it);
      srcs.insert(std::make_pair(oss.str(), theSet));
    }
  }

  for (std::vector< std::pair<Common::BagOfWords::BoWRelation*,Common::BagOfWords::AbstractBoWElement*> > ::const_iterator destItr=destTokens.begin();
        destItr!=destTokens.end();
        destItr++)
  {
    std::set<LinguisticGraphVertex> theSet;
//     std::cerr << "collectiong vertices for " << (*destItr)->getOutputUTF8String() << std::endl;
//     std::cerr << "  inserting " << dest << std::endl;
    theSet.insert(dest);
    dests.insert(std::make_pair((*destItr).second->getOutputUTF8String(),theSet));
    delete (*destItr).first;
    delete (*destItr).second;
  }
  if (compoundsHeads.find(dest) != compoundsHeads.end())
  {
    std::set<const BoWTerm*>::const_iterator it, it_end;
    it = (*(compoundsHeads.find(dest))).second.begin();
    it_end = (*(compoundsHeads.find(dest))).second.end();
    for (; it != it_end; it++)
    {
      std::ostringstream oss;
      oss << (*it)->getOutputUTF8String();
      std::set<LinguisticGraphVertex> theSet;
//       std::cerr << "collectiong vertices for " << **it << std::endl;
      collectVertices(theSet, *it);
      dests.insert(std::make_pair(oss.str(),theSet));
    }
  }

  for (
        std::map<std::string, std::set<LinguisticGraphVertex> >::const_iterator sit = srcs.begin();
        sit != srcs.end();
        sit++
      )
  {
    for (
          std::map<std::string, std::set<LinguisticGraphVertex> >::const_iterator dit = dests.begin();
          dit != dests.end();
          dit++
        )
    {
      std::set<LinguisticGraphVertex> intersection;
      std::insert_iterator<std::set<LinguisticGraphVertex> > ii(intersection, intersection.begin());
      std::set_intersection( (*sit).second.begin(), (*sit).second.end(),
                             (*dit).second.begin(), (*dit).second.end(),
                             ii);
      if (intersection.empty())
      {
        outputStream << (*sit).first << "#" << rel << "#" << (*dit).first << std::endl;
      }
    }
  }
}

VxToTermsMap DepTripletLogger::getCompoundsHeads(
    const Common::AnnotationGraphs::AnnotationData* annotationData,
    const SyntacticData* syntacticData,
    AnalysisContent& analysis,
    AnalysisGraph* posAgraph,
    const LinguisticGraph& anagraph,
    const LinguisticGraph& posgraph) const
{
  LIMA_UNUSED(analysis);
  SALOGINIT;
  LDEBUG << "DepTripletDumper: ========================================";
  LDEBUG << "DepTripletDumper: getting compounds heads";
  const LinguisticGraphVertex begin = posAgraph->firstVertex();
  const LinguisticGraphVertex end = posAgraph->lastVertex();

  VxToTermsMap result;

  // go through the graph, add BoWTokens that are not in complex terms
  // Don't use visitor to avoid throwing exceptions

  LinguisticGraphVertex firstVx = posAgraph->firstVertex();
  LinguisticGraphVertex lastVx = posAgraph->lastVertex();

  std::set< std::string > alreadyStored;
  std::set<LinguisticGraphVertex> visited;
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

    for (boost::tie(outItr,outItrEnd)=out_edges(v,posgraph);
         outItr!=outItrEnd;
         outItr++)
    {
      LinguisticGraphVertex next=target(*outItr,posgraph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }

    if (v != firstVx && v != lastVx)
    {
      /// @todo replace v in LDEBUGs below by matching annotation vertices
//     LDEBUG << "hasAnnotation("<<v<<", CompoundTokenAnnotation): "
//         << annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation"));
//       LDEBUG << "hasAnnotation("<<v<<", SpecificEntity): "
//         << annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("SpecificEntity"));
      //std::set< uint32_t > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead");
      std::set< AnnotationGraphVertex > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead");
      if (!cpdsHeads.empty())
      {
        std::set< AnnotationGraphVertex >::const_iterator cpdsHeadsIt, cpdsHeadsIt_end;
        cpdsHeadsIt = cpdsHeads.begin(); cpdsHeadsIt_end = cpdsHeads.end();
        for (; cpdsHeadsIt != cpdsHeadsIt_end; cpdsHeadsIt++)
        {
          AnnotationGraphVertex agv  = *cpdsHeadsIt;
          std::vector<std::pair<BoWRelation*, BoWToken*> > bowTokens = 
            m_bowGenerator->buildTermFor(agv, agv, anagraph, posgraph, 0, 
                                         syntacticData, annotationData, visited);
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
              const BoWTerm* bt = dynamic_cast<const BoWTerm*>((*bowItr).second);
              if (bt != 0)
              {
                getCompoundsHeads(result, bt);
              }
              alreadyStored.insert(elem);
            }
          }
        }
      }
      else
      {
        std::vector<std::pair<BoWRelation*, AbstractBoWElement*> > bowTokens=m_bowGenerator->createAbstractBoWElement(v, anagraph, posgraph, 0, annotationData, visited);

        for (std::vector<std::pair<BoWRelation*, AbstractBoWElement*> >::const_iterator bowItr=bowTokens.begin();
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
            const BoWTerm* bt = dynamic_cast<const BoWTerm*>((*bowItr).second);
            if (bt != 0)
            {
              getCompoundsHeads(result, bt);
            }
            alreadyStored.insert(elem);
          }
        }
      }
    }
  }

  return result;
}

void DepTripletLogger::getCompoundsHeads(
                                         VxToTermsMap& result,
                                         const BoWTerm* bt) const
{
  if (result.find(bt->getVertex()) == result.end())
  {
    result.insert(std::make_pair(bt->getVertex(), std::set<const BoWTerm*>()));
  }
  result[bt->getVertex()].insert(dynamic_cast<const BoWTerm*>(bt));
  std::deque<BoWComplexToken::Part>::const_iterator partsit, partsit_end;
  partsit = bt->getParts().begin(); partsit_end = bt->getParts().end();
  for (; partsit!=partsit_end; partsit++)
  {
    if ( dynamic_cast<const BoWTerm*>((*partsit).get<1>()) != 0)
    {
      getCompoundsHeads(result, dynamic_cast<const BoWTerm*>((*partsit).get<1>()));
    }
  }
}

void DepTripletLogger::collectVertices(
        std::set<LinguisticGraphVertex>& theSet,
        const BoWToken* term) const
{
//   std::cerr << "  inserting " << term->getVertex() << std::endl;
  theSet.insert(term->getVertex());
  if ( dynamic_cast<const BoWTerm*>(term) != 0)
  {
    std::deque<BoWComplexToken::Part>::const_iterator partsit, partsit_end;
    partsit = dynamic_cast<const BoWTerm*>(term)->getParts().begin();
    partsit_end = dynamic_cast<const BoWTerm*>(term)->getParts().end();
    for (; partsit!=partsit_end; partsit++)
    {
      collectVertices(theSet, (*partsit).get<1>());
    }
  }
}


} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
