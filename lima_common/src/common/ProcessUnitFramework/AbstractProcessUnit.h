// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
