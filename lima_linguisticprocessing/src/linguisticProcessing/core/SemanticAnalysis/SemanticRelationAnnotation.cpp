// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "SemanticRelationAnnotation.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{


int DumpSemanticRelation::dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const
{
  PROCESSORSLOGINIT;
  try
  {
    ga.value<SemanticRelationAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& e)
  {
    LERROR << "This annotation is not a SemanticRelationAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}

} // closing namespace SemanticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
