// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "DictionaryData.h"

#include "common/LimaCommon.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"

#include <QtGlobal>
#include <QtCore/QFileInfo>

#include <iostream>
#include <fstream>


using namespace std;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

DictionaryData::DictionaryData() :
    m_data(0),
    m_entriesAddr(),
    m_lingPropertiesAddr()
{}


DictionaryData::~DictionaryData()
{
  if (m_data)
  {
    delete [] m_data;
  }
}

void DictionaryData::loadBinaryFile(const std::string& file)
{
  ANALYSISDICTLOGINIT;
  LDEBUG << "DictionaryData::loadBinaryFile" << file;
  if( !QFileInfo::exists(file.c_str()))
//  if( !boost::filesystem3::exists(file))
  {
    std::string mess = "DictionaryData::loadBinaryFile file ";
    mess.append(file).append(" not found!");
    throw( std::logic_error( mess ) );
  }
  uint64_t dataSize = QFileInfo(file.c_str()).size();
  LDEBUG << "DictionaryData::loadBinaryFile data size: " << dataSize;
  m_data = new unsigned char [dataSize];
  if (m_data == NULL)
  {
    std::string mess = "DictionaryData::loadBinaryFile memory allocation error";
    throw( std::logic_error( mess ) );
  }

  // load data
  FILE *dataFile = fopen(file.c_str(), "rb");
  if (dataFile == NULL)
  {
    LIMA_EXCEPTION( "DictionaryData::loadBinaryFile error cannot open data file"
                    << file.c_str() );
  }
  uint64_t readSize = fread(m_data, 1, dataSize, dataFile);        //_dataSize = max
  fclose(dataFile);
  if (readSize != dataSize)
  {
    std::string mess = "DictionaryData::loadBinaryFile totalDataReadSize != _dataSize ";
    throw( std::logic_error( mess ) );
  }

  // parseEntries
  unsigned char* p=m_data;
  uint64_t nbEntries=readCodedInt(p);
  m_entriesAddr.resize(nbEntries);
  uint64_t read;
  for (vector<unsigned char*>::iterator entryItr=m_entriesAddr.begin();
       entryItr!=m_entriesAddr.end();
       entryItr++)
  {
    *entryItr = p;
    // go to next entry
    read=readCodedInt(p);
    if (read == 1)
    {
      // 1 means delete, next in is length
      read=readCodedInt(p);
    }
    p += read;
  }
  LDEBUG << "read " << nbEntries << " entries";

  // parseLingProperties
  uint64_t nbLingProp=readCodedInt(p);
  m_lingPropertiesAddr.resize(nbLingProp);
  for(vector<unsigned char*>::iterator lingItr=m_lingPropertiesAddr.begin();
      lingItr!=m_lingPropertiesAddr.end();
      lingItr++)
  {
    *lingItr=p;
    read = readCodedInt(p);
    p += read;
  }
  LDEBUG << "read " << nbLingProp << " lingPropsSet";
  Q_ASSERT((uint64_t)(p-m_data) == dataSize);
}

uint64_t DictionaryData::readCodedInt(unsigned char* &p)
{
  uint64_t val = 0;
//  cerr << "start read" << endl;
  do
  {
//    cerr << "val = " << val << " *p = " << (int) *p << endl;
    val = (val <<7) + ((*p >> 1) & 0x7F);
  }
  while (*(p++) & 0x1);
//  cerr << "end read val=" << val << endl;
  return(val);
}

}

}

}
