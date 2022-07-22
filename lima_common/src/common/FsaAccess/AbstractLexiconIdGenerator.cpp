// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
