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
                          testContentDict16.cpp  -  description
                             -------------------
    begin                : lun jun 2 2003
    copyright            : (C) 2003 by Olivier Mesnard
    email                : olivier.mesnard@cea.fr
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  compact dictionnary based on finite state automata                     *
 *  implemented with Boost Graph library                                   *
 *                                                                         *
 ***************************************************************************/


#include <string>
using namespace std;

#include "common/LimaCommon.h"

#include "common/time/traceUtils.h"

// string and file handling Utilities
#include "common/Data/strwstrtools.h"

// accessMethod
#include "common/FsaAccess/FsaAccessSpare16.h"
// generic dictionaries
#include "common/StringMap/SimpleDataDico.h"
#include "common/StringMap/IndirectDataDico.h"

// generic dictionaries
#include "linguisticProcessing/core/AnalysisDict/AnalysisDico.h"

// #include "common/linguisticData/linguisticData.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::StringMap;
using namespace Lima::Common::LinguisticData;

// options
typedef struct ParamStruct {
  std::string key;
  int offset;
  std::string keyFileName;
  std::string dataFileName;
  std::string accessMethod;
  bool withAssert;
  std::string contentType;
  std::string codeFileName;
  std::string language;
} Param;

void testSimpleDataDico(const Param& param );
void testAnalysisDico(const Param& param );

int main(int argc, char *argv[])
{
  cerr << "testContentDict16 begin..." << endl;

  setlocale(LC_ALL, "");

#ifdef DEBUG_CD
  FSAALOGINIT;
  LDEBUG <<  "testContentDict16 begin..." << LENDL;
#endif

  // options reading
  Param param = {
    std::string(),  // key
    0,              // offset
    std::string(),  // keyFileName
    std::string(),  // dataFileName
    std::string(),  // accessMethod
    false,          // withAssert
    std::string(),  // contentType
    std::string(),  // codeFileName
    std::string()   // language
  };

  for (int i = 1 ; i < argc; i++) {
    std::string arg(argv[i]);
    int pos = -1;
    if (arg == "--help")
    {
      std::cerr << "usage: " << argv[0]
                << " --help" << std::endl;
      std::cerr << "       " << argv[0]
                << " [--key=<word>]"
                << " [--offset=<int>]"
                << " [--keyFileName=<filename>]"
                << " [--dataFileName=<filename>]"
                << " [--accessMethod=Fsa|Tree]"
                << " [--contentType=simple|lingProp]"
                << " [--codeFileName=<filename>]"
                << " [--language=fre|eng]"
                << " [--withAssert]"
                << std::endl;
      return 0;
    }
    else if ( (pos = arg.find("--keyFileName=")) != -1 ){
      param.keyFileName = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--dataFileName=")) != -1 ){
      param.dataFileName = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--key=")) != -1 ){
      param.key = arg.substr(pos+6);
    }
    else if ( (pos = arg.find("--offset=")) != -1 ){
      param.offset = atoi( (arg.substr(pos+9)).c_str() );
    }
    else if ( arg.compare("--withAssert") == 0 ){
      param.withAssert = true;
    }
    else if ( (pos = arg.find("--accessMethod=")) != -1 ){
      param.accessMethod = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--contentType=")) != -1 ){
      param.contentType = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--codeFileName=")) != -1 ){
      param.codeFileName = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--language=")) != -1 ){
      param.language = arg.substr(pos+11);
    }
  }

  cerr << "testContentDict16: ";
  if(param.keyFileName.size()) {
    cerr << "--keyFileName='" << param.keyFileName << "' ";
  }
  if(param.dataFileName.size()) {
    cerr << "--dataFileName='" << param.dataFileName << "' ";
  }
  if(param.withAssert) {
    cerr << "--withAssert ";
  }
  if(param.accessMethod.size()) {
    cerr << "--accessMethod='" << param.accessMethod << "' ";
  }
  if(param.contentType.size()) {
    cerr << "--contentType='" << param.contentType << "' ";
  }
  cerr << "--key="<< param.key << " ";
  cerr << "--offset="<< param.offset << " ";
  if(param.codeFileName.size()) {
    cerr << "--codeFileName='" << param.codeFileName << "' ";
  }
  if(param.language.size()) {
    cerr << "--language='" << param.language << "' ";
  }
  cerr << endl;

  if( !param.contentType.compare(std::string("freq")) ) {
    cerr << "testSimpleDataDico..." <<  endl;
    // Test dictionnaire simple (type dictionnaire de fr�uence)
    testSimpleDataDico(param);
  }
  if( !param.contentType.compare(std::string("lingProp")) ) {
    cerr << "testAnalysisDataDico..." <<  endl;
    // Test dictionnaire d'analyse
    testAnalysisDico( param );
  }

  return EXIT_SUCCESS;
}

void testSimpleDataDico(const Param& param ) {
  typedef int simpleDataElement;
  typedef std::vector<simpleDataElement> simpleStoredSet;
  typedef Lima::Common::FsaAccess::FsaAccessSpare16 accessMethod;
  typedef Lima::Common::StringMap::SimpleDataDico<accessMethod, simpleDataElement,simpleStoredSet> SimpleDicoType;

  SimpleDicoType* dico = new SimpleDicoType(-1);
  dico->parseAccessMethod(param.keyFileName);
  dico->parseData(param.dataFileName);
  dico->getSize();
  Lima::LimaString limaKey(param.key);
  simpleDataElement val = dico->getElement(limaKey);
  std::cout << "simpleDico->getElement(" << param.key << ") =" << val << std::endl;
}

typedef Lima::LinguisticProcessing::Dictionary::DictionaryEntry analysisDataElement;
typedef FsaAccessSpare16 accessMethod;
typedef Lima::LinguisticProcessing::AnalysisDict::AnalysisDico<accessMethod> MyAnalysisBaseDico;

int displayEntry( MyAnalysisBaseDico &dictionary, analysisDataElement& entry,
                  LimaString& term  );


void testAnalysisDico(const Param& param ) {
  string resourcesPath="/usr/share/apps/lima/resources/";
  string commonConfigFile=string("lima-common.xml");
  string configDir="/usr/share/config/lima/";
  deque<string> langs;
  langs.push_back(param.language);
  // initialize common
  Lima::Common::LinguisticData::LinguisticData::changeable().init(
    resourcesPath,
    configDir,
    commonConfigFile,
    langs);
  Lima::LinguisticProcessing::Dictionary::DictionaryCode* dicocode =
    new Lima::LinguisticProcessing::Dictionary::DictionaryCode();
  dicocode->setLanguage(MediaticData::single().getMediaId(param.language));
  dicocode->parse(param.codeFileName);
  MyAnalysisBaseDico* dico = new MyAnalysisBaseDico();
  dico->setDicoCode(dicocode);
  dico->parseAccessMethod(param.keyFileName);
  dico->parseData(param.dataFileName);
  dico->getSize();
  Lima::LimaString limaKey(param.key);
  analysisDataElement entry = dico->getEntry(limaKey);
  std::cout << "analysysDico->getElement(" << param.key << ") =" << std::endl;
  displayEntry( *dico, entry, limaKey );

}

int displayEntry( MyAnalysisBaseDico &dictionary, analysisDataElement& entry,
                  LimaString& term  )
{
  LinguisticCode MACRO = Common::LinguisticData::LinguisticData::single().getCategory("MACRO");
  LinguisticCode MICRO = Common::LinguisticData::LinguisticData::single().getCategory("MICRO");
  LinguisticCode GENDER = Common::LinguisticData::LinguisticData::single().getCategory("GENDER");
  LinguisticCode NUMBER = Common::LinguisticData::LinguisticData::single().getCategory("NUMBER");
  LinguisticCode PERSON = Common::LinguisticData::LinguisticData::single().getCategory("PERSON");
  LinguisticCode TIME = Common::LinguisticData::LinguisticData::single().getCategory("TIME");
  LinguisticCode SYNTAX = Common::LinguisticData::LinguisticData::single().getCategory("SYNTAX");
  std::string term8= Common::Misc::limastring2utf8stdstring(term);

  std::cout << "<k>" << term8
  << "</k> (" << Lima::Common::Misc::convertString(term) << ");" << std::endl;

  if( entry.hasLingInfo() )
  {
    Lima::LinguisticProcessing::Dictionary::LingInfoEntry lingInfo = entry.nextLingInfo();
    while (!lingInfo.isEmpty())
    {
      std::cout << "<i>";

      Lima::LimaString lemma = lingInfo.lemma();
      std::string lemma8= Common::Misc::limastring2utf8stdstring(lemma);
      std::cout << "<l>" << lemma8 << "</l>";

      Lima::LimaString normalized = lingInfo.normalized();
      std::string normalized8= Common::Misc::limastring2utf8stdstring(normalized);
      std::cout << "<n>" << normalized8 << "</n>" << std::endl;

      //      std::cout << "  lemma = '" << Lima::Common::Misc::convertString(lemma) << "'"
      //                << " , normalized = '" << Lima::Common::Misc::convertString(normalized) << "'" << std::endl;

      Lima::LinguisticProcessing::Dictionary::LingPropertyEntry property = lingInfo.nextLingProperty();
      //      std::cout << "  lingProp data = " << std::endl;
      while (!property.isEmpty())
      {
        int data = property.data();
        std::cout << "<p>" << data << "</p> ";
        std::cout << "(" << "MAC = " << Lima::Common::Misc::wstring2string(property.stringCode(MACRO)) << ", ";
        std::cout << "MAC_MIC = " << Lima::Common::Misc::wstring2string(property.stringCode(MICRO)) << ", ";
        std::cout << "GEND = " << Lima::Common::Misc::wstring2string(property.stringCode(GENDER)) << ", ";
        std::cout << "NUMB = " << Lima::Common::Misc::wstring2string(property.stringCode(NUMBER)) << ", ";
        std::cout << "PERS = " << Lima::Common::Misc::wstring2string(property.stringCode(PERSON)) << ", ";
        std::cout << "TIME = " << Lima::Common::Misc::wstring2string(property.stringCode(TIME)) << ", ";
        std::cout << "SYNTAX = " << Lima::Common::Misc::wstring2string(property.stringCode(SYNTAX)) << ") " << std::endl;
        property = lingInfo.nextLingProperty();
      }
      std::cout << "</i>" << std::endl;
      lingInfo = entry.nextLingInfo();
    }
  }
  else
  {
    //   std::cout << "No LingInfo." << std::endl;
  }

  if( entry.hasAccented() )
  {
    //    std::cout << "desacc: ";
    Lima::LimaString accented = entry.nextAccented();
    while (accented.length() != 0)
    {
      std::string accented8= Common::Misc::limastring2utf8stdstring(accented);
      std::cout << "<a>" << accented8 << "</a>";
      accented = entry.nextAccented();
    }
    std::cout << std::endl;
  }
  else
  {
    //    std::cout << "No reaccenc." << std::endl;
  }

  if( entry.hasIdiomatic() )
  {
    std::cout << "Idiomatic: ";
    int idiomatic = entry.nextIdiomatic();
    while (idiomatic != 0)
    {
      std::cout << "<id_exp>" << idiomatic << "</id_exp>";
      idiomatic = entry.nextIdiomatic();
    }
  }
  else
  {
    //    std::cout << "No idiomatic. " << std::endl;
  }

  if( entry.hasHyphenLingInfo() )
  {
    Lima::LinguisticProcessing::Dictionary::LingInfoEntry hyphenLingInfo = entry.nextHyphenLingInfo();
    while (!hyphenLingInfo.isEmpty())
    {
      Lima::LimaString hyphen = hyphenLingInfo.lemma();
      std::string hyphen8= Common::Misc::limastring2utf8stdstring(hyphen);
      std::cout << "<h>" << hyphen8 << "</h>";
      hyphenLingInfo = entry.nextHyphenLingInfo();
    }
    std::cout << std::endl;
  }
  else
  {
    //    std::cout << "No hyphenLingInfo. " << std::endl;
  }

  if( entry.hasConcatenated() )
  {
    //    std::cout << "Concatenated: " << std::endl;
    Lima::LinguisticProcessing::Dictionary::ConcatenatedEntry concatenatedEntry = entry.nextConcatenated();

    int i = 1;
    while (!concatenatedEntry.isEmpty())
    {
      std::cout << "<c>";
      Lima::LinguisticProcessing::Dictionary::SingleConcatenatedEntry singleConcatenatedEntry = concatenatedEntry.nextSingleConcatenated();
      while (!singleConcatenatedEntry.isEmpty())
      {
        Lima::LimaString component = singleConcatenatedEntry.component();
        std::string component8= Common::Misc::limastring2utf8stdstring(component);
        std::cout << "'"<< component8 << "',";
        singleConcatenatedEntry = concatenatedEntry.nextSingleConcatenated();
      }
      std::cout << "</c>";
      concatenatedEntry = entry.nextConcatenated();
      i++;
    }
    std::cout << std::endl;
  }
  else
  {
    //    std::cout << "No concatenated. " << std::endl;
  }

  if( entry.hasDefaultProperty() )
  {
    std::cout << "entry has defaultProperty" << std::endl;
    Lima::LinguisticProcessing::Dictionary::LingPropertyEntry property = entry.nextLingProperty();
    while (!property.isEmpty())
    {
      int data = property.data();
      std::cout << "<p>" << data << "</p> ";
      std::cout << "(" << "MAC = " << Lima::Common::Misc::wstring2string(property.stringCode(MACRO)) << ", ";
      std::cout << "MAC_MIC = " << Lima::Common::Misc::wstring2string(property.stringCode(MICRO)) << ", ";
      std::cout << "GEND = " << Lima::Common::Misc::wstring2string(property.stringCode(GENDER)) << ", ";
      std::cout << "NUMB = " << Lima::Common::Misc::wstring2string(property.stringCode(NUMBER)) << ", ";
      std::cout << "PERS = " << Lima::Common::Misc::wstring2string(property.stringCode(PERSON)) << ", ";
      std::cout << "TIME = " << Lima::Common::Misc::wstring2string(property.stringCode(TIME)) << ", ";
      std::cout << "SYNTAX = " << Lima::Common::Misc::wstring2string(property.stringCode(SYNTAX)) << ") " << std::endl;
      /*
            uint64_t macroCategoryNumFullToken = property.code(MACRO);
            uint64_t microCategoryNumFullToken = property.code(MICRO);
      //      std::cout << "lingProp data = " << property.data() << ", " << std::endl;
            std::cout << "lingProp data = macro" << macroCategoryNumFullToken
                      << "lingProp data = micro" << microCategoryNumFullToken << ", " << std::endl;
      */
      property = entry.nextLingProperty();
    }
    std::cout << std::endl;
  }
  else
  {
    //    std::cout << "No default properties. " << std::endl;
  }
}
