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
