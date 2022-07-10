// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

    virtual void endAnalysis() override;
    virtual void endDocument() override;
    virtual void handle(const char* buf, int length) override ;
    virtual void startAnalysis() override;
    virtual void startDocument(const Common::Misc::GenericDocumentProperties& props) override;
    virtual void startNode( const std::string& elementName, bool forIndexing ) override;
    virtual void endNode( const Common::Misc::GenericDocumentProperties& props ) override;
    
    Common::BagOfWords::BoWText& getBowText();

    void set_filePath(std::string path){m_filePath=path;};
    virtual void setOut( std::ostream* out ) override {LIMA_UNUSED(out);}
    
private:

  std::ostringstream* m_bowstream;
  BowTextWriter* m_writer;
  Common::BagOfWords::BoWText m_bowtext;
  std::string m_filePath;
};

}

}

#endif
