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

    virtual void endAnalysis();
    virtual void endDocument();
    virtual void handle(const char* buf, int length);
    virtual void startAnalysis();
    virtual void startDocument(const Common::Misc::GenericDocumentProperties& props);
    virtual void startNode( const std::string& elementName, bool forIndexing );
    virtual void endNode( const Common::Misc::GenericDocumentProperties& props );
  
    void setOut(std::ostream* out);
private:
  BowTextWriter(const BowTextWriter&);
  BowTextWriter& operator=(const BowTextWriter&);
  BowTextWriterPrivate* m_d;
};

}

}

#endif
