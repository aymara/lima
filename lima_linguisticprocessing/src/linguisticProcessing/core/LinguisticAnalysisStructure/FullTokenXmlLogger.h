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

/** @brief      logger for xml-formatted linguistic data in graph.
  *
  * @file       MAxmlLogger.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  * @version    $Id$
  */

#ifndef LIMA_LINGUISTICPROCESSING_FTXMLLOGGER_H
#define LIMA_LINGUISTICPROCESSING_FTXMLLOGGER_H

#include "LinguisticAnalysisStructureExport.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"
#include "linguisticProcessing/core/LinguisticProcessors/AbstractLinguisticLogger.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace LinguisticAnalysisStructure
{

#define FULLTOKENXMLLOGGER_CLASSID "FullTokenXmlLogger"

class LIMA_LINGUISTICANALYSISSTRUCTURE_EXPORT FullTokenXmlLogger : public AbstractLinguisticLogger
{

public:

  FullTokenXmlLogger();
  virtual ~FullTokenXmlLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

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
