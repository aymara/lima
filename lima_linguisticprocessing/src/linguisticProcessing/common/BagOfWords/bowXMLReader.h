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

#ifndef BOWXMLREADER_H
#define BOWXMLREADER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowToken.h"
#include "bowComplexToken.h"
#include "bowText.h"
#include "bowDocument.h"
#include <stack>

#include <QtCore/QDate>

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWXmlReaderPrivate;
/**
 * @brief      XML input/output for BoW elements
 * @author     Besancon Romaric <romaric.besancon@cea.fr>
 * @date       Wed May  5 2004
 **/
class LIMA_BOW_EXPORT BoWXmlReader
{
 public:
  BoWXmlReader(const std::string& filename,
               std::ostream& output);
  virtual ~BoWXmlReader();

  // -----------------------------------------------------------------------
  // local functions

 private:
   BoWXmlReaderPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
