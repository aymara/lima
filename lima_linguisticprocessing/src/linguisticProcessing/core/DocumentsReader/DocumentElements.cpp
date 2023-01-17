// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file   DocumentElements.cpp
 * @author Besancon Romaric
 * @date   Wed Oct 29 13:31:44 2003
 * @author Gael de Chalendar
 * @date   Wed Apr 08 2015
 ***********************************************************************/

#include "DocumentElements.h"

#include "common/Data/strwstrtools.h"
#include <fstream>
#include <string>
#include <deque>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#include <QtCore/QTextStream>

#include "linguisticProcessing/core/XmlProcessingCommon.h"
#include "contentDocument.h"

using namespace std;
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
 // to handle dates (parsing)

namespace Lima {
namespace DocumentsReader {


AbstractStructuredDocumentElement::AbstractStructuredDocumentElement(
    const QString& elementName, unsigned int firstBytePos )
: m_elementName(elementName), m_offset(firstBytePos)
{
  // transmit offset to the part.
  addPart(QString(), m_offset);
}

AbstractStructuredDocumentElement::~AbstractStructuredDocumentElement() {
  for( deque<ContentElementPart*>::iterator cIt = begin() ; cIt != end() ; cIt++ ) {
    delete *cIt;
  }
}

unsigned long int AbstractStructuredDocumentElement::getOffset() {
 return m_offset;
}

void AbstractStructuredDocumentElement::setOffset( unsigned long int offset)
{
  m_offset = offset;
}


void AbstractStructuredDocumentElement::addPart(const Lima::LimaString& text,
                                     const unsigned int offset )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "AbstractStructuredDocumentElement::addPart:" << text.left(50) << "(...)" << offset;
#endif
  push_back(new ContentElementPart(text,offset));
}

void AbstractStructuredDocumentElement::addToCurrentOffset(const Lima::LimaString& value)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "AbstractStructuredDocumentElement::addToCurrentOffset:" << value.left(50) << "(...)";
#endif
  if( isDiscarding() )
  {
    addSpaces(value.length());
  }
  else
  {
//     m_offset += value.size();
    back()->addText(value);
  }
#ifdef DEBUG_LP
  LDEBUG << "AbstractStructuredDocumentElement::addToCurrentOffset end offset = " << m_offset;
  LDEBUG << "AbstractStructuredDocumentElement::addToCurrentOffset "<<this<<" text:" << back()->getText();
#endif
}

void AbstractStructuredDocumentElement::addSpaces(int nbSpaces) {
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "AbstractStructuredDocumentElement::addSpaces(" << nbSpaces << ")";
#endif
  back()->addText(QString(nbSpaces, ' ') );
#ifdef DEBUG_LP
  LDEBUG << "AbstractStructuredDocumentElement::addSpaces text:" << back()->getText();
#endif
}

//***********************************************************************
// output
//***********************************************************************

std::ostream& operator << (std::ostream& out, const PropagatedProperties& properties)
{
  properties.print(out);
  return out;
}

QTextStream& operator << (QTextStream& out, const PropagatedProperties& properties)
{
  properties.print(out);
  return out;
}

QDebug& operator << (QDebug& out, const PropagatedProperties& properties)
{
  properties.print(out);
  return out;
}


void PropagatedProperties::print(std::ostream& out) const
{
  for(auto iter=begin(); iter!=end(); iter++) {
      out << iter->first.getId() << "," << storageTypeTag[iter->first.getStorageType()].toStdString()
          << "," << cardinalityTypeTag[iter->first.getValueCardinality()].toStdString() << ": ";
      for(auto str : iter->second) {
          out << str << ", ";
      }
      out << endl;
  }
  return;
}
void PropagatedProperties::print(QTextStream& out) const
{
  for(auto iter=begin(); iter!=end(); iter++) {
      out << QString::fromStdString(iter->first.getId()) << "," << storageTypeTag[iter->first.getStorageType()]
          << "," << cardinalityTypeTag[iter->first.getValueCardinality()] << ": ";
      for(auto str : iter->second) {
          out << QString::fromStdString(str) << ", ";
      }
      out << endl;
  }
  return;
}
void PropagatedProperties::print(QDebug& out) const
{
  for(auto iter=begin(); iter!=end(); iter++) {
      out << QString::fromStdString(iter->first.getId()) << "," << storageTypeTag[iter->first.getStorageType()]
          << "," << cardinalityTypeTag[iter->first.getValueCardinality()] << ": ";
      for(auto str : iter->second) {
          out << QString::fromStdString(str) << ", ";
      }
      out << endl;
  }
  return;
}
//***********************************************************************


DiscardableDocumentElement::DiscardableDocumentElement( const QString& elementName, unsigned int firstBytePos )
    : AbstractStructuredDocumentElement(elementName, firstBytePos)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "DiscardableDocumentElement::DiscardableDocumentElement(" << elementName << ")";
#endif
}

PresentationDocumentElement::PresentationDocumentElement(
    const QString& elementName,
    unsigned int firstBytePos,
    const PropagatedProperties& toBePropagated )
    : AbstractStructuredDocumentElementWithProperties(elementName, firstBytePos, toBePropagated)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "PresentationDocumentElement::PresentationDocumentElement(" << elementName << ")";
#endif
  m_toBePropagated = toBePropagated;
}

IgnoredDocumentElement::IgnoredDocumentElement(
    const QString& elementName,
    unsigned int firstBytePos,
    const DocumentPropertyType& propType,
    const PropagatedProperties& toBePropagated )
    : AbstractStructuredDocumentElementWithProperties(elementName, firstBytePos, toBePropagated)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "IgnoredDocumentElement::IgnoredDocumentElement(" << elementName << ")";
#endif
  m_propType = propType;
}

DocumentPropertyElement::DocumentPropertyElement(
    const QString& elementName,
    unsigned int firstBytePos,
    const DocumentPropertyType& propType )
     : AbstractStructuredDocumentElement(elementName, firstBytePos) {
  m_propType = propType;
}

// Call when some character content is added
void DocumentPropertyElement::addToCurrentOffset(const Lima::LimaString& value) {
  if( hasPropType() )
    m_propertyValue.append(value);
}

AbstractStructuredDocumentElementWithProperties::AbstractStructuredDocumentElementWithProperties(
    const QString& elementName,
    unsigned int firstBytePos,
    const PropagatedProperties& toBePropagated ) :
  AbstractStructuredDocumentElement(elementName, firstBytePos), m_toBePropagated(toBePropagated)
{
}

AbstractStructuredDocumentElementWithProperties::~AbstractStructuredDocumentElementWithProperties()
{
}

HierarchyDocumentElement::HierarchyDocumentElement(
    const QString& elementName,
    unsigned int firstBytePos,
    const PropagatedProperties& toBePropagated  ) :
  AbstractStructuredDocumentElementWithProperties(elementName, firstBytePos, toBePropagated)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "HierarchyDocumentElement::HierarchyDocumentElement(" << elementName << ")";
#endif
  setPropagatedValue(toBePropagated);
}

IndexingDocumentElement::IndexingDocumentElement(
    const QString& elementName, unsigned int firstBytePos,
    const DocumentPropertyType& propType,
    const PropagatedProperties& toBePropagated )
  : AbstractStructuredDocumentElementWithProperties(elementName, firstBytePos, toBePropagated)
{
  m_propType = propType;
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "IndexingDocumentElement::IndexingDocumentElement(" << elementName << ")";
#endif
  setPropagatedValue(toBePropagated);
}

IndexingDocumentElement::~IndexingDocumentElement()
{}

// // Call when some character content is added
// void IndexingDocumentElement::addToCurrentOffset(const Lima::LimaString& value) {
//   if( hasPropType() )
//     m_propertyValue.append(value);
//   AbstractStructuredDocumentElement::addToCurrentOffset(value);
// }

// TODO: transfert this code in parent class ?
void AbstractStructuredDocumentElementWithProperties::setPropagatedValue(
    const PropagatedProperties& toBePropagated )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue" << getElementName() << this << "," << toBePropagated.size() << "properties";
#endif

  if(toBePropagated.size()) {
#ifdef DEBUG_LP
    LDEBUG << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue properties:" << toBePropagated;
#endif
  }

  for(auto it = toBePropagated.begin() ; it != toBePropagated.end() ; it++  )
  {
    const DocumentPropertyType& property = (*it).first;
    std::vector<std::string> v_data = (*it).second;
    if (v_data.empty())
    {
#ifdef DEBUG_LP
    LDEBUG << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue do not try to propagate empty value for" << property.getId();
#endif
      continue;
    }
    if(property.getValueCardinality() != CARDINALITY_MULTIPLE) {
#ifdef DEBUG_LP
    LDEBUG << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue add propagated value for later use, for" << property.getId() << v_data;
#endif
      m_toBePropagated.insert(std::make_pair(property,v_data));
    }
    else {
#ifdef DEBUG_LP
    LDEBUG << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue append propagated value for later use, for" << property.getId() << v_data;
#endif
      for(std::string data : v_data) {
          auto ite_Beg = m_toBePropagated[property].begin(),
              ite_End = m_toBePropagated[property].end();
          if(std::find(ite_Beg,ite_End,data) == ite_End) {
              m_toBePropagated[property].push_back(data);
          }
      }
    }

#ifdef DEBUG_LP
    LTRACE << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue propagating" << property.getId() << v_data;
#endif
    for(std::string data : v_data) {
      if (data.empty())
      {
#ifdef DEBUG_LP
        LDEBUG << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue do not try to propagate empty value for" << property.getId();
#endif
        continue;
      }
#ifdef DEBUG_LP
    LTRACE << "AbstractStructuredDocumentElementWithProperties::setPropagatedValue propagating" << property.getId() << data;
#endif
      switch (property.getStorageType()) {

      case STORAGE_DATE: {
        QDate dateValue;
        QDate dateIgnore;
        ContentStructuredDocument::parseDate(data,dateValue,dateIgnore);
        GenericDocumentProperties::setDateValue(property.getId(), dateValue);
        }
        break;
      case STORAGE_UTF8_STRING:
        if( property.getValueCardinality() == CARDINALITY_MULTIPLE)
          GenericDocumentProperties::addStringValue(property.getId(), data);
        else
          GenericDocumentProperties::setStringValue(property.getId(), data);
        break;
      case STORAGE_INTEGER:
      default:
        GenericDocumentProperties::setIntValue(property.getId(), atoi(data.c_str()));
        break;
      }
    }
  }
}

void AbstractStructuredDocumentElementWithProperties::addProperty(
    const DocumentPropertyType& propType, const string& value )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "AbstractStructuredDocumentElementWithProperties::addProperty" << getElementName() << propType.getId() << value;
#endif
  // insert property if it's not present
  if (m_toBePropagated.find(propType) == m_toBePropagated.end())
  {
    auto result = m_toBePropagated.insert(std::make_pair(propType, std::vector<std::string>(1,value) ));
    if (!result.second)
    {
      DRLOGINIT;
      LERROR << "AbstractStructuredDocumentElementWithProperties::addProperty FAILED to add" << getElementName() << propType.getId() << value;
    }
  }
  else
  {
    // replace property value or append if cardinality is multi-valued
    if (propType.getValueCardinality() == CARDINALITY_MULTIPLE) {
#ifdef DEBUG_LP
    LDEBUG << "AbstractStructuredDocumentElementWithProperties::addProperty append" << getElementName() << propType.getId() << "value" << m_toBePropagated[propType] << "with" << value;
#endif
        m_toBePropagated[propType].push_back( value );
    }
    else {
#ifdef DEBUG_LP
    LDEBUG << "AbstractStructuredDocumentElementWithProperties::addProperty set" << getElementName() << propType.getId() << "value" << m_toBePropagated[propType] << "by" << value;
#endif
        m_toBePropagated[propType] = std::vector<std::string>(1, value);
    }
  }
}

const PropagatedProperties&
AbstractStructuredDocumentElementWithProperties::getPropertyList() const
{
  return m_toBePropagated;
}

} // namespace DocumentsReader
}

