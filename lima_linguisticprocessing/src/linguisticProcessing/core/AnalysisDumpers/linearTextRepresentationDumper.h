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

/** =========================================================================
    @file       linearTextRepresentationDumper.h

    @version    $Id$
    @date       created:       jan 6, 2005
    @date       last revised:  jun 28, 2005

    @author     Olivier Ferret
    @brief      dump the list of all the BoWToken of a text according to
                their position

    Copyright (C) 2005-2012 by CEA LIST

    ========================================================================= */

#ifndef LIMA_LINGUISTICPROCESSING_LINEARTEXTREPRESENTATIONDUMPER_H
#define LIMA_LINGUISTICPROCESSING_LINEARTEXTREPRESENTATIONDUMPER_H

#include "AnalysisDumpersExport.h"
#include "LTRTextBuilder.h"
#include "StopList.h"

#include "common/MediaProcessors/MediaProcessUnit.h"
// #include "common/AbstractFactoryPattern/InitializableObject.h"

#include <iostream>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

#define LTRDUMPER_CLASSID "LTRDumper"

/**
  * @class  LinearTextRepresentationDumper
  * @brief  dump the list of all the BoWToken of a text according to
            their position
  */

class LIMA_ANALYSISDUMPERS_EXPORT LinearTextRepresentationDumper : 
    public MediaProcessUnit
{
public:
  /// **** @name  process unit management
  //@{
    virtual void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager) override;

    virtual LimaStatusCode process(AnalysisContent& analysis) const override;
  //}@

private:
  MediaId m_language;
  StopList* m_stopList;
  std::string m_handler;
};


} // AnalysisDumpers
} // LinguisticProcessing
} // Lima


#endif  // LIMA_LINGUISTICPROCESSING_LINEARTEXTREPRESENTATIONDUMPER_H
