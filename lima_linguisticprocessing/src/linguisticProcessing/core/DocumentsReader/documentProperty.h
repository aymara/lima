// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file   abstractDocumentXMLParser.h
 * @author Olivier Mesnard
 * @date   16 Sept 2005
 *
 * @brief
 *
 *
 ***********************************************************************/
#ifndef XMLREADERDOCUMENTPROPERTYTYPE_H
#define XMLREADERDOCUMENTPROPERTYTYPE_H

#include "documentsreader_export.h"
#include "linguisticProcessing/core/abstractReaderResource.h"

namespace Lima {
namespace DocumentsReader {

/** a type meaning the storage type of an attribute */
typedef enum {
  STORAGE_NONE,
  STORAGE_UTF8_STRING,
  STORAGE_INTEGER,
  STORAGE_DATE,
  STORAGE_DATE_INTERVAL,
  STORAGE_WEIGHTED_PROPERTY
} StorageType;
#define MAX_STORAGE_TYPE int(STORAGE_WEIGHTED_PROPERTY)+1

static const QString storageTypeTag[MAX_STORAGE_TYPE]={
  "none",
  "string",
  "integer",
  "date",
  "date_interval",
  "weighted_property"
};

/** a type meaning the cardinality of an attribute */
typedef enum {
  CARDINALITY_NONE,
  CARDINALITY_OPTIONAL,
  CARDINALITY_MANDATORY,
  CARDINALITY_MULTIPLE
} CardinalityType;
#define MAX_CARDINALITY_TYPE CARDINALITY_MULTIPLE+1

static const QString cardinalityTypeTag[MAX_STORAGE_TYPE]={
  "none",
  "optional",
  "mandatory",
  "multi-valued"
};

#define STANDARDDOCUMENTPROPERTYTYPE_CLASSID "StandardDocumentPropertyType"
#define EXTENSIONDOCUMENTPROPERTYTYPE_CLASSID "ExtensionDocumentPropertyType"
class DocumentPropertyTypePrivate;
class DOCUMENTSREADER_EXPORT DocumentPropertyType : public AbstractReaderResource {
public:
  DocumentPropertyType();
  DocumentPropertyType(const DocumentPropertyType& dpt);
  DocumentPropertyType& operator=(const DocumentPropertyType& dpt);
  virtual ~DocumentPropertyType();
  void init( Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;

  const StorageType& getStorageType() const;
  const CardinalityType& getValueCardinality() const;
  const long int& getStorageSize() const;
  const std::deque< QString >& getElementTagNames() const;
  const std::deque< std::pair<QString,QString> > & getAttributeTagNames() const;
  bool isValueInElement( const QString& elementName ) const;

  StorageType& getStorageType();
  CardinalityType& getValueCardinality();
  long int& getStorageSize();

  bool operator<(const DocumentPropertyType& dpt) const;

protected:
  DocumentPropertyTypePrivate* m_d;
};

class DOCUMENTSREADER_EXPORT StandardDocumentPropertyType : public DocumentPropertyType {
public:
  StandardDocumentPropertyType();
  void init( Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;
};

class DOCUMENTSREADER_EXPORT ExtensionDocumentPropertyType : public DocumentPropertyType {
public:
  ExtensionDocumentPropertyType();
  void init( Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager) override;
};

} // namespace DocumentsReader
} // namespace Lima

#endif
