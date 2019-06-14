/*
    Copyright 2002-2019 CEA LIST

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
