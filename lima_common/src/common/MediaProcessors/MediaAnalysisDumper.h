// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_MEDIAANALYSISDUMPER_H
#define LIMA_COMMON_MEDIAANALYSISDUMPER_H

#include "common/ProcessUnitFramework/AbstractProcessUnit.h"
#include "common/LimaCommon.h"

namespace Lima
{
namespace Common {
namespace XMLConfigurationFiles {
class GroupConfigurationStructure;
}
}

struct LIMA_MEDIAPROCESSORS_EXPORT MediaAnalysisDumperInitializationParameters
{
    MediaId media;
};

class LIMA_MEDIAPROCESSORS_EXPORT MediaAnalysisDumper : public AbstractProcessUnit<MediaAnalysisDumper,MediaAnalysisDumperInitializationParameters>
{
public:

    /**
    * @brief initialize with parameters from configuration file.
    * @param unitConfiguration @IN : <group> tag in xml configuration file that
    *        contains parameters to initialize the object.
    * @param manager @IN : manager that asked for initialization and carries init params
    * Use it to initialize other objects of same kind.
    * @throw InvalidConfiguration when parameters are invalids.
    */
    virtual void init (
        Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
        Manager* manager ) override = 0;

    /**
      * @brief Process on data in analysisContent.
      * This method should not return any exception
      * @param analysis AnalysisContent object on which to process
      * @throw UndefinedMethod if this method hasn't been specialized
      */
    virtual LimaStatusCode process (
        AnalysisContent& analysis ) const override = 0;


};


} // Lima

#endif
