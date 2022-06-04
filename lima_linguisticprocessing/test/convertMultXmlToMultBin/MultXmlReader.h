/************************************************************************
 *
 * @file       MultXmlReader.h
 * @author     Gael de Chalendar (gael.de-chalendar@cea.fr)
 * @date       Mon Apr 7 2014
 * copyright   Copyright (C) 2014 by CEA LIST
 * Project     lima_xmlprocessings
 *
 * Parser for XML representation of Amose analysed multimedia documents
 *
 ***********************************************************************/

#ifndef MULTXMLREADER_H
#define MULTXMLREADER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Handler/shiftFrom.h"

namespace Lima {
namespace XmlReader {

class MultXmlReaderPrivate;
/**
 * @brief Parser for XML representation of Amose analysed multimedia documents
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
