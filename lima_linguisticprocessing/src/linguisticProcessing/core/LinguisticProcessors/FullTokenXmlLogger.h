// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** @brief      logger for xml-formatted linguistic data in graph.
  *
  * @file       MAxmlLogger.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  * @version    $Id$
  */

#ifndef LIMA_LINGUISTICPROCESSING_FTXMLLOGGER_H
#define LIMA_LINGUISTICPROCESSING_FTXMLLOGGER_H

#include "LinguisticProcessorsExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{

#define FULLTOKENXMLLOGGER_CLASSID "FullTokenXmlLogger"

class LIMA_LINGUISTICPROCESSORS_EXPORT FullTokenXmlLogger : public AbstractLinguisticLogger
{

public:

  FullTokenXmlLogger();
  virtual ~FullTokenXmlLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const override;

protected:

private:

  std::string m_graphId;
  const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;
  MediaId m_language;

  void dump(
    std::ostream& fileName,
    LinguisticAnalysisStructure::AnalysisGraph& tTokenList) const;

};


} // LinguisticAnalysisStructure
} // LinguisticProcessing
} // Lima

#endif
