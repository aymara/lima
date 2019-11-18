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
  bool isEqual(const QString& a, const QString& b) const;
  /* return true if the current word is included in the original word */
  bool isInclude(const QString& original, const QString& currentWord) const;
  /* return true if the current word is an acronym of the original word */
  bool isAcronym(const QString& original, const QString& currentWord) const;

  void addNewForm(const QString& original, const QString& currentWord);
  bool exist(const QString& original) const;

  std::vector<LinguisticAnalysisStructure::Token> searchCoreference(
    const LinguisticAnalysisStructure::Token& tok);

  void storeSpecificEntity (const SpecificEntities::SpecificEntityAnnotation* se) const;

  std::vector<LinguisticAnalysisStructure::Token>& getToken();
  std::vector<LinguisticAnalysisStructure::Token>& getAnnotations();
  std::vector< std::vector<QString> >& getAcronym();
  void storeAllToken(const LinguisticAnalysisStructure::Token& token);
  void storeAnnot(const LinguisticAnalysisStructure::Token& token);

private:

  std::vector< std::vector<QString> > m_acronyms;  // the vector of acronyms
  std::vector<LinguisticAnalysisStructure::Token> allTokens;       /* all token in the graph */
  std::vector<LinguisticAnalysisStructure::Token> storedAnnotation;

};

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif // COREFERENCEENGINE_H
