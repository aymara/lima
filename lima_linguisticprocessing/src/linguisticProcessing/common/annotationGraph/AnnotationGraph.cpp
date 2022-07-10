// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
