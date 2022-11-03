// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
  *
  * @file        escaper.h
  * @author      Victor Bocharov (victor.bocharov@cea.fr)

  *              Copyright (c) 2019 by CEA
  * @date        Created on Feb, 15 2019
  * @version     $Id$
  *
  */

#ifndef LIMA_COMMON_MISC_ESCAPER_H
#define LIMA_COMMON_MISC_ESCAPER_H

#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

namespace Lima {

struct character_escaper
{
    template<typename FindResultT>
    std::string operator()(const FindResultT& Match) const
    {
        std::string s;
        for (typename FindResultT::const_iterator i = Match.begin();
             i != Match.end();
             i++) {
            s += str(boost::format("\\x%02x") % static_cast<int>(*i));
        }
        return s;
    }
};

}

#endif
