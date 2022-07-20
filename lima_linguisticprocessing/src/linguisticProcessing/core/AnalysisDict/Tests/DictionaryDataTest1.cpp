// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip


// To be defined as a variable set at build time
#define UNIT_TEST

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE DictionaryDataTest
#include <boost/test/unit_test.hpp>

#include "linguisticProcessing/core/AnalysisDict/DictionaryData.h"

using namespace Lima;

// create DictionaryData and check segment adress
BOOST_AUTO_TEST_CASE( DictionaryDataTest1 )
{
  
  //value of LIMA_RESOURCE_DIR is set by find_package(LIMADATA) and LIMALDATAConfig.cmake;
  std::string resourcesPath("/home/gael/Projets/Lima/lima/../Dist/master/debug/share/apps/lima/resources");
  // create and read DictionaryData
  LinguisticProcessing::AnalysisDict::DictionaryData dicoData;
  std::string dicoFile(resourcesPath);
  dicoFile.append("/LinguisticProcessings/fre/dicoDat-fre.dat");
  dicoData.loadBinaryFile(dicoFile);

  // uint64_t getSize() const;
  uint64_t size = dicoData.getSize();
  BOOST_REQUIRE(size == 660156);

  // test unsigned char* getEntryAddr(uint64_t index) const;
  uint64_t index(0);
  unsigned char*entryAddr = dicoData.getEntryAddr(index);
  std::cout << "dicoData.getEntryAddr[0]=" << hex << static_cast<void*>(entryAddr) << std::endl;
  std::cout << "dicoData..m_data=" << hex << static_cast<void*>(dicoData.m_data) << std::endl;
//  BOOST_REQUIRE();
  // Que tester???
  // Il faudrait un petit dictionnaire bien contrôlé pour effectuer un jeu de test efficace
  
  
  // test inline unsigned char* getLingPropertiesAddr(uint64_t index) const;
  // unsigned char*
  // getLingPropertiesAddr(index);

  // test public uint64_t getSize() const;
  
  // test public static uint64_t readCodedInt(unsigned char* &p);
  
}
