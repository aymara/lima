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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/

#include "StopList.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/FileUtils.h"
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
  const string& resourcesPath=Common::MediaticData::MediaticData::single().getResourcesPath();
  QString stopListFileName;
  try
  {
    stopListFileName = Common::Misc::findFileInPaths(resourcesPath.c_str(), unitConfiguration.getParamsValueAtKey("file").c_str());
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
