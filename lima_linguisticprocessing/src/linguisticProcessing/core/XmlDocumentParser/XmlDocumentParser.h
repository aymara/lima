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
#ifndef XMLDOCUMENTPARSER_H
#define XMLDOCUMENTPARSER_H

#include <wchar.h>
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima {
namespace XmlReader {

    // Se retrouve dans le ficheir de configuration mm-analysis-art.xml
#define MODULE_CLASSID "xmldocumentparser"

class XmlDocumentParser : public MediaProcessUnit
{

    public:
        XmlDocumentParser();
        virtual ~XmlDocumentParser();

        void init (
            Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager );

        LimaStatusCode process (
            AnalysisContent& analysis ) const;

    protected:

    private:

};

} // namespace XmlDocumentParser
} // namespace Lima

#endif
