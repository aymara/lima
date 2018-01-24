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
//
// C++ Implementation : AntResourcesIdent
//
// Description: analyse les identifiants d'une ressource binaire Ant'inno
//
// Author: Jean-Yves Sage <jean-yves.sage@antinno.fr>, (C) 2010-2011
//
// Copyright: See COPYING file that comes with this distribution
//
////////////////////////////////////////////////////////////

#ifndef RESOURCESIDENT_H
#define RESOURCESIDENT_H

#include <sstream>
#include <time.h>
#include <string>

#define RESOURCESIDENT_STRING "RID"

namespace Lima {
namespace Common {
namespace Misc {
  
//brief This class extracts identifiers from binary resources files

class ResourcesIdent
{
public:
  //------------------------------------------------------------------------------------------------------------------------------
  //param header header in memory
  //param headerSize header size (for check)
  ResourcesIdent(const char *header, const ::std::size_t headerSize)
    : _pHeader(header), _pHeaderSize(headerSize)
  {
  }
  //------------------------------------------------------------------------------------------------------------------------------
  //return string ready to display   */ 
  ::std::string toHumanReadableString()
  {
	  unsigned char *currentPtr = (unsigned char*)_pHeader; 
	  //UNSIGNED indispensable pour calculer les valeurs des entiers
	  ::std::ostringstream resultoss;
    //lit les noms d'identifiants
    const ::std::size_t namesSize = _readInt4LE(currentPtr);
    const ::std::string names = ::std::string((char*)currentPtr, namesSize);
	  currentPtr += namesSize;
    //lit les valeurs

    const ::std::size_t valuesNb = _readInt4LE(currentPtr) / 4;
    ::std::size_t ptrb = 0;
    for (::std::size_t i=0; i<valuesNb; i++) {
		if (ptrb == ::std::string::npos) {
			resultoss<<"INVALIDFILE A"<<std::endl;
			break;   //plus de valeurs que de noms
		}
    const ::std::size_t value = _readInt4LE(currentPtr);
    ::std::size_t ptre = names.find(",", ptrb);
    ::std::string oneName = names.substr(ptrb, ptre-ptrb);
		oneName.resize(20, ' ');
		resultoss<<oneName<<_asciiDate(value)<<"  ("<<value<<")"<<std::endl;
        ptrb = ptre;
        if (ptrb != ::std::string::npos) ptrb++;       //saute la virgule
    }
    if (ptrb != ::std::string::npos) resultoss<<"INVALIDFILE B"<<std::endl;   //plus de noms que de valeurs
	  if ((char*)currentPtr != _pHeader + _pHeaderSize) resultoss<<"INVALIDFILE C"<<std::endl;
	  return resultoss.str();
  }
  //------------------------------------------------------------------------------------------------------------------------------

private:

  //------------------------------------------------------------------------------------------------------------------------------
  const char *_pHeader;
  const ::std::size_t _pHeaderSize;
  //------------------------------------------------------------------------------------------------------------------------------
  ::std::size_t _readInt4LE(unsigned char*& ptr)
  {
	  //on n'utilise pas les ptr++ pour eviter les optimisations du compilateur
	  const ::std::size_t value = ptr[0] + ptr[1]*0x100 + ptr[2]*0x10000 + ptr[3]*0x1000000;
	  ptr +=4;
    return value;
  }
  //------------------------------------------------------------------------------------------------------------------------------
  ::std::string _asciiDate(const time_t date)
  {
      const ::std::string ascDate(ctime(&date));
      const ::std::string::size_type nowStop = ascDate.rfind(' ');
      return ascDate.substr(0,nowStop+5);
  }
  //------------------------------------------------------------------------------------------------------------------------------
}; // end class

} // namespace Misc
} // namespace Commmon
} // namespace Lima

#endif
