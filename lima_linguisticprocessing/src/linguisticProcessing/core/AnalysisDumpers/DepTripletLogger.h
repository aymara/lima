// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   A logger used to output dependency relations in a parsable manner     *
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSISDEPTRIPLELOGGER_H
#define LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSISDEPTRIPLELOGGER_H

#include "AnalysisDumpersExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"

#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "common/MediaticData/mediaticData.h"

namespace Lima
{

namespace Common
{
  namespace BagOfWords
  {
    class BoWToken;
    class BoWTerm;
  }
  namespace AnnotationGraphs
  {
    class AnnotationData;
  }
}

namespace LinguisticProcessing
{
namespace Compounds
{
  class BowGenerator;
}

namespace LinguisticAnalysisStructure
{
  class AnalysisGraph;
}

namespace SyntacticAnalysis
{


#define DEPTRIPLETLOGGER_CLASSID "DepTripletLogger"

typedef std::map< LinguisticGraphVertex, std::set<const Common::BagOfWords::BoWTerm*> > VxToTermsMap ;

/**
  * A logger used to output dependency relations in a parsable manner
  * (term1-dependance-term2)
  * @author Gael de Chalendar
  * @author Olivier Mesnard
  */
class LIMA_ANALYSISDUMPERS_EXPORT DepTripletLogger : public AbstractLinguisticLogger
{
public:
  DepTripletLogger();

  virtual ~DepTripletLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  void dumpDepWithCompounds(
          std::ostream& outputStream,
                             const LinguisticGraph& anagraph,
                             const LinguisticGraph& posgraph,
                             const SyntacticAnalysis::SyntacticData* syntData,
          const Common::AnnotationGraphs::AnnotationData* annotationData,
          LinguisticGraphVertex src,
          LinguisticGraphVertex dest,
          const std::string& rel,
          const VxToTermsMap& compoundsHeads) const;

  void dumpDependencyRelations(std::ostream& outputStream,
                               const LinguisticGraph& anagraph,
                               const LinguisticGraph& posgraph,
                               const SyntacticAnalysis::SyntacticData* syntData,
                               const Common::AnnotationGraphs::AnnotationData* annotationData,
                               const VxToTermsMap& compoundsHeads) const;

  VxToTermsMap getCompoundsHeads(
      const Common::AnnotationGraphs::AnnotationData* annotationData,
      const SyntacticAnalysis::SyntacticData* syntacticData,
                                  AnalysisContent& analysis,
                                  LinguisticAnalysisStructure::AnalysisGraph* posAgraph,
                                  const LinguisticGraph& anagraph,
                                  const LinguisticGraph& posgraph) const;

  void getCompoundsHeads(VxToTermsMap&, const Common::BagOfWords::BoWTerm* bt) const;

  void collectVertices(
        std::set<LinguisticGraphVertex>& theSet,
                                         const Common::BagOfWords::BoWToken* term
                                       ) const;

  MediaId m_language;
  std::set<std::string> m_relation_names;

  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;

  Compounds::BowGenerator* m_bowGenerator;
};

/*
struct lFullToken
{
  inline bool operator()(const Data::FullToken* f1, const Data::FullToken* f2) const
  {
    if (f1->getPosition()!=f2->getPosition()) return f1->getPosition()<f2->getPosition();
    return f1->getLength()<f2->getLength();
  }
};
*/

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_SYNTACTICANALYSISDEPTRIPLELOGGER_H
