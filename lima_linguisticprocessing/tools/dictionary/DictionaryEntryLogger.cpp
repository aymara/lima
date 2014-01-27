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
#include "DictionaryEntryLogger.h"
#include "common/Data/strwstrtools.h"

using namespace std;
using namespace Lima::Common::Misc;
using namespace Lima::Common::PropertyCode;

namespace Lima
{

DictionaryEntryLogger::DictionaryEntryLogger(
  std::ostream* out,
  const FsaStringsPool* sp,
  const Common::PropertyCode::PropertyCodeManager* manager)
    : AbstractDictionaryEntryHandler(),
    m_out(out),
    m_sp(sp),
    m_manager(manager)
{}


DictionaryEntryLogger::~DictionaryEntryLogger()
{}


void DictionaryEntryLogger::foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  *m_out << "foundLingInfos : " <<
  "l=\"" << limastring2utf8stdstring((*m_sp)[lemma]) << "\" " <<
  "n=\"" << limastring2utf8stdstring((*m_sp)[norm]) << "\" " << endl;
}

void DictionaryEntryLogger::deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  *m_out << "deleteLingInfos : " <<
  "l=\"" << limastring2utf8stdstring((*m_sp)[lemma]) << "\" " <<
  "n=\"" << limastring2utf8stdstring((*m_sp)[norm]) << "\" " << endl;
}

void DictionaryEntryLogger::endLingInfos()
{
  *m_out << "endLingInfos" << endl;
}

void DictionaryEntryLogger::foundConcatenated()
{
  *m_out << "foundConcatenated" << endl;
}

void DictionaryEntryLogger::deleteConcatenated()
{
  *m_out << "deleteConcatenated" << endl;
}

void DictionaryEntryLogger::foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form)
{
  *m_out << "foundComponent : form=\"" << limastring2utf8stdstring((*m_sp)[form]) << "\" pos=\"" << position << "\" len=\"" << length << "\"" << endl;
}

void DictionaryEntryLogger::endComponent()
{
  *m_out << "endComponent" << endl;
}

void DictionaryEntryLogger::endConcatenated()
{
  *m_out << "endConcatenated" << endl;
}

void DictionaryEntryLogger::foundAccentedForm(StringsPoolIndex form)
{
  *m_out << "foundAccentedForm : form=\"" << limastring2utf8stdstring((*m_sp)[form]) << "\"" << endl;
}

void DictionaryEntryLogger::deleteAccentedForm(StringsPoolIndex form)
{
  *m_out << "deleteAccentedForm : form=\"" << limastring2utf8stdstring((*m_sp)[form]) << "\"" << endl;
}

void DictionaryEntryLogger::endAccentedForm()
{
  *m_out << "endAccentedForm" << endl;
}

void DictionaryEntryLogger::foundProperties(LinguisticCode lings)
{
  *m_out << "foundProperties : ";
  for (std::map<std::string,Common::PropertyCode::PropertyManager>::const_iterator propItr=m_manager->getPropertyManagers().begin();
       propItr!=m_manager->getPropertyManagers().end();
       propItr++)
  {
    if (! propItr->second.getPropertyAccessor().empty(lings))
    {
      *m_out << propItr->first << "=" << propItr->second.getPropertySymbolicValue(lings) << ", ";
    }
  }
  *m_out << endl;
}

}
