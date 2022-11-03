// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "ConcatenatedDataHandler.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

ConcatenatedDataHandler::ConcatenatedDataHandler(LinguisticGraph* outputGraph,
    LinguisticAnalysisStructure::Token* sourceToken,
    LinguisticAnalysisStructure::MorphoSyntacticType type,
    const FsaStringsPool* sp) :
    m_graph(outputGraph),
    m_srcToken(sourceToken),
    m_stringsPool(sp),
    m_concatVertices(),
    m_currentToken(0),
    m_currentData(0),
    m_currentElement()
{
  m_currentElement.type=type;
}


ConcatenatedDataHandler::~ConcatenatedDataHandler()
{}

void ConcatenatedDataHandler::foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  m_currentElement.lemma=lemma;
  m_currentElement.normalizedForm=norm;
}

void ConcatenatedDataHandler::endLingInfos()
{}

void ConcatenatedDataHandler::foundConcatenated()
{
  m_concatVertices.push_back(std::vector<LinguisticGraphVertex>());
}

void ConcatenatedDataHandler::foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form)
{
  // create vertex and link if necessary
  LinguisticGraphVertex componentVertex=add_vertex(*m_graph);
  std::vector<LinguisticGraphVertex>& concatenated=m_concatVertices.back();
  if (concatenated.size()>0)
  {
    add_edge(concatenated.back(),componentVertex,*m_graph);
  }
  concatenated.push_back(componentVertex);
  
  m_currentToken=new Token(form,(*m_stringsPool)[form],m_srcToken->position()+position,length,m_srcToken->status());
  put(vertex_token,*m_graph,componentVertex,m_currentToken);
  m_currentData=new MorphoSyntacticData();
  put(vertex_data,*m_graph,componentVertex,m_currentData);
  
  m_currentElement.inflectedForm=form;
}

void ConcatenatedDataHandler::endComponent()
{
  m_currentData=0;
  m_currentToken=0;
}

void ConcatenatedDataHandler::endConcatenated()
{
}

void ConcatenatedDataHandler::foundProperties(LinguisticCode lings)
{
  if (m_currentData) {
    m_currentElement.properties=lings;
    m_currentData->push_back(m_currentElement);
  }
}


}

}

}
