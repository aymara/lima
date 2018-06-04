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
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/

#include "languageData.h"

// #include "linguisticData.h"
#include "common/misc/stringspool.h"
#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileParser.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/tools/FileUtils.h"
#include "linguisticProcessing/common/PropertyCode/PropertyManager.h"
#include "linguisticProcessing/common/PropertyCode/PropertyCodeManager.h"

#include <string>
#include <fstream>
#include <QtCore>

using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::PropertyCode;


namespace Lima
{
namespace Common
{
namespace MediaticData
{

class LanguageDataPrivate
{
  friend class LanguageData;
  
  LanguageDataPrivate(LanguageData* ld);

  virtual ~LanguageDataPrivate();

  void initPropertyCode(
    const std::string& resourcesPath,
    XMLConfigurationFileParser& conf);

  //void initSentenceBreakMicros(XMLConfigurationFileParser& conf);
  void initEmptyMicroCategories(XMLConfigurationFileParser& conf);
  void initEmptyMacroCategories(XMLConfigurationFileParser& conf);
  void initHomoSyntagmaticChainsAndRelationsTypes(XMLConfigurationFileParser& configParser);
  void initCompoundTensesDefinitions(XMLConfigurationFileParser& configParser);
  void initConjugatedVerbs(XMLConfigurationFileParser& conf);
  void initPropositionIntroductors(XMLConfigurationFileParser& conf);
  void initNounPhraseHeadMicroCategories(XMLConfigurationFileParser& conf);
  void initDefiniteMicroCategories(XMLConfigurationFileParser& conf);
  void initSyntacticRelations(XMLConfigurationFileParser& conf);
  void initLimaToLanguageCodeMapping(XMLConfigurationFileParser& conf);

  LanguageData* m_p;
  MediaId m_language;

  PropertyCode::PropertyCodeManager m_propCodeManager;

  std::list< LinguisticCode > m_sentenceBreakMicros;

  /**
  * the lists of the empty macrocategories
  * it is a subset of the macroCategories
  * Note that the lists of empty macrocategories and macro categories for extract
  * should be disjoined
  */
  std::set< LinguisticCode > m_emptyMacroCategories;

  /**
  * the list of the empty microcategories
  */
  std::set< LinguisticCode > m_emptyMicroCategories;

  /**
  * the list of conjugated verbs used in syntagmatic chains paths disambiguation
  */
  std::set< LinguisticCode > m_conjugatedVerbs;

  /**
  * the list of micro categories that can be head of noun phrases
  */
  std::set< LinguisticCode > m_nounPhraseHeadMicroCategories;

  /**
  * the list of determiners micro categories that make a noun phrase definite
  */
  std::set< LinguisticCode > m_definiteMicroCategories;

  /**
  * the list of proposition introductors used in syntagmatic chains paths disambiguation
  */
  std::set< LinguisticCode > m_propositionIntroductors;

  std::multimap<ChainsType, uint64_t> m_homoSyntagmaticChainsAndRelationsTypes;
  
  std::map< std::pair<LinguisticCode, LinguisticCode>, LinguisticCode > m_compoundTenseDefiniton;

  static const std::string s_emptycateg;

  std::map< LinguisticCode, ConceptType > m_macro2ConceptMapping;

  std::map< std::string, SyntacticRelationId > m_syntacticRelations;
  
  std::map<QString, QString> m_limaToLanguageCodeMapping;

public:
    QDebug single(MediaId m_language);
}
; // end class

LanguageDataPrivate::LanguageDataPrivate(LanguageData* ld) :
  m_p(ld)
{
}

LanguageDataPrivate::~LanguageDataPrivate() 
{
}


const std::string LanguageDataPrivate::s_emptycateg = SYMBOLIC_NONE_1;


const PropertyCode::PropertyCodeManager& LanguageData::getPropertyCodeManager() const
  { return m_d->m_propCodeManager; }

bool LanguageData::isAnEmptyMacroCategory(LinguisticCode id) const
  { return m_d->m_emptyMacroCategories.find(id)!=m_d->m_emptyMacroCategories.end(); }

bool LanguageData::isAnEmptyMicroCategory(LinguisticCode id) const
  { return m_d->m_emptyMicroCategories.find(id)!=m_d->m_emptyMicroCategories.end(); }

bool LanguageData::isAConjugatedVerb(LinguisticCode id) const
  { return m_d->m_conjugatedVerbs.find(id)!=m_d->m_conjugatedVerbs.end(); }

bool LanguageData::isAPropositionIntroductor(LinguisticCode id) const
  { return m_d->m_propositionIntroductors.find(id)!=m_d->m_propositionIntroductors.end(); }

std::set< LinguisticCode >&  LanguageData::getNounPhraseHeadMicroCategories()
  { return m_d->m_nounPhraseHeadMicroCategories; }

const std::set< LinguisticCode >&  LanguageData::getNounPhraseHeadMicroCategories() const
  { return m_d->m_nounPhraseHeadMicroCategories; }

const std::set< LinguisticCode >&  LanguageData::getDefiniteMicroCategories() const
  { return m_d->m_definiteMicroCategories; }

const std::set< LinguisticCode >&  LanguageData::getConjugatedVerbs() const
  { return m_d->m_conjugatedVerbs; }

LanguageData::LanguageData() : MediaData(), m_d(new LanguageDataPrivate(this))
{
}

LanguageData::LanguageData(const LanguageData& ld) : MediaData(ld), m_d(new LanguageDataPrivate(*ld.m_d))
{
}

LanguageData::~LanguageData()
{
  delete m_d;
}


void  LanguageData::initialize(
  MediaId lang,
  const std::string& resourcesPath,
  XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  LINFO << "LanguageData::initialize lang:" << lang << "; resourcesPath:" << resourcesPath;
  MediaData::initialize(lang, resourcesPath, conf);
  m_d->m_language = lang;
  m_d->initPropertyCode(resourcesPath,conf);
  //m_d->initSentenceBreakMicros(conf);
  m_d->initEmptyMacroCategories(conf);
  m_d->initEmptyMicroCategories(conf);
  m_d->initHomoSyntagmaticChainsAndRelationsTypes(conf);
  m_d->initCompoundTensesDefinitions(conf);
  m_d->initConjugatedVerbs(conf);
  m_d->initPropositionIntroductors(conf);
  m_d->initNounPhraseHeadMicroCategories(conf);
  m_d->initDefiniteMicroCategories(conf);
  m_d->initSyntacticRelations(conf);
  m_d->initLimaToLanguageCodeMapping(conf);

}

void LanguageDataPrivate::initPropertyCode(
  const std::string& resourcesPathsStd,
  XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  LINFO << "LanguageDataPrivate::initPropertyCode initializes the property coding system with resources path" << resourcesPathsStd;
  try
  {
    QStringList resourcesPaths= QString::fromUtf8(resourcesPathsStd.c_str()).split(LIMA_PATH_SEPARATOR);
    bool propertyCodeFileFound = false;
    QString propertyCodeFile = conf.getModuleGroupParamValue("LinguisticData","Categories","PropertyCodeFile").c_str();
    Q_FOREACH(QString resourcesPath, resourcesPaths)
    {
      QString propertyFile(resourcesPath + "/" + propertyCodeFile);
#ifdef DEBUG_LP
        LDEBUG << "LanguageDataPrivate::initPropertyCode trying property file" << propertyFile;
#endif
      QFileInfo propertyFileInfo(propertyFile);
      if (propertyFileInfo.exists())
      {
#ifdef DEBUG_LP
        LDEBUG << "LanguageDataPrivate::initPropertyCode reading property file" << propertyFileInfo.filePath();
#endif
        m_propCodeManager.readFromXmlFile(propertyFileInfo.filePath().toUtf8().constData());
        propertyCodeFileFound = true;
        // Read at most one property code file for a language
        break;
      }
    }
    if (!propertyCodeFileFound)
    {
      LERROR << "No property code file"<<propertyCodeFile<<"found in paths:" << resourcesPaths;
      throw InvalidConfiguration();
    }
  }
  catch (std::exception& e)
  {
    LERROR << "Error while reading PropertyFile file: " << e.what();
    throw InvalidConfiguration();
  }
#ifdef DEBUG_LP
  LDEBUG << "LanguageDataPrivate::initPropertyCode DONE";
#endif
}

/*
void LanguageDataPrivate::initSentenceBreakMicros(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  LINFO << "initialize sentence break micros";
  const PropertyManager& microManager=m_propCodeManager.getPropertyManager("MICRO");
  try
  {
    deque<string> sbm=conf.getModuleGroupListValues("LinguisticData","Categories","sentenceBreakMicros");
    for (deque<string>::const_iterator it=sbm.begin();
         it!=sbm.end();
         it++)
    {
      LinguisticCode micro=microManager.getPropertyValue(*it);
      LDEBUG << "insert " << *it << " with code " << micro;
      m_sentenceBreakMicros.push_back(micro);
    }
  }
  catch (NoSuchList& )
  {
    LERROR << "No sentence break micros defines for language " << MediaticData::single().media(m_language);
    throw InvalidConfiguration();
  }
}
*/

void LanguageDataPrivate::initEmptyMicroCategories(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  // initializes the lists of empty micro categories
  LINFO << "initializes the lists of empty micro categories";
  m_emptyMicroCategories.clear();
  std::deque< std::string >& emptyMicroCategoriesList =
    conf.getModuleGroupListValues("LinguisticData","Categories","emptyMicro");
  const PropertyManager& microManager=m_propCodeManager.getPropertyManager("MICRO");
  for (std::deque< std::string >::iterator it = emptyMicroCategoriesList.begin(); it != emptyMicroCategoriesList.end(); it++)
  {
    LinguisticCode mic=microManager.getPropertyValue(*it);
#ifdef DEBUG_LP
    LDEBUG << "add " << mic << " to empty micros";
#endif
    m_emptyMicroCategories.insert(mic);
  }
}

void LanguageDataPrivate::initConjugatedVerbs(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  // initializes the lists of empty micro categories
  LINFO << "initializes the lists conjugated verbs";
  m_conjugatedVerbs.clear();
  std::deque< std::string >& conjugatedVerbsList =
    conf.getModuleGroupListValues("LinguisticData","Categories","conjugatedVerbs");
  const PropertyManager& microManager=m_propCodeManager.getPropertyManager("MICRO");
  for (std::deque< std::string >::iterator it = conjugatedVerbsList.begin(); it != conjugatedVerbsList.end(); it++)
  {
    LinguisticCode mic=microManager.getPropertyValue(*it);
#ifdef DEBUG_LP
    LDEBUG << "add " << mic << " to conjugated verbs";
#endif
    m_conjugatedVerbs.insert(mic);
  }
}

void LanguageDataPrivate::initPropositionIntroductors(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  // initializes the lists of empty micro categories
  LINFO << "initializes the list of proposition introductors";
  m_propositionIntroductors.clear();
  std::deque< std::string >& propositionIntroductorsList =
    conf.getModuleGroupListValues("LinguisticData","Categories","propositionIntroductors");
  const PropertyManager& microManager=m_propCodeManager.getPropertyManager("MICRO");
  for (std::deque< std::string >::iterator it = propositionIntroductorsList.begin(); it != propositionIntroductorsList.end(); it++)
  {
    LinguisticCode mic=microManager.getPropertyValue(*it);
#ifdef DEBUG_LP
    LDEBUG << "add " << mic << " to proposition introductors";
#endif
    m_propositionIntroductors.insert(mic);
  }
}

void LanguageDataPrivate::initEmptyMacroCategories(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;

  // initializes the lists of empty macro categories
  LINFO << "initializes the lists of empty macro categories:";
  m_emptyMacroCategories.clear();
  auto& emptyMacroCategoriesList = 
      conf.getModuleGroupListValues("LinguisticData","Categories","emptyMacro");
  const PropertyManager& macroManager = m_propCodeManager.getPropertyManager(
      "MACRO");
  for (auto it = emptyMacroCategoriesList.cbegin();
       it != emptyMacroCategoriesList.cend(); it++)
  {
    m_emptyMacroCategories.insert(macroManager.getPropertyValue(*it));
  }
}

void LanguageDataPrivate::initHomoSyntagmaticChainsAndRelationsTypes(
  XMLConfigurationFileParser& configParser)
{
  LDATALOGINIT;
  LINFO << "initializes the associations between chains types and homosyntagmatic relations types.";

  try
  {
    std::deque< std::string > relationsList =
      configParser.getModuleGroupListValues(
        "LinguisticData",
        "SyntacticAnalysis",
        "NominalHomoSyntagmaticRelations");

    std::deque< std::string >::const_iterator it, it_end;
    it = relationsList.begin(); it_end = relationsList.end();
    for (; it != it_end; it++)
    {
      map<string,uint64_t>::const_iterator r=m_syntacticRelations.find(*it);
      if (r!=m_syntacticRelations.end())
      {
        m_homoSyntagmaticChainsAndRelationsTypes.
          insert(std::make_pair(NOMINAL, (*r).second));
      }
    }
  }
  catch (const NoSuchList& )
  {
    LERROR << "LinguisticData/SyntacticAnalysis/NominalHomoSyntagmaticRelations parameter not found.";
    throw InvalidConfiguration();
  }
  try
  {
    std::deque< std::string > relationsList =
      configParser.getModuleGroupListValues(
        "LinguisticData",
        "SyntacticAnalysis",
        "VerbalHomoSyntagmaticRelations");

    std::deque< std::string >::const_iterator it, it_end;
    it = relationsList.begin();
    it_end = relationsList.end();
    for (; it != it_end; it++)
    {
      map<string,uint64_t>::const_iterator r=m_syntacticRelations.find(*it);
      if (r!=m_syntacticRelations.end())
      {
        m_homoSyntagmaticChainsAndRelationsTypes.
          insert(std::make_pair(VERBAL, (*r).second));
      }
    }
  }
  catch (const NoSuchList& )
  {
    LWARN << "LinguisticData/SyntacticAnalysis/VerbalHomoSyntagmaticRelations parameter not found.";
    throw InvalidConfiguration();
  }
}


void LanguageDataPrivate::initCompoundTensesDefinitions(
  XMLConfigurationFileParser& configParser)
{
  LDATALOGINIT;
  LINFO << "initializes the compound tenses definitions";
  
  std::string resourcesPath=MediaticData::single().getResourcesPath();

  std::string compoundTensesDefinitionsFile;
  try
  {
    compoundTensesDefinitionsFile = configParser.getModuleGroupParamValue(
                                      "LinguisticData",
                                      "SyntacticAnalysis",
                                      "CompoundTensesDefFile");
  }
  catch (const NoSuchParam& )
  {
    LINFO << "LinguisticProcessors/SyntacticAnalysis/CompoundTensesDefFile parameter not found for language " 
          << MediaticData::single().media(m_language) << ".";
    return;
  }
  if (compoundTensesDefinitionsFile.find_first_of("/")!=0)
  {
    QStringList resourcesPaths = 
        QString::fromUtf8(resourcesPath.c_str()).split(LIMA_PATH_SEPARATOR);
    Q_FOREACH(QString resPath, resourcesPaths)
    {
      if  (QFileInfo::exists(resPath + "/" + compoundTensesDefinitionsFile.c_str()))
      {
        compoundTensesDefinitionsFile = 
            (resPath + "/" 
              + compoundTensesDefinitionsFile.c_str()).toUtf8().constData();
        break;
      }
    }
  }

  std::ifstream ifl(compoundTensesDefinitionsFile.c_str(), std::ifstream::binary);
  size_t linesCounter = 0;

  if (!ifl)
  {
    LWARN << "Compound tenses definition file not found: " 
          << compoundTensesDefinitionsFile;
    return;
  }

  const PropertyManager& microManager = m_propCodeManager.getPropertyManager(
      "MICRO");
  QString timeCode = m_p->getLimaToLanguageCodeMappingValue("TIME");
  const PropertyManager& tenseManager = m_propCodeManager.getPropertyManager(
      timeCode.toUtf8().constData());

  std::string line = Lima::Common::Misc::readLine(ifl);
  Misc::chomp(line);
  linesCounter++;
  while (ifl.good() && !ifl.eof())
  {
    if ( (line != "") && (line[0] != '#') )
    {
#ifdef DEBUG_LP
      LDEBUG << "read line : " << line;
#endif

      size_t index = 0;

      size_t next = line.find_first_of(";", index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find first delimiter in line: " << line;
        throw InvalidConfiguration();
      }
      LinguisticCode mode = microManager.getPropertyValue(line.substr(0, next));

      index = next+1;
      next=line.find_first_of(";",index);
      if (next == std::string::npos)
      {
        LERROR << "Cannot find second delimiter in line: " << line;
        throw InvalidConfiguration();
      }
      LinguisticCode auxTense = tenseManager.getPropertyValue(line.substr(index, next-index));

      LinguisticCode compoundTense = tenseManager.getPropertyValue(line.substr(next+1));

      m_compoundTenseDefiniton.insert(std::make_pair( std::make_pair(mode, auxTense), compoundTense));
    }
    line = Lima::Common::Misc::readLine(ifl);
    Misc::chomp(line);
    linesCounter++;
  }
}

bool LanguageData::isTypeARelationForChain(
  ChainsType chainType,
  uint64_t relationType) const
{
  if (m_d->m_homoSyntagmaticChainsAndRelationsTypes.find(chainType) ==
      m_d->m_homoSyntagmaticChainsAndRelationsTypes.end())
    return false;

  std::pair<
  std::multimap<ChainsType, uint64_t>::const_iterator,
  std::multimap<ChainsType, uint64_t>::const_iterator >
  itPair = m_d->m_homoSyntagmaticChainsAndRelationsTypes.equal_range(chainType);
  for (; itPair.first != itPair.second; itPair.first++)
  {
    if ( (*(itPair.first)).second == relationType )
      return true;
  }
  return false;
}

void LanguageDataPrivate::initNounPhraseHeadMicroCategories(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  LINFO << "init noun phrase head micro categories";
  const PropertyManager& microManager=m_propCodeManager.getPropertyManager("MICRO");
  try
  {
    // loads syntactic analysis resources
    std::deque< std::string > nphmcl = conf.getModuleGroupListValues("LinguisticData", "SyntacticAnalysis", "NounPhraseHeadCategs");
    for (std::deque< std::string >::iterator it = nphmcl.begin(); it != nphmcl.end(); it++)
    {
      LinguisticCode mic=microManager.getPropertyValue(*it);
      m_nounPhraseHeadMicroCategories.insert(mic);
    }
  }
  catch (NoSuchList& )
  {
    LERROR << "No list 'NounPhraseHeadCategs' in 'SyntacticAnalysis' group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
  catch (NoSuchGroup& )
  {
    LERROR << "No group 'SyntacticAnalysis' in common language configuration file for language " << (int)m_language;
    throw InvalidConfiguration();
  }
}

void LanguageDataPrivate::initDefiniteMicroCategories(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  LINFO << "init definite micro categories";
  const PropertyManager& microManager=m_propCodeManager.getPropertyManager("MICRO");
  try
  {
    // loads syntactic analysis resources
    std::deque< std::string > dcl = conf.getModuleGroupListValues("LinguisticData", "SyntacticAnalysis", "DefiniteCategs");
    for (std::deque< std::string >::iterator it = dcl.begin(); it != dcl.end(); it++)
    {
      LinguisticCode mic=microManager.getPropertyValue(*it);
      m_definiteMicroCategories.insert(mic);
    }
  }
  catch (NoSuchList& )
  {
    LERROR << "No list 'DefiniteCategs' in 'SyntacticAnalysis' group for language " << (int)m_language;
    throw InvalidConfiguration();
  }
  catch (NoSuchGroup& )
  {
    LERROR << "No group 'SyntacticAnalysis' in common language configuration file for language " << (int)m_language;
    throw InvalidConfiguration();
  }
}

void LanguageDataPrivate::initSyntacticRelations(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  LINFO << "init syntacticRelations";

  m_syntacticRelations.clear();

  try
  {
    GroupConfigurationStructure& group=
      conf.getModuleGroupConfiguration("LinguisticData","SyntacticRelations");
    deque<string>& relations=group.getListsValueAtKey("relationList");

    // init syntactic relations
    for (deque<string>::const_iterator it=relations.begin(),
           it_end=relations.end(); it!=it_end; it++) {
      // Take care not to store same relation twice
      if(m_syntacticRelations[*it] == 0)
      {
        m_syntacticRelations[*it]=m_syntacticRelations.size();
      }
    }
  }
  catch (NoSuchList& )
  {
    LERROR << "No map 'relationList' in common language configuration file for language "
    << (int)m_language;
    throw InvalidConfiguration();
  }
  catch (NoSuchGroup& )
  {
    LERROR << "No group 'SyntacticRelations' in common language configuration file for language "
    << (int)m_language;
    throw InvalidConfiguration();
  }
}

void LanguageDataPrivate::initLimaToLanguageCodeMapping(XMLConfigurationFileParser& conf)
{
  LDATALOGINIT;
  LINFO << "init LimaToLanguageCodeMapping";

  m_limaToLanguageCodeMapping.clear();

  try
  {
    GroupConfigurationStructure& group = conf.getModuleGroupConfiguration("LinguisticData",
                                                                          "Categories");
    std::map<std::string,std::string>& smapping = group.getMapAtKey("limaToLanguageCodeMapping");

    // init syntactic relations
    for (auto it = smapping.cbegin(); it != smapping.cend(); it++) 
    {
      m_limaToLanguageCodeMapping[QString::fromUtf8((*it).first.c_str())] =
          QString::fromUtf8((*it).second.c_str());
    }
  }
  catch (NoSuchMap& )
  {
    LDEBUG << "No map 'LinguisticData/Categories/llimaToLanguageCodeMapping' in common language configuration file for language "
    << (int)m_language;
  }
  catch (NoSuchGroup& )
  {
    LERROR << "No group 'LinguisticData/Categories' in common language configuration file for language "
    << (int)m_language;
    throw InvalidConfiguration();
  }
}


LinguisticCode LanguageData::compoundTense(LinguisticCode mode, LinguisticCode auxTense) const
{
  std::pair<LinguisticCode, LinguisticCode> def = std::make_pair(mode, auxTense);
  std::map< std::pair<LinguisticCode, LinguisticCode>, LinguisticCode >::const_iterator defIt;
  defIt = m_d->m_compoundTenseDefiniton.find(def);
  if (defIt == m_d->m_compoundTenseDefiniton.end())
  {
    LDATALOGINIT;
    LWARN << "ask for unknown compound tense definition: '" << mode << "/" << auxTense << "'";
    return ID_NONE_1;
  }

  return (*defIt).second;
}

LinguisticCode LanguageData::compoundTense(std::string& mode, std::string& auxTense) const
{
  const PropertyManager& microManager=m_d->m_propCodeManager.getPropertyManager("MICRO");
  const PropertyManager& tenseManager=m_d->m_propCodeManager.getPropertyManager("TIME");
  LinguisticCode res = compoundTense(microManager.getPropertyValue(mode), tenseManager.getPropertyValue(auxTense));
  if (res == ID_NONE_1)
  {
    LDATALOGINIT;
    LWARN << "ask for unknown compound tense definition: '" << mode << "/" << auxTense << "'";
    return ID_NONE_1;
  }

  return res;
}

const std::string& LanguageData::getSyntacticRelationName(SyntacticRelationId id) const 
{
  static std::string nullString("");
  // linear search
  for (map<string,SyntacticRelationId>::const_iterator 
         it=m_d->m_syntacticRelations.begin(),
         it_end=m_d->m_syntacticRelations.end(); it!=it_end; it++) {
    if ((*it).second==id) {
#ifdef DEBUG_LP
      LDATALOGINIT;
      LDEBUG << "found " << id << " <=> " << (*it).first;
#endif
      return (*it).first;
    }
  }
  return nullString;
}

SyntacticRelationId LanguageData::getSyntacticRelationId(const std::string& name) const
{
  map<string,SyntacticRelationId>::const_iterator it=m_d->m_syntacticRelations.find(name);
  if (it!=m_d->m_syntacticRelations.end()) {
#ifdef DEBUG_LP
    LDATALOGINIT;
    LDEBUG << "LanguageData::getSyntacticRelationId found" << name << " <=> " << (*it).second;
#endif
    return (*it).second;
  }
  LDATALOGINIT;
  LERROR << "LanguageData::getSyntacticRelationId NOT found" << name << "!" ;
  return 0;
}

const std::map<QString, QString>& LanguageData::getLimaToLanguageCodeMapping() const
{
  return m_d->m_limaToLanguageCodeMapping;
}

QString LanguageData::getLimaToLanguageCodeMappingValue(const QString& code) const
{
  auto it = m_d->m_limaToLanguageCodeMapping.find(code);
  if (it == m_d->m_limaToLanguageCodeMapping.cend())
  {
    return code;
  }
  else
  {
    return (*it).second;
  }
}

SimpleFactory<MediaData,LanguageData> languageDataFactory(LANGUAGEDATA_CLASSID);

} // LinguisticData
} // Common
} // Lima
