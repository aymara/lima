// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
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
  *m_out << "foundProperties : " << lings.toString() << endl;
}

}
