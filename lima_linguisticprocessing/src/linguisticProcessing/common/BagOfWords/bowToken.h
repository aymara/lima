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
 * @file     bowToken.h
 * @author   Besancon Romaric
 * @date     Tue Oct  7 17:13:26 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 * @brief
 *
 ***********************************************************************/

#ifndef BOWTOKEN_H
#define BOWTOKEN_H

#include "linguisticProcessing/common/BagOfWords/AbstractBoWElement.h"
#include "common/LimaCommon.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/common/misc/positionLengthList.h"
#include <iostream>
#include <string>
#include <stdexcept>
#include <map>
#include <set>
#include <memory>

namespace Lima {
namespace Common {
namespace PropertyCode {
  class PropertyManager;
}
namespace BagOfWords {

class BoWTokenPrivate;
/**
 * This class contains the representation of an element of the bag of words. 
 * This element is characterized by a normalized form, a grammatical category, 
 * its position and length. It can be a simple word, a multiterm or a named 
 * entity.
 **/
class LIMA_BOW_EXPORT BoWToken : public AbstractBoWElement
{
  friend class BoWTokenPrivate;
public:

  BoWToken(const Lima::LimaString& lemma,
           const uint64_t category,
           const uint64_t position,
           const uint64_t length);
  BoWToken(const Lima::LimaString& str,
           const uint64_t position=0,
           const uint64_t length=0);
  BoWToken(const BoWToken& tok);
  BoWToken();

  virtual ~BoWToken();

  virtual void clear() override {}

  virtual BoWToken* clone() const override;
//   virtual BoWToken* clone(const std::map<BoWToken*,BoWToken*>&) const;

  virtual Lima::LimaString getLemma(void) const;
  virtual Lima::LimaString getInflectedForm(void) const;
  LinguisticCode getCategory(void) const;
  uint64_t getPosition(void) const override;
  uint64_t getLength(void) const override;

  /** size of the BoWToken is the number of parts in the token */
  virtual uint64_t size(void) const override;

  void setLemma(const Lima::LimaString& lemma);
  void setInflectedForm(const Lima::LimaString& inflectedForm);
  void setCategory(LinguisticCode c);
  void setPosition(const uint64_t pos) override;
  void setLength(const uint64_t len) override;

  virtual Lima::LimaString getString(void) const override;

  static void setInternalSeparator(const LimaChar c);
  static void setCompoundSeparator(const LimaChar c);

  static LimaChar getInternalSeparator();
  static LimaChar getCompoundSeparator();

  virtual BoWType getType() const override;

  uint64_t getVertex() const;
  void setVertex(uint64_t vertex);

  /**
   * returns a singleton made of the value of the @ref getVertex function.
   * 
   * Subclasses like BoWComplexToken reimplement this method to return the set
   * of vertices of all their parts.
   *
   * @return a singleton made of the value of the @ref getVertex function
   */
  virtual std::set< uint64_t > getVertices() const override;

  /** get a string of the BoWToken for output function
    * @param macroAccessor if not null, will output the human readable form of the macrocategory.
    * otherwise output the integer code
    */
  virtual std::string getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager = 0) const override;

  /** function used to identify a bowtoken in a text (used in BowDumper) */
  virtual std::string getIdUTF8String() const override;

  friend LIMA_BOW_EXPORT std::ostream& operator << (std::ostream&, const BoWToken&);
  friend LIMA_BOW_EXPORT QDebug& operator << (QDebug&, const BoWToken&);
  
  /**
    * @brief redefine equality
    * @author Benoit Mathieu
    * return true if :<br>
    *   - objects have the same BowType<br>
    *   - objects have the same category<br>
    *   - objects have the same lemma<br>
    *   - if objects are of BoWNamedEntity type, objects have the same NamedEntityType.
    */
  bool operator==(const BoWToken&) const;

  /**
    * @brief redefine inequality as negation of equality.
    * @author Benoit Mathieu
    */
  bool operator!=(const BoWToken&) const;


  /**
   * get the list of (position,length) couples of the elements
   * of the token (if several)
   *
   * @return
   */
  virtual Common::Misc::PositionLengthList getPositionLengthList() const;

  /**
   * add the offset to the position of the token
   */
  virtual void addToPosition(const uint64_t offset);

  friend LIMA_BOW_EXPORT std::ostream& Lima::Common::Misc::operator << (std::ostream& os,
                                                    const Common::Misc::PositionLengthList& p);
  friend LIMA_BOW_EXPORT QDebug& Lima::Common::Misc::operator << (QDebug& os,
                                                    const Common::Misc::PositionLengthList& p);
  
  static void setUseOnlyLemma(const bool b);
  static bool getUseOnlyLemma();

  /** only for debug */
  friend LIMA_BOW_EXPORT std::ostream& operator << (std::ostream& os,
                                                    const std::map<BoWToken*,uint64_t>& refMap);
  friend LIMA_BOW_EXPORT QDebug& operator << (QDebug& os,
                                                    const std::map<BoWToken*,uint64_t>& refMap);
  
protected:
  BoWToken(BoWTokenPrivate& d);
  BoWToken& operator=(const BoWToken& tok);
  bool operator==(const BoWToken& tok);
  BoWTokenPrivate* m_d;
};
//**************************************************************
// exceptions
//**************************************************************
class LIMA_BOW_EXPORT InputErrorException : public std::runtime_error {
 public:
  InputErrorException(const std::string& mes):std::runtime_error(mes){};
};

//**************************************************************
// functions
//**************************************************************
} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
