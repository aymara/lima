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
public:
    LTRTextHandler();

    virtual ~LTRTextHandler();

    virtual void endAnalysis();
    virtual void handle(const char* buf, int length) ;
    virtual void startAnalysis();

    void startDocument(const Common::Misc::GenericDocumentProperties&);
    void endDocument();
    void startNode( const std::string& elementName, bool forIndexing );
    void endNode(const Common::Misc::GenericDocumentProperties& props);
    
    Common::BagOfWords::LTR_Text& getLTRText();

    virtual void setOut( std::ostream* /*out*/ ) {}
    
private:
  std::stringstream m_ltrstream;
  Common::BagOfWords::LTR_Text m_ltrtext;
};

}

}

#endif
