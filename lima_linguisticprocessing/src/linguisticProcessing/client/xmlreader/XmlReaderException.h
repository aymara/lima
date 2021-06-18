/**
    Copyright 2007-2021 CEA LIST

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
 @author      Claire Mouton (Claire.Mouton@cea.fr)
 @date        Created on Apr, 24 2007
 */
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
