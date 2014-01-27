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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <fstream>
#include <iterator>
#include <vector>
#include <iostream>
#include <cstdlib>

#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/languageData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/LimaString.h"
#include "common/misc/fsaStringsPool.h"
#include "common/FsaAccess/FsaAccessSpare16.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractAnalysisDictionary.h"
#include "linguisticProcessing/core/AnalysisDict/EnhancedAnalysisDictionary.h"
#include "DictionaryEntryLogger.h"

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima;
using namespace Lima::Common::FsaAccess;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::AnalysisDict;


// options
typedef struct ParamStruct
{
  std::string language;
  std::string dicoId;
  std::string resdir;
  std::string access;
  std::string keyFileName;
  std::string dataFileName;
  std::string defaultKeyFileName;
  std::string defaultDataFileName;
  std::string key;
  std::string keyFile;
  int offset;
  bool superword;
  bool withDebug;
}
Param;

void displayEntry(
  const Lima::LinguisticProcessing::AnalysisDict::DictionaryEntry& entry,
  const FsaStringsPool& sp,
  DictionaryEntryLogger& logger);

void usage(int argc, char* argv[]);

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  
  Param param = {
                  std::string(""),
                  std::string("none"),
                  std::string(""),
                  std::string("fsa"),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  std::string(""),
                  -1,
                  false,
                  false
                };


  for (int i = 1 ; i < argc; i++)
  {
    std::string arg(argv[i]);
    std::string::size_type pos = std::string::npos;
    if (arg == "--help")
    {
      usage(argc, argv);
    }
    if ( (pos = arg.find("--language=")) != std::string::npos )
    {
      param.language = arg.substr(pos+11);
    }
    else if ( (pos = arg.find("--dicoId=")) != std::string::npos )
    {
      param.dicoId = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--resdir=")) != std::string::npos )
    {
      param.resdir = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--access=")) != std::string::npos )
    {
      param.access = arg.substr(pos+9);
    }
    else if ( (pos = arg.find("--keyFileName=")) != std::string::npos )
    {
      param.keyFileName = arg.substr(pos+14);
    }
    else if ( (pos = arg.find("--dataFileName=")) != std::string::npos )
    {
      param.dataFileName = arg.substr(pos+15);
    }
    else if ( (pos = arg.find("--key=")) != std::string::npos )
    {
      param.key = arg.substr(pos+6);
    }
    else if ( (pos = arg.find("--withDebug")) != std::string::npos )
    {
      param.withDebug = true;
    }
    else if ( (pos = arg.find("--keyFile=")) != std::string::npos )
    {
      param.keyFile = arg.substr(pos+10);
    }
    else if ( (pos = arg.find("--offset=")) != std::string::npos )
    {
      sscanf((arg.substr(pos+9)).c_str(), "%d", &(param.offset) );
      //      std::cerr << "offset = " << param.offset << std::endl;
    }
    else if ( (pos = arg.find("--superword")) != std::string::npos )
    {
      param.superword=true;
    }
  }


  std::string resourcesPath=string(getenv("LIMA_RESOURCES"));
  std::string configDir=string(getenv("LIMA_CONF"));
  std::string commonConfigFile="/lima-common.xml";
  deque<string> langs;
  langs.push_back(param.language);
  MediaticData::changeable().init(resourcesPath,configDir,commonConfigFile,langs);

  AbstractAnalysisDictionary* dictionary(0);


  if (param.dicoId != "none")
  {
    cout << "initialize Dictionary from configuration file : " << endl;
    cout << " --language='" << param.language << "'" << endl;
    cout << " --dicoId='" << param.dicoId << "'" << endl;


    string configPath=Common::MediaticData::MediaticData::single().getConfigPath();
    cout << "load language " << param.language << endl;
    MediaId langid=MediaticData::single().getMediaId(param.language);
    string file;
    try
    {
      Common::XMLConfigurationFiles::XMLConfigurationFileParser configuration(configPath + "/lima-analysis.xml");
      file=configPath + "/" + configuration.getModuleGroupParamValue(
             "lima-coreclient",
             "mediaProcessingDefinitionFiles",
             param.language);
    }
    catch (NoSuchParam& )
    {
      cerr << "no language definition file for language " << param.language << endl;
      throw InvalidConfiguration();
    }

    XMLConfigurationFileParser langParser(file);

    // initialize resources
    try
    {
      ModuleConfigurationStructure& module=langParser.getModuleConfiguration("Resources");
      LinguisticResources::changeable().initLanguage(
        langid,
        module,
        true);
    }
    catch (NoSuchModule &)
    {
      cerr << "no module 'Resources' in configuration file " << file << endl;
      throw InvalidConfiguration();
    }

    // get dictionary
    AbstractResource* res=LinguisticResources::single().getResource(langid,param.dicoId);
    dictionary=static_cast<AbstractAnalysisDictionary*>(res);
    if (dictionary==0)
    {
      cerr << "can't get dictionary " << param.dicoId << " for language " << param.language << endl;
      return -1;
    }
  }
  else
  {

    cout << "initialize Dictionary from parameters : ";
    cout << "\t--language='" << param.language << "'" << endl;
    if (param.language.empty())
    {
      std::cerr << "ERROR language parameter is empty" << std::endl << std::endl;
      usage(argc, argv);
    }
    cout << "\t--resdir='" << param.resdir << "'" << endl;
    cout << "\t--keyFileName='" << param.keyFileName << "' " << endl;
    if (param.keyFileName.empty())
    {
      std::cerr << "ERROR keyFileName parameter is empty" << std::endl << std::endl;
      usage(argc, argv);
    }
    cout << "\t--dataFileName='" << param.dataFileName << "' " << endl;
    if (param.dataFileName.empty())
    {
      std::cerr << "ERROR dataFileName parameter is empty" << std::endl << std::endl;
      usage(argc, argv);
    }
    if (param.resdir != "")
    {
      param.keyFileName = param.resdir + "/" + param.keyFileName;
      param.dataFileName = param.resdir + "/" + param.dataFileName;
    }

    std::cout << "build accessMethod ..." << std::endl;
    FsaAccessSpare16* fsaAccess=new FsaAccessSpare16();
    fsaAccess->read(param.keyFileName);
    
    std::cout << "register mainkeys" << std::endl;
    MediaId langid=MediaticData::single().getMediaId(param.language);
    FsaStringsPool& sp= Common::MediaticData::MediaticData::changeable().stringsPool(langid);
    sp.registerMainKeys(fsaAccess);

    std::cout << "build EnhancedAnalysisDictionary... " << std::endl;
    dictionary=new EnhancedAnalysisDictionary(
                 &sp,
                 fsaAccess,
                 param.dataFileName);

    std::cout << "EnhancedAnalysisDictionary end" << std::endl;
  }

  MediaId langid=MediaticData::single().getMediaId(param.language);
  
  std::cout << "process test" << std::endl;
  std::vector<Lima::LimaString> terms;

  if (param.keyFile.compare(std::string("") )!= 0)
  {
    std::ifstream inputList(param.keyFile.c_str(), std::ios::in | std::ios::binary );
    if ( !inputList.is_open() )
    {
      std::cerr <<  "Cannot open input list of words " << param.keyFile << std::endl;
      return EXIT_FAILURE;
    }
    std::cout <<  "Read " << param.keyFile << "...." << std::endl;
    for( ; ; )
    {
      LimaString term;
      term = Lima::Common::Misc::utf8stdstring2limastring(Lima::Common::Misc::readLine(inputList));
      if(term.isEmpty())
      {
        break;
      }
      terms.push_back(term);
    }
  }
  else
  {
    terms.push_back(Common::Misc::utf8stdstring2limastring(param.key));
  }
  
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(langid);
  const Common::PropertyCode::PropertyCodeManager& pcm = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(langid)).getPropertyCodeManager();
  DictionaryEntryLogger del(&cout,&sp,&pcm);

  for (vector<LimaString>::const_iterator termItr=terms.begin();
       termItr!=terms.end();
       termItr++)
  {
    if (param.offset > -1 )
    {
      // search subkeys
      std::pair<DictionarySubWordIterator, DictionarySubWordIterator> res =
        dictionary->getSubWordEntries(param.offset ,*termItr);
      std::cout << "===== SubWords of '" << Lima::Common::Misc::limastring2utf8stdstring(*termItr) << "' at offset " << param.offset << " =====" << std::endl;
      for( ; res.first != res.second ; res.first++ )
      {
        std::cout << "** Display Entry from offset " << param.offset << " to " << (*(res.first)).first << std::endl;
        int offset = (*(res.first)).first;
        LimaString term = (*termItr).mid(param.offset, offset - param.offset );
        const Lima::LinguisticProcessing::AnalysisDict::DictionaryEntry& entry=(*(res.first)).second;
        displayEntry(entry,sp,del);
      }
    }
    else if (param.superword)
    {
      // search superwords
      std::pair< DictionarySuperWordIterator, DictionarySuperWordIterator > res =
        dictionary->getSuperWordEntries(*termItr);
      std::cout << "===== Superwords of '" << Lima::Common::Misc::limastring2utf8stdstring(*termItr) << "' =====" << std::endl;
      for( ; res.first != res.second ; res.first++ )
      {
        std::cout << Lima::Common::Misc::limastring2utf8stdstring(*(res.first)) << std::endl;
      }
    }
    else
    {
      // search key
      std::cout << "===== Entry for '"  << Lima::Common::Misc::limastring2utf8stdstring(*termItr) << "' =====" << std::endl;
      const Lima::LinguisticProcessing::AnalysisDict::DictionaryEntry& entry=dictionary->getEntry(*termItr);
      displayEntry( entry, sp, del );
    }
  }
  return EXIT_SUCCESS;
}

void displayEntry( 
  const Lima::LinguisticProcessing::AnalysisDict::DictionaryEntry& entry,
  const FsaStringsPool& sp,
  DictionaryEntryLogger& logger)
{
  if (entry.isEmpty()) { 
    cout << "DictionaryEntry : empty" << endl;
    return;
  }
  
  cout << "DictionaryEntry : form=\"" << Lima::Common::Misc::limastring2utf8stdstring(sp[entry.getEntryId()]) << "\" ";
  if (entry.isFinal()) {
    cout << " final=\"true\" ";
  } else {
    cout << " final=\"false\" ";
  }
  cout << endl;
  if (entry.hasLingInfos()) {
    cout << "- has linguistic infos : " << endl;
    entry.parseLingInfos(&logger);
  } else {
    cout << "- has no linguistic infos" << endl;
  }
  if (entry.hasConcatenated()) {
    cout << "- has concatenated infos : " << endl;
    entry.parseConcatenated(&logger);
  } else {
    cout << "- has no concatenated infos" << endl;
  }
  if (entry.hasAccentedForms()) {
    cout << "- has accented forms : " << endl;
    entry.parseAccentedForms(&logger);
  } else {
    cout << "- has no accented forms" << endl;
  }
}

/*
int displayEntry( AbstractAnalysisDictionary* dictionary, Lima::LinguisticProcessing::Dictionary::DictionaryEntry& entry,
                  const LimaString& term,
                  MediaId language  )
{


  std::string term8= Common::Misc::limastring2utf8stdstring(term);

  std::cout << "<k>" << term8
  << "</k> (" << Lima::Common::Misc::convertString(term) << ");" << std::endl;

  bool hasAnyInfo=false;

  if( entry.hasLingInfo() )
  {
    hasAnyInfo=true;
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

      Lima::LinguisticProcessing::Dictionary::LingPropertyEntry property = lingInfo.nextLingProperty();
      while (!property.isEmpty())
      {
        int data = property.data();
        std::cout << "<p>" << data << "</p> ( ";
        for (std::map<std::string,Common::PropertyCode::PropertyManager>::const_iterator propItr=pcm.getPropertyManagers().begin();
             propItr!=pcm.getPropertyManagers().end();
             propItr++)
        {
          if (! propItr->second.getPropertyAccessor().empty(data)) {
            std::cout << propItr->first << " = " << propItr->second.getPropertySymbolicValue(data) << ", ";
          }
        }
        std::cout << " )" << endl;
        property = lingInfo.nextLingProperty();
      }
      std::cout << "</i>" << std::endl;
      lingInfo = entry.nextLingInfo();
    }
  }

  if( entry.hasAccented() )
  {
    hasAnyInfo=true;
    Lima::LimaString accented = entry.nextAccented();
    while (accented.length() != 0)
    {
      std::string accented8= Common::Misc::limastring2utf8stdstring(accented);
      std::cout << "<a>" << accented8 << "</a>";
      accented = entry.nextAccented();
    }
    std::cout << std::endl;
  }

  if( entry.hasIdiomatic() )
  {
    hasAnyInfo=true;
    std::cout << "Idiomatic: ";
    int idiomatic = entry.nextIdiomatic();
    while (idiomatic != 0)
    {
      std::cout << "<id_exp>" << idiomatic << "</id_exp>" << std::endl;
      idiomatic = entry.nextIdiomatic();
    }
  }

  if( entry.hasHyphenLingInfo() )
  {
    hasAnyInfo=true;
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

  if( entry.hasConcatenated() )
  {
    hasAnyInfo=true;
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

  if( entry.hasDefaultProperty() )
  {
    hasAnyInfo=true;
    std::cout << "entry has defaultProperty" << std::endl;
    Lima::LinguisticProcessing::Dictionary::LingPropertyEntry property = entry.nextLingProperty();
    while (!property.isEmpty())
    {
      int data = property.data();
      std::cout << "<p>" << data << "</p> ";
        std::cout << "<p>" << data << "</p> ( ";
        for (std::map<std::string,Common::PropertyCode::PropertyManager>::const_iterator propItr=pcm.getPropertyManagers().begin();
             propItr!=pcm.getPropertyManagers().end();
             propItr++)
        {
          if (! propItr->second.getPropertyAccessor().empty(data)) {
            std::cout << propItr->first << " = " << propItr->second.getPropertySymbolicValue(data) << ", ";
          }
        }
        std::cout << " )" << endl;
      property = entry.nextLingProperty();
    }
    std::cout << std::endl;
  }

  if (!hasAnyInfo)
  {
    std::cout << "Empty Entry" << std::endl;
  }
}
*/

void usage(int argc, char* argv[])
{
  LIMA_UNUSED(argc)
  std::cerr << "USAGE : " << argv[0] << " [CONFIG] [COMMANDS]" << std::endl;
  std::cerr << "where [CONFIG] is :" << std::endl;
  std::cerr << "  * To load dictionary from configuration file" << std::endl;
  std::cerr << "    --language=<language trigram>" << std::endl;
  std::cerr << "    --dicoId=<config file id>" << std::endl;
  std::cerr << "  * To specify dictionary configuration" << std::endl;
  std::cerr << "    --language=<language trigram>" << std::endl;
  std::cerr << "   [--resdir=<directory>] : default is current dir" << std::endl;
  std::cerr << "   [--access=<fsa|...>] : default is fsa" << std::endl;
  std::cerr << "    --keyFileName=<filename>" << std::endl;
  std::cerr << "    --dataFileName=<filename>" << std::endl;
  std::cerr << "where [COMMANDS] are : " << std::endl;
  std::cerr << "   [--key=<key>]" << std::endl;
  std::cerr << "   [--keyFile=<filename>]" << std::endl;
  std::cerr << "   [--offset=<offset>] : if >=0, then search subword in given key(s) from offset" << std::endl;
  std::cerr << "   [--superword] : if set then search superwords from given key(s)" << std::endl;
  std::cerr << "   [--withDebug]" << std::endl;
  exit(0);
}
