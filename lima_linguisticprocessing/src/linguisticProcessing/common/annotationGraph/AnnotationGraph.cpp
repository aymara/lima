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
/** @brief       A graph that stores any data (annotations) referencing
  *              primarily nodes of a text anlaysis
  *
  * @file        AnnotationGraph.cpp
  * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *              Copyright (c) 2012 by CEA
  * @date        Created on Nov, 8 2004
  * @version     $Id:  $
  *
  */

#include "AnnotationGraph.h"

namespace Lima {
namespace Common {
namespace AnnotationGraphs {

  LIMA_ANNOTATIONGRAPH_EXPORT QDebug& operator<< (QDebug& os, const AnnotationGraphEdge& e)
  {
    os << e.m_source << "->" << e.m_target;
    return os;
  }

  
} // closing namespace AnnotationGraphs
} // closing namespace Common
} // closing namespace Lima
