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
