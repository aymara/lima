
#ifndef ghdghscjenicfhermfuchhmfmaixfxdsqksdogqùjefqojxefoejkg
#define ghdghscjenicfhermfuchhmfmaixfxdsqksdogqùjefqojxefoejkg

/*
Code copié de AntResourcesIdent.h

Sans doute à améliorer

FW 30/10/2013


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
#include <sstream>
#include <time.h>
#include <string>

namespace antinno {
  
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
			resultoss<<"INVALIDFILE A"<<endl;
			break;   //plus de valeurs que de noms
		}
    const ::std::size_t value = _readInt4LE(currentPtr);
    ::std::size_t ptre = names.find(",", ptrb);
    ::std::string oneName = names.substr(ptrb, ptre-ptrb);
		oneName.resize(20, ' ');
		resultoss<<oneName<<_asciiDate(value)<<"  ("<<value<<")"<<endl;
        ptrb = ptre;
        if (ptrb != ::std::string::npos) ptrb++;       //saute la virgule
    }
    if (ptrb != ::std::string::npos) resultoss<<"INVALIDFILE B"<<endl;   //plus de noms que de valeurs
	  if ((char*)currentPtr != _pHeader + _pHeaderSize) resultoss<<"INVALIDFILE C"<<endl;
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

} // end namespace

#endif
