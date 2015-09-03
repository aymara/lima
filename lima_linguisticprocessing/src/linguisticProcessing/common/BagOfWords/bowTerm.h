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
 * @file     bowTerm.h
 * @author   Besancon Romaric
 * @date     Tue Oct  7 2003
 * copyright Copyright (C) 2003 by CEA LIST
 *
 * @brief
 * 
 ***********************************************************************/

#ifndef BOWTERM_H
#define BOWTERM_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "bowToken.h"
#include "bowComplexToken.h"
#include <deque>

namespace Lima {
namespace Common {
namespace BagOfWords {

class BoWTermPrivate;
  
/**
  * This is a complex token used to represent a multiword term
  *
  * @author Gael de Chalendar
  */
class LIMA_BOW_EXPORT BoWTerm : public BoWComplexToken
{
 public:
  BoWTerm();
  BoWTerm(const BoWTerm&);

  BoWTerm(const Lima::LimaString& lemma,
          const uint64_t category, 
          const uint64_t position,
          const uint64_t length);

  BoWTerm(const Lima::LimaString& lemma, const uint64_t category, const uint64_t position, const uint64_t length, std::deque< boost::shared_ptr< Lima::Common::BagOfWords::BoWToken > >& parts, const uint64_t head);

  virtual ~BoWTerm();

  virtual BoWTerm* clone() const;
//   virtual BoWTerm* clone(const std::map<BoWToken*,BoWToken*>&) const;

  BoWTerm& operator=(const BoWTerm&);

  virtual BoWType getType() const { return BOW_TERM; }

  /** get a string of the BoWToken for output function */
  virtual std::string getOutputUTF8String(const Common::PropertyCode::PropertyManager* macroManager = 0) const;
  virtual std::string getIdUTF8String(void) const;

protected:
  BoWTerm(BoWTermPrivate&);
};
//**************************************************************
// inline functions
//**************************************************************
// inline BoWTerm* BoWTerm::clone() const 
// { 
//   return new BoWTerm(*this); 
// }
// inline BoWTerm* BoWTerm::clone(const std::map<BoWToken*,BoWToken*>& map) const 
// {
//   return new BoWTerm(*this,map);
// }

LIMA_BOW_EXPORT std::ostream& operator<<(std::ostream& os, const BoWTerm& t);

} // namespace BagOfWords
} // namespace Common
} // namespace Lima

#endif
