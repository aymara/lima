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
 * @file       bowXMLWriter.cpp
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Wed May  5 2004
 * copyright   Copyright (C) 2004 by CEA LIST
 * 
 ***********************************************************************/

#include "bowXMLWriter.h"
#include "bowToken.h"
#include "bowComplexToken.h"
#include "BoWRelation.h"
#include "bowText.h"
#include "bowDocument.h"
#include "indexElement.h"
#include "bowTerm.h"
#include "bowNamedEntity.h"
#include "bowDocumentST.h"
#include "bowTokenIterator.h"
#include "indexElementIterator.h"
#include "BoWPredicate.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/linguisticData/languageData.h"
#include "common/PropertyCode/PropertyCodeManager.h"
#include "common/Data/genericDocumentProperties.h"


using namespace std;

namespace Lima {
namespace Common {
namespace BagOfWords {


class BoWXMLWriterPrivate
{
friend class BoWXMLWriter;

  BoWXMLWriterPrivate(std::ostream& os);
  virtual ~BoWXMLWriterPrivate();


  std::ostream& m_outputStream;

  uint64_t m_currentTokId;

  std::string m_spaces;         /**< for pretty indentations */

  Lima::MediaId m_language;

  // private functions
  void setSpaces(const std::string& s);

  void writeBoWToken(const AbstractBoWElement* token);
  void writeBoWRelation(const BoWRelation* relation);
  void writeComplexTokenParts(const BoWComplexToken* token);
  void writeBoWTokenList(const BoWText* text,
                         const bool useIterator,
                         const bool useIndexIterator);
  void writeGenericDocumentProperties(const Misc::GenericDocumentProperties* prop);
  void writePredicateRoles(const BoWPredicate* term);
  template<typename PropertyType>
  void writeProperty(const std::string& name,
                       const std::string& type,
                       const PropertyType& value);
  void writeIndexElement(const IndexElement& element);

  // encode possible XML entities
  std::string xmlString(const std::string& str) const;

  // for indentations
  void incIndent() { m_spaces+="  "; }
  void decIndent() { m_spaces.erase(m_spaces.size()-2); }

  void setLanguage(const Lima::MediaId lang);
  void setLanguage(const std::string& lang);
};

void BoWXMLWriter::incIndent() { m_d->incIndent() ; }
void BoWXMLWriter::decIndent() { m_d->decIndent(); }
  
BoWXMLWriterPrivate::BoWXMLWriterPrivate(std::ostream& os):
m_outputStream(os),
m_currentTokId(0),
m_spaces(""),
m_language(0)
{
}

BoWXMLWriterPrivate::~BoWXMLWriterPrivate()
{
}

//**********************************************************************
// writer functions
//**********************************************************************
BoWXMLWriter::BoWXMLWriter(std::ostream& os):
    m_d(new BoWXMLWriterPrivate(os))
{
}

BoWXMLWriter::~BoWXMLWriter()
{
  delete m_d;
}

/*
void BoWXMLWriter::setOutputStream(std::ostream& os) {
  m_os = &os;
}
*/

void BoWXMLWriter::writeBoWDocumentsHeader() {
  m_d->m_outputStream << "<?xml-stylesheet type=\"text/xsl\" href=\"bow.xslt\"?>" << endl;
  m_d->m_outputStream <<"<bowDocuments>" << endl;
}

void BoWXMLWriter::writeBoWDocumentsFooter() {
  m_d->m_outputStream <<"</bowDocuments>" << endl;
}

void BoWXMLWriter::openSBoWNode( 
  const Lima::Common::Misc::GenericDocumentProperties* /*unused properties*/,
  const std::string& elementName ) {
  m_d->m_outputStream << m_d->m_spaces << "<hierarchy elementName=\"" << elementName << "\">" << std::endl;
  m_d->incIndent();
}

void BoWXMLWriter::openSBoWIndexingNode(
  const Lima::Common::Misc::GenericDocumentProperties* /*unused properties*/,
  const std::string& elementName ) {
  m_d->m_currentTokId=0;
  m_d->m_outputStream << m_d->m_spaces << "<hierarchy elementName=\"" << elementName << "\" indexingNode=\"yes\">" << std::endl;
  m_d->incIndent();
}

void BoWXMLWriter::closeSBoWNode() {
  m_d->m_outputStream <<m_d->m_spaces <<"</hierarchy>" << std::endl;
  m_d->decIndent();
}

void BoWXMLWriter::processSBoWText( const BoWText* boWText, bool useIterator,
                         bool useIndexIterator) {
  m_d->m_language = Common::MediaticData::MediaticData::single().getMediaId ( boWText->lang );

  m_d->writeBoWTokenList(boWText,useIterator,useIndexIterator);
}

void BoWXMLWriter::processProperties( 
 const Misc::GenericDocumentProperties* properties, bool /*unused useIterators*/,
                         bool /*useIndexIterator*/) {
  m_d->writeGenericDocumentProperties(properties);
}

void BoWXMLWriter::writeBoWText(
                                const BoWText* text,
                                const bool useIterator,
                                const bool useIndexIterator) {
  //m_d->m_language = Common::MediaticData::MediaticData::single().getMediaId ( text->lang );

  m_d->m_language = Common::MediaticData::MediaticData::single().getMediaId ( text->lang );
  m_d->m_spaces="";
  m_d->m_outputStream <<"<bowText>" << std::endl;
  m_d->incIndent();
  m_d->writeBoWTokenList(text,useIterator,useIndexIterator);
  m_d->decIndent();
  m_d->m_outputStream <<"</bowText>" << std::endl;
}
void BoWXMLWriter::writeBoWToken(const BoWToken* token)
{
   m_d->writeBoWToken(token);
}
// definition of template can be in .cpp because all needed 
// instanciations are in this definition file
template<typename PropertyType>
void BoWXMLWriterPrivate::writeProperty( const std::string& name,
                                 const std::string& type, 
                                 const PropertyType& value) {
  m_outputStream <<m_spaces << "<property name=\"" << name
     << "\" type=\""<< type
     << "\" value=\"" << value
     << "\"/>" << std::endl;
}

void BoWXMLWriterPrivate::writeGenericDocumentProperties(
  const Misc::GenericDocumentProperties* prop) {

  m_outputStream <<m_spaces << "<properties>" << std::endl;
  incIndent();

  // write values of properties whose type is int
  typedef Misc::GenericDocumentProperties::IntPropertiesIterator IntIter;
  {
    std::pair<IntIter,IntIter> ret = prop->getIntProperties();
    for( IntIter it = ret.first ; it != ret.second ; it++ ) {
      std::pair<std::string,uint64_t> elmt = *it;
      m_outputStream <<m_spaces << "<property name=\"" << elmt.first << "\""
         << " type=\"int\""
         << " value=\"" << elmt.second << "\"/>"  
         << std::endl;
    }
  }
  typedef Misc::GenericDocumentProperties::StringPropertiesIterator StringIter;
  {
    std::pair<StringIter,StringIter> ret = prop->getStringProperties();
    for( StringIter it = ret.first ; it != ret.second ; it++ ) {
      std::pair<std::string,std::string> elmt = *it;
      m_outputStream <<m_spaces << "<property name=\"" << elmt.first << "\""
         << " type=\"string\""
         << " value=\"" << xmlString(elmt.second) << "\"/>"  
         << std::endl;
    }
  }
  typedef Misc::GenericDocumentProperties::DatePropertiesIterator DateIter;
  {
    std::pair<DateIter,DateIter> ret = prop->getDateProperties();
    for( DateIter it = ret.first ; it != ret.second ; it++ ) {
      std::pair<std::string,QDate> elmt = *it;
      m_outputStream <<m_spaces << "<property name=\"" << elmt.first << "\""
         << " type=\"date\""
         << " value=\"" << elmt.second.toString("yyyyMMdd").toUtf8().data() << "\"/>"
         << std::endl;
    }
  }
  typedef Misc::GenericDocumentProperties::DateIntervalPropertiesIterator DateIntervIter;
  {
    std::pair<DateIntervIter,DateIntervIter> ret = prop->getDateIntervalProperties();
    for( DateIntervIter it = ret.first ; it != ret.second ; it++ ) {
      std::pair<std::string,std::pair<QDate,QDate> > elmt = *it;
      m_outputStream <<m_spaces << "<property name=\"" << elmt.first << "\""
         << " type=\"dateRange\""
         << " valueBegin=\"" << (elmt.second).first.toString("yyyyMMdd").toUtf8().data() << "\""
         << " valueEnd=\"" << (elmt.second).second.toString("yyyyMMdd").toUtf8().data()
         << "\"/>" << std::endl;
    }
  }
  std::pair<Misc::MultiValuedPropertyIterator<std::string>,
            Misc::MultiValuedPropertyIterator<std::string> > svnit =
     prop->getStringPropertyNames();
//  m_outputStream << "BoWXMLWriter::writeGenericDocumentProperties: has multi-valued string properties "
//         << (svnit.first != svnit.second ? "yes" : "no" ) << std::endl;
  for( ; svnit.first != svnit.second; (svnit.first)++ ) {
    std::string propName = *(svnit.first);
    std::pair<Misc::StringPropMultIter,Misc::StringPropMultIter> svit = prop->getMultipleStringPropValue(propName);
    for( ; svit.first != svit.second ;  (svit.first)++ ) {
      m_outputStream <<m_spaces << "<property name=\"" << propName << "\""
         << " type=\"multString\""
         << " value=\"" << xmlString(*(svit.first)) << "\"/>"  
         << std::endl;
    }
  }
  std::pair<Misc::MultiValuedPropertyIterator<std::pair<std::string,float> >,
            Misc::MultiValuedPropertyIterator<std::pair<std::string,float> > > swnit =
    prop->getWeightedPropPropertyNames();  
//  m_outputStream << "BoWXMLWriter::writeGenericDocumentProperties: has multi-valued string properties "
//         << (swnit.first != swnit.second ? "yes" : "no" ) << std::endl;
  for( ; swnit.first != swnit.second; (swnit.first)++ ) {
    std::string propName = *(swnit.first);
    std::pair<Misc::WeightedPropMultIter,Misc::WeightedPropMultIter> svit = prop->getMultipleWeightedPropValue(propName);
    for( ; svit.first != svit.second ;  (svit.first)++ ) {
      m_outputStream <<m_spaces << "<property name=\"" << propName << "\""
         << " type=\"multWeightedId\""
         << "\" value=\"" << xmlString((*(svit.first)).first)
         << "\" weight=\"" << (*(svit.first)).second << "\"/>"  
         << std::endl;
    }
  }
  
  decIndent();
  m_outputStream <<m_spaces << "</properties>" << std::endl;
}

void BoWXMLWriterPrivate::writePredicateRoles(const BoWPredicate* term)
{
  m_outputStream <<m_spaces << "<roles>" << std::endl;
  incIndent();
  for (auto it = term->roles().begin(); it != term->roles().end(); it++)
  {
    m_outputStream <<m_spaces << "<role type=\""
       <<  (it.key().isNull() ? "" : Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName(it.key())))
       << "\" >" << std::endl;
    incIndent();
    writeBoWToken(&*it.value());
    decIndent();
    m_outputStream <<m_spaces  << "</role>" << std::endl;
  }
  decIndent();
  m_outputStream <<m_spaces << "</roles>" << std::endl;
  term->roles();
}

void BoWXMLWriterPrivate::writeBoWTokenList(
                                     const BoWText* text,
                                     const bool useIterator,
                                     const bool useIndexIterator) {
  
  m_outputStream <<m_spaces << "<tokens>" << std::endl;
  incIndent();
  if (useIterator)
  {
    BoWTokenIterator it(*text);
    while (! it.isAtEnd())
    { 
      writeBoWToken( &*it.getElement());
      it++;
    }
  }
  else if (useIndexIterator)
  {
    IndexElementIterator it(*text);
    while (! it.isAtEnd())
    { 
      writeIndexElement(it.getElement());
      it++;
    }
  }
  else
  {
    for (BoWText::const_iterator tok=text->begin();
        tok!=text->end(); tok++) {
      writeBoWToken( &**tok);
    }
  }
  decIndent();
  m_outputStream <<m_spaces << "</tokens>" << std::endl;
}

void BoWXMLWriterPrivate::writeIndexElement(
                  const IndexElement& element) {
  m_outputStream << m_spaces << "<term "
     << "id=\"" << element.getId() << "\"";
  if (element.empty()) {
    m_outputStream << "/>" << endl;
    return;
  }
  if (element.isSimpleTerm()) {
    std::string cat = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(static_cast<Lima::LinguisticCode>(element.getCategory()));

    m_outputStream << " lemma=\"" << xmlString(Common::Misc::limastring2utf8stdstring(element.getSimpleTerm()))
       << "\" category=\"" << cat
       << "\" position=\"" << element.getPosition()
       << "\" length=\"" << element.getLength() << "\"";
    if (element.isNamedEntity()) {
      m_outputStream << " neType=\"" << element.getNamedEntityType() << "\"";
      m_outputStream << " type=\"" << BoWType::BOW_NAMEDENTITY << "\"";
    }
    else {
      m_outputStream << " type=\"" << BoWType::BOW_TOKEN << "\"";
    }
    m_outputStream << "/>" << endl;
    return;
  }
  
  // compound
  if (element.isNamedEntity()) {
    m_outputStream << " neType=\"" << element.getNamedEntityType() << "\"";
    m_outputStream << " type=\"" << BoWType::BOW_NAMEDENTITY << "\"";
  }
  else {
    m_outputStream << " type=\"" << BoWType::BOW_TERM << "\"";
  }
  m_outputStream << ">" << endl
     << m_spaces << "  <structure>" << endl;
  
  for (uint64_t i(0),size=element.getStructure().size(); i<size; i++) {
    m_outputStream << m_spaces << "    <termRef id=\""
       << element.getStructure()[i]
       << "\" rel=\"" << element.getRelations()[i] 
       << "\"/>" << endl;
    
  }
  m_outputStream << m_spaces << "  </structure>" << endl
     << m_spaces << "</term>" << endl;
}

void BoWXMLWriterPrivate::writeBoWRelation(const BoWRelation* relation) {
    m_outputStream <<m_spaces << "<bowRelation "
       << "realization=\"" 
       << xmlString(Misc::limastring2utf8stdstring(relation->getRealization( )))
       << "\" type=\"" << relation->getType() << "\"/>"
       << std::endl;
}

void BoWXMLWriterPrivate::writeBoWToken(
                                 const Lima::Common::BagOfWords::AbstractBoWElement* token ) {
  m_currentTokId++;
  const BoWToken* tok = 0;
  switch(token->getType()) {
  case BoWType::BOW_TOKEN: {
    tok = static_cast<const BoWToken*>(token);
    std::string cat = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(static_cast<Lima::LinguisticCode>(tok->getCategory()));

    m_outputStream <<m_spaces << "<bowToken "
       << "id=\"" << m_currentTokId
       << "\" lemma=\"" << xmlString(Misc::limastring2utf8stdstring(tok->getLemma()))
       << "\" category=\"" << cat
       <<"\" position=\"" << tok->getPosition() 
       << "\" length=\"" << tok->getLength() << "\"" 
       << "/>" << std::endl;
    break;
  }
  case BoWType::BOW_PREDICATE: {
    const BoWPredicate* term=static_cast<const BoWPredicate*>(token);
    m_outputStream <<m_spaces << "<bowPredicate "
       << "id=\"" << m_currentTokId
       << "\" lemma=\"" << xmlString(Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName(term->getPredicateType())))
       <<"\" position=\"" << term->getPosition() 
       << "\" length=\"" << term->getLength() << "\"" 
       << ">" << std::endl;
    incIndent();
    writePredicateRoles(term);
    decIndent();
    m_outputStream <<m_spaces << "</bowPredicate>" << std::endl;
    break;
  }
  case BoWType::BOW_TERM: {
    const BoWTerm* term=static_cast<const BoWTerm*>(token);
    std::string cat = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(static_cast<Lima::LinguisticCode>(term->getCategory()));

    m_outputStream <<m_spaces << "<bowTerm "
       << "id=\"" << m_currentTokId
       << "\" lemma=\"" << xmlString(Misc::limastring2utf8stdstring(term->getLemma()))
       << "\" category=\"" << cat
       <<"\" position=\"" << term->getPosition()
       << "\" length=\"" << term->getLength() << "\""
       << ">" << std::endl;
    incIndent();
    writeComplexTokenParts( term);
    decIndent();
    m_outputStream <<m_spaces << "</bowTerm>" << std::endl;
    break;
  }
  case BoWType::BOW_NAMEDENTITY: {
    const BoWNamedEntity* ne=static_cast<const BoWNamedEntity*>(token);
    std::string cat = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(static_cast<Lima::LinguisticCode>(ne->getCategory()));

    m_outputStream <<m_spaces << "<bowNamedEntity "
       << "id=\"" << m_currentTokId
       << "\" lemma=\"" << xmlString(Misc::limastring2utf8stdstring(ne->getLemma()))
       << "\" category=\"" << cat
       <<"\" position=\"" << ne->getPosition() 
       << "\" length=\"" << ne->getLength() 
       << "\" type=\""  
       << Misc::limastring2utf8stdstring(MediaticData::MediaticData::single().getEntityName(ne->getNamedEntityType())) 
       << "\"" 
       << ">" << std::endl;
    incIndent();
    writeComplexTokenParts( ne);
    for (std::map<std::string, LimaString>::const_iterator
           feature=ne->getFeatures().begin();
         feature!=ne->getFeatures().end(); feature++) {
      m_outputStream <<m_spaces << "<feature name=\"" << (*feature).first
         << "\" value=\"" 
         << xmlString(Common::Misc::limastring2utf8stdstring((*feature).second))
         << "\"/>" <<std::endl;
    }
    decIndent();
    m_outputStream <<m_spaces << "</bowNamedEntity>" << std::endl;
    break;
  }
  default:
    BOWLOGINIT;
    LERROR << "Invalid BoW token: no type token";
  }
}

void BoWXMLWriterPrivate::writeComplexTokenParts(const BoWComplexToken* token) {
  if (token->getParts().empty()) {
    return;
  }
  m_outputStream <<m_spaces << "<parts head=\""<< token->getHead() << "\">" << std::endl;
  incIndent();
  for (std::deque<BoWComplexToken::Part>::const_iterator
         part=token->getParts().begin(); 
       part!=token->getParts().end(); part++)
  {
//     m_outputStream <<m_spaces << "<part>" << std::endl;
//     incIndent();
    if ((*part).getBoWRelation()!=0)
    {
      writeBoWRelation( &*(*part).getBoWRelation());
    }
    writeBoWToken( &*(*part).getBoWToken());

//     decIndent();
//     m_outputStream <<m_spaces << "</part>" << std::endl;
  }
  decIndent();
  m_outputStream <<m_spaces << "</parts>" << std::endl;
}

// helper function: encode XML entities (to ensure the output to be valid XML)
std::string BoWXMLWriterPrivate::xmlString(const std::string& s) const
{
  static char charsToEncode[4]={'&','"','<','>'};
  static string xmlEntities[4]={"&amp;","&quot;","&lt;","&gt;"};
  // &amp; first is important because otherwise, replace & in other entities

  string str(s);
  for (uint64_t i=0; i<4; i++) {
    //uint64_t prev=0;
    //uint64_t k=str.find(charsToEncode[i],prev);
    // uint64_t prev=0;
    //uint64_t k=str.find(charsToEncode[i],prev);
    string::size_type prev=0;
    string::size_type k=str.find(charsToEncode[i],prev);
    while (k!=string::npos) {
      str.replace(k,1,xmlEntities[i]);
      prev=k+1; //+1 is enough even is more characters have been added
      k=str.find(charsToEncode[i],prev);
    }
  }
  return str;
}

void BoWXMLWriterPrivate::setSpaces(const std::string& s)
{
  m_spaces=s;
}

void BoWXMLWriter::setSpaces(const std::string& s)
{
  m_d->setSpaces(s);
}

void BoWXMLWriter::setLanguage(const Lima::MediaId lang)
{
  m_d->setLanguage(lang);
}

void BoWXMLWriterPrivate::setLanguage(const Lima::MediaId lang)
{
  m_language = lang;
}

void BoWXMLWriter::setLanguage(const std::string& lang)
{
  m_d->setLanguage(lang);
}

void BoWXMLWriterPrivate::setLanguage(const std::string& lang)
{
  m_language = Common::MediaticData::MediaticData::single().getMediaId (lang);
}

void BoWXMLWriter::writeIndexElement(const IndexElement& element)
{
  m_d->writeIndexElement(element);
}


} // end namespace
} // end namespace
} // end namespace
