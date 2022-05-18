// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
