// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
{
  BOWLOGINIT;
  LDEBUG << "BowTextWriter::startAnalysis";
  if (m_d->m_out == 0)
  {
    BOWLOGINIT;
    LERROR << "BowTextWriter Error: output stream is not set !";
    return;
  }
  Common::BagOfWords::BoWBinaryWriter writer;
  writer.writeHeader(*m_d->m_out,Common::BagOfWords::BOWFILE_TEXT);

}

void BowTextWriter::startDocument(const Common::Misc::GenericDocumentProperties&)
{}
    
void BowTextWriter::startNode( const std::string& , bool  )
{}

void BowTextWriter::endNode( const Common::Misc::GenericDocumentProperties&  )
{}

}

}
