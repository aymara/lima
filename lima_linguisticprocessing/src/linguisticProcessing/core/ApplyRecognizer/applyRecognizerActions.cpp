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
/************************************************************************
 *
 * @file       applyRecognizerActions.cpp
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue Jan 25 2005
 * copyright   Copyright (C) 2005-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "linguisticProcessing/core/Automaton/constraintFunctionFactory.h"
#include "applyRecognizerActions.h"
#include "linguisticProcessing/core/Automaton/recognizerData.h"
#include "linguisticProcessing/client/LinguisticProcessingException.h"
#include "common/Data/strwstrtools.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::Common;
using namespace std;

namespace Lima {
namespace LinguisticProcessing {
namespace ApplyRecognizer {

//**********************************************************************
// factories for constraint functions defined in this class
ConstraintFunctionFactory<CreateAlternative>
CreateAlternativeFactory(CreateAlternativeId);

ConstraintFunctionFactory<StoreInData>
StoreInDataFactory(StoreInDataId);

//**********************************************************************
// utility functions for CreateAlternative action
std::pair<Token*,MorphoSyntacticData*> CreateAlternative::
createAlternativeToken(const RecognizerMatch& recognizedExpression) const
{
  LimaString formStr=recognizedExpression.concatString();
  StringsPoolIndex formId=(*m_stringsPool)[formStr];
  Token* newToken=new Token(formId,
                            formStr,
                            recognizedExpression.positionBegin(),
                            recognizedExpression.length());
  MorphoSyntacticData* newData=new MorphoSyntacticData();

  LimaString lemmaString;
  EntityFeatures::const_iterator f=recognizedExpression.features().
    find(DEFAULT_ATTRIBUTE);
  if (f!=recognizedExpression.features().end()) {
    lemmaString=boost::any_cast<const LimaString&>((*f).getValue());
  }
  else {
    lemmaString=recognizedExpression.getString();
  }
  
  StringsPoolIndex lemma=(*m_stringsPool)[lemmaString];
  LinguisticCode idiomProperty=recognizedExpression.getLinguisticProperties();
//   LOGINIT("LP::MorphologicAnalysis");
//   LDEBUG << "Idiomatic property=" << idiomProperty;
  
  if (recognizedExpression.getHead() == 0) {
//     APPRLOGINIT;
//     LDEBUG << "Expression " << lemmaString << " has no head";

    // set TStatus from first token
    Token* firstToken=recognizedExpression.getToken(recognizedExpression.begin());
    if (firstToken!=0) {
      newToken->setStatus(firstToken->status());
    }
    else {
      // set default t_status to null (changed from alpha small on 20110729 by GC)
      // nothing to do: null by default
/*      TStatus* status=
      new TStatus(T_SMALL,    //AlphaCapitalType
                  T_NOT_ROMAN,//AlphaRomanType
                  false, //isHyphen,
                  false, //isPossessive,
                  false, //isConcatAbbrev,
                  T_NULL_NUM, //NumericType,
                  T_ALPHA // StatusType
                  );
      status->setDefaultKey(Common::Misc::utf8stdstring2limastring("t_small"));
      newToken->setStatus(status);*/
    }

    // set simple morphoData
    LinguisticElement lingElt;
    lingElt.inflectedForm=newToken->form();
    lingElt.lemma=lemma;
    lingElt.normalizedForm=lemma;
    lingElt.properties=idiomProperty;
    lingElt.type=IDIOMATIC_EXPRESSION; 
    newData->push_back(lingElt);
    return make_pair(newToken,newData);
  }

  Token* headToken=recognizedExpression.getHeadToken();
  MorphoSyntacticData* headData=recognizedExpression.getHeadData();
  
  // copy TStatus of the head
  newToken->setStatus(headToken->status());

  std::set<LinguisticCode> compatibleProperties;
  getCompatibleProperties(headData,idiomProperty,
                          compatibleProperties);
  
  // ensure that there is always a property, 
  // if none found, keep base one
  if (compatibleProperties.empty()) {
    APPRLOGINIT;
    LWARN << "head of expression " << (*m_stringsPool)[lemma] << " has no compatible properties, use"<< idiomProperty;
    compatibleProperties.insert(idiomProperty);
  }

  LinguisticElement lingElt;
  lingElt.inflectedForm=newToken->form();
  lingElt.lemma=lemma;
  lingElt.normalizedForm=lemma;
  lingElt.type=IDIOMATIC_EXPRESSION; 
  
  std::set<LinguisticCode>::const_iterator
    prop=compatibleProperties.begin(),
    prop_end=compatibleProperties.end();
  for (; prop!=prop_end; prop++) {
    lingElt.properties=*prop;
    newData->push_back(lingElt);
  }
  
  return make_pair(newToken,newData);
}

bool CreateAlternative::
getCompatibleProperties(const MorphoSyntacticData* headData,
                        const LinguisticCode& baseProperty,
                        std::set<LinguisticCode>& newProperties) const {
  
  LinguisticCode newProperty; // completed property
  MorphoSyntacticData::const_iterator
    it=headData->begin(),
    it_end=headData->end();
  for (; it!=it_end; it++) {
    if (isCompatible(baseProperty,(*it).properties,newProperty)) {
      newProperties.insert(newProperty);
    }
  }
  return (!newProperties.empty());
}

bool CreateAlternative::
isCompatible(const LinguisticCode& baseProperty,
             const LinguisticCode& property,
             LinguisticCode& newProperty) const {

  //check compatibility only on macro
  if (!m_macroAccessor->empty(baseProperty) && 
      !m_macroAccessor->equal(property,baseProperty)) {
    return false;
  }
  // if compatible, complete expression property (baseProperty)
  // with head property (property)
  newProperty=baseProperty;
  const std::map<std::string,Common::PropertyCode::PropertyManager>& 
    managers=m_propertyCodeManager->getPropertyManagers();
  for (map<string,Common::PropertyCode::PropertyManager>::const_iterator propIt=managers.begin();
       propIt!=managers.end();
       propIt++) {
       const Common::PropertyCode::PropertyAccessor& acc=propIt->second.getPropertyAccessor();
    if (acc.empty(newProperty)) {
      acc.writeValue(property,newProperty);
    }
  }
  return true;
}

LinguisticGraphVertex CreateAlternative::
addAlternativeVertex(Token* token,
                     MorphoSyntacticData* data,
                     LinguisticGraph* graph) const
{
  LinguisticGraphVertex altVertex = add_vertex(*graph);
  VertexTokenPropertyMap tokenMap = get(vertex_token, *graph);
  VertexDataPropertyMap  dataMap  = get(vertex_data, *graph);
  tokenMap[altVertex]= token;
  dataMap[altVertex] = data;
  return altVertex;
}

/**
 * create an alternative branch
 * 
 * @param startVertex where to start on the graph : the start vertex
 * is the first node to be avoided by the alternative : all previous
 * vertices attached to this node are branched to the alternative
 * @param alternativeFirstVertex the first vertex of the alternative
 * @param graph the graph
 */
void CreateAlternative::createBeginAlternative(
                       LinguisticGraphVertex startVertex,
                       LinguisticGraphVertex alternativeFirstVertex,
                       LinguisticGraph& graph) const
{
//   APPRLOGINIT;
  // add edges from vertices preceding startVertex to alternativeFirstVertex
  LinguisticGraphInEdgeIt it_begin,it_end;
  boost::tie(it_begin,it_end)=in_edges(startVertex,graph);
  for (LinguisticGraphInEdgeIt it(it_begin); it!=it_end; it++)
  {
    LinguisticGraphVertex previousVertex=source(*it,graph);
    LinguisticGraphEdge newEdge;
    bool ok;
    boost::tie(newEdge, ok) = add_edge(previousVertex, alternativeFirstVertex, graph);
    if (!ok) throw LinguisticProcessingException();
//      LDEBUG << "  added initial edge " << newEdge;
  }
}

/**
 * attach the end of an alternative to main path
 * 
 * @param alternativeLastVertex last vertex of the alternative to re-attach
 * to the path
 * @param endVertex where to attach it on the graph : the endVertex is the
 * last vertex avoided by the alternative : the alternative returns to the
 * main path after the endVertex
 * @param graph the graph
 */
void CreateAlternative::attachEndOfAlternative(
                       LinguisticGraphVertex alternativeLastVertex,
                       LinguisticGraphVertex endVertex,
                       LinguisticGraph& graph) const
{
//   APPRLOGINIT;
  // add edges from alternativeLastVertex to vertices following endVertex
  LinguisticGraphOutEdgeIt it_begin,it_end;
  boost::tie(it_begin,it_end)=out_edges(endVertex,graph);
  for (LinguisticGraphOutEdgeIt it(it_begin); it!=it_end; it++)
  {
    LinguisticGraphVertex nextVertex=target(*it,graph);
    LinguisticGraphEdge newEdge;
    bool ok;
    boost::tie(newEdge, ok) = add_edge(alternativeLastVertex, nextVertex, graph);
    if (!ok) throw LinguisticProcessingException();
//      LDEBUG << "  added final edge " << newEdge;
  }
}

//**********************************************************************
CreateAlternative::CreateAlternative(MediaId language,
                                     const LimaString& complement):
ConstraintFunction(language,complement),
m_macroAccessor(0),
m_propertyCodeManager(0)
{

  m_propertyCodeManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager());
  m_macroAccessor=&(m_propertyCodeManager->getPropertyAccessor("MACRO"));
  m_stringsPool=&(Common::MediaticData::MediaticData::changeable().stringsPool(language));
  
  if (m_propertyCodeManager==0) {
    APPRLOGINIT;
    LERROR << "cannot acces property code manager for language " << (int) language;
    throw LimaException();
  }
  if (m_macroAccessor==0) {
    APPRLOGINIT;
    LERROR << "cannot initialize MACRO property accessor for language " << (int) language;
    throw LimaException();
  }

}

bool CreateAlternative::
operator()(RecognizerMatch& result,
           AnalysisContent& analysis) const
{

//   APPRLOGINIT;
//   LDEBUG << "in CreateAlternative action";
  // need dictionary
  RecognizerData* recoData=static_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* graph=anagraph->getGraph();
  
  if (result.isContiguous()) {
    // only one part : terms in expression are adjacent -> easy part
    
    // check if there is an overlap first
    if (recoData->matchOnRemovedVertices(result)) {
      // ignore current idiomatic expression, continue
      APPRLOGINIT;
      LWARN << "recognized entity ignored: " 
        << result.concatString()
            << ": overlapping with a previous one";
      return false;
    }
    
    // create the new token
    pair<Token*,MorphoSyntacticData*> newToken=
      createAlternativeToken(result);
//     LDEBUG << "create alternative token " << newToken.first->stringForm();
    
    // add the vertex
    LinguisticGraphVertex alternativeVertex = 
      addAlternativeVertex(newToken.first, newToken.second,graph);

//     LDEBUG << "add alternative vertex " << alternativeVertex;
    
    //create the alternative with this only vertex
    createBeginAlternative(result.front().getVertex(),
                           alternativeVertex,*graph);
    attachEndOfAlternative(alternativeVertex,
                           result.back().getVertex(),*graph);
   
    // if expression is not contextual, only keep alternative
    if (! result.isContextual()) {
      recoData->storeVerticesToRemove(result,graph);
    }
  }
  else { // several parts
//     LDEBUG << "non adjacent recognized expression found: "
//            << result.concatString();
        // check if there is an overlap first
    if (recoData->matchOnRemovedVertices(result))
    {
      // ignore current expression, continue
      APPRLOGINIT;
    LWARN << "alternative expression ignored: " 
        << result.concatString()
            << ": overlapping with a previous one";
      return false;
    }

    // create the new token
    pair<Token*,MorphoSyntacticData*> newToken=
      createAlternativeToken(result);
    
    // add the vertex
    LinguisticGraphVertex altVertex = 
      addAlternativeVertex(newToken.first,newToken.second,graph);
    
    //create the alternative with this vertex and duplicate of other vertives
    deque<LinguisticGraphVertex> alternative;
    LinguisticGraphVertex headVertex=result.getHead();
//     LDEBUG << "headVertex = " << headVertex;
/*    if (headVertex!=0) {
      LDEBUG << "=> " << get(vertex_token,*graph,headVertex)->stringForm();
    }*/
    bool foundHead=false;
    for (RecognizerMatch::const_iterator matchItr=result.begin();
         matchItr!=result.end();
         matchItr++)
    {
      if (!matchItr->isKept())
      {
        // duplicate this vertex
//         LDEBUG << "duplication vertex " << matchItr->getVertex();;
        Token* token=get(vertex_token,*graph,matchItr->getVertex());
        MorphoSyntacticData* data=new MorphoSyntacticData(*get(vertex_data,*graph,matchItr->getVertex()));
        LinguisticGraphVertex dupVx=add_vertex(*graph);
        put(vertex_token,*graph,dupVx,token);
        put(vertex_data,*graph,dupVx,data);
        alternative.push_back(dupVx);
      }
      else
      {
//         LDEBUG << "kept vertex " << matchItr->getVertex();
        if (matchItr->getVertex()==headVertex)
        {
          foundHead=true;
//           LDEBUG << "add head vertex " << altVertex;
          alternative.push_back(altVertex);
        }
      }
    }
    if (!foundHead) {
      APPRLOGINIT;
      LWARN << "head token has not been found in non contiguous expression. "
            << "Alternative token is placed first";
      alternative.push_front(altVertex);
    }
    
    // link alternatives
//     LDEBUG << "alternative has " << alternative.size() << " vertex";
    createBeginAlternative(result.front().getVertex(),
                           alternative.front(),*graph);
    {
      deque<LinguisticGraphVertex>::const_iterator idItr=alternative.begin();
      LinguisticGraphVertex lastAltVx=*idItr;
      idItr++;
      while (idItr!=alternative.end())
      {
        add_edge(lastAltVx,*idItr,*graph);
        lastAltVx=*idItr;
        idItr++;
      }
    }
    attachEndOfAlternative(alternative.back(),
                           result.back().getVertex(),*graph);
    
    // if expression is not contextual, only keep alternative
    if (! result.isContextual())
    {
      recoData->storeVerticesToRemove(result,graph);
    }
  }
  return true;
}

//**********************************************************************
StoreInData::StoreInData(MediaId language,
                         const LimaString& complement):
ConstraintFunction(language,complement)
{
}

bool StoreInData::
operator()(RecognizerMatch& result,
           AnalysisContent& analysis) const
{
//   APPRLOGINIT;
//   LDEBUG << "add result in data:" << result;
  RecognizerData* recoData=dynamic_cast<RecognizerData*>(analysis.getData("RecognizerData"));
  if (recoData==0) {
    APPRLOGINIT;
    LWARN << "StoreInData: cannot find RecognizerData in AnalysisContent";
    return false;
  }
  recoData->addResult(result);
  return true;
}


} // end namespace
} // end namespace
} // end namespace
