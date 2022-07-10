// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "FakeHandler.h"

namespace Lima
{

namespace LinguisticProcessing
{

FakeHandler::FakeHandler()
    : AbstractTextualAnalysisHandler()
{}


FakeHandler::~FakeHandler()
{}

void FakeHandler::startDocument(const Common::Misc::GenericDocumentProperties& /*props*/)
{
  // do nothing
}


/** notify the end of the document */
void FakeHandler::endDocument()
{
  // do nothing
}


void FakeHandler::endAnalysis() {}
void FakeHandler::startAnalysis() {}

/** gives content. Content is a serialized form of the expected resultType */
void FakeHandler::handle(const char[],int /*length*/) 
{
  // do nothing
}


}

}
