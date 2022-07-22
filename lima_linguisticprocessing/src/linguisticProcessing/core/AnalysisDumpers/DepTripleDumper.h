// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSDEPTRIPLEDUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSDEPTRIPLEDUMPER_H

#include "AnalysisDumpersExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "common/MediaticData/mediaticData.h"

#include <boost/shared_ptr.hpp>

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

namespace AnalysisDumpers
{


#define DEPTRIPLEDUMPER_CLASSID "DepTripleDumper"

typedef std::map< LinguisticGraphVertex, std::set< boost::shared_ptr< Common::BagOfWords::BoWTerm > > > VxToTermsMap ;

/**
@author Olivier Mesnard
dumper pour sortir les triplets (term1-dependance-term2)
*/
class LIMA_ANALYSISDUMPERS_EXPORT DepTripleDumper : public MediaProcessUnit
{
public:
  DepTripleDumper();

  virtual ~DepTripleDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

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
                               LinguisticGraphVertex src,
                               LinguisticGraphVertex dest,
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

  void getCompoundsHeads(VxToTermsMap&, boost::shared_ptr< Common::BagOfWords::BoWTerm > bt) const;

  void collectVertices(
        std::set<LinguisticGraphVertex>& theSet,
        boost::shared_ptr< Common::BagOfWords::BoWToken > term
      ) const;

  MediaId m_language;
  std::set<std::string> m_relation_names;

  const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
  const Common::PropertyCode::PropertyAccessor* m_microAccessor;

  Compounds::BowGenerator* m_bowGenerator;
  std::string m_handler;
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

} // closing namespace AnalysisDumpers
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSDEPTRIPLEDUMPER_H
