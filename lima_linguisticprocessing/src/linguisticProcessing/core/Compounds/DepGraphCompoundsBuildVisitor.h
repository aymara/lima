// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr> \n
  * @date        Created on Nov, 19 2003
  */

#ifndef LIMA_SYNTACTICANALYSIS_DEPGRAPHCOMPOUNDSBUILDVISITOR_H
#define LIMA_SYNTACTICANALYSIS_DEPGRAPHCOMPOUNDSBUILDVISITOR_H

#include "CompoundsExport.h"
#include "CompoundTokenAnnotation.h"

#include "common/misc/fsaStringsPool.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"
#include "linguisticProcessing/core/Automaton/recognizer.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

// To use tuples
#include "boost/tuple/tuple.hpp"
//Comparison operators can be included with:
#include "boost/tuple/tuple_comparison.hpp"
// To use tuple input and output operators,
#include "boost/tuple/tuple_io.hpp"

#include <string>

namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{
  class AnnotationData;
}
}


namespace LinguisticProcessing
{
namespace Compounds
{
class CompoundsBuilderFromSyntacticData;

class DepGraphCompoundsBuildVisitorPrivate;

class LIMA_COMPOUNDS_EXPORT DepGraphCompoundsBuildVisitor
{
public:


  DepGraphCompoundsBuildVisitor(const Lima::LinguisticProcessing::Compounds::CompoundsBuilderFromSyntacticData* cpbfsd,
                                MediaId language,
                                SyntacticAnalysis::SyntacticData* syntacticData,
                                DependencyGraph* depGraph,
                                LinguisticAnalysisStructure::AnalysisGraph* iter,
                                LinguisticGraphVertex startVertex,
                                LinguisticGraphVertex stopVertex,
//                                 uint64_t depGraphMaxBranchingFactor,
                                Common::AnnotationGraphs::AnnotationData* annotationData,
                                bool useChains = false);

  virtual ~DepGraphCompoundsBuildVisitor();

  void discover_vertex(LinguisticGraphVertex v);

private:
  DepGraphCompoundsBuildVisitorPrivate* m_d;

}; // end of class DepGraphCompoundsBuildVisitor


} // closing namespace Compounds
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_SYNTACTICANALYSIS_DEPGRAPHCOMPOUNDSBUILDVISITOR_H
