// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

// clazy:skip

#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE EnhancedAnalysisDictionaryTest
#include <boost/test/unit_test.hpp>

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

using namespace Lima;

// conversion functions
BOOST_AUTO_TEST_CASE( EnhancedAnalysisDictionaryTest0 )
{
//   std::cout << "build accessMethod ..." << std::endl;
//   FsaAccessSpare16* fsaAccess=new FsaAccessSpare16();
//   fsaAccess->read(param.keyFileName);
//   
//   std::cout << "register mainkeys" << std::endl;
//   MediaId langid=MediaticData::single().getMediaId(param.language);
//   FsaStringsPool& sp= Common::MediaticData::MediaticData::changeable().stringsPool(langid);
//   sp.registerMainKeys(fsaAccess);
//   
//   std::cout << "build EnhancedAnalysisDictionary... " << std::endl;
//   dictionary=new EnhancedAnalysisDictionary(
//     &sp,
//     fsaAccess,
//     param.dataFileName);
//   
//   std::cout << "EnhancedAnalysisDictionary end" << std::endl;
// 
//   std::pair<DictionarySubWordIterator, DictionarySubWordIterator> proclicticSubWord_it = m_procliticDictionary->getSubWordEntries(0, hyperWordString);
//   BOOST_REQUIRE( Common::Misc::limastring2utf8stdstring(Common::Misc::utf8stdstring2limastring("abcé")) == "abcé" );
}
