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
#ifndef LIMA_WORDSENSEDISAMBIGUATION_COMMONTYPEDEFS_H
#define LIMA_WORDSENSEDISAMBIGUATION_COMMONTYPEDEFS_H


#include "WordSenseAnalysisExport.h"
#include <map>
#include <string>
#include <set>
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{
  
  
typedef std::map<std::string, std::set<uint64_t> >SemanticContext;


  
class LIMA_WORDSENSEANALYSIS_EXPORT TargetWordWithContext {
  public:
    std::set<std::string> lemmas;
    LinguisticGraphVertex vertex;
    SemanticContext context;
    TargetWordWithContext(std::set<std::string>& lemmas,
        LinguisticGraphVertex v,
        SemanticContext& context)
        : lemmas(lemmas), vertex(v), context(context) {}
};
  
  
typedef enum {
    B_MOST_FREQUENT,
    B_ROMANSEVAL_MOST_FREQUENT,
    B_JAWS_MOST_FREQUENT,
    S_WSI_MRD,
    S_WSI_DS,
    S_UNKNOWN
} Mode;

typedef enum {
    M_ROMANSEVAL_SENSES,
    M_JAWS_SENSES,
    M_UNKNOWN
} Mapping;


}
}
}


#endif // LIMA_WORDSENSEDISAMBIGUATION_COMMONTYPEDEFS_H
