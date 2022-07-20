// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA - LIST                                      *
 *                                                                         *
 ***************************************************************************/



#ifndef LIMA_COMMONS_LINGUISTICDATA_LANGUAGEDATA_H
#define LIMA_COMMONS_LINGUISTICDATA_LANGUAGEDATA_H

#include "common/LimaCommon.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/MediaticData/mediaData.h"

#include <stdexcept>
#include <list>
#include <vector>
#include <set>

namespace Lima
{

namespace Common
{
  namespace PropertyCode {
    class PropertyCodeManager;
  }
  namespace XMLConfigurationFiles {
    class XMLConfigurationFileParser;
  }
namespace MediaticData
{
#define LANGUAGEDATA_CLASSID "LanguageData"

#define ID_NONE_1 L_NONE
#define SYMBOLIC_NONE_1 "NONE_1"

/** syntactic chains type */
typedef enum {NO_CHAIN_TYPE=0, NOMINAL, VERBAL} ChainsType;

typedef uint64_t SyntacticRelationId;

class LanguageDataPrivate;
/**
  * @brief Holds linguistic data for one language
  * @author Benoit Mathieu <mathieub@zoe.cea.fr>
  * @date
  *
  * Cette classe contient toutes les informations linguistiques intrinseques
  * a une langue. Ces informations disponibles doivent etre generales et non
  * specifiques a un module.
  *
  */

class LIMA_LINGUISTICDATA_EXPORT LanguageData : public MediaData
{
public:

  LanguageData();

  virtual ~LanguageData();

  /** @brief initialize languageData with the given module.
    * @return statusCode
    * @retval SUCCESS_ID success
    * @retval INVALID_CONFIGURATION error in configuration
    */
  virtual void initialize(
    MediaId lang,
    const std::string& resourcesPath,
    XMLConfigurationFiles::XMLConfigurationFileParser& conf) override;

  const PropertyCode::PropertyCodeManager& getPropertyCodeManager() const;

  /** @return the codes corresponding to NamedEntities categories */
  const std::vector<std::set<LinguisticCode> >& getMicrosForNENormalization() const;

  /** @return true if it is an empty macrocategory */
  bool isAnEmptyMacroCategory(LinguisticCode id) const ;

  /** @return true if the given micro-category numerical value is an empty micro category ; false elsewhere */
  bool isAnEmptyMicroCategory(LinguisticCode id) const ;

  bool isAConjugatedVerb(LinguisticCode id) const;

  bool isAPropositionIntroductor(LinguisticCode id) const;

  bool isTypeARelationForChain(ChainsType, uint64_t) const;

  LinguisticCode compoundTense(LinguisticCode mode, LinguisticCode auxTense) const;
  LinguisticCode compoundTense(std::string& mode, std::string& auxTense) const;

  std::set< LinguisticCode >&  getNounPhraseHeadMicroCategories();

  const std::set< LinguisticCode >&  getNounPhraseHeadMicroCategories() const;
  const std::set< LinguisticCode >&  getDefiniteMicroCategories() const;
  const std::set< LinguisticCode >&  getConjugatedVerbs() const;

  typedef std::map<std::string,uint64_t> EntityTypes;
  typedef std::map<uint64_t,std::string> EntityNames;

  LinguisticCode getEntityType(const EntityTypes& entityTypes,
                               const std::string& typeName) const;
  LinguisticCode getEntityType(const std::string& entity,
                               const std::string& typeName) const;

  // same but using common types identifiers (defined in linguisticData)
  LinguisticCode getEntityCommonType(const std::string& entity,
                                     const std::string& typeName) const;

  const std::string& getEntityName(const std::string& entity,
                                   LinguisticCode type) const;

  const EntityTypes& getEntityTypes(const std::string& entity) const;
  // not a const reference because the result object is built
  // inside the function
  EntityNames getEntityNames(const std::string& entity) const;

  const std::string& getSyntacticRelationName(SyntacticRelationId id) const;
  SyntacticRelationId getSyntacticRelationId(const std::string& name) const;

  /** \name Codes mapping
   *  Mapping between internal LIMA codes and Language code. */
  /**@{*/
  /**
   * The return map contains a mapping between current LIMA internal codes and
   * language codes. For example for Universal Dependencies, LIMA TIME is
   * associated to Tense. */
  const std::map<QString, QString>& getLimaToLanguageCodeMapping() const;
  /**
   * Access to an element of the code mapping. Return the parameter itself if
   * the mapping does not contain it.
   */
  QString getLimaToLanguageCodeMappingValue(const QString& code) const;
  /**@} */

private:
  LanguageData(const LanguageData& ld);
  LanguageData& operator=(const LanguageData& ld);
  LanguageDataPrivate* m_d;
};

} // MediaticData
} // Common
} // Lima

#endif
