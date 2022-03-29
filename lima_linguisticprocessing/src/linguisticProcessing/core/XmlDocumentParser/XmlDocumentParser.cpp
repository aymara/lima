/*
    Copyright 2004-2021 CEA LIST

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
#include "XmlDocumentParser.h"

#include "common/AbstractFactoryPattern/Lima::SimpleFactory.h"

#include <string>
#include <iostream>


using namespace Lima::Common::XMLConfigurationFiles;
using namespace std;


namespace Lima {
namespace XmlReader{


Lima::SimpleFactory<MediaProcessUnit,XmlDocumentParser> moduleFactory ( MODULE_CLASSID );

XmlDocumentParser::XmlDocumentParser()  {}

XmlDocumentParser::~XmlDocumentParser()
{           }

void XmlDocumentParser::init (
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager )
throw ( InvalidConfiguration,std::exception )
{

}

LimaStatusCode XmlDocumentParser::process (
    AnalysisContent& analysis ) const
{

    cout << "*** XmlDocumentParser::process( AnalysisContent& analysis ) " << endl;

    return SUCCESS_ID;
}

} //namespace XmlDocumentParser
} // namespace Lima

