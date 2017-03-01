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
#ifndef LIMA_LINGUISTICPROCESSING_POSTAGGERDISAMBIGUATEDGRAPHXMLLOGGER_H
#define LIMA_LINGUISTICPROCESSING_POSTAGGERDISAMBIGUATEDGRAPHXMLLOGGER_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"

namespace Lima
{

namespace LinguisticProcessing
{

namespace PosTagger
{

/**
@author Benoit Mathieu
*/
class LIMA_POSTAGGER_EXPORT DisambiguatedGraphXmlLogger : public AbstractLinguisticLogger
{
public:
  DisambiguatedGraphXmlLogger();

  virtual ~DisambiguatedGraphXmlLogger();

  void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager)
  ;

  LimaStatusCode process(
    AnalysisContent& analysis) const;

private:
  MediaId m_language;
  const Common::PropertyCode::PropertyManager* m_macroManager;
  const Common::PropertyCode::PropertyManager* m_microManager;

};

}

}

}

#endif // LIMA_LINGUISTICPROCESSING_POSTAGGERDISAMBIGUATEDGRAPHXMLLOGGER_H
