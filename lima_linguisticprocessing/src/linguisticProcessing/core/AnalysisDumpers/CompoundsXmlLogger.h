// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef COMPOUNDSXMLLOGGERSPECIFICENTITIESXMLLOGGER_H
#define COMPOUNDSXMLLOGGERSPECIFICENTITIESXMLLOGGER_H

#include "AnalysisDumpersExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"

namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{
  class AnnotationData;
}
namespace BagOfWords
{
  class BoWToken;
  class BoWTerm;
}
}
namespace LinguisticProcessing
{
namespace Compounds
{
class BowGenerator;

/**
@author Gael de Chalendar
*/
#define COMPOUNDSXMLLOGGER_CLASSID "CompoundsXmlLogger"
class LIMA_ANALYSISDUMPERS_EXPORT CompoundsXmlLogger : public AbstractLinguisticLogger
{
public:
  CompoundsXmlLogger();

  virtual ~CompoundsXmlLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

private:
  void dumpLimaData(
      std::ostream& os,
      uint64_t sentNum,
      const LinguisticGraphVertex begin,
      const LinguisticGraphVertex end,
                      const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                      const LinguisticAnalysisStructure::AnalysisGraph* posgraph,
                      const SyntacticAnalysis::SyntacticData* syntacticData,
      const Common::AnnotationGraphs::AnnotationData* annotationData,
      const uint64_t offsetBegin=0) const;

  uint64_t getPosition(
      const uint64_t position,
      const uint64_t offsetBegin) const;


  void outputCompound(
      std::ostream& os,
      const Common::BagOfWords::BoWToken* compound,
      const uint64_t offsetBegin) const;

  MediaId m_language;

  BowGenerator* m_bowGenerator;
};

} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima

#endif
