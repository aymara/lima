// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        XmlSyntagmaticMatrixFileHandler.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr)
  * @date        Created on  : Mon Aug 04 2003
  *              Copyright   : (c) 2003 by CEA
  * @version     $Id$
  *
  */


#ifndef XMLSYNTAGMATICMATRIXFILEHANDLER_H
#define XMLSYNTAGMATICMATRIXFILEHANDLER_H

#include "SyntacticAnalysisExport.h"
#include "SyntagmaticMatrix.h"
#include "TokenFilter.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "common/misc/fsaStringsPool.h"

#include <QtXml/QXmlDefaultHandler>

#include <string>


namespace Lima {
namespace LinguisticProcessing {
namespace SyntacticAnalysis {

/**This is a SAX document handler the Detect configuration files
  *@author Gael de Chalendar
  */

class LIMA_SYNTACTICANALYSIS_EXPORT XMLSyntagmaticMatrixFileHandler : public QXmlDefaultHandler
{
public:
    // -----------------------------------------------------------------------
    //  Constructors
    // -----------------------------------------------------------------------
    XMLSyntagmaticMatrixFileHandler(SyntagmDefStruct& theMatrices,MediaId language);
    virtual ~XMLSyntagmaticMatrixFileHandler();


    // -----------------------------------------------------------------------
    //  Implementations of the SAX DocumentHandler interface
    // -----------------------------------------------------------------------
    bool endElement(const QString & namespaceURI, 
                    const QString & name, 
                    const QString & qName) override;

    bool characters(const QString& chars) override;

    bool startElement(const QString & namespaceURI, 
                      const QString & name, 
                      const QString & qName, 
                      const QXmlAttributes & attributes) override;

    // -----------------------------------------------------------------------
    //  Implementations of the SAX ErrorHandler interface
    // -----------------------------------------------------------------------
    bool warning(const QXmlParseException & exception) override;
    bool error(const QXmlParseException & exception) override;
    bool fatalError(const QXmlParseException & exception) override;

private:
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

} // closing namespace SyntacticAnalysis
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif
