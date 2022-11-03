// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
