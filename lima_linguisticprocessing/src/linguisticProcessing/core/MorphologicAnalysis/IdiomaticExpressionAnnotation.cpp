// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "IdiomaticExpressionAnnotation.h"

#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace Lima::LinguisticProcessing::Automaton;


namespace Lima
{

namespace LinguisticProcessing
{

namespace MorphologicAnalysis
{

IdiomaticExpressionAnnotation::IdiomaticExpressionAnnotation(const RecognizerMatch& entity)
{
  m_vertices.reserve(entity.size());
  Automaton::RecognizerMatch::const_iterator it, it_end;
  it = entity.begin(); it_end = entity.end();
  for (; it != it_end; it++)
  {
    if ( (*it).m_elem.second )
    {
      m_vertices.push_back((*it).m_elem.first);
    }
  }
}


IdiomaticExpressionAnnotation::~IdiomaticExpressionAnnotation()
{
}

void IdiomaticExpressionAnnotation::dump(std::ostream& os)
{
  os << "ie_vertices:";

  //std::vector< const uint64_t >
  std::vector< LinguisticGraphVertex >::const_iterator it, it_end;
  it = m_vertices.begin(); it_end = m_vertices.end();
  if (it != it_end)
  {
    os << *it;
    it++;
  }
  for (; it != it_end; it++)
  {
    os << "," << *it;
  }
}

  int DumpIdiomaticExpressionAnnotation::dump(
      std::ostream& os,
      Common::AnnotationGraphs::GenericAnnotation& ga) const
{
  MORPHOLOGINIT;
  try
  {
    ga.value<IdiomaticExpressionAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& )
  {
    LERROR << "This annotation is not a IdiomaticExpressionAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}


}

}

}
