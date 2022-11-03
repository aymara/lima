// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
