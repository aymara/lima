/*
    Copyright 2002-2022 CEA LIST

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

#ifndef SYMBOLICCODEXMLREADER_H
#define SYMBOLICCODEXMLREADER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

#include <string>
#include <map>


class SymbolicCodeXmlReaderPrivate;
class LIMA_PROPERTYCODE_EXPORT SymbolicCodeXmlReader
{
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  SymbolicCodeXmlReader(const Lima::Common::PropertyCode::PropertyCodeManager& pcm,
                        std::map<std::string,Lima::LinguisticCode>& outputMap);
  ~SymbolicCodeXmlReader();

  bool parse(QIODevice *device);

  QString errorString() const;

private:
  SymbolicCodeXmlReaderPrivate* m_d;
};

#endif
