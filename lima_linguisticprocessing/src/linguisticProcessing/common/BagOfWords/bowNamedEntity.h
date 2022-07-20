// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     bowNamedEntity.h
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 * @brief
 *
 ***********************************************************************/

#ifndef BOWNAMEDENTITY_H
#define BOWNAMEDENTITY_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowComplexToken.h"
#include "common/MediaticData/EntityType.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWNamedEntityPrivate;
/**
  * This is a complex token used to represent a named entity token
  * 
  * @author Gael de Chalendar
  */
class LIMA_BOW_EXPORT BoWNamedEntity : public BoWComplexToken
{
  friend class BoWNamedEntityPrivate;
public:
  BoWNamedEntity();
  BoWNamedEntity(const BoWNamedEntity&);

  BoWNamedEntity(const Lima::LimaString& lemma,
                 const Common::MediaticData::EntityType theType,
                 const uint64_t position,
                 const uint64_t length);

  BoWNamedEntity(const Lima::LimaString& lemma,
                 const LinguisticCode theCategory,
                 const Common::MediaticData::EntityType theType,
                 const uint64_t position,
                 const uint64_t length);

  virtual ~BoWNamedEntity();

  virtual BoWNamedEntity* clone() const override;
//   virtual BoWNamedEntity* clone(const std::map<BoWToken*,BoWToken*>&) const;

  BoWNamedEntity& operator=(const BoWNamedEntity&);
  bool operator==(const BoWNamedEntity&);

  Common::MediaticData::EntityType getNamedEntityType(void) const;
  void setNamedEntityType(const Common::MediaticData::EntityType&);

  const std::map<std::string, LimaString>& getFeatures() const; 
  void setFeature(const std::string& attribute,
                  const LimaString& value); 
  
  virtual BoWType getType() const override { return BoWType::BOW_NAMEDENTITY; }

  /**
   * get a string of the features, of the kind :
   * attribute1=value1;attribute2=value2
   */
  std::string getFeaturesUTF8String(void) const;
  /** get a string of the BoWToken for output function */
  virtual std::string getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager = 0) const override;
  virtual std::string getIdUTF8String(void) const override;
  
  /**
   * returns a singleton made of the value of the @ref getVertex function.
   * 
   * vertices of the named entity parts are not known as they can be generated 
   * either from before PoS graph or from PoS graph. In the former case, there 
   * is no vertices in the PoS graph that can be associated to the NE parts 
   * vertices
   *
   * @return a singleton made of the value of the @ref getVertex function
   */
  virtual std::set< uint64_t > getVertices() const override
  {
    return BoWToken::getVertices();
  }
  
protected:
    BoWNamedEntity(BoWNamedEntityPrivate&);
};


} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
