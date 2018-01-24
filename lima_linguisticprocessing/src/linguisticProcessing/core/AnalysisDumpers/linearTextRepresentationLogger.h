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
    @file       linearTextRepresentationLogger.h

    @version    $Id$
    @date       created:       jun 28, 2005
    @date       last revised:  jun 29, 2005

    @author     Olivier Ferret
    @brief      dump on a file a linear representation of the graph resulting
                from the linguistic analysis of a document

    Copyright (C) 2005-2012 by CEA LIST

    ========================================================================= */

#ifndef LIMA_LINGUISTICPROCESSING_LINEARTEXTREPRESENTATIONLOGGER_H
#define LIMA_LINGUISTICPROCESSING_LINEARTEXTREPRESENTATIONLOGGER_H

#include "AnalysisDumpersExport.h"
#include "StopList.h"
#include "LTRTextBuilder.h"

#include "common/MediaProcessors/MediaProcessUnit.h"
// #include "common/AbstractFactoryPattern/InitializableObject.h"


#define LTRLOGGER_CLASSID "LinearTextRepresentationLogger"

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {


/**
  * @class  LinearTextRepresentationLogger
  * @brief  dump on a file a linear representation of the graph resulting
  *         from the linguistic analysis of a document
  */

class LIMA_ANALYSISDUMPERS_EXPORT LinearTextRepresentationLogger : public MediaProcessUnit {

// data
// ----
private:
    std::string m_outputSuffix;
    MediaId m_language;
    StopList* m_stopList;


// methods
// -------
public:
    /// **** @name  process unit management
    //@{
    virtual void init(
        Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
        Manager* manager) override;
    virtual LimaStatusCode process(AnalysisContent& analysis) const override;
    //}@

};


} // AnalysisDumpers
} // LinguisticProcessing
} // Lima


#endif  // LIMA_LINGUISTICPROCESSING_LINEARTEXTREPRESENTATIONLOGGER_H
