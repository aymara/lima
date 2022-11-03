// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * 
 * @file       timeUtilsController.h
 * @author     Olivier Mesnard (olivier.mesnard@cea.fr)
 * @date       Mon Dec  12 2012
 * @version    $Id: timeUtilsController.h,v 1.3 2005/10/20 11:37:31 gael Exp $
 * copyright   Copyright (C) 2004-2012 by CEA LIST
 * Project     Lima common
 *
 * @brief      contains functions to trace some infos for debug
 *
 *
 ***********************************************************************/

#ifndef LIMA_TIMEUTILSCONTROLLER_H
#define LIMA_TIMEUTILSCONTROLLER_H

#include <string>

#include "common/LimaCommon.h"
#include "common/time/traceUtils.h"

namespace Lima {
  
  /**
   * This file contains a class to control log of informations about time,
   * such as logging cumulated time for a specific function. It uses internaly the
   * @ref TimeUtils class functions and static data.
   *
   * How to use it:
   *  - at the beginning of the function/method f you want count the time it takes, declare a local variable instance i of this class with a uniq name and (optionaly) a boolean indicating if you want to log out the lifetime of the object i, thus the time spent in the function f.
   *  - at the end of your program main function, call the TimeUtils::logAllCumulatedTime static function.
   *
   * @note To log the time passed in the main function itself, you must declare a pointer to an object of this class, initialize it in the heap with the new operator and call delete on this object before logging the accumulated time (see hereabove)
   *
   * @note This will not work as expected with recursive calls.
   * @note This will not work as expected in a multithreaded environment
   *
   */
  class LIMA_TIME_EXPORT TimeUtilsController
  {
  public:
    TimeUtilsController(const std::string& topic, bool logElapsedTime=false);
    ~TimeUtilsController();
    
  private:
    std::string m_topic;
    bool m_logElapsedTime;
  };
  
} // end namespace

#endif  // LIMA_TIMEUTILSCONTROLLER_H
