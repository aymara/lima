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
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/MorphologicAnalysis/IdiomaticExpressionAnnotation.h"
#include "IdiomaticAlternativesConstraints.h"

#include <queue>

using namespace std;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::ApplyRecognizer;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

// factories for constraint functions defined in this file
  ConstraintFunctionFactory<CreateIdiomaticAlternative>
      CreateIdiomaticAlternativeFactory(CreateIdiomaticAlternativeId);


CreateIdiomaticAlternative::CreateIdiomaticAlternative(
    MediaId language,
    const LimaString& complement):
  CreateAlternative(language,complement)
{
//   MORPHOLOGINIT;
//   LDEBUG << "CreateIdiomaticAlternative constructor with complement: " <<  complement;

}


void CreateIdiomaticAlternative::removeEdges(
    LinguisticGraph& graph, 
    const RecognizerMatch& match,
    AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
   MORPHOLOGINIT;
   LDEBUG << "IdiomaticAlternatives: removing edges";
#endif
/*  if (first == last)
  {
    LDEBUG << "  first and last are equal => ignoring";
    return;
  }*/
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  

  std::set< LinguisticGraphVertex > matchVertices;
  Automaton::RecognizerMatch::const_iterator matchIt, matchIt_end;

  // noeuds eventuellement pendants a verifier
  std::set< LinguisticGraphVertex > verticesToCheck;
  
  matchIt = match.begin();
  matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    matchVertices.insert((*matchIt).m_elem.first);
  }
  
  matchIt = match.begin();
  matchIt_end = match.end();
  // parcours des noeuds du match
  for (; matchIt != matchIt_end; matchIt++)
  {
    LinguisticGraphOutEdgeIt outIt, outIt_end;
    boost::tie (outIt, outIt_end) = boost::out_edges((*matchIt).m_elem.first, graph);
    // pour chaque arc sortant du noeud
    for (; outIt != outIt_end; outIt++)
    {
      // l'arc n'est pas membre du match
      if (matchVertices.find(target(*outIt, graph)) != matchVertices.end())
      {
        // on le stocke comme a supprimer
        recoData->setEdgeToBeRemoved(analysis, *outIt);
        // il va falloir verifier si le noeud cible de l'arc ne se retrouve pas pendant quand on aura supprime l'arc
        verticesToCheck.insert(target(*outIt, graph));
      }
    }

    LinguisticGraphInEdgeIt inIt, inIt_end;
    boost::tie (inIt, inIt_end) = boost::in_edges((*matchIt).m_elem.first, graph);
    // pour chaque arc entrant du noeud
    for (; inIt != inIt_end; inIt++)
    {
      // l'arc n'est pas membre du match
      if (matchVertices.find(source(*inIt, graph)) != matchVertices.end())
      {
        // on le stocke comme a supprimer
        recoData->setEdgeToBeRemoved(analysis, *inIt);
        // il va falloir verifier si le noeud source de l'arc ne se retrouve pas pendant quand on aura supprime l'arc
        verticesToCheck.insert(source(*inIt, graph));
      }
    }
  }
  // suppression des arcs stockes comme a supprimer
  recoData->removeEdges( analysis );

  //verification des noeuds extremites des arcs supprimes
  std::set< LinguisticGraphVertex >::const_iterator verticesToCheckIt, verticesToCheckIt_end;
  verticesToCheckIt = verticesToCheck.begin();
  verticesToCheckIt_end = verticesToCheck.end();
  for (;verticesToCheckIt != verticesToCheckIt_end; verticesToCheckIt++)
  {
    recoData->clearUnreachableVertices(analysis, *verticesToCheckIt);
  }

}


bool CreateIdiomaticAlternative::operator()(Automaton::RecognizerMatch& result,
                                            AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
    MORPHOLOGINIT;
    LDEBUG << "CreateIdiomaticAlternative, match is " << result;
    LDEBUG << "    expression is " << (result.isContiguous()?"":"non") <<
     " contiguous and" << (result.isContextual()?" non":"") << " absolute";
#endif
  if (result.empty()) return false;
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *(result.getGraph());
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData->dumpFunction("IdiomExpr") == 0)
  {
    annotationData->dumpFunction("IdiomExpr", new DumpIdiomaticExpressionAnnotation());
  }
  
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  
  std::set<LinguisticGraphVertex> addedVertices;
  // initialize the vertices to clear

  if (result.isContiguous())
  {
//     MORPHOLOGINIT;
//      LDEBUG << "contiguous idiomatic expression found: "
//          << result.concatString();

    // only one part : terms in expression are adjacent -> easy part

    // check if there is an overlap first
    if (recoData->matchOnRemovedVertices(result))
    {
      // ignore current idiomatic expression, continue
      MORPHOLOGINIT;
    LWARN << "idiomatic expression ignored: " << Common::Misc::limastring2utf8stdstring(result.concatString())
          << ": overlapping with a previous one";
      return false;
    }

    // create the new token
    std::pair<Token*,MorphoSyntacticData*> newToken = createAlternativeToken(result);

    // add the vertex
    LinguisticGraphVertex idiomaticVertex =
        addAlternativeVertex(newToken.first, newToken.second, const_cast<LinguisticGraph*>(graph.getGraph()));
    AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
    annotationData->addMatching("AnalysisGraph", idiomaticVertex, "annot", agv);
    annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("AnalysisGraph"), idiomaticVertex);
    IdiomaticExpressionAnnotation annot(result);
    GenericAnnotation ga(annot);
    annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("IdiomExpr"), ga);

    addedVertices.insert(idiomaticVertex);

    //create the alternative with this only vertex
    createBeginAlternative(result.front().getVertex(),
                            idiomaticVertex,const_cast<LinguisticGraph&>(*graph.getGraph()));
    attachEndOfAlternative(idiomaticVertex,
                            result.back().getVertex(),const_cast<LinguisticGraph&>(*graph.getGraph()));

    // if expression is not contextual, only keep alternative
    if (! result.isContextual())
    {
      recoData->storeVerticesToRemove(result,const_cast<LinguisticGraph*>(graph.getGraph()));
      removeEdges(const_cast<LinguisticGraph&>(*graph.getGraph()),
                 result, analysis);
      //recoData->setNextVertex(idiomaticVertex);
      // if match was on single token, use next vertices (to avoid loops)
      if (result.size() > 1) 
      {
        recoData->setNextVertex(idiomaticVertex);
      }
      else 
      {
        LinguisticGraphOutEdgeIt outItr,outItrEnd;
        boost::tie(outItr,outItrEnd) = out_edges(idiomaticVertex,*(graph.getGraph()));
        for (;outItr!=outItrEnd;outItr++) 
        {
          recoData->setNextVertex(target(*outItr, *(graph.getGraph())));
        }
      }
    }
  }
  else
  {
    // several parts : tough case
//     MORPHOLOGINIT;
//      LDEBUG << "non contiguous idiomatic expression found: "
//          << result.concatString();

    // check if there is an overlap first
    if (recoData->matchOnRemovedVertices(result))
    {
      // ignore current idiomatic expression, continue
      MORPHOLOGINIT;
    LWARN << "idiomatic expression ignored: " << Common::Misc::limastring2utf8stdstring(result.concatString())
          << ": overlapping with a previous one";
      return false;
    }

    // create the new token
    pair<Token*,MorphoSyntacticData*> newToken = createAlternativeToken(result);

    // add the vertex
    LinguisticGraphVertex idiomaticVertex =
        addAlternativeVertex(newToken.first,newToken.second,const_cast<LinguisticGraph*>(graph.getGraph()));
    addedVertices.insert(idiomaticVertex);
    AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
    annotationData->addMatching("AnalysisGraph", idiomaticVertex, "annot", agv);
    annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("AnalysisGraph"), idiomaticVertex);
    IdiomaticExpressionAnnotation annot(result);
    GenericAnnotation ga(annot);
    annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("IdiomExpr"), ga);

    //create the alternative with this vertex and duplicate of other vertices
    deque<LinguisticGraphVertex> idiomAlternative;
    LinguisticGraphVertex headVertex=result.getHead();
#ifdef DEBUG_LP
   LDEBUG << "headVertex = " << headVertex;
    if (headVertex!=0) 
    {
      LDEBUG << "=> " << Common::Misc::limastring2utf8stdstring(get(vertex_token,*graph.getGraph(),headVertex)->stringForm());
    }
#endif
    bool foundHead=false;
    bool keeping = false;
    std::pair< LinguisticGraphVertex, LinguisticGraphVertex > idiomPartBounds;
    std::set< std::pair< LinguisticGraphVertex, LinguisticGraphVertex > > edgesToRemove;
    RecognizerMatch::const_iterator matchItr=result.begin();
    for (; matchItr!=result.end(); matchItr++)
    {
      if (!matchItr->isKept())
      {
        if (keeping)
        {
          RecognizerMatch::const_iterator prevItr = matchItr - 1;
          idiomPartBounds.second = prevItr->getVertex();
          keeping = false;
#ifdef DEBUG_LP
          LDEBUG << "adding " << idiomPartBounds.first << " -> " << idiomPartBounds.second << " in edgesToRemove";
#endif
          edgesToRemove.insert(idiomPartBounds);
        }
        // duplicate this vertex
#ifdef DEBUG_LP
        LDEBUG << "duplication of vertex " << matchItr->getVertex();;
#endif
        Token* token=get(vertex_token,*graph.getGraph(),matchItr->getVertex());
        MorphoSyntacticData* data = 
          new MorphoSyntacticData(*get(vertex_data,*graph.getGraph(),matchItr->getVertex()));
        LinguisticGraphVertex dupVx = add_vertex(const_cast<LinguisticGraph&>(*graph.getGraph()));
        put(vertex_token,const_cast<LinguisticGraph&>(*graph.getGraph()),dupVx,token);
        put(vertex_data,const_cast<LinguisticGraph&>(*graph.getGraph()),dupVx,data);
        idiomAlternative.push_back(dupVx);
        AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
        annotationData->addMatching("AnalysisGraph", dupVx, "annot", agv);
        annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("AnalysisGraph"), dupVx);
        std::set< LinguisticGraphVertex > annotMatches = 
          annotationData->matches("AnalysisGraph",matchItr->getVertex(),"annot");
        for (std::set< LinguisticGraphVertex >::const_iterator annotIt(annotMatches.begin());
              annotIt != annotMatches.end(); annotIt++)
        {
          std::set< std::string > excepted;
          excepted.insert("AnalysisGraph");
          annotationData->cloneAnnotations(*annotIt, agv, excepted);
        }
        addedVertices.insert(dupVx);
//         verticesToRemove.insert(matchItr->getVertex());
      }
      else
      {
        if (!keeping)
        {
          idiomPartBounds.first = matchItr->getVertex();
          keeping = true;
        }
#ifdef DEBUG_LP
         LDEBUG << "kept vertex " << matchItr->getVertex();
#endif
        if (matchItr->getVertex()==headVertex)
        {
          foundHead=true;
#ifdef DEBUG_LP
           LDEBUG << "add head vertex " << idiomaticVertex;
#endif
          idiomAlternative.push_back(idiomaticVertex);
        }
      }
    }
    if (!foundHead) 
    {
      MORPHOLOGINIT;
      LWARN << "head token has not been found in non contiguous expression. "
          << "Idiomatic token is placed first";
      idiomAlternative.push_front(idiomaticVertex);
    }
    if (keeping)
    {
      RecognizerMatch::const_iterator prevItr = matchItr - 1;
      idiomPartBounds.second = prevItr->getVertex();
      keeping = false;
#ifdef DEBUG_LP
      LDEBUG << "adding " << idiomPartBounds.first << " -> " << idiomPartBounds.second << " in edgesToRemove";
#endif
      edgesToRemove.insert(idiomPartBounds);
    }

    // link alternatives
#ifdef DEBUG_LP
     LDEBUG << "idiomAlternative has " << idiomAlternative.size() << " vertex";
#endif
    createBeginAlternative(result.front().getVertex(),
                            idiomAlternative.front(),const_cast<LinguisticGraph&>(*graph.getGraph()));
    {
      deque<LinguisticGraphVertex>::const_iterator idItr=idiomAlternative.begin();
      LinguisticGraphVertex lastIdiomVx=*idItr;
      idItr++;
      while (idItr!=idiomAlternative.end())
      {
        LinguisticGraphEdge newEdge;
        bool ok;
        boost::tie(newEdge, ok) = add_edge(lastIdiomVx,*idItr,const_cast<LinguisticGraph&>(*graph.getGraph()));
#ifdef DEBUG_LP
         LDEBUG << "added new edge in alternatives linking: " << newEdge.m_source << " -> " << newEdge.m_target;
#endif
        lastIdiomVx=*idItr;
        idItr++;
      }
    }
    attachEndOfAlternative(idiomAlternative.back(),
                            result.back().getVertex(),const_cast<LinguisticGraph&>(*graph.getGraph()));

    // if expression is not contextual, only keep alternative
    if (! result.isContextual())
    {
#ifdef DEBUG_LP
      LDEBUG << "expression is not contextual, only keep alternative";
#endif
      std::set< std::pair< LinguisticGraphVertex, LinguisticGraphVertex > >::const_iterator edgesToRemoveIt, edgesToRemoveIt_end;
      edgesToRemoveIt = edgesToRemove.begin(); edgesToRemoveIt_end = edgesToRemove.end();
      for (; edgesToRemoveIt != edgesToRemoveIt_end; edgesToRemoveIt++)
      {
#ifdef DEBUG_LP
         LDEBUG << "Removing edge " << (*edgesToRemoveIt).first << " -> " << (*edgesToRemoveIt).second;
#endif
        removeEdges(const_cast<LinguisticGraph&>(*graph.getGraph()),
                   result, analysis);
      }

//       recoData->storeVerticesToRemove(result,*graph);
      // no need to check size: if several parts, more than one vertex
      recoData->setNextVertex(idiomaticVertex);
      
    }
  }
  RecognizerMatch::const_iterator matchItr=result.begin();
  for (; matchItr!=result.end(); matchItr++)
  {
    recoData->clearUnreachableVertices( analysis, (*matchItr).getVertex());
  }
//   recoData->clearUnreachableVertices( analysis, result.front().getVertex(), result.back().getVertex(), storedEdges);
  return true;
}


} // MorphologicAnalysis

} // LinguisticProcessing

} // Lima
