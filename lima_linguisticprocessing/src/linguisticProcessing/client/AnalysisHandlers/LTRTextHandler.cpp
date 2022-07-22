// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "LTRTextHandler.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

using namespace std;
using namespace Lima::Common::BagOfWords;

namespace Lima {
namespace LinguisticProcessing {

LTRTextHandler::LTRTextHandler()
  : AbstractTextualAnalysisHandler(),m_ltrstream(),m_ltrtext()
{
}


LTRTextHandler::~LTRTextHandler()
{
}

Common::BagOfWords::LTR_Text& LTRTextHandler::getLTRText()
{
  return m_ltrtext;
}


void LTRTextHandler::endAnalysis()
{
  // read from completed stream
  m_ltrtext.binaryReadFrom(m_ltrstream);
}


void LTRTextHandler::startAnalysis()
{
  m_ltrtext.clear();
  // reset stringstream
  m_ltrstream.str("");
}

void LTRTextHandler::handle(const char* buf, int length)
{
  // store in stream
  m_ltrstream.write(buf,length);
  //m_writer->handle(buf,length);
}

void LTRTextHandler::endDocument()
{
}

void LTRTextHandler::startDocument(const Common::Misc::GenericDocumentProperties&)
{
}

void LTRTextHandler::startNode( const std::string& /*elementName*/, bool /*forIndexing*/ )
{
}

void LTRTextHandler::endNode(const Common::Misc::GenericDocumentProperties& /*props*/)
{
}

} // end namespace
} // end namespace
