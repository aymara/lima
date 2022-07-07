// Copyright (C) 2002-2022 by CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef MULTXMLREADER_H
#define MULTXMLREADER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Handler/shiftFrom.h"

namespace Lima {
namespace XmlReader {

class MultXmlReaderPrivate;
/**
 * @brief Parser for XML representation of Amose analysed multimedia documents
 * @author     Gael de Chalendar (gael.de-chalendar@cea.fr)
 * @date       Mon Apr 7 2014
 */
class MultXmlReader
{
public:
  MultXmlReader(const std::string& filename, std::ostream& output, std::shared_ptr<const ShiftFrom> shiftFrom);
  ~MultXmlReader();

private:
  MultXmlReaderPrivate* m_d;

  MultXmlReader() = delete;
  MultXmlReader(const MultXmlReader&) = delete;
  MultXmlReader& operator=(const MultXmlReader&) = delete;
};

} // end namespace
} // end namespace

#endif
