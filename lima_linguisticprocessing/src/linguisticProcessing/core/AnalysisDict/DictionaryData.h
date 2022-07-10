// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_LINGUISTICPROCESSING_ANALYSISDICTDICTIONARYDATA_H
#define LIMA_LINGUISTICPROCESSING_ANALYSISDICTDICTIONARYDATA_H

#include "AnalysisDictExport.h"
#include <string>
#include <vector>

#define UNIT_TEST

class DictionaryDataTest1;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDict
{

/**
@author Benoit Mathieu
*/
class LIMA_ANALYSISDICT_EXPORT DictionaryData
{
#ifdef UNIT_TEST
  friend class ::DictionaryDataTest1; // to test private operation in DictionaryData
#endif
public:
  DictionaryData();

  virtual ~DictionaryData();

  void loadBinaryFile(const std::string& file);

  inline unsigned char* getEntryAddr(uint64_t index) const;
  inline unsigned char* getLingPropertiesAddr(uint64_t index) const;

  uint64_t getSize() const;
  
  static uint64_t readCodedInt(unsigned char* &p);

private:

  unsigned char* m_data;
  std::vector<unsigned char*> m_entriesAddr;
  std::vector<unsigned char*> m_lingPropertiesAddr;

};

inline unsigned char* DictionaryData::getEntryAddr(uint64_t index) const
{
  return m_entriesAddr.at(index);
}

inline unsigned char* DictionaryData::getLingPropertiesAddr(uint64_t index) const
{
  return m_lingPropertiesAddr.at(index);
}

inline uint64_t DictionaryData::getSize() const
{
  return m_entriesAddr.size();
}


}

}

}

#endif
