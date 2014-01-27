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
#include "Token.h"
#include "TStatus.h"
#include "common/Data/strwstrtools.h"

using namespace Lima::Common::MediaticData;

namespace Lima {

namespace LinguisticProcessing {

namespace LinguisticAnalysisStructure {

Token::Token(
      StringsPoolIndex form,
      const LimaString& stringForm,
      uint64_t position,
      uint64_t length) :
  m_form(form),
  m_stringForm(stringForm),
  m_position(position),
  m_length(length),
  m_status()
{}

Token::Token(
  StringsPoolIndex form,
  const LimaString& stringForm,
  uint64_t position,
  uint64_t length,
  const TStatus& status) :
  m_form(form),
  m_stringForm(stringForm),
  m_position(position),
  m_length(length),
  m_status(status)
{}

Token::Token(const Token& tok) :
  m_form(tok.m_form),
  m_stringForm(tok.m_stringForm),
  m_position(tok.m_position),
  m_length(tok.m_length),
  m_status(tok.m_status)
{
}

Token::~Token()
{
}

void Token::outputXml(std::ostream& xmlStream,const Common::PropertyCode::PropertyCodeManager&,const FsaStringsPool& sp) const
{
  xmlStream << "    <string>" << Common::Misc::transcodeToXmlEntities(m_stringForm) << "</string>" << std::endl;
  xmlStream << "    <position>" << position() << "</position>" << std::endl;
  xmlStream << "    <length>" << length() << "</length>" << std::endl;
  if (!m_alternatives.empty()) {
    xmlStream << "    <alternatives>" << std::endl;
    for (std::vector<StringsPoolIndex>::const_iterator it=m_alternatives.begin();
         it!=m_alternatives.end();
         it++)
    {
      xmlStream << "      <string>" << Common::Misc::transcodeToXmlEntities(sp[*it]) << "</string>" << std::endl;
    }
    xmlStream << "    </alternatives>" << std::endl;
  }
  xmlStream << "    <t_status>" << std::endl;
  m_status.outputXML(xmlStream);
  xmlStream << "    </t_status>" << std::endl;
}

}

}

}
