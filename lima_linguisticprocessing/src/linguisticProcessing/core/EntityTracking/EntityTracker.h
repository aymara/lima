// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef ENTITYTRACKER_H
#define ENTITYTRACKER_H

#include "EntityTrackingExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "common/MediaticData/EntityType.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"
#include "common/MediaProcessors/MediaProcessUnit.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include "CoreferenceEngine.h"

#include <vector>
#include <string.h>

namespace Lima
{
namespace LinguisticProcessing
{
namespace EntityTracking
{

#define ENTITYTRACKER_CLASSID "EntityTracker"

class LIMA_ENTITYTRACKING_EXPORT EntityTracker :  public MediaProcessUnit, std::vector< LinguisticGraphVertex>
{
public:

  EntityTracker();
  EntityTracker(const Automaton::RecognizerMatch& entity,
                           FsaStringsPool& sp);
  virtual ~EntityTracker();

  void init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
            Manager* manager) override;

  LimaStatusCode process(AnalysisContent& analysis) const override;

  
  /** @brief The functions that dumps a SpecificEntityAnnotation on an output stream */
  void dump(std::ostream& os);

  /* test if the two string are equals */
  inline bool isEqual(const std::string a,const std::string b) const {if (strcmp(a.c_str(),b.c_str())==0) return true; else return false;}
  /* return true if the current word is included in the original word */
  bool isInclude(const std::string original, const std::string currentWord) const;
  /* return true if the current word is an acronym of the original word */
  bool isAcronym(const std::string original, const std::string currentWord) const;

  void addNewForm(const std::string original, const std::string currentWord);
  bool exist(const std::string original, const std::vector< std::vector<std::string> > Acronyms) const;

  void searchCoreference(const std::string text);

  //bool checkCoreference(const Token& tok, CoreferenceEngine ref) const;
  void storeAllToken(const LinguisticAnalysisStructure::Token* tok);
/*  void storeSpecificEntity (const Lima::LinguisticProcessing::
        SpecificEntities::SpecificEntityAnnotation * se) const; */

private:
  
  std::vector< std::vector<std::string> > Acronyms;  // the vector of acronyms
  std::vector<LinguisticAnalysisStructure::Token> storedAnnotations;
  std::vector<LinguisticAnalysisStructure::Token> allToken;
  std::vector< std::vector<LinguisticAnalysisStructure::Token> > findedToken;    /* contient une structure de toutes les coréferences */
  
//   LinguisticGraphVertex m_head;
//   Common::MediaticData::EntityType m_type;    /**< the type of the entity */
//   Automaton::EntityFeatures m_features;
//   StringsPoolIndex m_string;
//   StringsPoolIndex m_normalizedString;
//   StringsPoolIndex m_normalizedForm;
//   uint64_t m_position;
//   uint64_t m_length;

  //  Linguistic properties and normalized form are given by the normalized
  //  form of the vertex in the morphological graph
  //
  //  LinguisticCode m_linguisticProperties; /**< associated ling prop */
  //  StringsPoolIndex m_normalizedForm; /**< the normalized form of the
  //                                        recognized entity*/

};

} // SpecificEntities
} // LinguisticProcessing
} // Lima

#endif // ENTITYTRACKER_H
