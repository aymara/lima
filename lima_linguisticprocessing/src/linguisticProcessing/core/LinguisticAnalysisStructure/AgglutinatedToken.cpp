// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** AgglutinatedToken is a fulltoken that is a agglutinated compound word.
  *
  * @file       AgglutinatedToken.cpp
  * @author     Benoit Mathieu (CEA)  <mathieub@zoe.cea.fr>
  *             Copyright  (c) 2003 by CEA
  * @date       Created on Jul, 8 2002
  *
  */


#include "AgglutinatedToken.h"
#include <set>

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{

AgglutinatedToken::AgglutinatedToken(const Token& ft) :
    Token(ft),
    m_cuttings()
{}

AgglutinatedToken::~ AgglutinatedToken()
{
//  std::cerr << "in deletion of agglutinated token " << stringForm() << std::endl;
  std::set<Token*> tokToDelete;
  std::set<MorphoSyntacticData*> dataToDelete;
  for (T_CUTTINGS::iterator it=m_cuttings.begin();
       it!=m_cuttings.end();
       it++)
  {
    for (Cutting::iterator part=it->begin();
         part!=it->end();
         part++)
    {
      tokToDelete.insert(part->first);
      dataToDelete.insert(part->second);
    }
  }
  for (std::set<Token*>::iterator it=tokToDelete.begin();
       it!=tokToDelete.end();
       it++)
  {
//    std::cerr << "delete " << (*it)->stringForm() << std::endl;
    delete *it;
  }
  for (std::set<MorphoSyntacticData*>::iterator it=dataToDelete.begin();
       it!=dataToDelete.end();
       it++)
  {
//    std::cerr << "delete data " << *it << std::endl;
    delete *it;
  }
}

void AgglutinatedToken::setCuttings(const T_CUTTINGS& cuttings)
{
  m_cuttings=cuttings;
}

const AgglutinatedToken::T_CUTTINGS& AgglutinatedToken::getCuttings() const
{
  return m_cuttings;
}

void AgglutinatedToken::outputXml(std::ostream& xmlStream,const Common::PropertyCode::PropertyCodeManager& pcm,const FsaStringsPool& sp) const
{
  Token::outputXml(xmlStream,pcm,sp);
  xmlStream << "    <agglutination_properties>" << std::endl;
  for (T_CUTTINGS::const_iterator it=m_cuttings.begin();
       it!=m_cuttings.end();
       it++)
  {
    xmlStream << "      <cutting>" << std::endl;
    for (Cutting::const_iterator cuttingItr=it->begin();
         cuttingItr!=it->end();
         cuttingItr++)
    {
      xmlStream << "        <cutting_part>" << std::endl;
      xmlStream << "          <token>" << std::endl;
      cuttingItr->first->outputXml(xmlStream,pcm,sp);
      xmlStream << "          </token>" << std::endl;
      cuttingItr->second->outputXml(xmlStream,pcm,sp);
      xmlStream << "        </cutting_part>" << std::endl;
    }
    xmlStream << "      </cutting>" << std::endl;
  }
  xmlStream << "    </agglutination_properties>" << std::endl;
}

} // closing namespace Data
} // closing namespace LinguisticProcessing
} // closing namespace Lima
