// Copyright 2004-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
