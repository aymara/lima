// Copyright 2007-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef LIMA_XMLREADER_XMLREADEREXCEPTION
#define LIMA_XMLREADER_XMLREADEREXCEPTION

#include "common/LimaCommon.h"
#include "XmlReaderClient_export.h"

namespace Lima {
    namespace XmlReader {

      class XMLREADERCLIENT_EXPORT XmlReaderException : public Lima::LimaException  {
            public:
                explicit XmlReaderException(const std::string& mess = "") :
                  Lima::LimaException(mess) {}
        };

    }
}

#endif
