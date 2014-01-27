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

#ifndef COREFERENCEENGINE_H
#define COREFERENCEENGINE_H

#include "EntityTrackingExport.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include <vector>
#include <string.h>

namespace Lima
{
namespace LinguisticProcessing
{
namespace EntityTracking
{
class LIMA_ENTITYTRACKING_EXPORT CoreferenceEngine
{
public:
    /* test if the two string are equals */
  inline bool isEqual(const std::string a,const std::string b) const {if (strcmp(a.c_str(),b.c_str())==0) return true; else return false;}
  /* return true if the current word is included in the original word */
  bool isInclude(const std::string original, const std::string currentWord) const;
  /* return true if the current word is an acronym of the original word */
  bool isAcronym(const std::string original, const std::string currentWord) const;

  void addNewForm(const std::string original, const std::string currentWord);
  bool exist(const std::string original) const;

  std::vector<LinguisticAnalysisStructure::Token> searchCoreference(const LinguisticAnalysisStructure::Token& tok);

  void storeSpecificEntity (const Lima::LinguisticProcessing::
        SpecificEntities::SpecificEntityAnnotation * se) const;

  inline std::vector<LinguisticAnalysisStructure::Token>& getToken(){return allTokens;}
  inline std::vector<LinguisticAnalysisStructure::Token>& getAnnotations() {return storedAnnotation;}
  inline std::vector< std::vector<std::string> > getAcronym() {return Acronyms;}
  inline void storeAllToken(const LinguisticAnalysisStructure::Token);
  inline void storeAnnot(const LinguisticAnalysisStructure::Token);

private:

  std::vector< std::vector<std::string> > Acronyms;  // the vector of acronyms
  std::vector<LinguisticAnalysisStructure::Token> allTokens;       /* all token in the graph */
  std::vector<LinguisticAnalysisStructure::Token> storedAnnotation;
 
};

/*inline void CoreferenceEngine::setToken(const vector<Token> &tok)
{
  for(vector<Token>::const_iterator it1=tok.begin(), it1_end=tok.end();
      it1 != it1_end;
      it1++)
  {
    allTokens.push_back(*it1);
  }
}
*/
inline void CoreferenceEngine::storeAllToken(const LinguisticAnalysisStructure::Token tok)
{
  allTokens.push_back(tok);
}

inline void CoreferenceEngine::storeAnnot(const LinguisticAnalysisStructure::Token tok)
{
  storedAnnotation.push_back(tok);
}

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif // COREFERENCEENGINE_H
