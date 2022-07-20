// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGLTRTEXTHANDLER_H
#define LIMA_LINGUISTICPROCESSINGLTRTEXTHANDLER_H

#include "AnalysisHandlersExport.h"

#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
#include "linguisticProcessing/common/linearTextRepresentation/ltrText.h"
#include "common/Data/DataTypes.h"

namespace Lima {

namespace LinguisticProcessing {

/**
 * @brief LTRTextHandler is a handler for LTR text that gives access to the resulting LTRText through an accessor
*/
class LIMA_ANALYSISHANDLERS_EXPORT LTRTextHandler : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
    LTRTextHandler();

    virtual ~LTRTextHandler();

    virtual void endAnalysis() override;
    virtual void handle(const char* buf, int length) override ;
    virtual void startAnalysis() override;

    void startDocument(const Common::Misc::GenericDocumentProperties&) override;
    void endDocument() override;
    void startNode( const std::string& elementName, bool forIndexing ) override;
    void endNode(const Common::Misc::GenericDocumentProperties& props) override;
    
    Common::BagOfWords::LTR_Text& getLTRText();

    virtual void setOut( std::ostream* /*out*/ ) override {}
    
private:
  std::stringstream m_ltrstream;
  Common::BagOfWords::LTR_Text m_ltrtext;
};

}

}

#endif
