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
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "SpecificEntitiesConstraints.h"
#include "SpecificEntitiesMicros.h"
#include <QStringList>
#include <queue>
#include <iostream>
#include <sstream>

// #include <assert.h>

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

namespace SpecificEntities
{

// factories for constraint functions defined in this file
ConstraintFunctionFactory<isAlphaPossessive>
  isAlphaPossessiveFactory(isAlphaPossessiveId);

ConstraintFunctionFactory<isASpecificEntity>
  isASpecificEntityFactory(isASpecificEntityId);

ConstraintFunctionFactory<CreateSpecificEntity>
  CreateSpecificEntityFactory(CreateSpecificEntityId);

ConstraintFunctionFactory<SetEntityFeature>
  SetEntityFeatureFactory(SetEntityFeatureId);

ConstraintFunctionFactory<AddEntityFeatureAsEntity>
  AddEntityFeatureAsEntityFactory(AddEntityFeatureAsEntityId);

ConstraintFunctionFactory<AddEntityFeature>
  AddEntityFeatureFactory(AddEntityFeatureId);

ConstraintFunctionFactory<AppendEntityFeature>
  AppendEntityFeatureFactory(AppendEntityFeatureId);

ConstraintFunctionFactory<ClearEntityFeatures>
  ClearEntityFeaturesFactory(ClearEntityFeaturesId);

ConstraintFunctionFactory<NormalizeEntity>
  NormalizeEntityFactory(NormalizeEntityId);


isAlphaPossessive::
isAlphaPossessive(MediaId language,
                  const LimaString& complement):
ConstraintFunction(language,complement)
{
}

bool isAlphaPossessive::operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
                                   const LinguisticGraphVertex& v,
                                   AnalysisContent& /*analysis*/) const
{
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
//  Token* token=get(vertex_token,*(graph.getGraph()),v);
  VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);
  const TStatus& status = tokenMap[v]->status();
  return( status.isAlphaPossessive() );
}


isASpecificEntity::
isASpecificEntity(MediaId language,
                  const LimaString& complement):
ConstraintFunction(language,complement),
m_type()
{
  if (! complement.isEmpty()) {
    m_type=Common::MediaticData::MediaticData::single().getEntityType(complement);
  }
}

bool isASpecificEntity::operator()(const LinguisticAnalysisStructure::AnalysisGraph& /*graph*/,
                                   const LinguisticGraphVertex& v,
                                   AnalysisContent& analysis) const
{
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData == 0)
  {
    return false;
  }
  bool annotFound = false;
  //std::set< uint64_t > matches = annotationData->matches(recoData->getGraphId(),v,"annot"); portage 32 64
  //for (std::set< uint64_t >::const_iterator it = matches.begin(); portage 32 64
  std::set< AnnotationGraphVertex > matches = annotationData->matches(recoData->getGraphId(),v,"annot");
  for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
       it != matches.end(); it++)
  {
    if (annotationData->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    {
      annotFound = true;
      break;
    }
  }
  
  if (!annotFound)
  {
    return false;
  }

  if (m_type == Common::MediaticData::EntityType())
  {
    return true;
  }
  else
  {
    //for (std::set< uint64_t >::const_iterator it = matches.begin(); portage 32 64
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
         it != matches.end(); it++)
    {
      if (annotationData->annotation(*it,
                                     Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>()->getType() == m_type)
      {
        return true;
      }
    }
    return false;
  }
  return false;
}


CreateSpecificEntity::CreateSpecificEntity(MediaId language,
                       const LimaString& complement):
ConstraintFunction(language,complement),
m_language(language)
{
#ifdef DEBUG_LP
  SELOGINIT;
#endif
  m_sp=&(Common::MediaticData::MediaticData::changeable().stringsPool(language));

  LimaString str=complement; // copy for easier parse (modify)
  LimaString sep=Common::Misc::utf8stdstring2limastring(",");
  if (!str.isEmpty()) {
    LimaString typeName;
    int j=str.indexOf(sep);
    if (j!=-1) {
      typeName=str.left(j);
      str=str.mid(j+1);
    }
    else {
      typeName=complement;
      str.clear();
    }
#ifdef DEBUG_LP
    LDEBUG << "CreateSpecificEntity: getting entity type "
           <<  Common::Misc::limastring2utf8stdstring(typeName);
#endif
    m_type=Common::MediaticData::MediaticData::single().getEntityType(typeName);
  }

  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));

  const Common::PropertyCode::PropertyManager& microManager=
      static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().
getPropertyManager("MICRO");

  if (!str.isEmpty())
  {
    //uint64_t currentPos = 0; portage 32 64
    int currentPos = 0;
    while (currentPos != str.size()+1)
    {
      int sepPos = str.indexOf(sep, currentPos);

      if (sepPos == -1)
      {
        sepPos = str.size();
      }
      std::string smicro=Common::Misc::limastring2utf8stdstring(str.mid(currentPos, sepPos-currentPos));
      LinguisticCode micro = microManager.getPropertyValue(smicro);
      m_microsToKeep.insert(micro);
//       LDEBUG << "Added " << smicro << " / " << micro << " to micros to keep";
      currentPos = sepPos+1;
    }
  }
}
    
    
    
    
    
// CreateSpecificEntity::CreateSpecificEntity(MediaId language,
//                        const LimaString& complement):
// ConstraintFunction(language,complement),
// m_language(language)
// {
//   SELOGINIT;
//   m_sp=&(Common::MediaticData::MediaticData::changeable().stringsPool(language));
// 
//   std::string str=Common::Misc::limastring2utf8stdstring(complement);
//   LDEBUG << "CreateSpecificEntity constructor with complement: " <<  str;
//   if (! str.empty()) {
//     //uint64_t i=str.find("group:"); portage 32 64
//     std::string::size_type i=str.find("group:");
//     if (i!=std::string::npos) {
//       //uint64_t j=str.find(","); portage 32 64
//       std::string::size_type j=str.find(",");
//       entityGroup=std::string(str,i+6,j-i-6);
//       LDEBUG << "CreateSpecificEntity: use group " << entityGroup;
//       if (j==std::string::npos) {
//  str.clear();
//       }
//       else {
//         str=std::string(str,j+1);
//       }
//     }
//     else LDEBUG << "CreateSpecificEntity: no group specified";
//   }
//   if (!str.empty()) {
//     LimaString typeName;
//     uint64_t j=str.find(sep);
//     if (j!=std::string::npos) {
//       typeName=LimaString(str,0,j);
//       str=LimaString(str,j+1);
//     }
//     else {
//       typeName=complement;
//       str.clear();
//     }
//     LDEBUG << "CreateSpecificEntity: getting entity type " 
//            <<  Common::Misc::limastring2utf8stdstring(typeName);
//     m_type=Common::MediaticData::MediaticData::single().getEntityType(typeName);
//   }
// 
//   m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
// 
//   const Common::PropertyCode::PropertyManager& microManager=
//       static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyManager("MICRO");
// 
//   if (!str.empty())
//   {
//     //uint64_t currentPos = 0; portage 32 64
//     std::string::size_type currentPos = 0;
//     while (currentPos != str.size()+1)
//     {
//       std::string::size_type sepPos = str.find(sep, currentPos);
// 
//       if (sepPos == LimaString::npos)
//       {
//         sepPos = str.size();
//       }
//       std::string smicro=Common::Misc::limastring2utf8stdstring(LimaString(str, currentPos, sepPos-currentPos));
//       LinguisticCode micro = microManager.getPropertyValue(smicro);
//       m_microsToKeep.insert(micro);
// //       LDEBUG << "Added " << smicro << " / " << micro << " to micros to keep";
//       currentPos = sepPos+1;
//     }
//   }
// }


/** @todo Verifier la Completude du mapping entre graphes morpho, synt et d'annotation */
bool CreateSpecificEntity::operator()(Automaton::RecognizerMatch& match,
                                      AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
   SELOGINIT;
   LDEBUG << "CreateSpecificEntity: create entity of type " << match.getType() << " on vertices " << match;
#endif
  if (match.empty()) return false;
  LinguisticGraphVertex v1 = (*(match.begin())).m_elem.first;
  LinguisticGraphVertex v2 = (*(match.rbegin())).m_elem.first;
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *(match.getGraph());
  
//     LDEBUG << "CreateSpecificEntity action between " << v1 << " and " << v2
//         << " with complement " << m_complement;
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));

  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    return false;
  }
  // do not create annotation if annotation of same type exists
  if (match.size() == 1) 
  {
    //&& (isASpecificEntity(0,LimaString())(graph,v1,analysis)))
    //std::set< uint64_t > matches = annotationData->matches(graph.getGraphId(),v1,"annot"); portage 32 64
    //for (std::set< uint64_t >::const_iterator it = matches.begin(); portage 32 64
    std::set< AnnotationGraphVertex > matches = annotationData->matches(graph.getGraphId(),v1,"annot");
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
         it != matches.end(); it++) {
      if (annotationData->hasAnnotation(*it, Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          && annotationData->annotation(*it,
                                        Common::Misc::utf8stdstring2limastring("SpecificEntity"))
          .pointerValue<SpecificEntityAnnotation>()->getType() == match.getType() ) {
        return false;
      }
    }
  }

/*  {
    std::set< uint32_t > annots = annotationData->matches(recoData.getGraphId(), v1, "annot");
    for ( std::set< uint32_t >::iterator it = annots.begin(); it != annots.end(); it++)
    {
      if (annotationData->hasAnnotation( *it, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
    }
  }*/
  if (annotationData->dumpFunction("SpecificEntity") == 0)
  {
    annotationData->dumpFunction("SpecificEntity", new DumpSpecificEntityAnnotation());
  }

  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  std::string graphId=recoData->getGraphId();
    
//   LDEBUG << "    match is " << match;

//   LDEBUG << "    Creating annotation ";
  SpecificEntityAnnotation annot(match,*m_sp);
  std::ostringstream oss;
  annot.dump(oss);
#ifdef DEBUG_LP
  LDEBUG << "CreateSpecificEntity: annot =  " << oss.str();

//   LDEBUG << "    Building new morphologic data for head "<< annot.getHead();
#endif
  // getting data
  LinguisticGraph* lingGraph = const_cast<LinguisticGraph*>(graph.getGraph());
//   LDEBUG << "There is " << out_degree(v2, *lingGraph) << " edges out of " << v2;
  VertexTokenPropertyMap tokenMap = get(vertex_token, *lingGraph);
  VertexDataPropertyMap dataMap = get(vertex_data, *lingGraph);

  LinguisticGraphVertex head = annot.getHead();
  if( head == 0 ) {
    // take status of last element in match for eng
    head = v2;
    // or take status of first element in match (in fre?)
    // head = v1;
  }
  const MorphoSyntacticData* dataHead = dataMap[head];

  // Preparer le Token et le MorphoSyntacticData pour le nouveau noeud. Construits
  // a partir des infos de l'entitee nommee
  StringsPoolIndex seFlex = annot.getString();
  StringsPoolIndex seLemma = annot.getNormalizedString();
  StringsPoolIndex seNorm = annot.getNormalizedForm();

//   LDEBUG << "    Creating LinguisticElement";
//   LDEBUG << "    Creating MorphoSyntacticData";
  // creer un MorphoSyntacticData
  MorphoSyntacticData* newMorphData = new MorphoSyntacticData();

  // all linguisticElements of this morphosyntacticData share common SE information
  LinguisticElement elem;
  elem.inflectedForm = seFlex; // StringsPoolIndex
  elem.lemma = seLemma; // StringsPoolIndex
  elem.normalizedForm = seNorm; // StringsPoolIndex
  elem.type = SPECIFIC_ENTITY; // MorphoSyntacticType

  if (! m_microsToKeep.empty()) { 
#ifdef DEBUG_LP
    LDEBUG << "CreateSpecificEntity, use micros from the rule ";
#endif
    // micros are given in the rules
    addMicrosToMorphoSyntacticData(newMorphData,dataHead,m_microsToKeep,elem);
  }
  else {
#ifdef DEBUG_LP
    LDEBUG << "CreateSpecificEntity, use micros from config file ";
#endif
    // use micros given in the config file : get the specific resource
    // (specific to modex) 
    // WARN : some hard coded stuff here in resource names
    EntityType seType=match.getType();
    if  (seType.getGroupId() == 0)
    {
      SELOGINIT;
      LERROR << "CreateSpecificEntity::operator() null group id:" << seType;
      delete newMorphData;
      return false;
    }
    const LimaString& resourceName =
      Common::MediaticData::MediaticData::single().getEntityGroupName(seType.getGroupId())+"Micros";
    AbstractResource* res=LinguisticResources::single().getResource(m_language,resourceName.toUtf8().constData());
#ifdef DEBUG_LP
    LDEBUG << "Entities resource name is : " << resourceName;
#endif
    if (res!=0) {
      SpecificEntitiesMicros* entityMicros=static_cast<SpecificEntitiesMicros*>(res);
      const std::set<LinguisticCode>* micros=entityMicros->getMicros(seType);
#ifdef DEBUG_LP
      if (logger.isDebugEnabled()) 
      {
        std::ostringstream oss;
        for (std::set<LinguisticCode>::const_iterator it=micros->begin(),it_end=micros->end();it!=it_end;it++) {
          oss << (*it) << ";";
        }
        LDEBUG << "CreateSpecificEntity, micros are " << oss.str();
      }
#endif
      addMicrosToMorphoSyntacticData(newMorphData,dataHead,*micros,elem);
    }
    else {
      // cannot find micros for this type: error
      SELOGINIT;
      LERROR << "CreateSpecificEntity: missing resource " << resourceName ;
      delete newMorphData;
      return false;
    }
  }

  const FsaStringsPool& sp=*m_sp;
  Token* newToken = new Token(
      seFlex,
      sp[seFlex],
      match.positionBegin(),
      match.length());

  // take posessive tstatus from head
  TStatus tStatus(T_NULL_CAPITAL,T_NULL_ROMAN,false,false,false,false,T_NULL_NUM,T_NULL_STATUS);
  const TStatus& headTStatus = tokenMap[head]->status();
  if(headTStatus.isAlphaPossessive()) {
    tStatus.setAlphaPossessive(true);
  }
  newToken->setStatus(tokenMap[head]->status());

  if (newMorphData->empty())
  {
    SELOGINIT;
    LERROR << "CreateSpecificEntity::operator() Found no morphosyntactic  data for new vertex. Abort.";
    delete newToken;
    delete newMorphData;
    assert(false);
    return false;
  }
//   LDEBUG << "    Updating morphologic graph "<< graphId;
  // creer le noeud et ses 2 arcs
  LinguisticGraphVertex newVertex;
  DependencyGraphVertex newDepVertex = 0;
  if (syntacticData != 0)
  {
    boost::tie (newVertex, newDepVertex) = syntacticData->addVertex();
  }
  else
  {
    newVertex = add_vertex(*lingGraph);
  }
  AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
  annotationData->addMatching(graphId, newVertex, "annot", agv);
  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring(graphId), newVertex);
  tokenMap[newVertex] = newToken;
  dataMap[newVertex] = newMorphData;
#ifdef DEBUG_LP
  LDEBUG << "      - new vertex " << newVertex << "("<<graphId<<"), " << newDepVertex
      << "(dep), " << agv << "(annot) added";

//   LDEBUG << "    Setting annotation ";
#endif
  GenericAnnotation ga(annot);

  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("SpecificEntity"), ga);
//   LDEBUG << "    Creating SE annotation edges between SE match vertices "
//       "annotation and the new annotation vertex";
  Automaton::RecognizerMatch::const_iterator matchIt, matchIt_end;
  matchIt = match.begin(); matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    std::set< AnnotationGraphVertex > matches = annotationData->matches(graphId,(*matchIt).m_elem.first,"annot");
    if (matches.empty())
    {
      SELOGINIT;
      LERROR << "CreateSpecificEntity::operator() No annotation 'annot' for" << (*matchIt).m_elem.first;
    }
    else
    {
      if( recoData->hasVertexAsEmbededEntity((*matchIt).m_elem.first) )
      {
#ifdef DEBUG_LP
        LDEBUG << "CreateSpecificEntity::operator(): vertex " << *(matches.begin()) << " is embeded";
#endif
        AnnotationGraphVertex src = *(matches.begin());
        annotationData->annotate( agv, src, Common::Misc::utf8stdstring2limastring("holds"), 1);
      }
    }
  }

  // creer les relations necessaires dans le graphe morphosyntaxique
  // 1. entre les noeuds avant v1 et le nouveau noeud
  std::vector< LinguisticGraphVertex > previous;
  LinguisticGraphInEdgeIt firstInEdgesIt, firstInEdgesIt_end;
  boost::tie(firstInEdgesIt, firstInEdgesIt_end) = in_edges(v1, *lingGraph);
  std::set< std::pair<LinguisticGraphVertex,LinguisticGraphVertex > > newEdgesToRemove;
  for (; firstInEdgesIt != firstInEdgesIt_end; firstInEdgesIt++)
  {
    LinguisticGraphVertex firstInVertex = source(*firstInEdgesIt, *lingGraph);
    previous.push_back(firstInVertex);
    if (shouldRemoveInitial(source(*firstInEdgesIt, *lingGraph),target(*firstInEdgesIt, *lingGraph), match))
    {
#ifdef DEBUG_LP
      LDEBUG << "        - storing edge " << source(*firstInEdgesIt, *lingGraph) <<" -> "<<target(*firstInEdgesIt, *lingGraph) << " to be removed";
#endif
/*      recoData->setEdgeToBeRemoved(analysis, *firstInEdgesIt);*/
      newEdgesToRemove.insert(std::make_pair(source(*firstInEdgesIt, *lingGraph),target(*firstInEdgesIt, *lingGraph)));
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "        - do not store initial edge " << source(*firstInEdgesIt, *lingGraph) <<" -> "<<target(*firstInEdgesIt, *lingGraph) << " to be removed";
#endif
    }
  }
  std::vector< LinguisticGraphVertex >::iterator pit, pit_end;
  pit = previous.begin(); pit_end = previous.end();
  for (; pit != pit_end; pit++)
  {
    /* Si X-Y doit etre supprime et que Z remplace Y , alors ne pas creer X-Z 
    autrement dit si *pit-v1 est dans recoData->m_edgesToRemove, ne pas creer l'arc */
    if (!recoData->isEdgeToBeRemoved(*pit, v1))
    {
      bool success;
      LinguisticGraphEdge e;
      boost::tie(e, success) = add_edge(*pit, newVertex, *lingGraph);
      if (success)
      {
#ifdef DEBUG_LP
        LDEBUG << "        - in edge " << e.m_source << " -> " << e.m_target << " added";
#endif
      }
      else
      {
        SELOGINIT
        LERROR << "        - in edge " << *pit << " ->" << newVertex << " NOT added";
      }
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "        - edge " << *pit << " - " << newVertex << " not added because " << *pit << " - " << v1 << " has to be removed";
#endif
    }
  }
  std::set< std::pair<LinguisticGraphVertex,LinguisticGraphVertex > >::iterator newEdgesToRemoveIt = newEdgesToRemove.begin();
  for (; newEdgesToRemoveIt != newEdgesToRemove.end(); newEdgesToRemoveIt++)
  {
    recoData->setEdgeToBeRemoved(analysis, edge( newEdgesToRemoveIt->first, newEdgesToRemoveIt->second, *lingGraph).first);
  }
#ifdef DEBUG_LP
  LDEBUG << "      - in edges added";
#endif
  
  
  // 2. entre le nouveau noeud et les noeuds qui etaient apres v2
#ifdef DEBUG_LP
  LDEBUG << "        there is " << out_degree(v2, *lingGraph) << " edges out of " << v2;
#endif
  LinguisticGraphOutEdgeIt secondOutEdgesIt, secondOutEdgesIt_end;
  boost::tie(secondOutEdgesIt, secondOutEdgesIt_end) = out_edges(v2, *lingGraph);
  std::vector< LinguisticGraphVertex > nexts;
  for (; secondOutEdgesIt != secondOutEdgesIt_end; secondOutEdgesIt++)
  {
#ifdef DEBUG_LP
    LDEBUG << "        looking at edge " << source(*secondOutEdgesIt, *lingGraph) << " -> " << target(*secondOutEdgesIt, *lingGraph);
#endif
    LinguisticGraphVertex secondOutVertex = target(*secondOutEdgesIt, *lingGraph);
    if (secondOutVertex ==  v2) continue;
    nexts.push_back(secondOutVertex);
    if (shouldRemoveFinal(source(*secondOutEdgesIt, *lingGraph),target(*secondOutEdgesIt, *lingGraph), match))
    {
#ifdef DEBUG_LP
      LDEBUG << "        - storing edge " << source(*secondOutEdgesIt, *lingGraph) << " -> " << target(*secondOutEdgesIt, *lingGraph) << " to be removed";
#endif
      recoData->setEdgeToBeRemoved(analysis, *secondOutEdgesIt);
    }
    else
    {
#ifdef DEBUG_LP
      LDEBUG << "        - do not store final edge " << source(*secondOutEdgesIt, *lingGraph) << " -> " << target(*secondOutEdgesIt, *lingGraph) << " to be removed";
#endif
    }
  }
  std::vector< LinguisticGraphVertex >::iterator nit, nit_end;
  nit = nexts.begin(); nit_end = nexts.end();
  for (; nit != nit_end; nit++)
  {
    bool success;
    LinguisticGraphEdge e;
    boost::tie(e, success) = add_edge(newVertex, *nit, *lingGraph);
    if (success)
    {
#ifdef DEBUG_LP
      LDEBUG << "        - out edge " << e.m_source << " -> " << e.m_target << " added";
#endif
    }
    else
    {
      SELOGINIT;
      LERROR << "        - out edge " << newVertex << " ->" << *nit << " NOT added";
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "      - out edges added";
#endif

  // 3. supprimer les arcs a remplacer
  recoData->removeEdges( analysis );
  
  // 4 specifier le noeud suivant a utiliser dans la recherche :
  // - nouveau noeud si l'expression reconnue etait composee de plusieurs noeuds
  // - les fils du nouveau noeud sinon (pour eviter les bouclages)
  if (annot.m_vertices.size() > 1)
  {
    recoData->setNextVertex(newVertex);
  }
  else
  {
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    boost::tie(outItr,outItrEnd) = out_edges(newVertex,*lingGraph);
    for (;outItr!=outItrEnd;outItr++)
    {
      recoData->setNextVertex(target(*outItr, *lingGraph));
    }
  }
  RecognizerMatch::const_iterator matchItr=match.begin();
  for (; matchItr!=match.end(); matchItr++)
  {
    recoData->clearUnreachableVertices( analysis, (*matchItr).getVertex());
  }
  
  return true;
}

void CreateSpecificEntity::addMicrosToMorphoSyntacticData(LinguisticAnalysisStructure::MorphoSyntacticData* newMorphData,
                               const LinguisticAnalysisStructure::MorphoSyntacticData* oldMorphData,
                               const std::set<LinguisticCode>& micros,
                               LinguisticAnalysisStructure::LinguisticElement& elem) const
{
  // try to filter existing microcategories
  for (MorphoSyntacticData::const_iterator it=oldMorphData->begin(), 
         it_end=oldMorphData->end(); it!=it_end; it++) {
    
    if (micros.find(m_microAccessor->readValue((*it).properties)) !=
        micros.end()) {
      elem.properties=(*it).properties;
      newMorphData->push_back(elem);
    }
  }
  // if no categories kept : assign all micros to keep
  if (newMorphData->empty()) {
    for (std::set<LinguisticCode>::const_iterator it=micros.begin(),
           it_end=micros.end(); it!=it_end; it++) {
      elem.properties=*it;
      newMorphData->push_back(elem);
    }
  }
}

bool CreateSpecificEntity::shouldRemoveInitial(
                                               LinguisticGraphVertex /*src*/, 
                                               LinguisticGraphVertex /*tgt*/, 
                                               const RecognizerMatch& match) const
{
#ifdef DEBUG_LP
  SELOGINIT;
#endif
  if (match.size() == 1)
  {
    return true;
  }
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *(match.getGraph());
  
  std::set< LinguisticGraphVertex > matchVertices;
  Automaton::RecognizerMatch::const_iterator matchIt, matchIt_end;
  
  matchIt = match.begin(); 
  matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    matchVertices.insert((*matchIt).m_elem.first);
  }
  
  matchIt = match.begin(); 
  matchIt_end = match.end()-1;
  if (boost::out_degree((*matchIt).m_elem.first,*graph.getGraph()) > 1)
  {
#ifdef DEBUG_LP
    LDEBUG << "removing edge (" << (*matchIt).m_elem.first << "," << (*(matchIt+1)).m_elem.first << ") because there is more than one path from the first vertex of the match.";
#endif
    boost::remove_edge((*matchIt).m_elem.first,(*(matchIt+1)).m_elem.first, *const_cast<LinguisticGraph*>(graph.getGraph()));
    return false;
  }
  matchIt++;
  for (; matchIt != matchIt_end; matchIt++)
  {
    if (boost::out_degree((*matchIt).m_elem.first,*graph.getGraph()) > 1)
    {
      LinguisticGraphOutEdgeIt outIt, outIt_end;
      boost::tie (outIt, outIt_end) = boost::out_edges((*matchIt).m_elem.first, *graph.getGraph());
      for (; outIt != outIt_end; outIt++) 
      {
        if (matchVertices.find(source(*outIt, *graph.getGraph())) != matchVertices.end())
        {
//            LDEBUG << "removing initial edge " << *outIt;
//           boost::remove_edge(*outIt, *const_cast<LinguisticGraph*>(graph.getGraph()));
          break;
        }
      }
      return false;
    }
  }
  
  return true;
}

bool CreateSpecificEntity::shouldRemoveFinal(
                                             LinguisticGraphVertex /*src*/, 
                                             LinguisticGraphVertex /*tgt*/, 
                                             const RecognizerMatch& match) const
{
#ifdef DEBUG_LP
  SELOGINIT;
#endif
  if (match.size() == 1)
  {
    return true;
  }
  const LinguisticAnalysisStructure::AnalysisGraph& graph = *(match.getGraph());
  
  std::set< LinguisticGraphVertex > matchVertices;
  Automaton::RecognizerMatch::const_iterator matchIt, matchIt_end;
  
  matchIt = match.begin(); 
  matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    matchVertices.insert((*matchIt).m_elem.first);
  }
  
  matchIt = match.begin()+1; 
  matchIt_end = match.end();
  for (; matchIt != matchIt_end; matchIt++)
  {
    if (boost::in_degree((*matchIt).m_elem.first,*graph.getGraph()) > 1)
    {
      LinguisticGraphInEdgeIt inIt, inIt_end;
      boost::tie (inIt, inIt_end) = boost::in_edges((*matchIt).m_elem.first, *graph.getGraph());
      for (; inIt != inIt_end; inIt++) 
      {
        if (matchVertices.find(source(*inIt, *graph.getGraph())) != matchVertices.end())
        {
#ifdef DEBUG_LP
          LDEBUG << "removing final edge " << source(*inIt, *graph.getGraph()) << " -> " << target(*inIt, *graph.getGraph());
#endif
          boost::remove_edge(*inIt, *const_cast<LinguisticGraph*>(graph.getGraph()));
          break;
        }
      }
      return false;
    }
  }
  
  return true;
}


//----------------------------------------------------------------------------------------
// SetEntityFeature : add a given feature to the recognized entity
// we do not have direct access to the RecognizerMatch of the entity when calling this function 
// (called during the matching process) => hence, store features in an AnalysisData and use 
// this Data in normalization functions or CreateSpecificEntity function to get the features.
// Use already existing RecognizerData (no need for another Data).
// CAREFUL: the features must be cleaned after use: explicit call to clearFeatures in case of 
// matching failure must be added in the rule.

SetEntityFeature::SetEntityFeature(MediaId language,
                                   const LimaString& complement):
ConstraintFunction(language,complement),
m_featureName(""),
m_featureType(QVariant::String)
{
  if (complement.size()) {
    QStringList complementElements = complement.split(":");
    m_featureName=complementElements.front().toUtf8().constData();
    complementElements.pop_front();
    if (!complementElements.empty()) {
      const QString& complementType = complementElements.front();
      m_featureType = QVariant::nameToType(complementType.toUtf8().constData());
      if (m_featureType != QVariant::Invalid) {
        if (complementType == "int") {
          m_featureType = QVariant::Int;
        }
        else if (complementType == "double") {
          m_featureType = QVariant::Double;
        }
        else {
          m_featureType = QVariant::String;
        }
      }
    }
  }
}

bool SetEntityFeature::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& vertex,
           AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "SetEntityFeature:: (one argument) start... ";
  LDEBUG << "SetEntityFeature::(feature:" << m_featureName << ", vertex:" << vertex << ")";
#endif
  // get RecognizerData: the data in which the features are stored
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData==0) {
    SELOGINIT;
    LERROR << "SetEntityFeature:: Error: missing RecognizerData";
    return false;
  }
  
  // get string from the vertex and associate it to the feature
  
  // get string from the vertex : 
  // @todo: if named entity, take normalized string, otherwise take lemma
  LimaString featureValue;
  Token* token=get(vertex_token,*(graph.getGraph()),vertex);
  if (token!=0) {
    featureValue=token->stringForm();
  }
  switch (m_featureType) {
    case QVariant::String:
#ifdef DEBUG_LP
      LDEBUG << "SetEntityFeature:: recoData->setEntityFeature(feature:" << m_featureName << ", featureValue:" << featureValue<< ")";
#endif
      recoData->setEntityFeature(m_featureName,featureValue);  
      break;
    case QVariant::Int:
      recoData->setEntityFeature(m_featureName,featureValue.toInt());  
      break;
      
    case QVariant::Double:
      recoData->setEntityFeature(m_featureName,featureValue.toDouble());  
      break;
    default:
      recoData->setEntityFeature(m_featureName,featureValue);  
  }
  uint64_t pos = (int64_t)(token->position());
  uint64_t len = (int64_t)(token->length());
  Automaton::EntityFeatures& features = recoData->getEntityFeatures();
  std::vector<EntityFeature>::iterator featureIt = 
    features.find(m_featureName);
  if( featureIt != recoData->getEntityFeatures().end() )
  {
    featureIt->setPosition(pos);
    featureIt->setLength(len);
  }
  
  return true;
}

bool SetEntityFeature::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& v1,
           const LinguisticGraphVertex& v2,
           AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
//  LERROR << "SetEntityFeature:: Error: version with two vertices parameters is not implemented";
//  return false;
  LDEBUG << "SetEntityFeature:: (two arguments) start... ";
  LDEBUG << "SetEntityFeature::(feature:" << m_featureName << ", v1:" << v1 << ", v2:" << v2 << ")";
#endif
  
  // get RecognizerData: the data in which the features are stored
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData==0) {
    SELOGINIT;
    LERROR << "SetEntityFeature:: Error: missing RecognizerData";
    return false;
  }
  // get string from the set of vertices between v1 and v2
  // @todo: if named entity, take normalized string, otherwise take lemma
  LimaString featureValue;
  const LinguisticGraph& lGraph = *(graph.getGraph());
  
  // (some code borrowed from SpecificEntitiesXmlLogger::process)
  // assert v2 follows v1 within a path composed with a direct sequence of out_edges
  // assert also there exist no ambiguities in the graph.
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  toVisit.push(v1);
  uint64_t pos = UNDEFPOSITION;
  uint64_t len = UNDEFLENGTH;
    
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  unsigned int nbEdges(0);
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (v != v2) {
      for (boost::tie(outItr,outItrEnd)=out_edges(v,lGraph); outItr!=outItrEnd; outItr++) 
      {
        LinguisticGraphVertex next=target(*outItr,lGraph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
          nbEdges++;
        }
      }
    }
    if( nbEdges > 1 ) {
      SELOGINIT;
      LWARN << "SetEntityFeature:: Warning: ambiguïties in graph";
    }

    Token* token=get(vertex_token,lGraph,v);
    if (v == v1) {
      pos = (int64_t)(token->position());
    }
    if (v == v2) {
      if( pos != UNDEFPOSITION )
        len = (int64_t)(token->position()) - pos + 1 + (int64_t)(token->length());
    }
    // @ todo: add separator, check non standard cases where separator is no whitespace.
    // see RecognizeMatch::getString()
    featureValue.append( token->stringForm());
  }
    
  switch (m_featureType) {
    case QVariant::String:
      recoData->setEntityFeature(m_featureName,featureValue);  
      break;
    case QVariant::Int:
      recoData->setEntityFeature(m_featureName,featureValue.toInt());  
      break;
    case QVariant::Double:
      recoData->setEntityFeature(m_featureName,featureValue.toDouble());  
      break;
    default:
      recoData->setEntityFeature(m_featureName,featureValue);  
  }
  Automaton::EntityFeatures& features = recoData->getEntityFeatures();
  std::vector<EntityFeature>::iterator featureIt = 
    features.find(m_featureName);
  if( featureIt != recoData->getEntityFeatures().end() )
  {
    featureIt->setPosition(pos);
    featureIt->setLength(len);
  }
  return true;
}

//----------------------------------------------------------------------------------------
// AddEntityFeatureAsEntity : assert the the vertex is a named entity.
// Add it to the list of components as an embeded entity (the list is used to create the link
// "holds" between the annotation of the embeded and the embedding entity.
// Remember the embedding entity is no yet created.

AddEntityFeatureAsEntity::AddEntityFeatureAsEntity(MediaId language,
                                   const LimaString& complement):
ConstraintFunction(language,complement),
m_featureName(""),
m_featureType(QVariant::UserType)
{
  if (complement.size()) {
    QStringList complementElements = complement.split(":");
    m_featureName=complementElements.front().toUtf8().constData();
    complementElements.pop_front();
    if (!complementElements.empty()) {
#ifdef DEBUG_LP
      SELOGINIT;
      LERROR << "AddEntityFeatureAsEntity::AddEntityFeatureAsEntity(): no type specification authorized for the feature ("
             << complementElements << ") the feature type is the type of the entity";
#endif
    }
  }
}

bool AddEntityFeatureAsEntity::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& /* unused graph */,
           const LinguisticGraphVertex& vertex,
           AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "AddEntityFeatureAsEntity:: (one argument) start... ";
  LDEBUG << "AddEntityFeatureAsEntity::(feature:" << m_featureName << ", vertex:" << vertex << ")";
#endif
  // get RecognizerData: the data in which the features are stored
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData==0) {
    SELOGINIT;
    LERROR << "AddEntityFeatureAsEntity:: Error: missing RecognizerData";
    return false;
  }
  // add the vertex to the list of embeded named entities
  recoData->addVertexAsEmbededEntity(vertex);
  return true;
}

//----------------------------------------------------------------------------------------
// AddEntityFeature : add a value for a given feature to the recognized entity
// we do not have direct access to the RecognizerMatch of the entity when calling this function 
// (called during the matching process) => hence, store features in an AnalysisData and use 
// this Data in normalization functions or CreateSpecificEntity function to get the features.
// Use already existing RecognizerData (no need for another Data).
// CAREFUL: the features must be cleaned after use: explicit call to clearFeatures in case of 
// matching failure must be added in the rule.

AddEntityFeature::AddEntityFeature(MediaId language,
                                   const LimaString& complement):
ConstraintFunction(language,complement),
m_featureName(""),
m_featureType(QVariant::String)
{
  if (complement.size()) {
    QStringList complementElements = complement.split(":");
    m_featureName=complementElements.front().toUtf8().constData();
    complementElements.pop_front();
    if (!complementElements.empty()) {
      const QString& complementType = complementElements.front();
      m_featureType = QVariant::nameToType(complementType.toUtf8().constData());
      if (m_featureType != QVariant::Invalid) {
        if (complementType == "int") {
          m_featureType = QVariant::Int;
        }
        else if (complementType == "double") {
          m_featureType = QVariant::Double;
        }
        else {
          m_featureType = QVariant::String;
        }
      }
    }
  }
}

bool AddEntityFeature::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& vertex,
           AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "AddEntityFeature:: (one argument) start... ";
  LDEBUG << "AddEntityFeature::(feature:" << m_featureName << ", vertex:" << vertex << ")";
#endif
  // get RecognizerData: the data in which the features are stored
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData==0) {
    SELOGINIT;
    LERROR << "AddEntityFeature:: Error: missing RecognizerData";
    return false;
  }
  
  // get string from the vertex and associate it to the feature
  
  // get string from the vertex : 
  // @todo: if named entity, take normalized string, otherwise take lemma
  LimaString featureValue;
  Token* token=get(vertex_token,*(graph.getGraph()),vertex);
  if (token!=0) {
    featureValue=token->stringForm();
  }
  switch (m_featureType) {
    case QVariant::String:
#ifdef DEBUG_LP
      LDEBUG << "AddEntityFeature:: recoData->addEntityFeature(feature:" << m_featureName << ", featureValue:" << featureValue<< ")";
#endif
      recoData->addEntityFeature(m_featureName,featureValue);  
      break;
    case QVariant::Int:
      recoData->addEntityFeature(m_featureName,featureValue.toInt());  
      break;
      
    case QVariant::Double:
      recoData->addEntityFeature(m_featureName,featureValue.toDouble());  
      break;
    default:
      recoData->addEntityFeature(m_featureName,featureValue);  
  }
  uint64_t pos = (int64_t)(token->position());
  uint64_t len = (int64_t)(token->length());
  Automaton::EntityFeatures& features = recoData->getEntityFeatures();
  /*
   * Comment être sûr que l'élément que l'on récupère est le dernier inséré?
   * ajoute  un élement à la fin du vecteur par convention
   * On peut développer une fonction features.findLast(m_featureName);
  */
  
  std::vector<EntityFeature>::iterator featureIt = 
    features.findLast(m_featureName);
  if( featureIt != recoData->getEntityFeatures().end() )
  {
    featureIt->setPosition(pos);
    featureIt->setLength(len);
  }
  
  return true;
}

bool AddEntityFeature::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& v1,
           const LinguisticGraphVertex& v2,
           AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "AddEntityFeature:: (two arguments) start... ";
  LDEBUG << "AddEntityFeature::(feature:" << m_featureName << ", v1:" << v1 << ", v2:" << v2 << ")";
#endif
  
  // get RecognizerData: the data in which the features are stored
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData==0) {
    SELOGINIT;
    LERROR << "AddEntityFeature:: Error: missing RecognizerData";
    return false;
  }
  // get string from the set of vertices between v1 and v2
  // @todo: if named entity, take normalized string, otherwise take lemma
  LimaString featureValue;
  const LinguisticGraph& lGraph = *(graph.getGraph());
  
  // (some code borrowed from SpecificEntitiesXmlLogger::process)
  // assert v2 follows v1 within a path composed with a direct sequence of out_edges
  // assert also there exist no ambiguities in the graph.
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  toVisit.push(v1);
  uint64_t pos = UNDEFPOSITION;
  uint64_t len = UNDEFLENGTH;
    
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  unsigned int nbEdges(0);
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (v != v2) {
      for (boost::tie(outItr,outItrEnd)=out_edges(v,lGraph); outItr!=outItrEnd; outItr++) 
      {
        LinguisticGraphVertex next=target(*outItr,lGraph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
          nbEdges++;
        }
      }
    }
    if( nbEdges > 1 ) {
      SELOGINIT;
      LWARN << "AddEntityFeature:: Warning: ambiguïties in graph";
    }
    
    Token* token=get(vertex_token,lGraph,v);
    if (v == v1) {
      pos = (int64_t)(token->position());
    }
    if (v == v2) {
      if( pos != UNDEFPOSITION )
        len = (int64_t)(token->position()) - pos + 1 + (int64_t)(token->length());
    }
    // @ todo: add separator, check non standard cases where separator is no whitespace.
    // see RecognizeMatch::getString()
    featureValue.append( token->stringForm());
  }
    
  switch (m_featureType) {
    case QVariant::String:
      recoData->setEntityFeature(m_featureName,featureValue);  
      break;
    case QVariant::Int:
      recoData->setEntityFeature(m_featureName,featureValue.toInt());  
      break;
    case QVariant::Double:
      recoData->setEntityFeature(m_featureName,featureValue.toDouble());  
      break;
    default:
      recoData->setEntityFeature(m_featureName,featureValue);  
  }
  Automaton::EntityFeatures& features = recoData->getEntityFeatures();
  std::vector<EntityFeature>::iterator featureIt = 
    features.find(m_featureName);
  if( featureIt != recoData->getEntityFeatures().end() )
  {
    featureIt->setPosition(pos);
    featureIt->setLength(len);
  }
  return true;
}

//----------------------------------------------------------------------------------------
// AppendEntityFeature : add a given feature to the recognized entity or append the
// value of a given feature if it already exists.
// Same remark as for SetEntityFeature about the relation with RecognizerMatch
// and RecognizerData.

AppendEntityFeature::AppendEntityFeature(MediaId language,
                                   const LimaString& complement):
ConstraintFunction(language,complement),
m_featureName(""),
m_featureType(QVariant::String)
{
  if (complement.size()) {
    QStringList complementElements = complement.split(":");
    m_featureName=complementElements.front().toUtf8().constData();
    complementElements.pop_front();
    if (!complementElements.empty()) {
      const QString& complementType = complementElements.front();
      m_featureType = QVariant::nameToType(complementType.toUtf8().constData());
      if (m_featureType != QVariant::Invalid) {
        if (complementType == "int") {
          m_featureType = QVariant::Int;
        }
        else if (complementType == "double") {
          m_featureType = QVariant::Double;
        }
        else {
          m_featureType = QVariant::String;
        }
      }
    }
  }
}

uint64_t AppendEntityFeature::minPos( const uint64_t pos1, const uint64_t pos2 ) const {
  if( pos1 == UNDEFPOSITION )
    return pos2;
  if( pos2 == UNDEFPOSITION )
    return pos1;
  if( pos1 < pos2 ) 
    return pos1;
  return pos2;
}

uint64_t AppendEntityFeature::maxPos( const uint64_t pos1, const uint64_t pos2 ) const {
  if( pos1 == UNDEFPOSITION )
    return pos2;
  if( pos2 == UNDEFPOSITION )
    return pos1;
  if( pos1 > pos2 ) 
    return pos1;
  return pos2;
}

bool AppendEntityFeature::
operator()(const LinguisticAnalysisStructure::AnalysisGraph& graph,
           const LinguisticGraphVertex& vertex,
           AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  SELOGINIT;
  LDEBUG << "AppendEntityFeature::() (one argument) start... ";
  LDEBUG << "AppendEntityFeature::() feature:" << m_featureName << ", vertex:" << vertex << ")";
#endif
  // get RecognizerData: the data in which the features are stored
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData==0) {
    SELOGINIT;
    LERROR << "AppendEntityFeature::() Error: missing RecognizerData";
    return false;
  }
  
  // get position/length from feature 
  // position is min of position.
  // length is span from min of position 
  // to max of position + augmented with length of last element.
  uint64_t pos = UNDEFPOSITION;
  uint64_t len = UNDEFLENGTH;
  Automaton::EntityFeatures& features = recoData->getEntityFeatures();
  std::vector<EntityFeature>::iterator featureIt = features.find(m_featureName);
  Token* token=get(vertex_token,*(graph.getGraph()),vertex);
//  Token* token=get(vertex_token,lGraph,vertex);
  // if feature already exists
  if( featureIt != recoData->getEntityFeatures().end() )
  {
    pos = minPos( featureIt->getPosition(), (int64_t)(token->position()) );
#ifdef DEBUG_LP
    LDEBUG << "AppendEntityFeature::() minPos=" << pos;
#endif
    uint64_t maxPos1 = UNDEFPOSITION;
    if( featureIt->getPosition() != UNDEFPOSITION ) {
      maxPos1 = featureIt->getPosition() + featureIt->getLength();
    }
#ifdef DEBUG_LP
    LDEBUG << "AppendEntityFeature::() maxPos1=" << maxPos1;
 #endif
   uint64_t maxPos2 = (int64_t)(token->position()) + (int64_t)(token->length());
#ifdef DEBUG_LP
    LDEBUG << "AppendEntityFeature::() maxPos2=" << maxPos2;
#endif
    uint64_t maxPos3 = maxPos( maxPos1, maxPos2 );
#ifdef DEBUG_LP
    LDEBUG << "AppendEntityFeature::() maxPos3=" << maxPos3;
#endif
    len = maxPos3 - pos;
#ifdef DEBUG_LP
    LDEBUG << "AppendEntityFeature::() len=" << len;
#endif
  }
  else {
    pos = (int64_t)(token->position());
    len = (int64_t)(token->length());
  }
  
  // get string from the vertex and associate it to the feature

  // get string from the vertex : 
  // @todo: if named entity, take normalized string, otherwise take lemma
  LimaString featureValue;
  if (token!=0) {
    featureValue=token->stringForm();
  }
  switch (m_featureType) {
    case QVariant::String:
      recoData->appendEntityFeature(m_featureName,featureValue);  
      break;
    case QVariant::Int:
      recoData->appendEntityFeature(m_featureName,featureValue.toInt());  
      break;
    case QVariant::Double:
      recoData->appendEntityFeature(m_featureName,featureValue.toDouble());  
      break;
    default:
      recoData->appendEntityFeature(m_featureName,featureValue);  
  }
  featureIt = features.find(m_featureName);
#ifdef DEBUG_LP
  LDEBUG << "AppendEntityFeature::() pos before = ("
         << featureIt->getPosition() << ","
         << featureIt->getLength() << ")";
#endif
  featureIt->setPosition(pos);
  featureIt->setLength(len);
#ifdef DEBUG_LP
  LDEBUG << "AppendEntityFeature::() pos after = (" << pos << "," << len << ")";
#endif
  
  return true;
}

// clear stored entity features, added by the SetEntityFeature function
ClearEntityFeatures::ClearEntityFeatures(MediaId language,
const LimaString& complement):
ConstraintFunction(language,complement)
{
}
bool ClearEntityFeatures::
operator()(AnalysisContent& analysis) const
{
// get RecognizerData: the data in which the features are stored
RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
if (recoData!=0) {
recoData->clearEntityFeatures();
}
return true;
}


//----------------------------------------------------
// Normalize entity using stored features
NormalizeEntity::NormalizeEntity(MediaId language,
const LimaString& complement):
ConstraintFunction(language,complement)
{
}
bool NormalizeEntity::
operator()(Automaton::RecognizerMatch& match,
AnalysisContent& analysis) const
{
// get stored features in recognizerData
RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
if (recoData==0) {
SELOGINIT;
LERROR << "NormalizeEntity:: Error: missing RecognizerData";
return false;
}
// assign stored features to RecognizerMatch features
match.features()=recoData->getEntityFeatures();
// must clear the stored features, once they are used (otherwise, will be kept for next entity)
recoData->clearEntityFeatures();
return true;
}

} // SpecificEntities
} // LinguisticProcessing
} // Lima
