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

  virtual BoWNamedEntity* clone() const;
//   virtual BoWNamedEntity* clone(const std::map<BoWToken*,BoWToken*>&) const;

  BoWNamedEntity& operator=(const BoWNamedEntity&);
  bool operator==(const BoWNamedEntity&);

  Common::MediaticData::EntityType getNamedEntityType(void) const;
  void setNamedEntityType(const Common::MediaticData::EntityType&);

  const std::map<std::string, LimaString>& getFeatures() const; 
  void setFeature(const std::string& attribute,
                  const LimaString& value); 
  
  virtual BoWType getType() const { return BoWType::BOW_NAMEDENTITY; }

  /**
   * get a string of the features, of the kind :
   * attribute1=value1;attribute2=value2
   */
  std::string getFeaturesUTF8String(void) const;
  /** get a string of the BoWToken for output function */
  virtual std::string getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager = 0) const;
  virtual std::string getIdUTF8String(void) const;
  
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
  virtual std::set< uint64_t > getVertices() const
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
