// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    std::shared_ptr<StopList> m_stopList;


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
