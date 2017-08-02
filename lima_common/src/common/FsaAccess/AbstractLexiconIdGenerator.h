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
 *   Copyright (C) 20036 by  CEA                                           *
 *   author Olivier MESNARD olivier.mesnard@cea.fr                         *
 *                                                                         *
 *  abstractLexiconIdAccessor.h  -  description                            *
 *  AbstractLexiconIdAccessor = interface class                            *
 *  find implementations in LexiconIdAccessor, defaultIdAccessor           *
 ***************************************************************************/
#ifndef FSA_ABSTRACT_LEXICON_ID_GENERATOR_H
#define FSA_ABSTRACT_LEXICON_ID_GENERATOR_H

#include "common/LimaCommon.h"

#include "AbstractLexiconIdAccessor.h"

namespace Lima {
namespace Common {

// abstract functor class usefull to inform client of AbstractLexiconIdGenerator
// of links created by getId(const std::vector<uint64_t>& structure )
class LIMA_FSAACCESS_EXPORT  AbstractLexiconIdGeneratorInformer {
public:
  virtual ~AbstractLexiconIdGeneratorInformer() {}
  virtual int operator() (
    uint64_t cmp1, uint64_t cmp2, uint16_t type, uint64_t newId);
  static AbstractLexiconIdGeneratorInformer* getInstance();
  static int16_t LINK_TYPE_DEP;
  static int16_t LINK_TYPE_EXT;

protected:
  AbstractLexiconIdGeneratorInformer() {};
  static AbstractLexiconIdGeneratorInformer* theInformer;
};

class LIMA_FSAACCESS_EXPORT AbstractLexiconIdGenerator : public AbstractLexiconIdAccessor
{
  public:
    AbstractLexiconIdGenerator(AbstractLexiconIdGeneratorInformer* informer) {
      m_informer = informer;
    }
    virtual ~AbstractLexiconIdGenerator() {}

    // implementation of operation inherited from interface IndexElementIdGenerator
    // rely on abstract operation implemented by concrete IdGenerator (fromMap or from fromFsaAccess)
    virtual uint64_t getId(const LimaString& word) const = 0;
    virtual uint64_t getId(const std::vector<uint64_t>& structure) const = 0;
  protected:
     AbstractLexiconIdGeneratorInformer* m_informer;
};

} // namespace Common
} // namespace Lima

#endif   //FSA_ABSTRACT_LEXICON_ID_GENERATOR_H
