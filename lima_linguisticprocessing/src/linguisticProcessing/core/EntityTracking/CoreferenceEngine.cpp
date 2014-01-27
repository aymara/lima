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
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool CoreferenceEngine::isInclude(const std::string original, const std::string currentWord) const
{
  if ((currentWord.size() > original.size()) ||(!isupper(currentWord[0])))
    return false;

  if ((currentWord.compare("is") == 0) ||
    (currentWord.compare("as") == 0) ||
    (currentWord.compare("a") == 0) ||
    (currentWord.compare("to") == 0) ||
    (currentWord.compare("and") == 0))
  {
    return false;
  }
  uint64_t comptCurrent(0);

  for (uint64_t i(0); i< original.size(); i++)
  {
    if (original[i] == currentWord[comptCurrent])
    {
      comptCurrent++;
    }
  }
  
  if (comptCurrent == currentWord.size())
    return true;
  else
    return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool CoreferenceEngine::isAcronym(const std::string original, const std::string currentWord) const
{
  for (std::vector< std::vector<std::string> >::const_iterator it = Acronyms.begin(), it_end = Acronyms.end();
       it != it_end;
       it++)
  {
    if (strcmp((*(*it).begin()).c_str(),original.c_str()) == 0)
    {
      for (std::vector<string>::const_iterator it_intern = (*it).begin(), it_int_end = (*it).end();
           it_intern != it_int_end;
           it_intern++)
      {
        if ((strcmp((*it_intern).c_str(),currentWord.c_str()) == 0) ||
            (isInclude(*it_intern,currentWord)))
          return true;
      }
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
void CoreferenceEngine::addNewForm(const std::string original, const std::string currentWord)
{
  for (std::vector< std::vector<std::string> >::iterator it = Acronyms.begin(), it_end = Acronyms.end();
       it != it_end;
       it++)
  {
    if (strcmp((*(*it).begin()).c_str(),original.c_str()) == 0)
    {
      (*it).push_back(currentWord);
      return;
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
bool CoreferenceEngine::exist(const std::string mot) const
{
  std::vector<std::string>::const_iterator iterat;
  for (std::vector< std::vector<std::string> >::const_iterator it = Acronyms.begin(), it_end = Acronyms.end();
       it != it_end;
       it++)
  {
    for (std::vector<std::string>::const_iterator it_inte=(*it).begin(), it_end_inter=(*it).end();
        it_inte != it_end_inter;
        it_inte++)
    {
      if (mot.compare(*it_inte) == 0){
        return true;}
//     iterat = find((*it).begin(),(*it).end(),mot);
//     cout <<mot<<endl;
//     if ((*iterat).compare("") != 0)
//     {
//       return true;
//     }
    }
  }
  return false;
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
vector<Token> CoreferenceEngine::searchCoreference(const Token& tok)
{
  vector<Token> newToken;
  //newToken.push_back(tok);
  /* Si le mot existe dans le vecteur des acronyms, ça ne sert à rien chercher ses acronyms parce qu'ils
     sont recherchés */
  vector<string> tempAcronyms;
  if (!exist(limastring2utf8stdstring(tok.stringForm()))) 
  {
    /* parcourir tous les noeuds */
    for(vector<Token>::const_iterator it1=allTokens.begin(), it1_end=allTokens.end();
        it1 != it1_end;
        it1++)
    {
      //string mot;
      /* Recherche dans le texte de toutes les formes de mot précédent dans tout le text */
      if ((isEqual(limastring2utf8stdstring(tok.stringForm()),limastring2utf8stdstring((*it1).stringForm()))) ||
          (isInclude(limastring2utf8stdstring(tok.stringForm()),limastring2utf8stdstring((*it1).stringForm())))/* ||
          (isAcronym(limastring2utf8stdstring(tok.stringForm()),limastring2utf8stdstring((*it1).stringForm())))*/)
      {
        tempAcronyms.push_back(limastring2utf8stdstring((*it1).stringForm()));
        newToken.push_back((*it1));
      }
    }
  }
  Acronyms.push_back(tempAcronyms);

  return newToken;
}

}
}
}
