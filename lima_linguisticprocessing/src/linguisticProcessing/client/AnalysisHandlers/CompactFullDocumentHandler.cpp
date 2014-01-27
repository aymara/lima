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
#include "CompactFullDocumentHandler.h"

namespace Lima
{

namespace LinguisticProcessing
{

CompactFullDocumentHandler::CompactFullDocumentHandler(std::ostream* out)
    : AbstractTextualAnalysisHandler(), m_out(out)
{}


CompactFullDocumentHandler::~CompactFullDocumentHandler()
{}

void CompactFullDocumentHandler::startDocument(const Common::Misc::GenericDocumentProperties&)
{}

void CompactFullDocumentHandler::endDocument()
{}

void CompactFullDocumentHandler::endAnalysis()
{}

void CompactFullDocumentHandler::startAnalysis()
{}


void CompactFullDocumentHandler::handle(const char* buf,int length) 
{
  if (m_out != 0) m_out->write(buf,length);
}

} // closing namespace LinguisticProcessing

} // closing namespace Lima
