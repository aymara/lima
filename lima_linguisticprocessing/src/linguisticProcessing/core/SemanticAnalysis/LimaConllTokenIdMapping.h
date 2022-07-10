// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMACONLLTOKENIDMAPPING_H
#define LIMACONLLTOKENIDMAPPING_H

#include "SemanticAnalysisExport.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include <map>

namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {


class LIMA_SEMANTICANALYSIS_EXPORT LimaConllTokenIdMapping : public std::map<int, std::map<int,LinguisticGraphVertex>>, public AnalysisData
{

public:
  LimaConllTokenIdMapping();
  ~LimaConllTokenIdMapping();

};


}
} // LinguisticProcessing
} // Lima

#endif
