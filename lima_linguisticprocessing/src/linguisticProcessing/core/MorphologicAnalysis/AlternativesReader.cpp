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
#include "AlternativesReader.h"
#include "MorphoSyntacticDataHandler.h"
#include "common/Data/strwstrtools.h"

using namespace Lima::LinguisticProcessing::AnalysisDict;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

AlternativesReader::AlternativesReader(
  bool confidentMode,
  bool tryDirect,
  bool tryUncapitalized,
  bool tryDesaccentedForm,
  FlatTokenizer::CharChart* charChart,
  FsaStringsPool* sp) :
    m_confidentMode(confidentMode),
    m_tryDirect(tryDirect),
    m_tryUncapitalized(tryUncapitalized),
    m_tryDesaccentedForm(tryDesaccentedForm),
    m_charChart(charChart),
    m_sp(sp)
{}

AlternativesReader::~AlternativesReader() {}

void AlternativesReader::readAlternatives(
  Token& token,
  const AnalysisDict::AbstractAnalysisDictionary& dico,
  AnalysisDict::AbstractDictionaryEntryHandler* lingInfosHandler,
  AnalysisDict::AbstractDictionaryEntryHandler* concatHandler,
  AnalysisDict::AbstractDictionaryEntryHandler* accentedHandler) const
{
  MORPHOLOGINIT;
  const LimaString& str=token.stringForm();
  LDEBUG << "AlternativesReader::readAlternatives (direct: "<<m_tryDirect<<" ; uncap: "<<m_tryUncapitalized<<" ; desac: "<<m_tryDesaccentedForm<<")" << str;
  if (m_tryDirect)
  {
    LDEBUG << "AlternativesReader::readAlternatives trying direct";
    DictionaryEntry entry=dico.getEntry(token.form(),str);
    if (!entry.isEmpty())
    {
      if (lingInfosHandler && entry.hasLingInfos())
      {
        LDEBUG << "AlternativesReader::readAlternatives direct, hasLingInfos";
        entry.parseLingInfos(lingInfosHandler);
        // if hasLingInfos, assume that infos has been read, then,
        // in confident mode, we can return
        if (m_confidentMode) return;
      }
      if (concatHandler && entry.hasConcatenated())
      {
        LDEBUG << "AlternativesReader::readAlternatives direct, hasConcatenated";
        entry.parseConcatenated(concatHandler);
        if (m_confidentMode) return;
      }
      if (accentedHandler && entry.hasAccentedForms())
      {
        LDEBUG << "AlternativesReader::readAlternatives direct, hasAccentedForms";
        entry.parseAccentedForms(accentedHandler);
        if (m_confidentMode) return;
      }
    }
  }
  if (m_tryUncapitalized && m_charChart)
  {
    LimaString lowerWord = m_charChart->toLower(str);
    LDEBUG << "AlternativesReader::readAlternatives trying lower:" << lowerWord;
    if (!(lowerWord.isEmpty())  && (lowerWord!=str))
    {
      MORPHOLOGINIT;
      LDEBUG << "add word "
      << "<marked>" << str << "</marked>"
      << "<lower>" << lowerWord << "</lower>";
      StringsPoolIndex idx=(*m_sp)[lowerWord];
      token.addOrthographicAlternatives(idx);
      DictionaryEntry entry=dico.getEntry(idx,lowerWord);
      if (!entry.isEmpty())
      {
        if (lingInfosHandler && entry.hasLingInfos())
        {
          entry.parseLingInfos(lingInfosHandler);
          // if hasLingInfos, assume that infos has been read, then,
          // in confident mode, we can return
          if (m_confidentMode) return;
        }
        if (concatHandler && entry.hasConcatenated())
        {
          entry.parseConcatenated(concatHandler);
          if (m_confidentMode) return;
        }
        if (accentedHandler && entry.hasAccentedForms())
        {
          entry.parseAccentedForms(accentedHandler);
          if (m_confidentMode) return;
        }
      }
    }
  }
  if (m_tryDesaccentedForm && m_charChart)
  {
    LimaString unmarked=m_charChart->unmark(str);
    LDEBUG << "AlternativesReader::readAlternatives trying desac:" << unmarked;
    if ((unmarked!=LimaString()) && (unmarked!=str))
    {
      MORPHOLOGINIT;
      LDEBUG << "add word " 
             << "<marked>" << Common::Misc::limastring2utf8stdstring(str) << "</marked>" 
             << "<unmarked>" << Common::Misc::limastring2utf8stdstring(unmarked) << "</unmarked>"
             << " to stringpool " << m_sp << LENDL;
      StringsPoolIndex idx=(*m_sp)[unmarked];
      LDEBUG << "-> StringPool returned index " << idx;
      token.addOrthographicAlternatives(idx);
      DictionaryEntry entry=dico.getEntry(idx,unmarked);
      LDEBUG << "entry.isEmpty:" << entry.isEmpty(); 
      
      if (!entry.isEmpty())
      {
        LDEBUG << "confident mode: " << m_confidentMode;
        LDEBUG << "lingInfosHandler: " << (void*)accentedHandler << "  entry.hasLingInfos:" << entry.hasLingInfos();
        if (lingInfosHandler && entry.hasLingInfos())
        {
          entry.parseLingInfos(lingInfosHandler);
          // if hasLingInfos, assume that infos has been read, then,
          // in confident mode, we can return
          if (m_confidentMode) return;
        }
        LDEBUG << "concatHandler: " << (void*)accentedHandler << "  entry.hasConcatenated:" << entry.hasConcatenated();
        if (concatHandler && entry.hasConcatenated())
        {
          entry.parseConcatenated(concatHandler);
          if (m_confidentMode) return;
        }
        LDEBUG << "accentedHandler: " << (void*)accentedHandler << "  entry.hasAccentedForms:" << entry.hasAccentedForms();
        if (accentedHandler && entry.hasAccentedForms())
        {
          entry.parseAccentedForms(accentedHandler);
          if (m_confidentMode) return;
        }
      }
    }
  }
  LDEBUG << "AlternativesReader::readAlternatives no alternative found";
}

}

}

}
