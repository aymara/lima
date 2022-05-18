// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_MEDIAPROCESSORSMEDIAPROCESSUNIT_H
#define LIMA_COMMON_MEDIAPROCESSORSMEDIAPROCESSUNIT_H

#include "common/ProcessUnitFramework/AbstractProcessUnit.h"
#include "common/LimaCommon.h"

namespace Lima
{

namespace Common {
namespace XMLConfigurationFiles {
  class GroupConfigurationStructure;
}
}
class AnalysisContent;

struct MediaProcessUnitInitializationParameters {
  MediaId media;
};

class LIMA_MEDIAPROCESSORS_EXPORT MediaProcessUnit : public AbstractProcessUnit<MediaProcessUnit,MediaProcessUnitInitializationParameters>
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
  virtual void init(
    Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override = 0;

  /**
    * @brief Process on data in analysisContent.
    * This method should not return any exception
    * @param analysis AnalysisContent object on which to process
    * @throw UndefinedMethod if this method hasn't been specialized
    */
  virtual LimaStatusCode process(AnalysisContent& analysis) const override = 0;
};



} // Lima

#endif
