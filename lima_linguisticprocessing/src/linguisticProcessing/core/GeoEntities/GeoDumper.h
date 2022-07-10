// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSGEODUMPER_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDUMPERSGEODUMPER_H

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

#define GEODUMPER_CLASSID "GeoDumper"

/**
@author Benoit Mathieu
*/
class GeoDumper : public MediaProcessUnit
{
public:
  GeoDumper();

  virtual ~GeoDumper();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;
    
private:
  MediaId m_language;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  const Common::PropertyCode::PropertyManager* m_propertyManager;
  std::string m_graph;
  std::string m_handler;
  std::string m_sep;
  std::string m_sepPOS;
  bool m_printPosition;
  std::string m_property;
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
