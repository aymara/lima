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
