// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#ifdef DEBUG_LP
  MORPHOLOGINIT;
#endif
  const LimaString& str=token.stringForm();
#ifdef DEBUG_LP
  LDEBUG << "AlternativesReader::readAlternatives (direct: " << m_tryDirect
          << " ; uncap: " << m_tryUncapitalized << " ; desac: "
          << m_tryDesaccentedForm << ")" << str;
#endif
  if (m_tryDirect)
  {
#ifdef DEBUG_LP
    LDEBUG << "AlternativesReader::readAlternatives trying direct";
#endif
    DictionaryEntry entry=dico.getEntry(token.form(),str);
    if (!entry.isEmpty())
    {
      if (lingInfosHandler && entry.hasLingInfos())
      {
#ifdef DEBUG_LP
        LDEBUG << "AlternativesReader::readAlternatives direct, hasLingInfos";
#endif
        entry.parseLingInfos(lingInfosHandler);
        // if hasLingInfos, assume that infos has been read, then,
        // in confident mode, we can return
        if (m_confidentMode) return;
      }
      if (concatHandler && entry.hasConcatenated())
      {
#ifdef DEBUG_LP
        LDEBUG << "AlternativesReader::readAlternatives direct, hasConcatenated";
#endif
        entry.parseConcatenated(concatHandler);
        if (m_confidentMode) return;
      }
      if (accentedHandler && entry.hasAccentedForms())
      {
#ifdef DEBUG_LP
        LDEBUG << "AlternativesReader::readAlternatives direct, hasAccentedForms";
#endif
        entry.parseAccentedForms(accentedHandler);
        if (m_confidentMode) return;
      }
    }
  }
  if (m_tryUncapitalized && m_charChart)
  {
    LimaString lowerWord = m_charChart->toLower(str);
#ifdef DEBUG_LP
    LDEBUG << "AlternativesReader::readAlternatives trying lower:" << lowerWord;
#endif
    if (!(lowerWord.isEmpty())  && (lowerWord!=str))
    {
#ifdef DEBUG_LP
      LDEBUG << "add word "
      << "<marked>" << str << "</marked>"
      << "<lower>" << lowerWord << "</lower>";
#endif
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
#ifdef DEBUG_LP
          LDEBUG << "lower word" << lowerWord << "found";
#endif
          if (m_confidentMode) return;
        }
        if (concatHandler && entry.hasConcatenated())
        {
          entry.parseConcatenated(concatHandler);
#ifdef DEBUG_LP
          LDEBUG << "lower word" << lowerWord << "concatenated found";
#endif
          if (m_confidentMode) return;
        }
        if (accentedHandler && entry.hasAccentedForms())
        {
          entry.parseAccentedForms(accentedHandler);
#ifdef DEBUG_LP
          LDEBUG << "lower word" << lowerWord << "accented found";
#endif
          if (m_confidentMode) return;
        }
      }
    }
  }
  if (m_tryDesaccentedForm && m_charChart)
  {
    LimaString unmarked=m_charChart->unmark(str);
#ifdef DEBUG_LP
    LDEBUG << "AlternativesReader::readAlternatives trying unmarked:" << unmarked;
#endif
    if ((unmarked!=LimaString()) && (unmarked!=str))
    {
#ifdef DEBUG_LP
      LDEBUG << "add word " 
             << "<marked>" << Common::Misc::limastring2utf8stdstring(str) << "</marked>" 
             << "<unmarked>" << Common::Misc::limastring2utf8stdstring(unmarked) << "</unmarked>"
             << " to stringpool " << m_sp;
#endif
      StringsPoolIndex idx=(*m_sp)[unmarked];
#ifdef DEBUG_LP
      LDEBUG << "-> StringPool returned index " << idx;
#endif
      token.addOrthographicAlternatives(idx);
      DictionaryEntry entry=dico.getEntry(idx,unmarked);
#ifdef DEBUG_LP
      LDEBUG << "entry.isEmpty:" << entry.isEmpty(); 
#endif
      
      if (!entry.isEmpty())
      {
 #ifdef DEBUG_LP
       LDEBUG << "confident mode: " << m_confidentMode;
        LDEBUG << "lingInfosHandler: " << (void*)accentedHandler << "  entry.hasLingInfos:" << entry.hasLingInfos();
#endif
        if (lingInfosHandler && entry.hasLingInfos())
        {
          entry.parseLingInfos(lingInfosHandler);
          // if hasLingInfos, assume that infos has been read, then,
          // in confident mode, we can return
          if (m_confidentMode) return;
        }
#ifdef DEBUG_LP
        LDEBUG << "concatHandler: " << (void*)accentedHandler << "  entry.hasConcatenated:" << entry.hasConcatenated();
 #endif
       if (concatHandler && entry.hasConcatenated())
        {
          entry.parseConcatenated(concatHandler);
          if (m_confidentMode) return;
        }
#ifdef DEBUG_LP
        LDEBUG << "accentedHandler: " << (void*)accentedHandler << "  entry.hasAccentedForms:" << entry.hasAccentedForms();
#endif
        if (accentedHandler && entry.hasAccentedForms())
        {
          entry.parseAccentedForms(accentedHandler);
          if (m_confidentMode) return;
        }
      }
    }
    if (token.status().getAlphaCapital()==LinguisticAnalysisStructure::T_ACRONYM)
    {
      StringsPoolIndex idx=(*m_sp)[unmarked];
      token = Token(idx, unmarked, token.position(), token.length(),token.status());
#ifdef DEBUG_LP
      LDEBUG << "AlternativesReader::readAlternatives is  an acronym; using simpler form" << unmarked;
#endif
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "AlternativesReader::readAlternatives no alternative found;";
#endif
}

}

}

}
