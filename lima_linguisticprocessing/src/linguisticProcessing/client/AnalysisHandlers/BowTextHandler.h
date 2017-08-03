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
#ifndef LIMA_LINGUISTICPROCESSINGBOWTEXTHANDLER_H
#define LIMA_LINGUISTICPROCESSINGBOWTEXTHANDLER_H

#include "AnalysisHandlersExport.h"
#include "BowTextWriter.h"

#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "common/Data/DataTypes.h"
#include "common/Handler/structureHandler.h"
#include "common/Handler/contentHandler.h"

namespace Lima {

namespace LinguisticProcessing {

/**
 * @brief BowTextHandler is a handler for BoW documents that gives access to the resulting BoWText through an accessor
 * @author Benoit Mathieu
*/
class LIMA_ANALYSISHANDLERS_EXPORT BowTextHandler : public AbstractTextualAnalysisHandler,public StructureHandler, public ContentHandler< Common::BagOfWords::BoWText >
{
  Q_OBJECT
public:
    BowTextHandler();

    virtual ~BowTextHandler();

    virtual void endAnalysis();
    virtual void endDocument();
    virtual void handle(const char* buf, int length) ;
    virtual void startAnalysis();
    virtual void startDocument(const Common::Misc::GenericDocumentProperties& props);
    virtual void startNode( const std::string& elementName, bool forIndexing );
    virtual void endNode( const Common::Misc::GenericDocumentProperties& props );
    
    Common::BagOfWords::BoWText& getBowText();

    void set_filePath(std::string path){m_filePath=path;};
    virtual void setOut( std::ostream* out ) {LIMA_UNUSED(out);}
    
private:

  std::ostringstream* m_bowstream;
  BowTextWriter* m_writer;
  Common::BagOfWords::BoWText m_bowtext;
  std::string m_filePath;
};

}

}

#endif
