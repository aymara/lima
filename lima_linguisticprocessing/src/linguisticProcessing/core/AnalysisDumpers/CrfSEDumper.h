/*
    Copyright 2002-2014 CEA LIST

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

#ifndef LIMA_LINGUISTICPROCESSING_CRFSEDUMPER_H
#define LIMA_LINGUISTICPROCESSING_CRFSEDUMPER_H

#include "AnalysisDumpersExport.h"

#include "WordFeatureExtractor.h"

//#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"
//#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
//#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
//#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
//#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define CRFSEDUMPER_CLASSID "CrfSEDumper"

class CrfSEDumperPrivate;

class LIMA_ANALYSISDUMPERS_EXPORT CrfSEDumper : public AbstractTextualAnalysisDumper 
{

public:
  CrfSEDumper();

  virtual ~CrfSEDumper();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  virtual LimaStatusCode process(
    AnalysisContent& analysis) const;

protected:
 
  CrfSEDumperPrivate *m_d;

};
}
}
}


#endif
