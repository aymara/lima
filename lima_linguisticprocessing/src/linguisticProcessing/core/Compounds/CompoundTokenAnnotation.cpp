// Copyright 2002-2022 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "CompoundTokenAnnotation.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

using namespace Lima::LinguisticProcessing::Automaton;

namespace Lima
{

namespace LinguisticProcessing
{

namespace Compounds
{

ConceptModifier::ConceptModifier() :
    m_realization(0),
    m_conceptType(std::numeric_limits<uint32_t>::max()),
    m_referencedAnnotations()
{
}

ConceptModifier::ConceptModifier(
    const StringsPoolIndex& realization,
    const Common::MediaticData::ConceptType type,
    const std::vector< uint64_t >& references) :
        m_realization(realization),
        m_conceptType(type),
        m_referencedAnnotations(references)
{
}

ConceptModifier::~ConceptModifier()
{
}

void ConceptModifier::dump(std::ostream& os,const FsaStringsPool& sp) const
{
  os << "<conceptModifier real=\""
      <<Common::Misc::limastring2utf8stdstring(sp[m_realization])
      <<"\" type=\""
//      << Common::MediaticData::MediaticData::single().getConceptName(m_conceptType)
      << m_conceptType
      <<"\" refs=\"";
  std::vector< uint64_t >::const_iterator it, it_end;
  it = m_referencedAnnotations.begin(); it_end = m_referencedAnnotations.end();
  if (it != it_end)
  {
    os << *it;
    it++;
  }
  for (; it != it_end; it++)
  {
    os << "," << *it;
  }
  os << "\" />";
}


void CompoundTokenAnnotation::dump(std::ostream& os,const FsaStringsPool& sp)
{
  os << "<cpdtok>" << std::endl;
  CompoundTokenAnnotation::const_iterator it, it_end;
  it = begin(); it_end = end();
  for (; it != it_end; it++)
  {
    it->dump(os,sp);
    os << std::endl;
  }
  os << "</cpdtok>" << std::endl;
}

int DumpCompoundTokenAnnotation::dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const
{
  PROCESSORSLOGINIT;
  try
  {
    ga.value<CompoundTokenAnnotation>().dump(os,m_sp);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& )
  {
    LERROR << "This annotation is not a CompoundTokenAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}


} // Compounds
} // LinguisticProcessing
} // Lima

