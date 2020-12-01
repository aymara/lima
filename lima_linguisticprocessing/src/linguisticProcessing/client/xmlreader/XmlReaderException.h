/**
 *   Copyright (C) 2007 by CEA - LIST- LIC2M
 *
 * @file        .h
 * @author      Claire Mouton (Claire.Mouton@cea.fr) \n
 *              Copyright (c) 2007 by CEA
 * @date        Created on Apr, 24 2007
 */
#include "common/LimaCommon.h"
#include "XmlReaderClient_export.h"

namespace Lima {
    namespace XmlReader {

      class XMLREADERCLIENT_EXPORT XmlReaderException : public Lima::LimaException  {
            public:
                explicit XmlReaderException(const std::string& mess = "") :
                  Lima::LimaException(mess) {}
        };

    } // XmlReader
} // FrCeaLic2m

