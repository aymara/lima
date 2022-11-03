// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
