/************************************************************************
 * @file   contentDocument.cpp
 * @author Besancon Romaric
 * @date   Wed Oct 29 13:31:44 2003
 *
 *
 ***********************************************************************/

#include "DocumentElements.h"

#include "common/Data/strwstrtools.h"
#include "contentDocument.h"

#include <fstream>
#include <string>
#include <deque>
#include <boost/lexical_cast.hpp>

#include "linguisticProcessing/core/XmlProcessingCommon.h"

using namespace std;
using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::Misc;
 // to handle dates (parsing)

namespace Lima {
namespace DocumentsReader {

unsigned int ContentStructuredDocument::getOffset() {
#ifdef DEBUG_LP
  DRLOGINIT;
#endif
  unsigned int result = 0;
  if( !empty() ) {
    result = back()->getOffset();
  }
#ifdef DEBUG_LP
  LDEBUG << "ContentStructuredDocument::getOffset = " << result;
#endif
  return result;
}

void ContentStructuredDocument::addPart(const Lima::LimaString& text,
                                     const unsigned int offset ) {
  back()->addPart(text,offset);
}

DocumentPropertyElement* ContentStructuredDocument::addPropertyChild(
    const QString& elementName, unsigned int parserOffset, const DocumentPropertyType& type ) {
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::addPropertyChild" << elementName << "for property" << type.getId();
#endif
  DocumentPropertyElement* result;
  result = new DocumentPropertyElement(elementName, parserOffset, type );
  push_back(result);
  return result;
}

HierarchyDocumentElement* ContentStructuredDocument::pushHierarchyChild(
    const QString& elementName, unsigned int parserOffset,
    const DocumentPropertyType& propType )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::pushHierarchyChild" << elementName << parserOffset;
  if( propType.getValueCardinality() != CARDINALITY_NONE ) {
    LDEBUG  << "and with property:" << propType.getId();
  }
  else {
    LDEBUG  << "and without property";
  }
#else
  LIMA_UNUSED(propType);
#endif
  HierarchyDocumentElement* result;
  if( empty() ) {
#ifdef DEBUG_LP
    LDEBUG  << "empty";
#endif
    std::map<DocumentPropertyType, std::string> toBePropagated;
    result = new HierarchyDocumentElement(elementName, parserOffset, toBePropagated);
  }
  else {
#ifdef DEBUG_LP
    LDEBUG  << "not empty";
#endif
    AbstractStructuredDocumentElement* lastElement = back();
    HierarchyDocumentElement* hierarchyElement = dynamic_cast<HierarchyDocumentElement*>(lastElement);
#ifdef DEBUG_LP
    LDEBUG  << "structuredElement:" << hierarchyElement;
#endif
    if( hierarchyElement != 0 ) {
        result = new HierarchyDocumentElement(elementName, parserOffset, hierarchyElement->getPropertyList());
    }
  }
  result->GenericDocumentProperties::setIntValue("offBegPrpty", parserOffset);

  push_back(result );
#ifdef DEBUG_LP
  LDEBUG << "ContentStructuredDocument::pushHierarchyChild: end";
#endif
  return result;
}

IndexingDocumentElement* ContentStructuredDocument::pushIndexingChild(
    const QString& elementName, unsigned int parserOffset,
    const DocumentPropertyType& propType )
{

#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::pushIndexingChild" << elementName<< parserOffset;
  if( propType.getValueCardinality() != CARDINALITY_NONE ) {
    LDEBUG  << "and with property:" << propType.getId();
  }
  else {
    LDEBUG  << "and without property";
  }
#endif
  IndexingDocumentElement* result = 0;
  assert( !empty() ) ;
  AbstractStructuredDocumentElement* lastElement = back();
  HierarchyDocumentElement* hierarchyElement = dynamic_cast<HierarchyDocumentElement*>(lastElement);
#ifdef DEBUG_LP
  LDEBUG  << "structuredElement:" << hierarchyElement;
#endif
  if( hierarchyElement != 0 )
  {
    result = new IndexingDocumentElement(elementName, parserOffset, propType, hierarchyElement->getPropertyList());
  }
  else
  {
    // Can have only hierarchy parent
    return 0;
  }
  result->GenericDocumentProperties::setIntValue("offBegPrpty", parserOffset);

  push_back(result );
#ifdef DEBUG_LP
  LDEBUG << "ContentStructuredDocument::pushIndexingChild: end";
#endif
  return result;
}

IgnoredDocumentElement* ContentStructuredDocument::pushIgnoredChild(
    const QString& elementName, unsigned int parserOffset, const DocumentPropertyType& propType )
{

#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::pushIgnoredChild" << elementName<< parserOffset;
  if( propType.getValueCardinality() != CARDINALITY_NONE ) {
    LDEBUG  << "and with property:" << propType.getId();
  }
  else {
    LDEBUG  << "and without property";
  }
#endif

  AbstractStructuredDocumentElement* lastElement = back();
  AbstractStructuredDocumentElementWithProperties* propertiesElement = dynamic_cast<AbstractStructuredDocumentElementWithProperties*>(lastElement);
  IgnoredDocumentElement* result = new IgnoredDocumentElement(elementName, parserOffset, propType, propertiesElement->getPropertyList());
#ifdef DEBUG_LP
  assert( !empty() ) ;
#endif

  push_back(result );
#ifdef DEBUG_LP
  LDEBUG << "ContentStructuredDocument::pushIgnoredChild: end";
#endif
  return result;
}

DiscardableDocumentElement* ContentStructuredDocument::pushDiscardableChild(const QString& elementName, unsigned int parserOffset )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::addDiscardableChild" << elementName << parserOffset;
#endif
  DiscardableDocumentElement* result;
  result = new DiscardableDocumentElement(elementName, parserOffset );
  push_back(result );
  return result;
}

PresentationDocumentElement* ContentStructuredDocument::pushPresentationChild(const QString& elementName, unsigned int parserOffset )
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::pushPresentationChild" << elementName << parserOffset <<back()->getOffset();
#endif

  PresentationDocumentElement* result;
#ifdef DEBUG_LP
  assert( !empty() );
#endif
  IndexingDocumentElement* structuredElement = dynamic_cast<IndexingDocumentElement*>(back());
  if( structuredElement != 0 )
  {
    result = new PresentationDocumentElement(elementName, parserOffset, structuredElement->getPropertyList() );
    result->addSpaces(parserOffset-back()->getOffset());
  }
  else
  {
    const std::map<DocumentPropertyType, std::string> emptyPropagated;
    result = new PresentationDocumentElement(elementName, parserOffset, emptyPropagated );
    result->addSpaces(parserOffset-back()->getOffset());
  }

  push_back(result);
  return result;
}

ContentStructuredDocument::~ContentStructuredDocument() {
  for( deque<AbstractStructuredDocumentElement*>::iterator cIt = begin() ; cIt != end() ; cIt++ ) {
    delete *cIt;
  }
}

void ContentStructuredDocument::popDiscardableElement(unsigned int parserOffset)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::popDiscardableElement" << parserOffset;
#endif
  AbstractStructuredDocumentElement* currentElement = back();
#ifdef DEBUG_LP
  assert( currentElement->nodeType() == NODE_DISCARDABLE );
#endif
  pop_back();
#ifdef DEBUG_LP
  assert( size() != 0 );
#endif

  if (!empty())
  {
    AbstractStructuredDocumentElement* newCurrent = back();
    newCurrent->addToCurrentOffset(QString(parserOffset-newCurrent->getOffset(),' '));
    newCurrent->setOffset(parserOffset);
  }
  delete currentElement;
}

void ContentStructuredDocument::popPropertyElement(unsigned int parserOffset)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::popPropertyElement" << parserOffset;
#endif
  AbstractStructuredDocumentElement* currentElement = back();
#ifdef DEBUG_LP
  assert( currentElement->nodeType() == NODE_PROPERTY );
#endif
  pop_back();
#ifdef DEBUG_LP
  assert( size() != 0 );
#endif
  AbstractStructuredDocumentElement* parentElement = back();
  // TODO Handle property element specifics
  parentElement->setOffset(parserOffset);
  delete currentElement;
}

void ContentStructuredDocument::popPresentationElement( unsigned int parserOffset ) {
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::popPresentationElement" << parserOffset;
#endif
  PresentationDocumentElement* currentElement = dynamic_cast<PresentationDocumentElement*>(back());
  currentElement->back()->addText(QString(parserOffset-currentElement->getOffset(),' '));
#ifdef DEBUG_LP
  assert( currentElement != 0 );
#endif
  pop_back();
  if (!empty())
  {
    AbstractStructuredDocumentElement* newCurrent = back();
    if (dynamic_cast<HierarchyDocumentElement*>(newCurrent) !=0)
    {
      dynamic_cast<HierarchyDocumentElement*>(newCurrent)->setPropagatedValue(currentElement->getPropertyList());
    }
    newCurrent->back()->addText(currentElement->back()->getText());
    newCurrent->setOffset(parserOffset);
  }
  delete currentElement;
}

void ContentStructuredDocument::popIndexingElement(unsigned int parserOffset)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::popIndexingElement" << parserOffset;
#endif
  IndexingDocumentElement* currentElement = dynamic_cast<IndexingDocumentElement*>(back());
#ifdef DEBUG_LP
  assert( currentElement != 0 );
#endif
  pop_back();
  if (currentElement->hasPropType() && !currentElement->getPropType().getId().empty())
  {
    currentElement->addProperty(currentElement->getPropType(),currentElement->back()->getText() .toUtf8().constData());
  }
  if (!empty())
  {
    AbstractStructuredDocumentElement* newCurrent = back();
    if (dynamic_cast<HierarchyDocumentElement*>(newCurrent) !=0)
    {
      dynamic_cast<HierarchyDocumentElement*>(newCurrent)->setPropagatedValue(currentElement->getPropertyList());
    }
    newCurrent->setOffset(parserOffset);
  }
  delete currentElement;
}

void ContentStructuredDocument::popIgnoredElement(unsigned int parserOffset)
{
#ifdef DEBUG_LP
  DRLOGINIT;
#endif
  IgnoredDocumentElement* currentElement = dynamic_cast<IgnoredDocumentElement*>(back());
#ifdef DEBUG_LP
  assert( currentElement != 0 );
  LDEBUG << "ContentStructuredDocument::popIgnoredElement poping" <<currentElement->getElementName() << "at" << parserOffset;
#endif
  pop_back();
  if (!empty())
  {
    AbstractStructuredDocumentElement* newCurrent = back();
#ifdef DEBUG_LP
    LDEBUG << "ContentStructuredDocument::popIgnoredElement new current element is" <<newCurrent->getElementName();
#endif
    if (dynamic_cast<HierarchyDocumentElement*>(newCurrent) !=0)
    {
#ifdef DEBUG_LP
      LDEBUG << "ContentStructuredDocument::popIgnoredElement propagating properties from" <<currentElement->getElementName() <<"to"<<newCurrent->getElementName();
#endif
      dynamic_cast<HierarchyDocumentElement*>(newCurrent)->setPropagatedValue(currentElement->getPropertyList());
    }
    newCurrent->setOffset(parserOffset);
  }
  delete currentElement;
}

// Cas des unit hierarchie
void ContentStructuredDocument::popHierarchyElement(unsigned int parserOffset)
{
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::popHierarchyElement" << parserOffset;
#endif
  HierarchyDocumentElement* currentElement = dynamic_cast<HierarchyDocumentElement*>(back());
#ifdef DEBUG_LP
  assert( currentElement != 0 );
#endif
  pop_back();
  if (!empty())
  {
    AbstractStructuredDocumentElement* newCurrent = back();
    if (dynamic_cast<HierarchyDocumentElement*>(newCurrent) !=0)
    {
      dynamic_cast<HierarchyDocumentElement*>(newCurrent)->setPropagatedValue(currentElement->getPropertyList());
    }
    newCurrent->setOffset(parserOffset);
  }
  delete currentElement;
}

void ContentStructuredDocument::setDataToPreviousElement( StructuredXmlDocumentHandler* processor ) {
#ifdef DEBUG_LP
  DRLOGINIT;
#endif
  ContentStructuredDocument::reverse_iterator rIt = rbegin();
  AbstractStructuredDocumentElement* currentElement = *rIt;
  const DocumentPropertyType& propType = currentElement->getPropType();
  //assert( currentElement->size() == 1 );
  const Lima::LimaString text = currentElement->getPropertyValue();
  std::string utf8text = Misc::limastring2utf8stdstring(text);
#ifdef DEBUG_LP
  LDEBUG << "ContentStructuredDocument::setDataToPreviousElement: set property "
         << propType.getId() << " to " << utf8text;
#endif
  rIt++;
#ifdef DEBUG_LP
  assert(rIt != rend());
#endif
  AbstractStructuredDocumentElement* parentElement = *rIt;
  setDataToElement( parentElement, propType, utf8text, processor );
}

void ContentStructuredDocument::setDataToLastElement( const DocumentPropertyType& property,
  const std::string& data, StructuredXmlDocumentHandler* processor ) {
  AbstractStructuredDocumentElement* absElement = back();

  setDataToElement( absElement, property, data, processor );
}

void ContentStructuredDocument::setDataToElement( AbstractStructuredDocumentElement* absElement,
  const DocumentPropertyType& property, const std::string& data,
  StructuredXmlDocumentHandler* processor ) {
#ifdef DEBUG_LP
  DRLOGINIT;
  LDEBUG << "ContentStructuredDocument::setDataToElement" << absElement->getElementName()
        << property.getId() << property.getStorageType() << property.getValueCardinality() << data;
#endif
  if(property.getId().empty()) {
    DRLOGINIT;
    LWARN << "ContentStructuredDocument::setDataToElement: property id is empty. Do not use it" << absElement->getElementName();
    return;
  }
  AbstractStructuredDocumentElementWithProperties* element = dynamic_cast<AbstractStructuredDocumentElementWithProperties*>(absElement);

  if( element == 0 )
  {
    DRLOGINIT;
    LWARN << "ContentStructuredDocument::setDataToElement: element is not a AbstractStructuredDocumentElementWithProperties" << absElement->getElementName();
    return;
  }

  switch (property.getStorageType()) {

    case STORAGE_DATE: {
      QDate dateBegin;
      QDate dateEnd;
      parseDate(data,dateBegin,dateEnd);

      element->setDateValue("date_begin", dateBegin);
      element->setDateValue("date_end", dateEnd);
      element->setDateIntervalValue(property.getId(), make_pair(dateBegin,dateEnd) );
      }
      break;
    case STORAGE_UTF8_STRING:
      if( property.getValueCardinality() == CARDINALITY_MULTIPLE)
        element->addStringValue(property.getId(), data);
      else
        element->setStringValue(property.getId(), data);
      break;
    case STORAGE_INTEGER:
      element->setIntValue(property.getId(), atoi(data.c_str()));
      break;
    default:
      break;
  }
  // ajout a la liste des proprietes a propager
  element->addProperty( property, data );
  processor->handleProperty( property, data);

#ifdef DEBUG_LP
  LDEBUG << "ContentStructuredDocument::setDataToElement" << element->getElementName() <<"field("  << property.getId()  << ")=["<< data <<"]" << element->getStringValue(property.getId()).first << (*element->getPropertyList().find(property)).second;
#endif
}

void ContentStructuredDocument::parseDate(const string& dateStr,
                                  QDate& dateBegin,
                                  QDate& dateEnd)
{
  dateBegin=QDate();
  dateEnd=QDate();

  char sep('/');

  //uint64_t firstSep,secondSep; portage 32 64
  std::string::size_type firstSep,secondSep;
  try {
    if ( (firstSep=dateStr.find(sep)) != string::npos ) {
      if ( (secondSep=dateStr.find(sep,firstSep+1)) != string::npos ) {
        // suppose day/month/year

        // from_uk_string only defined in recent versions of boost
//         dateBegin=boost::gregorian::from_uk_string(dateStr);
//         dateEnd=boost::gregorian::from_uk_string(dateStr);
        // current version knows only year/month/day

        string day(dateStr,0,firstSep);
        string month(dateStr,firstSep+1,secondSep-firstSep);
        string year(dateStr,secondSep+1);
        string newDateStr=year+'/'+month+'/'+day;
        dateBegin=QDate::fromString(newDateStr.c_str());
        dateEnd=QDate::fromString(newDateStr.c_str());
      }
      else {
        // one separator : suppose month/year
        // find end of month
        string month(dateStr,0,firstSep);
        string year(dateStr,firstSep+1);
        unsigned short monthNum=atoi(month.c_str());
        unsigned short yearNum=atoi(year.c_str());
        dateBegin=QDate(yearNum,monthNum,1);
        dateEnd=dateBegin.addMonths(1).addDays(-1);
      }
    }
    else if (! dateStr.empty()) {
      // no separator : suppose year
      unsigned short yearNum=atoi(dateStr.c_str());
      dateBegin=QDate(yearNum,01,01);
      dateEnd=QDate(yearNum,12,31);
    }
  }
  //catch (boost::bad_lexical_cast& e) {
  catch (std::exception& e) {
    DRLOGINIT;
    LWARN << "Warning: " << e.what();
    LWARN << "Failed parsing of date [" << dateStr << "]";
    // do nothing, keep not_a_date_time as default value
  }
}


} // namespace DocumentsReader
}

