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
/** @brief      logger for checking chinese segmentation
  *
  * @file       disambiguationLogger.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2003 by CEA
  * @version    $Id$
  */

#ifndef LIMA_POSTAGGER_DISAMBIGUATIONLOGGER_H
#define LIMA_POSTAGGER_DISAMBIGUATIONLOGGER_H

#include "PosTaggerExport.h"
#include "linguisticProcessing/common/misc/AbstractLinguisticLogger.h"

#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

#define DISAMBIGUATIONLOGGER_CLASSID "DisambiguationLogger"

class LIMA_POSTAGGER_EXPORT DisambiguationLogger : public AbstractLinguisticLogger
{

public:
  DisambiguationLogger();
  virtual ~DisambiguationLogger();

  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override
  ;

  virtual LimaStatusCode process(AnalysisContent& analysis) const override;

protected:

private:

  bool m_logAll;
  MediaId m_language;
  const Common::PropertyCode::PropertyManager* m_microManager;

};

} // PosTagger
} // namespace LinguisticProcessing
} // Lima


#endif
