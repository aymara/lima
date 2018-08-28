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
#include "AccentedConcatenatedDataHandler.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

AccentedConcatenatedDataHandler::AccentedConcatenatedDataHandler(LinguisticGraph* outputGraph,
    const LimaString& sourceStr,
    uint64_t positionOffset,
    const TStatus& status,
    LinguisticAnalysisStructure::MorphoSyntacticType type,
    const FsaStringsPool* sp,
    FlatTokenizer::CharChart* charChart) :
    m_graph(outputGraph),
    m_srcStr(sourceStr),
    m_positionOffset(positionOffset),
    m_status(status),
    m_stringsPool(sp),
    m_charChart(charChart),
    m_concatVertices(),
    m_currentToken(0),
    m_currentData(0),
    m_currentElement()
{
  m_currentElement.type=type;
  
  std::vector<unsigned char> mapping;
  LimaString desacc=m_charChart->unmarkWithMapping(m_srcStr,mapping);
  m_unmarkToTextMapping.resize(desacc.size()+1);
  unsigned char i=0;
  for (std::vector<unsigned char>::const_iterator it=mapping.begin();
       it!=mapping.end();
       it++,i++)
  {
    m_unmarkToTextMapping[*it]=i;
  }
  m_unmarkToTextMapping.back()=m_srcStr.size();
//  std::cerr << "m_unmarkToTextMapping size = " << m_unmarkToTextMapping.size() << std::endl;


  m_charChart->unmarkWithMapping(sourceStr,m_positionMapping);
  for (std::vector<unsigned char>::iterator it=m_positionMapping.begin();
       it!=m_positionMapping.end();
       it++)
  {
    *it = m_unmarkToTextMapping[*it];
  }
  m_positionMapping.push_back(m_unmarkToTextMapping.back());
//  std::cerr << "position mapping size = " << m_positionMapping.size() << std::endl;
//  std::copy(m_positionMapping.begin(),m_positionMapping.end(),std::ostream_iterator<int>(std::cerr,","));

}


AccentedConcatenatedDataHandler::~AccentedConcatenatedDataHandler()
{}

void AccentedConcatenatedDataHandler::foundAccentedForm(StringsPoolIndex form)
{
  const LimaString& accentedForm=(*m_stringsPool)[form];
#ifdef DEBUG_LP
  MORPHOLOGINIT;
  LDEBUG << "AccentedConcatenatedDataHandler::foundAccentedForm" << accentedForm;
#endif
  m_charChart->unmarkWithMapping(accentedForm,m_positionMapping);
  for (std::vector<unsigned char>::iterator it=m_positionMapping.begin();
       it!=m_positionMapping.end();
       it++)
  {
    if (*it < m_unmarkToTextMapping.size())
    {
      *it = m_unmarkToTextMapping[*it];
    }
  }
  m_positionMapping.push_back(m_unmarkToTextMapping.back());
//  std::cerr << "position mapping size = " << m_positionMapping.size() << std::endl;
//  std::copy(m_positionMapping.begin(),m_positionMapping.end(),std::ostream_iterator<int>(std::cerr,","));
}

void AccentedConcatenatedDataHandler::endAccentedForm()
{

}

void AccentedConcatenatedDataHandler::foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  m_currentElement.lemma=lemma;
  m_currentElement.normalizedForm=norm;
}

void AccentedConcatenatedDataHandler::endLingInfos()
{}

void AccentedConcatenatedDataHandler::foundConcatenated()
{
  m_concatVertices.push_back(std::vector<LinguisticGraphVertex>());
}

void AccentedConcatenatedDataHandler::foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form)
{
//  std::cerr << "found component : " << position << ", " << length << std::endl;
//  std::cerr << "mapping " << position << " => " << (int)m_positionMapping[position] << std::endl;
//  std::cerr << "mapping " << position + length << " => " << (int)m_positionMapping[position+length] << std::endl;
  // create vertex and link if necessary
  LinguisticGraphVertex componentVertex=add_vertex(*m_graph);
  std::vector<LinguisticGraphVertex>& concatenated=m_concatVertices.back();
  if (concatenated.size()>0)
  {
    add_edge(concatenated.back(),componentVertex,*m_graph);
  }
  concatenated.push_back(componentVertex);
  
  m_currentToken=new Token(
    form,
    (*m_stringsPool)[form],
    m_positionOffset+m_positionMapping[position],
    m_positionMapping[position + length] - m_positionMapping[position],
    m_status);
  put(vertex_token,*m_graph,componentVertex,m_currentToken);
  m_currentData=new MorphoSyntacticData();
  put(vertex_data,*m_graph,componentVertex,m_currentData);
  m_currentElement.inflectedForm=form;
//  std::cerr << "transcode component : " << m_currentToken->position() << ", " << m_currentToken->length() << std::endl;
}

void AccentedConcatenatedDataHandler::endComponent()
{
  m_currentData=0;
  m_currentToken=0;
}

void AccentedConcatenatedDataHandler::endConcatenated()
{
}

void AccentedConcatenatedDataHandler::foundProperties(LinguisticCode lings)
{
  if (m_currentData) {
    m_currentElement.properties=lings;
    m_currentData->push_back(m_currentElement);
  }
}


}

}

}
