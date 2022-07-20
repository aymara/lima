// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "CoreferenceEngine.h"

#include <fstream>
#include <queue>

using namespace std;
#include "common/Data/strwstrtools.h"

using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace EntityTracking
{

bool CoreferenceEngine::isEqual(const QString& a,
                                const QString& b) const
{
  return (a == b);
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool CoreferenceEngine::isInclude(const QString& original,
                                  const QString& currentWord) const
{
  if ((currentWord.size() > original.size()) ||(currentWord[0].isUpper()))
    return false;

  if ((currentWord == "is") ||
    (currentWord == "as") ||
    (currentWord == "a") ||
    (currentWord == "to") ||
    (currentWord == "and"))
  {
    return false;
  }
  int comptCurrent = 0;

  for (int i = 0; i < original.length(); i++)
  {
    if (original[i] == currentWord[comptCurrent])
    {
      comptCurrent++;
    }
  }

  return (comptCurrent == currentWord.length());
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool CoreferenceEngine::isAcronym(const QString& original,
                                  const QString& currentWord) const
{
  for (const auto& acronym : m_acronyms)
  {
    if (*acronym.begin() == original)
    {
      for (const auto& initial : acronym)
      {
        if ( (initial == currentWord) ||
            (isInclude(initial, currentWord)))
          return true;
      }
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void CoreferenceEngine::addNewForm(const QString& original,
                                   const QString& currentWord)
{
  for (auto& acronym : m_acronyms)
  {
    if (*acronym.begin() == original)
    {
      acronym.push_back(currentWord);
      return;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool CoreferenceEngine::exist(const QString& mot) const
{
  for (const auto& acronym : m_acronyms)
  {
    for (const auto& initial : acronym)
    {
      if (mot == initial)
      {
        return true;
      }
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
vector<Token> CoreferenceEngine::searchCoreference(const Token& tok)
{
  std::vector<Token> newToken;
  //newToken.push_back(tok);
  /* Si le mot existe dans le vecteur des acronyms, ça ne sert à rien chercher
   * ses acronyms parce qu'ils
   * sont recherchés */
  std::vector<QString> tempAcronyms;
  if (!exist(tok.stringForm()))
  {
    /* parcourir tous les noeuds */
    for(const auto& token : allTokens)
    {
      //string mot;
      /* Recherche dans le texte de toutes les formes de mot précédent dans tout le text */
      if ( ( isEqual(tok.stringForm(), token.stringForm())) ||
          ( isInclude(tok.stringForm(), token.stringForm()))/* ||
          (isAcronym(tok.stringForm(), token.stringForm()))*/)
      {
        tempAcronyms.push_back(token.stringForm());
        newToken.push_back(token);
      }
    }
  }
    m_acronyms.push_back(tempAcronyms);

  return newToken;
}

std::vector<LinguisticAnalysisStructure::Token>& CoreferenceEngine::getToken()
{
  return allTokens;
}

std::vector<LinguisticAnalysisStructure::Token>& CoreferenceEngine::getAnnotations()
{
  return storedAnnotation;
}

std::vector< std::vector<QString> >& CoreferenceEngine::getAcronym()
{
  return m_acronyms;
}

void CoreferenceEngine::storeAllToken(const LinguisticAnalysisStructure::Token& tok)
{
  allTokens.push_back(tok);
}

void CoreferenceEngine::storeAnnot(const LinguisticAnalysisStructure::Token& tok)
{
  storedAnnotation.push_back(tok);
}

}
}
}
