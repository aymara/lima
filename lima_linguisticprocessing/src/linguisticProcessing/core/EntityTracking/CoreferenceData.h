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
