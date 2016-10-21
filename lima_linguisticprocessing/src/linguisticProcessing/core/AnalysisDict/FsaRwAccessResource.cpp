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
#include "FsaRwAccessResource.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/FsaAccess/FsaAccessBuilderRandom16.h"

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common;
using namespace std;

namespace Lima
{
namespace LinguisticProcessing
{
namespace AnalysisDict
{

SimpleFactory<AbstractResource,FsaRwAccessResource> fsaRwAccessResourceFactory(FSARWACCESSRESSOURCE_CLASSID);

FsaRwAccessResource::FsaRwAccessResource()
//    : m_fsaAccess(0), m_fsaRwAccess(0)
    : FsaAccessResource(), m_fsaRwAccess(0)
{}


FsaRwAccessResource::~FsaRwAccessResource()
{
/*  
  if (m_fsaAccess!=0)
  {
    delete m_fsaAccess;
  }
*/
}

void FsaRwAccessResource::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="FsaAccess"&gt;</b>
   *    -  keyFile : file containing the compiled access keys
   */
    
  ANALYSISDICTLOGINIT;
  FsaAccess::FsaAccessBuilderRandom16* fsaAccess;
  try
  {
    QString keyfile = Common::Misc::findFileInPaths(Common::MediaticData::MediaticData::single().getResourcesPath().c_str(), unitConfiguration.getParamsValueAtKey("keyFile").c_str());
    fsaAccess=new FsaAccess::FsaAccessBuilderRandom16();

	  auto* const pFileName = keyfile.toUtf8().constData();
    ifstream fileIn(pFileName, ios::in | ios::binary);
    if (!fileIn.good()) {
      LERROR << "cannot open file " << pFileName;
      throw InvalidConfiguration();
    }
    char magicNumber[3];
    fileIn.read(magicNumber, 3);
    if (string(magicNumber, 3) == "Ant") {
      unsigned char intLe[4];	//UNSIGNED obligatoire
      fileIn.read((char*)intLe, 4);
      const std::size_t antLen = intLe[0] + intLe[1]*0x100 + intLe[2]*0x10000 + intLe[3]*0x1000000; 
      std::streamoff pos = fileIn.tellg();
      fileIn.seekg(pos+antLen, ios::beg);           //saute l'identification Antinno
    }
    else fileIn.seekg(0, ios::beg);         //pas un fichier repere par Antinno
    fsaAccess->read(fileIn);


    m_fsaAccess=fsaAccess;
    m_fsaRwAccess=fsaAccess;
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'keyFile' in FsaAccessResource group for language " << (int)  manager->getInitializationParameters().language;
    throw InvalidConfiguration();
  }
}


AbstractAccessByString* FsaRwAccessResource::getAccessByString() const
  { return m_fsaAccess;}


AbstractModifierOnAccessByString* FsaRwAccessResource::getRwAccessByString() const
  { return m_fsaRwAccess;}

}

}

}
