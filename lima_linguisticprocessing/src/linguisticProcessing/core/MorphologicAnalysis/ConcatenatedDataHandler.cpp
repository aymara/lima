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
  
#ifdef ANTINNO_SPECIFIC
  // FWI 04/04/2016
  // plantage de l'indexeur sauf si les 4 lignes suivantes sont commentées
  // apparement le fait de détruire *this plante nt.dll sans que la cause soit évidente
  // pour test : voir le doc "constitution 2011" en ARA sur la machine "lirac"
  // sur ma machine ça ne plante pas systématiquement...
  // A noter : désactiver le paramètre "parseConcatenated" dans SimpleWord permet de courtcircuiter le problème
  // -> à investiguer
#endif
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
