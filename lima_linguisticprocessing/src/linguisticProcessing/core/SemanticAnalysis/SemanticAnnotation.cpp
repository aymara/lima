// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       SemanticAnnotation.cpp
 * @author      (romaric.besancon@cea.fr)
 * @date       Thu Sep 13 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * 
 ***********************************************************************/

#include "SemanticAnnotation.h"

namespace Lima {
namespace LinguisticProcessing {
namespace SemanticAnalysis {

std::ostream& operator << (std::ostream& os, const SemanticAnnotation& annot) {
  return os << "SemanticAnnotation("<< annot.m_type << ")";
}

int DumpSemanticAnnotation::dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const
{
  PROCESSORSLOGINIT;
  try
  {
    ga.value<SemanticAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& e)
  {
    LERROR << "This annotation is not a SemanticAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}

} // end namespace
} // end namespace
} // end namespace
