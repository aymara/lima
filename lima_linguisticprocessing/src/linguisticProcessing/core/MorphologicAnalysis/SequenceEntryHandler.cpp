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

#include "SequenceEntryHandler.h"

#include <linguisticProcessing/LinguisticProcessingCommon.h>

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

void SequenceEntryHandler::startEntry(StringsPoolIndex form)
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->startEntry(form);
  }
}

void SequenceEntryHandler::endEntry()
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->endEntry();
  }
}

void SequenceEntryHandler::foundLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->foundLingInfos(lemma,norm);
  }
}

void SequenceEntryHandler::deleteLingInfos(StringsPoolIndex lemma,StringsPoolIndex norm)
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->deleteLingInfos(lemma,norm);
  }
}

void SequenceEntryHandler::endLingInfos()
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->endLingInfos();
  }
}

void SequenceEntryHandler::foundConcatenated()
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->foundConcatenated();
  }
}

void SequenceEntryHandler::deleteConcatenated()
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->deleteConcatenated();
  }
}

void SequenceEntryHandler::foundComponent(uint64_t position, uint64_t length,StringsPoolIndex form)
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->foundComponent(position,length,form);
  }
}

void SequenceEntryHandler::endComponent()
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->endComponent();
  }
}

void SequenceEntryHandler::endConcatenated()
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->endConcatenated();
  }
}

void SequenceEntryHandler::foundAccentedForm(StringsPoolIndex form)
{
  MORPHOLOGINIT;
  LDEBUG << "SequenceEntryHandler::foundAccentedForm" << form;
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->foundAccentedForm(form);
  }
}

void SequenceEntryHandler::deleteAccentedForm(StringsPoolIndex form)
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->deleteAccentedForm(form);
  }
}

void SequenceEntryHandler::endAccentedForm()
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->endAccentedForm();
  }
}

void SequenceEntryHandler::foundProperties(LinguisticCode lings)
{
  for (std::vector<AbstractDictionaryEntryHandler*>::iterator it=m_handlers.begin();
       it!=m_handlers.end();
       it++)
  {
    (*it)->foundProperties(lings);
  }
}

} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
