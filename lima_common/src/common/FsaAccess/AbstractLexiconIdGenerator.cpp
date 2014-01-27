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

#include "common/LimaCommon.h"
#include "AbstractLexiconIdGenerator.h"

namespace Lima {
namespace Common {

// default behavior of functor
int AbstractLexiconIdGeneratorInformer::operator() (
 uint64_t /* unused cmp1 */, uint64_t /* unused cmp2 */,
 uint16_t /* unused type */, uint64_t /* unused newId */)
{ 
  return 0;
}

AbstractLexiconIdGeneratorInformer* AbstractLexiconIdGeneratorInformer::theInformer = 0;

AbstractLexiconIdGeneratorInformer* AbstractLexiconIdGeneratorInformer::getInstance() {
  if( theInformer == 0 ) {
    theInformer = new AbstractLexiconIdGeneratorInformer();
  }
  return theInformer;
}
int16_t AbstractLexiconIdGeneratorInformer::LINK_TYPE_DEP = 1;
int16_t AbstractLexiconIdGeneratorInformer::LINK_TYPE_EXT = 2;

} // namespace Common
} // namespace Lima
