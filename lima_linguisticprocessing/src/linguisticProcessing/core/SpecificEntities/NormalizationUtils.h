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
/************************************************************************
 *
 * @file       NormalizationUtils.h
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * Project     s2lp
 * 
 * @brief      helper functions for normalization actions
 * 
 * 
 ***********************************************************************/

#ifndef NORMALIZATIONUTILS_H
#define NORMALIZATIONUTILS_H

#include "SpecificEntitiesExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima {
namespace LinguisticProcessing {

LIMA_SPECIFICENTITIES_EXPORT bool testMicroCategory(const std::set<LinguisticCode>* micros,
                       const Common::PropertyCode::PropertyAccessor* microAccessor,
                       const LinguisticCode properties);

LIMA_SPECIFICENTITIES_EXPORT bool testMicroCategory(const std::set<LinguisticCode>* micros,
                       const Common::PropertyCode::PropertyAccessor* microAccessor,
                       const LinguisticAnalysisStructure::MorphoSyntacticData* data);

// test if numeric form (tstatus=t_integer)
LIMA_SPECIFICENTITIES_EXPORT bool isInteger(LinguisticAnalysisStructure::Token* token);

} // end namespace
} // end namespace

#endif
