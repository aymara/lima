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
