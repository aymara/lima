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
/**
 * @file               S2Commons.h
 * @date               Created on  : Thu May 06 200
 * @author             Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

 *                     Benoit Mathieu <mathieub@zoe.cea.fr> 

 *                     Copyright (c) 2003-2012 by CEA LIST
 * @version            $Id$
 */

#ifndef LIMA_FSA_EXCEPTIONS_H
#define LIMA_FSA_EXCEPTIONS_H

#include "common/LimaCommon.h"


namespace Lima {
namespace Common {
namespace FsaAccess {

class LIMA_FSAACCESS_EXPORT FsaNotSaved : public LimaException {
public :
  FsaNotSaved(const std::string& reason) : LimaException(reason) {};
  virtual ~FsaNotSaved() throw() {};
};


} // namespace FsaAccess
} // namespace Common
} // namespace Lima


#endif // LIMA_FSA_EXCEPTIONS_H
