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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "BowTextWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/genericDocumentProperties.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"

using namespace std;

namespace Lima {

namespace LinguisticProcessing {

SimpleFactory<AbstractResource,BowTextWriter> bowTextWriterFactory(BOWTEXTWRITER_CLASSID);

class BowTextWriterPrivate
{
  friend class BowTextWriter;
  
public:
    BowTextWriterPrivate();

    virtual ~BowTextWriterPrivate();

  std::ostream* m_out;
};

BowTextWriterPrivate::BowTextWriterPrivate() : m_out(0)
{
}

BowTextWriterPrivate::~BowTextWriterPrivate()
{
}


BowTextWriter::BowTextWriter()
 : AbstractTextualAnalysisHandler(),m_d(new BowTextWriterPrivate())
{
}

BowTextWriter::~BowTextWriter()
{
  delete m_d;
}

void BowTextWriter::setOut(std::ostream* out)
{
  m_d->m_out = out;
  
  Common::BagOfWords::BoWBinaryWriter writer;
  writer.writeHeader(*m_d->m_out,Common::BagOfWords::BOWFILE_TEXT);
}

void BowTextWriter::endAnalysis()
{
  if (m_d->m_out == 0)
  {
    BOWLOGINIT;
    LERROR << "BowTextWriter Error: output stream is not set !";
    return;
  }
    m_d->m_out->flush();
}

void BowTextWriter::endDocument()
{
}

void BowTextWriter::handle(const char* buf, int length) 
{
  if (m_d->m_out == 0)
  {
    BOWLOGINIT;
    LERROR << "BowTextWriter Error: output stream is not set !";
    return;
  }
  m_d->m_out->write(buf,length);
}

void BowTextWriter::startAnalysis()
{}

void BowTextWriter::startDocument(const Common::Misc::GenericDocumentProperties&)
{}
    
void BowTextWriter::startNode( const std::string& , bool  )
{}

void BowTextWriter::endNode( const Common::Misc::GenericDocumentProperties&  )
{}

}

}
