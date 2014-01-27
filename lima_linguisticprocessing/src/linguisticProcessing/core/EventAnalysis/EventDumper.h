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
#ifndef LIMA_LINGUISTICPROCESSING_EVENTDUMPER_H
#define LIMA_LINGUISTICPROCESSING_EVENTDUMPER_H

#include "EventAnalysisExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
//#include "common/MediaProcessors/MediaAnalysisDumper.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"


namespace Lima
{
  namespace Common
  {
    namespace AnnotationGraphs
    {
      class AnnotationData;
    }
  }
  namespace LinguisticProcessing
  {
  }
}

namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {



#define EVENTDUMPER_CLASSID "EventDumper"


/**
@author Faiza Gara
dumper pour sortir les evenements qui ont ete reconnus dans le texte
*/
// class EventDumper : public MediaAnalysisDumper
class LIMA_EVENTANALISYS_EXPORT EventDumper : public MediaProcessUnit


{
public:
  EventDumper();

  virtual ~EventDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

private:
  MediaId m_language;
  std::string m_file;
  std::string m_handler;
};


} // closing namespace EventAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_ANNOTATION_EVENTDUMPER_H
