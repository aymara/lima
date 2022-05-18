// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     BoWRelation.h
 * @author   Gael de Chalendar
 * @date     Tue Oct  31 18:48:26 2006
 * copyright Copyright (C) 2006-2012 by CEA LIST
 *
 * @brief A relation associated used to type the relation between a part
 *        and its enclosing complex token. This will be used primarily to
 *        represent the type of the relation in compounds but could be 
 *        later be used as the basis to more structured indexing
 *
 ***********************************************************************/

#ifndef BOWRELATION_H
#define BOWRELATION_H

#include "common/LimaCommon.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Data/LimaString.h"
#include <iostream>
#include <string>
#include <stdexcept>

namespace Lima {
namespace Common {
namespace BagOfWords {

/**
 * the type of the BoWRelation is a numeric value currently (11/2006) taken 
 * from the ConceptType enum but that could be extended to be any property
 * id from an ontology
 */
class BoWRelation;
LIMA_BOW_EXPORT std::ostream& operator << (std::ostream&, const BoWRelation&);
LIMA_BOW_EXPORT QDebug& operator << (QDebug&, const BoWRelation&);

/**
 * @brief This class is used to type the relation between a part
 *        and its enclosing complex token. This will be used primarily to
 *        represent the type of the relation in compounds but could be 
 *        later be used as the basis to more structured indexing
 *
 **/
class LIMA_BOW_EXPORT BoWRelation
{
public:

  BoWRelation(const Lima::LimaString& realization,
           uint64_t type);
  BoWRelation(const Lima::LimaString& realization,
           uint64_t type, uint64_t syntype);
  BoWRelation(const BoWRelation& tok);
  BoWRelation();

  virtual ~BoWRelation();

  virtual BoWRelation* clone() const;

  virtual Lima::LimaString
      getRealization(void) const {return m_realization;};
  uint64_t getType(void)   const {return m_type;};
  uint64_t getSynType(void)   const {return s_type;};

//   void setRealization(const Lima::LimaString& realization)  
//     {m_realization = realization;};
  void setType(uint64_t type)  {m_type = type;};
  void setSynType(uint64_t type)  {s_type = type;};

  Lima::LimaString getString(void) const;

  static void setUseOnlyRealization(const bool b) { m_useOnlyRealization=b; }
  static bool getUseOnlyRealization() { return m_useOnlyRealization; }

  static void setCompoundSeparator(const LimaChar c);

  static LimaChar getCompoundSeparator();

  //@{ binary input/output
  virtual void read(std::istream& file);
  virtual void write(std::ostream& file) const;
  //@}

  /** get a string of the BoWRelation for output function */
//  virtual std::wstring getOutputString(void) const;
  virtual std::string getOutputUTF8String(void) const;

  /** function used to identify a bowtoken in a text (used in BowDumper) */
//  virtual std::wstring getIdString() const;
  virtual std::string getIdUTF8String() const;

//  friend std::wostream& operator << (std::wostream&, const BoWRelation&);
  friend LIMA_BOW_EXPORT std::ostream& operator << (std::ostream&, const BoWRelation&);
  friend LIMA_BOW_EXPORT QDebug& operator << (QDebug&, const BoWRelation&);
  
  /**
   * for parsing of input stream
   *
   */
  virtual void parse(const Lima::LimaString& str,
                     int current);

  /**
    * @brief redefine equality
    * @author Gael de Chalendar
    * return true if :<br>
    *   - objects have the same type<br>
    *   - objects have the same realization<br>
    */
  bool operator==(const BoWRelation&) const;

  /**
    * @brief redefine inequality as negation of equality.
    * @author Benoit Mathieu
    */
  bool operator!=(const BoWRelation&) const;

  static Lima::LimaString
    findNextElement(const Lima::LimaString& str,
                    int& current,
                    const LimaChar& separator);

 protected:
  Lima::LimaString m_realization;
  uint64_t m_type;
// added by Faiza: relation syntactic type
  uint64_t s_type;
  static LimaChar m_separator;
  static LimaChar m_compoundSeparator;

  static bool m_useOnlyRealization;


  /**
   * convert the spaces in the realization string into a non-space character
   * (defined by the m_compoundSeparator member)
   */
  void convertSpaces(void);

};

//**************************************************************
// inline functions
//**************************************************************
inline void BoWRelation::setCompoundSeparator(const LimaChar c)
{
    m_compoundSeparator=c;
}

inline LimaChar BoWRelation::getCompoundSeparator()
{
    return m_compoundSeparator;
}

inline BoWRelation* BoWRelation::clone() const
{
  return new BoWRelation(*this);
}


} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif // BOWRELATION_H
