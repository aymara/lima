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
/** @brief       Dependency graph text form displaying tools
  *
  * @file        SyntacticAnalysisTools.cpp
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)

  *              Copyright (c) 2005 by CEA
  * @date
  * @version     $Id$
  *
  */

#include "SyntacticAnalysisTools.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include <common/Data/readwritetools.h>
#include "common/Data/strwstrtools.h"
//using namespace boost;
using namespace std;
using namespace Lima;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

void SyntacticAnalysisTools::displayChains(const AnalysisGraph* anagraph,std::ostream& out)
{
    out << "start displaying chains" << endl;
    const LinguisticGraph* graph=anagraph->getGraph();
    std::list< std::pair< LinguisticGraphVertex, ChainIdStruct > > startPoints;
    std::list< LinguisticGraphVertex > toLookAt;
    std::set< LinguisticGraphVertex > alreadyVisited;
    toLookAt.push_back(anagraph->firstVertex());
//    LinguisticGraphVertex lastVx=anagraph->lastVertex();
    while (! toLookAt.empty() )
    {
      LinguisticGraphVertex current = toLookAt.front();
      toLookAt.pop_front();
      if (alreadyVisited.find(current) != alreadyVisited.end())
      {
        continue;
      }
      alreadyVisited.insert(current);
      SALOGINIT;
      LDEBUG << "looking at " << current;
      set< ChainIdStruct > chains=get(vertex_chain_id, *graph, current);
      for (set<ChainIdStruct>::iterator chainItr=chains.begin();
              chainItr!=chains.end();
              chainItr++)
      {
          ChainIdStruct chain = *chainItr;
          if ( (chain.elemType() == BEGIN) || (chain.elemType() == UNIGRAM) )
          {
            startPoints.push_back(std::make_pair(current, chain));
          }
      }
      LinguisticGraphOutEdgeIt it,itEnd;
      boost::tie(it,itEnd)=out_edges(current,*graph);
      for (;it!=itEnd;it++)
      {
        toLookAt.push_back(target(*it,*graph));
      }
    }
    std::list< std::pair<LinguisticGraphVertex, ChainIdStruct> >::iterator startPointsItr, startPointsItr_end;
    startPointsItr = startPoints.begin(); startPointsItr_end = startPoints.end();
    std::set< ChainIdStruct > alreadyDisplayed;
    for (; startPointsItr != startPointsItr_end; startPointsItr++)
    {
      if (alreadyDisplayed.find((*startPointsItr).second) == alreadyDisplayed.end())
      {
        displayChain((*startPointsItr).second, (*startPointsItr).first, anagraph, out);
        alreadyDisplayed.insert((*startPointsItr).second);
      }
    }
/*
    map<ChainIdStruct,set<LinguisticGraphVertex> > startPoints;
    map<ChainIdStruct,set<LinguisticGraphVertex> >::iterator startPointsItr;
    LinguisticGraphVertexIt it,itEnd;
    boost::tie(it,itEnd) = vertices(*graph);
    for (;it!=itEnd;it++)
    {
        LinguisticGraphVertex vertex = *it;
        if ( in_degree(vertex,*graph)==0 && out_degree(vertex,*graph)==0 )
          continue;
        set< ChainIdStruct > chains=get(vertex_chain_id, *graph, vertex);
        for (set<ChainIdStruct>::iterator chainItr=chains.begin();
                chainItr!=chains.end();
                chainItr++)
        {
            ChainIdStruct chain = *chainItr;
            if ( chain.chainType() != Lima::Common::MediaticData::NO_CHAIN_TYPE )
            {
                startPointsItr=startPoints.find(chain);
                if (startPointsItr==startPoints.end())
                {
                    bool success=false;
                    boost::tie(startPointsItr,success) =
                        startPoints.insert(make_pair(chain,set<LinguisticGraphVertex>()));
                }
                (*startPointsItr).second.insert(vertex);
            }
        }
    }

    LinguisticGraphEdgeIt ite, iteEnd;
    boost::tie(ite, iteEnd) = edges(*graph);
    for (;ite!=iteEnd;ite++)
    {
        LinguisticGraphEdge edge = *ite;
        set< ChainIdStruct > chains=get(edge_chain_id,*graph,edge );
        for (set<ChainIdStruct>::iterator chainItr=chains.begin();
                chainItr!=chains.end();
                chainItr++)
        {
            ChainIdStruct chain = *chainItr;
            if ( chain.chainType() != 0 )
            {
                startPointsItr=startPoints.find(chain);
                if (startPointsItr==startPoints.end())
                {
                    bool success=false;
                    boost::tie(startPointsItr,success) =
                        startPoints.insert(make_pair(chain,set<LinguisticGraphVertex>()));
                }
                startPointsItr->second.erase(target(edge,*graph));
            }
        }
    }

    out << "found " << startPoints.size() << " chains." << endl;

    for (map<ChainIdStruct,set<LinguisticGraphVertex> >::iterator chainItr=startPoints.begin();
            chainItr!=startPoints.end();
            chainItr++)
    {
        ChainIdStruct chain = chainItr->first;
        set<LinguisticGraphVertex>& chainVertices = chainItr->second;
        if (chainVertices.size() > 0)
        {
            LinguisticGraphVertex start=*(chainVertices.begin());
            displayChain(chain,start,graph,out);
        }
    }

*/
    out << "end display chains" << endl;

}

void SyntacticAnalysisTools::displayChain(
        const ChainIdStruct& chain,
        LinguisticGraphVertex start,
        const AnalysisGraph* anagraph,
        std::ostream& out)
{
    out << "Chain " << chain << "\t";
    auto graph = anagraph->getGraph();
    auto currentVx = start;
    LinguisticGraphOutEdgeIt it,itEnd;
    auto lastVx = anagraph->lastVertex();
    while (currentVx!=lastVx)
    {
        MorphoSyntacticData* data=get(vertex_data,*graph,currentVx);
        Token* token=get(vertex_token,*graph,currentVx);
        if ( (token != 0) && (data != 0) && (!data->empty()) )
            out << token->stringForm().toStdString()
                << "[" << currentVx << "]{"
                << data->begin()->properties << "} ";
        else
            out << token->stringForm().toStdString()
                << "[" << currentVx << "]{NA} ";
        boost::tie(it, itEnd) = out_edges(currentVx,*graph);
        currentVx = lastVx;
        for (;it!=itEnd;it++)
        {
            auto chains = get(vertex_chain_id, *graph, target(*it, *graph));
            if (chains.find(chain) != chains.end())
            {
                currentVx = target(*it,*graph);
                break;
            }
        }
    }
    out << endl;
}

void SyntacticAnalysisTools::
displayDependancies(const SyntacticData& data,
                    const MediaId language,
                    std::ostream& out,
                    const bool DisplayLemma)
{

    out << "start display dependency graph : " << endl;

    const DependencyGraph* depGraph=data.dependencyGraph();
    const LinguisticGraph* linguisticGraph=data.graph();

    DependencyGraphEdgeIt it,itEnd;
    boost::tie(it,itEnd)=edges(*depGraph);
    for (;it!=itEnd;it++)
    {

//         const ChainIdStruct chain= get(edge_depchain_id,*depGraph,*it);
//         if (chain.chainId() != ChainIdStruct::noid) {
//           out << "chain " << chain.chainId();
//         }

        const DepRelTypeProp relType=get(edge_deprel_type,*depGraph,*it);
        out << " " << static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relType) << " :\t";

        DependencyGraphVertex src = source(*it,*depGraph);
        DependencyGraphVertex targ = target(*it,*depGraph);
        LinguisticGraphVertex s=data.tokenVertexForDepVertex(src);
        MorphoSyntacticData* currentData=get(vertex_data,*linguisticGraph,s);
        Token* token=get(vertex_token,*linguisticGraph,s);
        if ( (token != 0) && (currentData != 0) && (!currentData->empty()) )
    {
    if (DisplayLemma == true)
      out << Common::MediaticData::MediaticData::single().stringsPool(language)[currentData->front().lemma].toStdString()
          << "[" << src << "|" << s << "]{"
          << currentData->begin()->properties << "} ";
    else
    //    out << currentData->front().lemma
            out << token->stringForm().toStdString()
                << "[" << src << "|" << s << "]{"
                << currentData->begin()->properties << "} ";
    }
        else
            out << token->stringForm().toStdString()
                << "[" << src << "|" << s << "]{NA} ";
        LinguisticGraphVertex t=data.tokenVertexForDepVertex(targ);
        currentData=get(vertex_data,*linguisticGraph,t);
        token=get(vertex_token,*linguisticGraph,t);
        if ( (token != 0) && (currentData != 0) && (!currentData->empty()) )
        {
          if (DisplayLemma == true)
            out << Common::MediaticData::MediaticData::single().stringsPool(language)[currentData->front().lemma].toStdString()
                << "[" << targ << "|" << t << "]{"
    << currentData->begin()->properties << "} ";
          else
    //      out << currentData->front().lemma
            out << token->stringForm().toStdString()
                << "[" << targ << "|" << t << "]{"
                << currentData->begin()->properties << "} ";
        }
        else
            out << token->stringForm().toStdString()
                << "[" << targ << "|" << t << "]{NA} ";
      out << std::endl;
    }

    out << "end display dependency graph" << endl;

}

void SyntacticAnalysisTools::
displayRelationsOrderOfArguments(const SyntacticData& data,
                                 const MediaId language,
                                 std::ostream& out)
{

  const DependencyGraph* depGraph=data.dependencyGraph();
  const LinguisticGraph* linguisticGraph=data.graph();

  DependencyGraphEdgeIt it,itEnd;
  boost::tie(it,itEnd)=edges(*depGraph);
  for (;it!=itEnd;it++) {

    const DepRelTypeProp relType=get(edge_deprel_type,*depGraph,*it);
    out << static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relType) << " ";

    LinguisticGraphVertex srcVertex=
      data.tokenVertexForDepVertex(source(*it,*depGraph));
    LinguisticGraphVertex tgtVertex=
      data.tokenVertexForDepVertex(target(*it,*depGraph));

    MorphoSyntacticData* srcData=get(vertex_data,*linguisticGraph,srcVertex);
    MorphoSyntacticData* tgtData=get(vertex_data,*linguisticGraph,tgtVertex);

    Token* srcToken = get(vertex_token,*linguisticGraph,srcVertex);
    Token* tgtToken = get(vertex_token,*linguisticGraph,tgtVertex);

    if (srcData==0 || tgtData==0 || srcToken==0 || tgtToken==0) {
      continue;
    }

    uint64_t srcPosition=srcToken->position();
    uint64_t tgtPosition=tgtToken->position();

    string srcMacroCat,tgtMacroCat,srcMicroCat,tgtMicroCat;
    try {
      const Common::PropertyCode::PropertyCodeManager& pcm=
        static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager();
      const Common::PropertyCode::PropertyManager& macroManager=pcm.getPropertyManager("MACRO");
      const Common::PropertyCode::PropertyManager& microManager=pcm.getPropertyManager("MICRO");
      srcMacroCat=macroManager.getPropertySymbolicValue(srcData->firstValue(macroManager.getPropertyAccessor()));
      srcMicroCat=microManager.getPropertySymbolicValue(srcData->firstValue(microManager.getPropertyAccessor()));
      tgtMacroCat=macroManager.getPropertySymbolicValue(tgtData->firstValue(macroManager.getPropertyAccessor()));
      tgtMicroCat=microManager.getPropertySymbolicValue(tgtData->firstValue(microManager.getPropertyAccessor()));
    }
    catch (LimaException& e) {
      CORECLIENTLOGINIT;
      LERROR << e.what();
      continue;
    }

    if (srcPosition>tgtPosition) {
      // target of relation before source of relation in text
      out << "TARGET:" << tgtMacroCat << "/" << tgtMicroCat << " "
          << "SOURCE:" << srcMacroCat << "/" << srcMicroCat << endl;
    }
    else {
      // source of relation before target of relation in text
      out << "SOURCE:" << srcMacroCat << "/" << srcMicroCat << " "
          << "TARGET:" << tgtMacroCat << "/" << tgtMicroCat << endl;
    }
  }
}

void SyntacticAnalysisTools::
displayRelationsDistanceOfArguments(const SyntacticData& data,
                                    const MediaId language,
                                    std::ostream& out)
{
  const DependencyGraph* depGraph=data.dependencyGraph();
  const LinguisticGraph* linguisticGraph=data.graph();

  DependencyGraphEdgeIt it,itEnd;
  boost::tie(it,itEnd)=edges(*depGraph);
  for (;it!=itEnd;it++) {

    const DepRelTypeProp relType=get(edge_deprel_type,*depGraph,*it);
    out << static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relType) << " ";

    // output macro/micro of source and target
    LinguisticGraphVertex srcVertex=
      data.tokenVertexForDepVertex(source(*it,*depGraph));
    LinguisticGraphVertex tgtVertex=
      data.tokenVertexForDepVertex(target(*it,*depGraph));

    MorphoSyntacticData* srcData=get(vertex_data,*linguisticGraph,srcVertex);
    MorphoSyntacticData* tgtData=get(vertex_data,*linguisticGraph,tgtVertex);

    string srcMacroCat,tgtMacroCat,srcMicroCat,tgtMicroCat;
    try {
      const Common::PropertyCode::PropertyCodeManager& pcm=
        static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager();
      const Common::PropertyCode::PropertyManager& macroManager=pcm.getPropertyManager("MACRO");
      const Common::PropertyCode::PropertyManager& microManager=pcm.getPropertyManager("MICRO");
      srcMacroCat=macroManager.getPropertySymbolicValue(srcData->firstValue(macroManager.getPropertyAccessor()));
      srcMicroCat=microManager.getPropertySymbolicValue(srcData->firstValue(microManager.getPropertyAccessor()));
      tgtMacroCat=macroManager.getPropertySymbolicValue(tgtData->firstValue(macroManager.getPropertyAccessor()));
      tgtMicroCat=microManager.getPropertySymbolicValue(tgtData->firstValue(microManager.getPropertyAccessor()));
    }
    catch (LimaException& e) {
      CORECLIENTLOGINIT;
      LERROR << e.what();
      continue;
    }

    out << srcMacroCat << "/" << srcMicroCat << " "
        << tgtMacroCat << "/" << tgtMicroCat << " ";

    // output distance between source and target
    Token* srcToken = get(vertex_token,*linguisticGraph,srcVertex);
    Token* tgtToken = get(vertex_token,*linguisticGraph,tgtVertex);
    uint64_t srcPosition=srcToken->position();
    uint64_t tgtPosition=tgtToken->position();
    uint64_t srcLength=srcToken->length();
    uint64_t tgtLength=tgtToken->length();

    if (srcPosition>tgtPosition+tgtLength) {
      // target of relation before source of relation in text
      out << srcPosition-(tgtPosition+tgtLength);
    }
    else if (tgtPosition>srcPosition+srcLength) {
      // source of relation before target of relation in text
      out << tgtPosition-(srcPosition+srcLength);
    }
    else {
      // can be equal (l', n'...) or
      // negative (aux -> length of auxiliary has changed)
      out << 0;
    }
    out << endl;
  }
}
// ajout Ludovic
void SyntacticAnalysisTools::displayRelationsXMLFormat(const SyntacticData& data,
                                    const MediaId language,
                                    std::ostream& xmlStream)
{
  const DependencyGraph* depGraph=data.dependencyGraph();
  const LinguisticGraph* linguisticGraph=data.graph();

  xmlStream << "<syntactic_relations>" << std::endl;

  DependencyGraphEdgeIt it,itEnd;
  boost::tie(it,itEnd)=edges(*depGraph);
  for (;it!=itEnd;it++) {

    const DepRelTypeProp relType=get(edge_deprel_type,*depGraph,*it);
//     xmlStream << LinguisticData::single().languageData(language).getSyntacticRelationName(relType) << " ";

    //string relLabel = LinguisticData::single().languageData(language).getSyntacticRelationName(relType);
    string relLabel = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getSyntacticRelationName(relType);

    // output macro/micro of source and target
    LinguisticGraphVertex srcVertex=
      data.tokenVertexForDepVertex(source(*it,*depGraph));
    LinguisticGraphVertex tgtVertex=
      data.tokenVertexForDepVertex(target(*it,*depGraph));

    MorphoSyntacticData* srcData=get(vertex_data,*linguisticGraph,srcVertex);
    MorphoSyntacticData* tgtData=get(vertex_data,*linguisticGraph,tgtVertex);

    string srcMacroCat,tgtMacroCat,srcMicroCat,tgtMicroCat;
    try {
      const Common::PropertyCode::PropertyCodeManager& pcm= static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager();
      const Common::PropertyCode::PropertyManager& macroManager=pcm.getPropertyManager("MACRO");
      const Common::PropertyCode::PropertyManager& microManager=pcm.getPropertyManager("MICRO");
      srcMacroCat=macroManager.getPropertySymbolicValue(srcData->firstValue(macroManager.getPropertyAccessor()));
      srcMicroCat=microManager.getPropertySymbolicValue(srcData->firstValue(microManager.getPropertyAccessor()));
      tgtMacroCat=macroManager.getPropertySymbolicValue(tgtData->firstValue(macroManager.getPropertyAccessor()));
      tgtMicroCat=microManager.getPropertySymbolicValue(tgtData->firstValue(microManager.getPropertyAccessor()));
    }
    catch (LimaException& e) {
      CORECLIENTLOGINIT;
      LERROR << e.what();
      continue;
    }

    // output distance between source and target
    Token* srcToken = get(vertex_token,*linguisticGraph,srcVertex);
    Token* tgtToken = get(vertex_token,*linguisticGraph,tgtVertex);
    uint64_t srcPosition=srcToken->position();
    uint64_t tgtPosition=tgtToken->position();
    uint64_t srcLength=srcToken->length();
    uint64_t tgtLength=tgtToken->length();

    string scrString=limastring2utf8stdstring(srcToken->stringForm());
    string tgtString=Lima::Common::Misc::limastring2utf8stdstring(tgtToken->stringForm());
    uint64_t pathDistance=0;

    if (srcPosition>tgtPosition+tgtLength) {
      // target of relation before source of relation in text
      pathDistance = srcPosition-(tgtPosition+tgtLength);
    }
    else if (tgtPosition>srcPosition+srcLength) {
      // source of relation before target of relation in text
      pathDistance = tgtPosition-(srcPosition+srcLength);
    }
    else {
      // can be equal (l', n'...) or
      // negative (aux -> length of auxiliary has changed)
      pathDistance = 0;
    }
    xmlStream << "<syntactic_relation label=\"" << relLabel << "\"" << " dist=\"" <<  pathDistance<< "\">"
  << "<source position=\"" << srcPosition
  << "\" length=\"" << srcLength
  << "\" string=\"" << scrString
  << "\" pos_cat=\"" << srcMicroCat
  << "\"/>"
  << "<target position=\"" << tgtPosition
  << "\" length=\"" << tgtLength
  << "\" string=\"" << tgtString
  << "\" pos_cat=\"" << tgtMicroCat
  << "\"/>"
  << "</syntactic_relation>" << endl;
  }
  xmlStream << "</syntactic_relations>" << std::endl;
}

} // end namespace
} // end namespace
} // end namespace
