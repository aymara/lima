// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief       Dependency graph text form displaying tools
  *
  * @file        SyntacticAnalysisTools.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2005 by CEA
  * @date        
  * @version     $Id$
  *
  */

#ifndef SYNTACTIC_ANALYSIS_TOOLS_H
#define SYNTACTIC_ANALYSIS_TOOLS_H

#include "SyntacticAnalysisExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "DependencyGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/ChainIdStruct.h"
#include "SyntacticData.h"

#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

class LIMA_SYNTACTICANALYSIS_EXPORT SyntacticAnalysisTools {
    
    public:
    
    static void displayChains(const LinguisticAnalysisStructure::AnalysisGraph* graph,
                              std::ostream& out);
    
    static void displayChain(const LinguisticAnalysisStructure::ChainIdStruct& chain,
                             LinguisticGraphVertex start,
                             const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                             std::ostream& out);
    
    static void displayDependancies(const SyntacticData& data,
                                    const MediaId language,
                                    std::ostream& out,
            const bool DisplayLemma=false);
    
    static void 
      displayRelationsOrderOfArguments(const SyntacticData& data,
                                       const MediaId language,
                                       std::ostream& out);


    static void displayRelationsDistanceOfArguments(const SyntacticData& data,
                                          const MediaId language,
                                          std::ostream& out);
            
    static void displayRelationsXMLFormat(const SyntacticData& data,
                                          const MediaId language,
                                           std::ostream& out);
    
};

} // end namespace 
} // end namespace 
} // end namespace 

#endif
