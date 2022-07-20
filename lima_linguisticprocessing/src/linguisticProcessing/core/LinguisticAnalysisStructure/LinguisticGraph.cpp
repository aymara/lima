// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  * @brief        A graph structure for linguistic analysis
  *
  * @file         LinguisticGraph.cpp
  * @author       Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *
  */

#include "LinguisticGraph.h"

QDebug& operator<< (QDebug& qd, LinguisticGraphEdge& edge)
{
  qd << edge.m_source << "->" << edge.m_target;
  return qd;
}
