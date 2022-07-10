// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSINGBOWTEXTWRITER_H
#define LIMA_LINGUISTICPROCESSINGBOWTEXTWRITER_H

#include "AnalysisHandlersExport.h"
#include "AbstractTextualAnalysisHandler.h"

#include <fstream>

namespace Lima {
namespace Common {
  namespace Misc {
    class GenericDocumentProperties;
  }
}
namespace LinguisticProcessing {

#define BOWTEXTWRITER_CLASSID "BowTextWriter"
  
class BowTextWriterPrivate;
/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISHANDLERS_EXPORT BowTextWriter : public AbstractTextualAnalysisHandler
{
  Q_OBJECT
public:
    BowTextWriter();

    virtual ~BowTextWriter();

    virtual void endAnalysis() override;
    virtual void endDocument() override;
    virtual void handle(const char* buf, int length) override;
    virtual void startAnalysis() override;
    virtual void startDocument(const Common::Misc::GenericDocumentProperties& props) override;
    virtual void startNode( const std::string& elementName, bool forIndexing ) override;
    virtual void endNode( const Common::Misc::GenericDocumentProperties& props ) override;
  
    void setOut(std::ostream* out) override;
private:
  BowTextWriter(const BowTextWriter&);
  BowTextWriter& operator=(const BowTextWriter&);
  BowTextWriterPrivate* m_d;
};

}

}

#endif
