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
