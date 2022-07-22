// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       NormalizationUtils.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Jun 13 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "NormalizationUtils.h"

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {

//**********************************************************************
// some helper functions
bool testMicroCategory(const std::set<LinguisticCode>* micros,
                       const Common::PropertyCode::PropertyAccessor* microAccessor,
                       const LinguisticCode properties) 
{
  LinguisticCode code = microAccessor->readValue(properties);
  return (micros->find(code) != micros->end());
}

bool testMicroCategory(const std::set<LinguisticCode>* micros,
                       const Common::PropertyCode::PropertyAccessor* microAccessor,
                       const MorphoSyntacticData* data)
{
  MorphoSyntacticData::const_iterator  
    it=data->begin(),
    it_end=data->end();
  for (; it!=it_end; it++) {
    if (testMicroCategory(micros,microAccessor,(*it).properties)) {
      return true;
    }
  }
  return false;
}

bool isInteger(Token* t) {
  const TStatus& status=t->status();
  switch(status.getNumeric()) {
  case T_INTEGER: return true;
  default: return false;
  }
}

} // end namespace
} // end namespace
