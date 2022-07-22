// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "StopList.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"

#include "common/AbstractFactoryPattern/SimpleFactory.h"

#include <iostream>
#include <fstream>

using namespace std;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<AbstractResource,StopList> stopListFactory(STOPLIST_CLASSID);

StopList::StopList()
    : AbstractResource()
{}


StopList::~StopList()
{}

/** @addtogroup ResourceConfiguration
 * - <b>&lt;group name="..." class="SentenceBoundsFinder"&gt;</b>
 */
void StopList::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                     Manager* manager)

{
  LIMA_UNUSED(manager);
  DUMPERLOGINIT;
  QString stopListFileName;
  try
  {
    stopListFileName = getResourceFileName(unitConfiguration.getParamsValueAtKey("file").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No param 'file' in StopList configuration group ! ";
    throw InvalidConfiguration();
  }

  std::ifstream stopListFile(stopListFileName.toUtf8().constData(), std::ifstream::binary);
  if (!stopListFile) {
    LERROR << "invalid file " << stopListFileName;
    throw InvalidConfiguration();
  }
  
  LimaString wword = Common::Misc::utf8stdstring2limastring(Common::Misc::readLine(stopListFile));
  LDEBUG << "Loading stop list file: " << stopListFileName;
  while (!wword.isEmpty())
  {
    insert(wword);
    wword = Common::Misc::utf8stdstring2limastring(Common::Misc::readLine(stopListFile));
  }
}



}

}
}
