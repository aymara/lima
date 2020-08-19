/*
    Copyright 2002-2020 CEA LIST

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
 *   Copyright (C) 2004-2020 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_PROCESSUNITFRAMEWORK_ABSTRACTPROCESSUNIT_H
#define LIMA_COMMON_PROCESSUNITFRAMEWORK_ABSTRACTPROCESSUNIT_H

#include "common/LimaCommon.h"
#include "common/AbstractFactoryPattern/InitializableObject.h"

namespace Lima
{

class AnalysisContent;
/** @brief      Abstraction of a processUnit
  *
  * @file       AbstractProcessUnit.h
  * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
  *             Copyright (c) 2004 by CEA
  * @date       Created on May 2004
  * @version    $Id$
  *
  */
template <typename Base,typename InitializationParameters = NoParameters>
class AbstractProcessUnit : public InitializableObject<Base,InitializationParameters>
{
public:

  /**
    * @brief Process on data in analysisContent.
    * This method should not return any exception but UndefinedMethod
    * @param analysis AnalysisContent object on which to process
    */
  virtual LimaStatusCode process(
    AnalysisContent& analysis) const = 0;

};


} // Lima

#endif
