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
#include "BasicHandler.h"

using namespace std;

namespace Lima
{

BasicHandler::BasicHandler(
  std::ostream* out)
    : AbstractDictionaryEntryHandler(),
    m_out(out)
{}


BasicHandler::~BasicHandler()
{}


void BasicHandler::foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  *m_out << "foundLingInfos : " <<
  "l=\"" << lemma << "\" " <<
  "n=\"" << norm << "\" " << endl;
}

void BasicHandler::deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  *m_out << "deleteLingInfos : " <<
  "l=\"" << lemma << "\" " <<
  "n=\"" << norm << "\" " << endl;
}

void BasicHandler::endLingInfos()
{
  *m_out << "endLingInfos" << endl;
}

void BasicHandler::foundConcatenated()
{
  *m_out << "foundConcatenated" << endl;
}

void BasicHandler::deleteConcatenated()
{
  *m_out << "deleteConcatenated" << endl;
}

void BasicHandler::foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form)
{
  *m_out << "foundComponent : form=\"" << form << "\" pos=\"" << position << "\" len=\"" << length << "\"" << endl;
}

void BasicHandler::endComponent()
{
  *m_out << "endComponent" << endl;
}

void BasicHandler::endConcatenated()
{
  *m_out << "endConcatenated" << endl;
}

void BasicHandler::foundAccentedForm(StringsPoolIndex form)
{
  *m_out << "foundAccentedForm : form=\"" << form << "\"" << endl;
}

void BasicHandler::deleteAccentedForm(StringsPoolIndex form)
{
  *m_out << "deleteAccentedForm : form=\"" << form << "\"" << endl;
}

void BasicHandler::endAccentedForm()
{
  *m_out << "endAccentedForm" << endl;
}

void BasicHandler::foundProperties(LinguisticCode lings)
{
  *m_out << "foundProperties : " << lings << endl;
}

}
