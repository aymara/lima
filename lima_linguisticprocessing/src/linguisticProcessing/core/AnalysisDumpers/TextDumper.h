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

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

#define TEXTDUMPER_CLASSID "TextDumper"

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDUMPERS_EXPORT TextDumper : public AbstractTextualAnalysisDumper
{
public:
  TextDumper();

  virtual ~TextDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;
    
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
  bool m_followGraph;
  std::string m_verbTenseFlag; //Ajout

  // private member functions
  void outputVertex(std::ostream& out, 
                    const LinguisticAnalysisStructure::Token* ft,
                    const std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*>& data,
                    const FsaStringsPool& sp,
                    uint64_t offset=0) const;
                    
  void outputString(std::ostream& out,const std::string& str) const;
                                      
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
