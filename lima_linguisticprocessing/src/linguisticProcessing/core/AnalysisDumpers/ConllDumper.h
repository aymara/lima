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
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTDUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSTEXTDUMPER_H

#include "AnalysisDumpersExport.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractTextualAnalysisDumper.h"

#include "common/MediaProcessors/MediaProcessUnit.h"

#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include "linguisticProcessing/common/PropertyCode/PropertyAccessor.h"

namespace Lima
{
namespace Common {
  namespace AnnotationGraphs {
    class AnnotationData;
  }
}
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{
  class AnalysisGraph;
}
namespace SpecificEntities {
  class SpecificEntityAnnotation;
}
namespace AnalysisDumpers
{

#define CONLLDUMPER_CLASSID "ConllDumper"

/**
@author Gael de Chalendar
*/
class LIMA_ANALYSISDUMPERS_EXPORT ConllDumper : public AbstractTextualAnalysisDumper
{
public:
  ConllDumper();

  virtual ~ConllDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager);

  LimaStatusCode process(
    AnalysisContent& analysis) const;
    
private:
  MediaId m_language;
  std::string m_property;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  const Common::PropertyCode::PropertyManager* m_propertyManager;
  const Common::PropertyCode::PropertyManager* m_timeManager; //Ajout
  const Common::PropertyCode::PropertyAccessor* m_timeAccessor; //Ajout

  std::string m_graph;
  std::string m_sep;
  std::string m_sepPOS;
  std::string m_verbTenseFlag; //Ajout

  // private member functions
  std::string outputVertex(std::ostream& out,
                    const LinguisticAnalysisStructure::Token* ft,
                    const std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*>& data,
                    const FsaStringsPool& sp,
                    LinguisticGraphVertex v, 
                    const Common::AnnotationGraphs::AnnotationData* annotationData,
                    uint64_t offset,
                    const std::string& previousNE,
                    const std::map<int, std::string>& positions) const;

  void outputString(std::ostream& out,const std::string& str) const;
                                      
  std::string outputSpecificEntity(std::ostream& out,
                            const SpecificEntities::SpecificEntityAnnotation* se,
                            const std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*>& data,
                            const FsaStringsPool& sp,
                            const uint64_t offset,
                            const std::string& previousNE,
                            const std::map<int, std::string>& positions) const;


};

struct lTokenPosition
{
  inline bool operator()(const LinguisticAnalysisStructure::Token* f1, const LinguisticAnalysisStructure::Token* f2) const
  {
    if (f1->position()!=f2->position()) return f1->position()<f2->position();
    return f1->length()<f2->length();
  }
};

}

}

}

#endif
