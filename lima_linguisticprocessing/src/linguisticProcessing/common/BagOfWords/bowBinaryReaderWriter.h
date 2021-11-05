/*
    Copyright 2002-2013 CEA LIST

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
/************************************************************************
 *
 * @file       bowBinaryReaderWriter.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Tue Mar 20 2007
 * copyright   Copyright (C) 2007 by CEA LIST
 * Project     Common
 *
 * @brief      reader writer of bow in binary format
 *
 *
 ***********************************************************************/

#ifndef BOWBINARYREADERWRITER_H
#define BOWBINARYREADERWRITER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <iostream>
#include <vector>
#include <map>
#include <QtCore/QMap>
#include <boost/shared_ptr.hpp>

namespace Lima {
namespace Common {
namespace BagOfWords {

class AbstractBoWDocumentHandler;
class AbstractBoWElement;
class BoWDocument;
class BoWText;
class BoWToken;
class BoWRelation;
class BoWNamedEntity;
class BoWPredicate;
enum BoWBlocType: unsigned char;
// reader/writer are kept independent from BoW (instead of having
// read/write functions in each BoW element) because it is easier
// to handle common mappings (pointers, entity types etc).

enum BoWFileType : unsigned char {
  BOWFILE_NOTYPE,
  BOWFILE_TEXT,
  BOWFILE_DOCUMENT,
  BOWFILE_DOCUMENTST,
  BOWFILE_SDOCUMENT
};

// reader and writer are not symmetrical : writer simply write infos;
// reader must handle mappings
class BoWBinaryReaderPrivate;
class LIMA_BOW_EXPORT BoWBinaryReader
{
 public:
  BoWBinaryReader();
  virtual ~BoWBinaryReader();

  void readHeader(std::istream& file);
  void readBoWText(std::istream& file,
                   BoWText& bowText);
  BoWBlocType readBoWDocumentBlock(std::istream& file,
                       BoWDocument& document,
                       AbstractBoWDocumentHandler& handler,
                       bool useIterator,
                       bool useIndexIterator);
  boost::shared_ptr< Lima::Common::BagOfWords::AbstractBoWElement > readBoWToken(std::istream& file);
  void readSimpleToken(std::istream& file,
                       boost::shared_ptr< BoWToken > token);
  void readPredicate(std::istream& file,
                   boost::shared_ptr< BoWPredicate > bowPred);

  BoWFileType getFileType() const;
  std::string getFileTypeString() const;

private:
  BoWBinaryReader(const BoWBinaryReader&);
  BoWBinaryReader& operator=(const BoWBinaryReader&);
  BoWBinaryReaderPrivate* m_d;
};

class BoWBinaryWriterPrivate;
class LIMA_BOW_EXPORT BoWBinaryWriter
{
 public:
  BoWBinaryWriter(const QMap< uint64_t, uint64_t >& shiftFrom = QMap< uint64_t, uint64_t >());
  virtual ~BoWBinaryWriter();

  void writeHeader(std::ostream& file,
                   BoWFileType type) const;
  void writeBoWText(std::ostream& file,
                    const BoWText& bowText) const;
  void writeBoWDocument(std::ostream& file,
                        const BoWDocument& bowText) const;
  void writeBoWToken(std::ostream& file,
                     const boost::shared_ptr< AbstractBoWElement > bowToken) const;
  void writeSimpleToken(std::ostream& file,
                        const boost::shared_ptr< BoWToken > token) const;
  void writePredicate(std::ostream& file,
                        const boost::shared_ptr< BoWPredicate > token) const;

 private:
  BoWBinaryWriter(const BoWBinaryWriter&);
  BoWBinaryWriter& operator=(const BoWBinaryWriter&);
  BoWBinaryWriterPrivate* m_d;
};

} // end namespace
} // end namespace
} // end namespace

#endif
