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
/**
  * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
  * @date       Created on  Mon Aug 04 2003
  */


// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include "XmlSyntagmaticMatrixFileReader.h"
#include "XmlSyntagmaticMatrixFileExceptions.h"
#include "common/MediaticData/mediaticData.h"

#include "common/Data/strwstrtools.h"

#include <QXmlStreamReader>

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

class XmlSyntagmaticMatrixFileReaderPrivate
{
    friend class XmlSyntagmaticMatrixFileReader;
public:
  // -----------------------------------------------------------------------
  //  Constructors
  // -----------------------------------------------------------------------
  XmlSyntagmaticMatrixFileReaderPrivate(SyntagmDefStruct& theMatrices, MediaId language);
  ~XmlSyntagmaticMatrixFileReaderPrivate() = default;

  bool parse(QIODevice *device);
  void readPrecedenceMatrix();
  void readChain();
  void readFilter();
  void readName();
  void readAttribute();
  void readCategory();
  void readMatrix();
  void readLine();
  void readSucc();
  void readBeginnings();
  void readEndings();

  QString errorString() const;

  QXmlStreamReader m_reader;
    /**
    * The structure where data will be stored
    */
    SyntagmDefStruct& m_matrices;

    /** This map stores the associations between the filters and their IDs */
    std::map< Common::MediaticData::ChainsType, std::map< std::string, TokenFilter > > m_tokenFilters;

    std::string m_currentElement;
    std::string m_currentFilterId;
    std::string m_currentFilterName;
    std::set< StringsPoolIndex > m_currentFilterAttributes;
    std::set< LinguisticCode > m_currentFilterCategories;
    Common::MediaticData::ChainsType m_currentChainType;
    Common::MediaticData::ChainsType m_currentMatrixType;
    std::string m_currentMatrixLineId;
    MediaId m_language;
};

// ---------------------------------------------------------------------------
//  XMLSyntagmaticMatrixFileHandler: Constructors and Destructor
// ---------------------------------------------------------------------------
XmlSyntagmaticMatrixFileReaderPrivate::XmlSyntagmaticMatrixFileReaderPrivate(SyntagmDefStruct& matrices,
                                                                             MediaId language) :
        m_matrices(matrices),
        m_language(language)
{
}

XmlSyntagmaticMatrixFileReader::XmlSyntagmaticMatrixFileReader(SyntagmDefStruct& matrices,MediaId language) :
        m_d(new XmlSyntagmaticMatrixFileReaderPrivate(matrices, language))
{
}

bool XmlSyntagmaticMatrixFileReader::parse(QIODevice *device)
{
  return m_d->parse(device);
}

// <?xml version="1.0" encoding="UTF-8" ?>
// <!DOCTYPE matrice_precedence SYSTEM "recochaines.dtd">
// <matrice_precedence>
//   …
// </matrice_precedence>
bool XmlSyntagmaticMatrixFileReaderPrivate::parse(QIODevice *device)
{
  XMLCFGLOGINIT;
  LTRACE << "parse";
  m_reader.setDevice(device);
  if (m_reader.readNextStartElement())
  {
    if (m_reader.name() == QLatin1String("matrice_precedence"))
    {
      readPrecedenceMatrix();
    }
    else
    {
      m_reader.raiseError(QObject::tr("The file is not a LIMA XML precedence matrix file."));
    }
  }
  return !m_reader.error();
}

// <matrice_precedence>
//   <chaine type="nominale">
//     …
//   </chaine>
//   <matrice type="nominale">
//     …
//   </matrice>
//   …
// </matrice_precedence>
void XmlSyntagmaticMatrixFileReaderPrivate::readPrecedenceMatrix()
{
    SALOGINIT;
    LTRACE << "readPrecedenceMatrix";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("matrice_precedence"));

    while (m_reader.readNextStartElement())
    {
      if (m_reader.name() == QLatin1String("chaine"))
        readChain();
      if (m_reader.name() == QLatin1String("matrice"))
        readMatrix();
      else
        m_reader.raiseError(QObject::tr("Expected a chaine or matrice but got a %1.").arg(m_reader.name()));
    }
}

//   <chaine type="nominale">
//     <filtre ID="FN1">
//       …
//     </filtre>
//     …
//   </chaine>
void XmlSyntagmaticMatrixFileReaderPrivate::readChain()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("chain"));

    std::string currentChainType = (m_reader.attributes().value("type")).toUtf8().data();
    if (currentChainType == "nominale") m_currentChainType = NOMINAL;
    else if (currentChainType == "verbale") m_currentChainType = VERBAL;
    else m_currentChainType = NO_CHAIN_TYPE;
//        std::cerr << m_currentChainType << std::endl;
    while (m_reader.readNextStartElement())
    {
      if (m_reader.name() == QLatin1String("filtre"))
        readChain();
      else
        m_reader.raiseError(QObject::tr("Expected a filtre but got a %1.").arg(m_reader.name()));
    }
    m_currentMatrixLineId = "";
    m_currentChainType = NO_CHAIN_TYPE;
}

//     <filtre ID="FN1">
//       <nom>DET-ART-CON</nom>
//       <categorie>DETDES</categorie>
//       …
//       <attribut>…</categorie>
//       …
//     </filtre>
void XmlSyntagmaticMatrixFileReaderPrivate::readFilter()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("filtre"));

    m_currentFilterId = (m_reader.attributes().value("ID")).toUtf8().data();
    m_currentFilterAttributes = std::set< StringsPoolIndex >();
    m_currentFilterCategories = std::set< LinguisticCode >();
//        std::cerr << m_currentFilterId << std::endl;
    while (m_reader.readNextStartElement())
    {
      if (m_reader.name() == QLatin1String("nom"))
        readName();
      else if (m_reader.name() == QLatin1String("categorie"))
        readCategory();
      else if (m_reader.name() == QLatin1String("attribut"))
        readAttribute();
      else
        m_reader.raiseError(QObject::tr("Expected a nom, a categorie or a attribut but got a %1.").arg(m_reader.name()));
    }
    TokenFilter filter(m_currentFilterId, m_currentFilterName, m_currentFilterAttributes, m_currentFilterCategories);
//      std::cerr << "New Filter is: " << *filter << std::endl;
    m_tokenFilters[m_currentChainType][m_currentFilterId] = filter;
    m_currentFilterId = "";
    m_currentFilterName = "";
    m_currentFilterAttributes = std::set< StringsPoolIndex >();
    m_currentFilterCategories = std::set< LinguisticCode >();
}

//       <nom>DET-ART-CON</nom>
void XmlSyntagmaticMatrixFileReaderPrivate::readName()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("nom"));
//      <!ELEMENT nom (#PCDATA ) >
    m_currentFilterName = m_reader.text().toString().toStdString();
//        std::cerr << "Current filter name is: " << m_currentFilterName << std::endl;
}

//       <attribut>…</categorie>
void XmlSyntagmaticMatrixFileReaderPrivate::readAttribute()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("attribut"));

//      <!ELEMENT attribut (#PCDATA ) >

    m_currentFilterAttributes.insert(Common::MediaticData::MediaticData::changeable().stringsPool(m_language)[
        m_reader.text().toString()]);
}

//       <categorie>DETDES</categorie>
void XmlSyntagmaticMatrixFileReaderPrivate::readCategory()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("categorie"));

//      <!ELEMENT categorie (#PCDATA ) >

    LinguisticCode filterCategory;
    if (m_reader.text()[0].isDigit())
    {
        filterCategory = LinguisticCode::fromString(m_reader.text().toString().toStdString());
    }
    else
    {
        filterCategory = static_cast<const Common::MediaticData::LanguageData&>(
            MediaticData::single().mediaData(m_matrices.language())).getPropertyCodeManager()
                .getPropertyManager("MICRO").getPropertyValue(m_reader.text()
                .toString().toStdString());
    }
    m_currentFilterCategories.insert(filterCategory);
//        std::cerr << "Current category is: " << filterCategory << std::endl;
}

//   <matrice type="nominale">
//     <ligne REF="FN1">
//       <succ REF="FN3 FN8 FN12 FN13 PREF"/>
//     </ligne>
//     …
//     <debuts REF="FV1 FV4 FV5 FV6 FV8 FV9 FV11 FV12 FV13 FV14 FV17 FV18 FV19 FV20 FV22 FV23 FV26 FV31 FV34"/>
//     …
//     <fins REF="FV1 FV2 FV4 FV5 FV6 FV8 FV9 FV10 FV15 FV19 FV21 FV24 FV28 FV29 FV30 FV33"/>
//     …
//   </matrice>
void XmlSyntagmaticMatrixFileReaderPrivate::readMatrix()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("matrice"));

    auto currentMatrixType = m_reader.attributes().value("type");
    if (currentMatrixType == "nominale") m_currentMatrixType = NOMINAL;
    else if (currentMatrixType == "verbale") m_currentMatrixType = VERBAL;
    else m_currentMatrixType = NO_CHAIN_TYPE;

    while (m_reader.readNextStartElement())
    {
      if (m_reader.name() == QLatin1String("ligne"))
        readLine();
      else if (m_reader.name() == QLatin1String("debuts"))
        readBeginnings();
      else if (m_reader.name() == QLatin1String("fins"))
        readEndings();
      else
        m_reader.raiseError(QObject::tr("Expected a ligne, a debuts or a fins but got a %1.").arg(m_reader.name()));
    }
    m_currentMatrixType = NO_CHAIN_TYPE;
}

//     <ligne REF="FN1">
//       <succ REF="FN3 FN8 FN12 FN13 PREF"/>
//     </ligne>
void XmlSyntagmaticMatrixFileReaderPrivate::readLine()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("ligne"));

    m_currentMatrixLineId = m_reader.attributes().value("REF").toUtf8().data();
    while (m_reader.readNextStartElement())
    {
      if (m_reader.name() == QLatin1String("succ"))
        readChain();
      else
        m_reader.raiseError(QObject::tr("Expected a succ but got a %1.").arg(m_reader.name()));
    }
}

//       <succ REF="FN3 FN8 FN12 FN13 PREF"/>
void XmlSyntagmaticMatrixFileReaderPrivate::readSucc()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("succ"));
//        std::cerr << "Creating new succ for " << m_currentMatrixLineId << " (" << m_currentMatrixType << ")" << std::endl;
    using namespace boost;
    std::string successorsStr = (m_reader.attributes().value("REF")).toUtf8().data();
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

//     <debuts REF="FV1 FV4 FV5 FV6 FV8 FV9 FV11 FV12 FV13 FV14 FV17 FV18 FV19 FV20 FV22 FV23 FV26 FV31 FV34"/>
void XmlSyntagmaticMatrixFileReaderPrivate::readBeginnings()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("debuts"));
    using namespace boost;
    std::string beginningsStr = (m_reader.attributes().value("REF")).toUtf8().data();
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

//     <fins REF="FV1 FV2 FV4 FV5 FV6 FV8 FV9 FV10 FV15 FV19 FV21 FV24 FV28 FV29 FV30 FV33"/>
void XmlSyntagmaticMatrixFileReaderPrivate::readEndings()
{
    SALOGINIT;
    LTRACE << "readChain";
    Q_ASSERT(m_reader.isStartElement() && m_reader.name() == QLatin1String("fins"));
//        std::cerr << "Creating new fins" << std::endl;
    using namespace boost;
    std::string endingsStr = (m_reader.attributes().value("REF")).toUtf8().data();
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

QString XmlSyntagmaticMatrixFileReader::errorString() const
{
  XMLCFGLOGINIT;
  auto errorStr = QObject::tr("%1, Line %2, column %3")
          .arg(m_d->m_reader.errorString())
          .arg(m_d->m_reader.lineNumber())
          .arg(m_d->m_reader.columnNumber());
  LERROR << errorStr;
  return errorStr;
}

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima
