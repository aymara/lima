// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDict {


inline StringsPoolIndex AbstractDictionaryEntry::getEntryId() const
{
  return m_entryId;
}

inline bool AbstractDictionaryEntry::isFinal() const
{
  return m_isFinal;
}

inline bool AbstractDictionaryEntry::isEmpty() const
{
  return m_isEmpty;
}

inline bool AbstractDictionaryEntry::hasLingInfos() const
{
  return m_hasLingInfos;
}

inline bool AbstractDictionaryEntry::hasConcatenated() const
{
  return m_hasConcatenated;
}

inline bool AbstractDictionaryEntry::hasAccentedForms() const
{
  return m_hasAccentedForms;
}

inline StringsPoolIndex DictionaryEntry::getEntryId() const
{
  return m_delegate->getEntryId();
}

inline bool DictionaryEntry::isFinal() const
{
  return m_delegate->isFinal();
}

inline bool DictionaryEntry::isEmpty() const
{
  return (!m_delegate || m_delegate->isEmpty());
}

inline bool DictionaryEntry::hasLingInfos() const
{
  return m_delegate->hasLingInfos();
}

inline void DictionaryEntry::parseLingInfos(AbstractDictionaryEntryHandler* handler) const
{
  return m_delegate->parseLingInfos(handler);
}

inline bool DictionaryEntry::hasConcatenated() const
{
  return m_delegate->hasConcatenated();
}

inline void DictionaryEntry::parseConcatenated(AbstractDictionaryEntryHandler* handler) const
{
  return m_delegate->parseConcatenated(handler);
}

inline bool DictionaryEntry::hasAccentedForms() const
{
  return m_delegate->hasAccentedForms();
}

inline void DictionaryEntry::parseAccentedForms(AbstractDictionaryEntryHandler* handler) const
{
  return m_delegate->parseAccentedForms(handler);
}

} // AnalysisDict
} // LinguisticProcessing
} // Lima
