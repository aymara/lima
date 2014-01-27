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
#include "FsaAccessResource.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/MediaticData/mediaticData.h"
#include "common/FsaAccess/FsaAccessSpare16.h"

using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common;
using namespace std;

namespace Lima
{
namespace Common {
  namespace FsaAccess {
  extern template class LIMA_FSAACCESS_EXPORT FsaAccessReader16<class boost::adjacency_list<struct boost::vecS,struct boost::vecS,struct boost::bidirectionalS,struct boost::property<enum vertex_count_t,class std::vector<int,class std::allocator<int> >,struct boost::property<enum boost::vertex_name_t,uint64_t,struct boost::property<enum vertex_text_t,class QString,struct boost::no_property> > >,struct boost::no_property,struct boost::no_property,struct boost::listS> >;
  }
}
namespace LinguisticProcessing
{
namespace AnalysisDict
{

SimpleFactory<AbstractResource,FsaAccessResource> fsaAccessResourceFactory(FSAACCESSRESSOURCE_CLASSID);

FsaAccessResource::FsaAccessResource()
    : AbstractAccessResource(),m_fsaAccess(0)
{}


FsaAccessResource::~FsaAccessResource()
{
  if (m_fsaAccess!=0)
  {
    delete m_fsaAccess;
  }
}

void FsaAccessResource::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  /** @addtogroup ResourceConfiguration
   * - <b>&lt;group name="..." class="FsaAccess"&gt;</b>
   *    -  keyFile : file containing the compiled access keys
   */
    
  ANALYSISDICTLOGINIT;
  try
  {
    string keyfile=Common::MediaticData::MediaticData::single().getResourcesPath() + "/" + unitConfiguration.getParamsValueAtKey("keyFile");
    FsaAccess::FsaAccessSpare16* fsaAccess=new FsaAccess::FsaAccessSpare16();
    LDEBUG << "FsaAccessResource::init read keyFile" << keyfile.c_str();
    fsaAccess->read(keyfile);
    m_fsaAccess=fsaAccess;
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'keyFile' in FsaAccessResource group for language " << (int)  manager->getInitializationParameters().language << LENDL;
    throw InvalidConfiguration();
  }
  catch (AccessByStringNotInitialized& )
  {
    LERROR << "keyfile "
           << Common::MediaticData::MediaticData::single().getResourcesPath()
           << "/"
           << unitConfiguration.getParamsValueAtKey("keyFile")
           << " no found for language " 
           << (int)  manager->getInitializationParameters().language << LENDL;
    throw InvalidConfiguration();
  }
}

AbstractAccessByString* FsaAccessResource::getAccessByString() const
  { return m_fsaAccess;}

}

}

}
