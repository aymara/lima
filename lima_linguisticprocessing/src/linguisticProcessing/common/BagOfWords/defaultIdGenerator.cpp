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
 *
 * @file       defaultIdGenerator.cpp
 * @author     Besancon Romaric (romaric.besancon@cea.fr)
 * @date       Tue Feb  7 2006
 * copyright   Copyright (C) 2006-2012 by CEA LIST
 * 
 ***********************************************************************/

#include "defaultIdGenerator.h"

namespace Lima {
namespace Common {
namespace BagOfWords {

DefaultIdGenerator::
DefaultIdGenerator(AbstractLexiconIdGeneratorInformer* informer, const uint64_t firstId)
 : AbstractLexiconIdGenerator(informer), m_firstId(firstId), m_lastId(firstId),
   m_simpleTermCount(0), m_compoundTermCount(0)  
{
}

DefaultIdGenerator::~DefaultIdGenerator() 
{
}

} // end namespace
} // end namespace
} // end namespace
