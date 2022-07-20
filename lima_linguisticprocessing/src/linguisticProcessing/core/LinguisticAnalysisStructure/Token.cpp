// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

void Token::outputXml(std::ostream& xmlStream,
                      const Common::PropertyCode::PropertyCodeManager&,
                      const FsaStringsPool& sp) const
{
  xmlStream << "    <string>"
            << Common::Misc::transcodeToXmlEntities(m_stringForm).toStdString()
            << "</string>" << std::endl;
  xmlStream << "    <position>" << position() << "</position>" << std::endl;
  xmlStream << "    <length>" << length() << "</length>" << std::endl;
  if (!m_alternatives.empty()) {
    xmlStream << "    <alternatives>" << std::endl;
    for (auto it = m_alternatives.cbegin(); it != m_alternatives.cend();
         it++)
    {
      xmlStream << "      <string>"
                << Common::Misc::transcodeToXmlEntities(sp[*it]).toStdString()
                << "</string>" << std::endl;
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
