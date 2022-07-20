// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file       XmlSyntagmaticMatrixFileHandler.cpp
  * @author     Gael de Chalendar (Gael.de-Chalendar@cea.fr)

  *             Copyright (c) 2003 by CEA
  * @date       Created on  Mon Aug 04 2003
  *
  */


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "XmlSyntagmaticMatrixFileHandler.h"
#include "XmlSyntagmaticMatrixFileExceptions.h"
#include "common/MediaticData/mediaticData.h"

#include "common/Data/strwstrtools.h"


#include<boost/tokenizer.hpp>

#include <sstream>
#include <iostream>

using namespace std;

using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

// ---------------------------------------------------------------------------
//  XMLSyntagmaticMatrixFileHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
XMLSyntagmaticMatrixFileHandler::XMLSyntagmaticMatrixFileHandler(SyntagmDefStruct& matrices,MediaId language) :
        m_matrices(matrices),
        m_currentChainType(NO_CHAIN_TYPE),
        m_language(language)
{
}

XMLSyntagmaticMatrixFileHandler::~XMLSyntagmaticMatrixFileHandler()
{
}


// ---------------------------------------------------------------------------
//  XMLSyntagmaticMatrixFileHandler: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
bool XMLSyntagmaticMatrixFileHandler::error(const QXmlParseException& e)
{
    SALOGINIT;
    LERROR << "Error at file " << e.systemId()
        << ", line " << e.lineNumber()
        << ", char " << e.columnNumber()
       << "  Message: " << e.message();
       return false;
}

bool XMLSyntagmaticMatrixFileHandler::fatalError(const QXmlParseException& e)
{
    SALOGINIT;
    LERROR << "Fatal Error at file " << e.systemId()
         << ", line " << e.lineNumber()
         << ", char " << e.columnNumber()
         << "  Message: " << e.message();
         return false;
}

bool XMLSyntagmaticMatrixFileHandler::warning(const QXmlParseException& e)
{
  SALOGINIT;
  LWARN << "Warning at file " << e.systemId()
        << ", line " << e.lineNumber()
        << ", char " << e.columnNumber()
        << "  Message: " << e.message();
        return true;
}


// ---------------------------------------------------------------------------
//  XMLSyntagmaticMatrixFileHandler: Overrides of the SAX DTDHandler interface
// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
//  XMLSyntagmaticMatrixFileHandler: Overrides of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
bool XMLSyntagmaticMatrixFileHandler::characters(const     QString&    chars)
{
    std::string str = limastring2utf8stdstring(chars);

    if (m_currentElement == "nom")
    {
//      <!ELEMENT nom (#PCDATA ) >
        m_currentFilterName = str;
//        std::cerr << "Current filter name is: " << m_currentFilterName << std::endl;
    }
    else if (m_currentElement == "attribut")
    {
//      <!ELEMENT attribut (#PCDATA ) >

        //string2wstring(attribute, str);
        m_currentFilterAttributes.insert(Common::MediaticData::MediaticData::changeable().stringsPool(m_language)[chars]);
//        std::wcerr << L"Current attribute is: " << attribute << std::endl;
    }
    else if (m_currentElement == "categorie")
    {
//      <!ELEMENT categorie (#PCDATA ) >

        LinguisticCode filterCategory;
        if ( isdigit(str[0]))
        {
            filterCategory = LinguisticCode::fromString(str);
        }
        else
        {
            filterCategory = static_cast<const Common::MediaticData::LanguageData&>(MediaticData::single().mediaData(m_matrices.language())).getPropertyCodeManager().getPropertyManager("MICRO").getPropertyValue(str);
        }
        m_currentFilterCategories.insert(filterCategory);
//        std::cerr << "Current category is: " << filterCategory << std::endl;
    }
    return true;
}


bool XMLSyntagmaticMatrixFileHandler::startElement(const QString & namespaceURI, const QString & name, const QString & qName, const QXmlAttributes & attributes)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  //  <!ELEMENT matrice_précedence (chaine+, matrice+)>

    if (name == 0)
    {
        SALOGINIT;
        LERROR << "Error: receiving a null element name while parsing matrix file";
        return false;
    }

    const QString& stringName = name;

//    std::cerr << "Starting element: " << stringName << std::endl;

    if (stringName == ("matrice"))
//      <!ELEMENT matrice (ligne+, debuts+, fin+)>
//      <!ATTLIST matrice type (nominale | verbale) #REQUIRED>
    {
        QString currentMatrixType = attributes.value("type");
        if (currentMatrixType == "nominale") m_currentMatrixType = NOMINAL;
        else if (currentMatrixType == "verbale") m_currentMatrixType = VERBAL;
        else m_currentMatrixType = NO_CHAIN_TYPE;
    }
    else if (stringName == ("ligne"))
//      <!ELEMENT ligne (succ*)>
//      <!ATTLIST ligne REF IDREF #IMPLIED>
    {
        m_currentMatrixLineId = attributes.value("REF").toUtf8().data();
    }
    else if (stringName == ("succ"))
//      <!ELEMENT succ EMPTY>
//      <!ATTLIST succ REF IDREFS #IMPLIED>
    {
//        std::cerr << "Creating new succ for " << m_currentMatrixLineId << " (" << m_currentMatrixType << ")" << std::endl;
        using namespace boost;
        std::string successorsStr = (attributes.value("REF")).toUtf8().data();
        std::set< std::string > successors;

        tokenizer<> tok(successorsStr);
        copy(tok.begin(), tok.end(),
            std::insert_iterator< std::set< std::string > >(successors, successors.end()));

        SyntagmaticMatrixRow row;
        std::set< std::string >::iterator it = successors.begin();
        for (; it != successors.end(); it++)
        {
//            std::cerr << "Inserting " << (*it) << " / " << *(m_tokenFilters[m_currentMatrixType][(*it)]) << " in row " << std::endl;
            row.m_filters.insert(m_tokenFilters[m_currentMatrixType][(*it)]);
        }
//        std::cerr << "New row is " << row << std::endl;
        TokenFilter filter = m_tokenFilters[m_currentMatrixType][m_currentMatrixLineId];
//        std::cerr << "Filter is " << filter << std::endl;
        if (m_currentMatrixType == NOMINAL)
          m_matrices.nominal().filters().insert(std::make_pair(filter, row));
        else if (m_currentMatrixType == VERBAL)
          m_matrices.verbal().filters().insert(std::make_pair(filter, row));
    }
    else if (stringName == ("debuts"))
//      <!ELEMENT debuts EMPTY>
//      <!ATTLIST debuts REF IDREFS #IMPLIED>
    {
//        std::cerr << "Creating new debuts" << std::endl;
        using namespace boost;
        std::string beginningsStr = (attributes.value("REF")).toUtf8().data();
        std::set< std::string > beginnings;

        tokenizer<> tok(beginningsStr);
        copy(tok.begin(), tok.end(),
            std::insert_iterator< std::set< std::string > >(beginnings, beginnings.end()));

        SyntagmaticMatrixRow row;
        std::set< std::string >::iterator it = beginnings.begin();
        for (; it != beginnings.end(); it++)
        {
          row.m_filters.insert(m_tokenFilters[m_currentMatrixType][(*it)]);
        }
        if (m_currentMatrixType == NOMINAL)
        {
            m_matrices.nominalBegin() = row;
//            std::cerr << "New debuts is " << m_matrices.nominalBegin() << std::endl;
        }
        else if (m_currentMatrixType == VERBAL)
        {
            m_matrices.verbalBegin() = row;
//            std::cerr << "New debuts is " << m_matrices.verbalBegin() << std::endl;
        }
    }
    else if (stringName == ("fins"))
//      <!ELEMENT fins EMPTY>
//      <!ATTLIST fins REF IDREFS #IMPLIED>
    {
//        std::cerr << "Creating new fins" << std::endl;
        using namespace boost;
        std::string endingsStr = (attributes.value("REF")).toUtf8().data();
        std::set< std::string > endings;

        tokenizer<> tok(endingsStr);
        copy(tok.begin(), tok.end(),
            std::insert_iterator< std::set< std::string > >(endings, endings.end()));

        SyntagmaticMatrixRow row;
        std::set< std::string >::iterator it = endings.begin();
        for (; it != endings.end(); it++)
        {
          row.m_filters.insert(m_tokenFilters[m_currentMatrixType][(*it)]);
        }
//        std::cerr << "New fins is " << row << std::endl;
        if (m_currentMatrixType == NOMINAL)
            m_matrices.nominalEnd() = row;
        else if (m_currentMatrixType == VERBAL)
            m_matrices.verbalEnd() = row;
    }
    else if (stringName == ("chaine"))
//      <!ELEMENT chaine  (filtre+) >
//      <!ATTLIST chaine type (nominale | verbale) #REQUIRED>
    {
      std::string currentChainType = (attributes.value("type")).toUtf8().data();
        if (currentChainType == "nominale") m_currentChainType = NOMINAL;
        else if (currentChainType == "verbale") m_currentChainType = VERBAL;
        else m_currentChainType = NO_CHAIN_TYPE;
//        std::cerr << m_currentChainType << std::endl;
    }
    else if (stringName == ("filtre"))
//      <!ELEMENT filtre ( nom, attribut*, categorie+)>
//      <!ATTLIST filtre ID ID #REQUIRED>
    {
      m_currentFilterId = (attributes.value("ID")).toUtf8().data();
        m_currentFilterAttributes =std::set< StringsPoolIndex >();
        m_currentFilterCategories = std::set< LinguisticCode >();
//        std::cerr << m_currentFilterId << std::endl;
    }
    else if ( (stringName == ("nom")) || (stringName == ("attribut"))
            || (stringName == ("categorie")) )
    {
//      <!ELEMENT nom (#PCDATA ) >
        m_currentElement = stringName.toUtf8().data();
//        std::cerr << "Current element is: " << m_currentElement << std::endl;
    }
    return true;
}

bool XMLSyntagmaticMatrixFileHandler::endElement(const QString & namespaceURI, const QString & name, const QString & qName)
{
  LIMA_UNUSED(namespaceURI);
  LIMA_UNUSED(qName);
  if (name == 0)
    {
        SALOGINIT;
        LERROR << "Error: receiving a null closing element name while parsing matrix file";
        return false;
    }
    const QString& stringName = name;
//    std::cerr << "Finishing element: " << stringName << std::endl;
    // set the current module name and create its entry if necessary
    if ( stringName == ("matrice") )
    {
      m_currentMatrixType = NO_CHAIN_TYPE;
    }
    else if ( stringName == ("chaine") )
    {
      m_currentChainType = NO_CHAIN_TYPE;
    }
    else if (stringName == ("filtre"))
//  <!ELEMENT filtre ( nom, attribut*, categorie+)>
//  <!ATTLIST filtre ID ID #REQUIRED>
    {
      TokenFilter filter(m_currentFilterId, m_currentFilterName, m_currentFilterAttributes, m_currentFilterCategories);
//      std::cerr << "New Filter is: " << *filter << std::endl;
      m_tokenFilters[m_currentChainType][m_currentFilterId] = filter;
      m_currentFilterId = "";
      m_currentFilterName = "";
      m_currentFilterAttributes = std::set< StringsPoolIndex >();
      m_currentFilterCategories = std::set< LinguisticCode >();
    }
    else if ( (stringName == ("nom")) || (stringName == ("attribut"))
            || (stringName == ("categorie")) )
    {
//      <!ELEMENT nom (#PCDATA ) >
        m_currentElement = "";
//        std::cerr << "Current element is: " << m_currentElement << std::endl;
    }
    else if ( stringName == ("chaine") )
    {
      m_currentMatrixLineId = "";
    }
    return true;

}


} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
