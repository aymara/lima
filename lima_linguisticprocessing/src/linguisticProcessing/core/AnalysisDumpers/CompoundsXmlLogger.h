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
