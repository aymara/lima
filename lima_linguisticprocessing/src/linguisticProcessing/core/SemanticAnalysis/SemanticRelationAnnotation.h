// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef PATTERNPROCESSING_SEMANTICANALYSIS_H
#define PATTERNPROCESSING_SEMANTICANALYSIS_H

#include "SemanticAnalysisExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

/**
 * An annotation between two annotation graph vertices denoting the semantic
 * relation(s) holding between the two tokens. 
 * 
 * If there is more than one relation between the two vertices, the value of 
 * the type should be the list of the relation types concatenated with a '-'.
 */
class LIMA_SEMANTICANALYSIS_EXPORT SemanticRelationAnnotation
{
public:
  SemanticRelationAnnotation(const std::string& type) : m_type(type) {}

  virtual ~SemanticRelationAnnotation() {}

  inline std::string& type();
  inline const std::string& type() const;
  inline void type(const std::string& newType);
  inline void dump(std::ostream& os);

private:

protected:
  std::string m_type;
};

inline std::string& SemanticRelationAnnotation::type()
{
  return m_type;
}
inline const std::string& SemanticRelationAnnotation::type() const
{
  return m_type;
}
inline void SemanticRelationAnnotation::type(const std::string& newType)
{
  m_type = newType;
}
inline void SemanticRelationAnnotation::dump(std::ostream& os)
{
  os << m_type;
}


class LIMA_SEMANTICANALYSIS_EXPORT DumpSemanticRelation : public Common::AnnotationGraphs::AnnotationData::Dumper
{
  public:
    virtual int dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const override;
};


} // closing namespace SemanticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // PATTERNPROCESSING_SEMANTICANALYSIS_H
