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
  std::string resourcesPath("@LIMA_RESOURCE_DIR@");
  // create and read DictionaryData
  LinguisticProcessing::AnalysisDict::DictionaryData dicoData;
  std::string dicoFile(resourcesPath);
  dicoFile.append("/LinguisticProcessings/fre/dicoDat-fre.dat");
  dicoData.loadBinaryFile(dicoFile);

  // uint64_t getSize() const;
  uint64_t size = dicoData.getSize();
  BOOST_REQUIRE(size == 660157);

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
