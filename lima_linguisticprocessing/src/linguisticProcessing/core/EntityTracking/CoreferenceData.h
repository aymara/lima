// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef COREFERENCEDATA_H
#define COREFERENCEDATA_H

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
  
class LIMA_ENTITYTRACKING_EXPORT CoreferenceData :
  public AnalysisData,
  public std::vector< std::vector<LinguisticAnalysisStructure::Token> >
{
public:
  CoreferenceData();
  ~CoreferenceData();

private:
  
};

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif // COREFERENCEENGINE_H
