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


#include "common/Data/strwstrtools.h"
#include <iostream>
#include <fstream>
#include <stdlib.h>

#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima::Common::Misc;

int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  string encoding(argv[2]);
  
  string configDir(getenv("LIMA_CONF"));
  
  if( argc < 3 ) {
    std::cerr << "troncature d'une chaine" << std::endl;
    std::cerr << "usage: " << argv[0]
                           << " src_filename"
                           << " dst_filename"
                           << " max_size" << std::endl;
    std::cerr << " src8_filename: fichier contenant la chaine source en utf-8"
              << " dst8_filename: fichier contenant la chaine destination en utf-8"
              << " max_size:  taille max de la chaine source";
  }
  string phrase_origin_utf8;
  ifstream fin(argv[1], std::ifstream::binary);
  readStream(fin,phrase_origin_utf8);
  
  std::cerr << argv[0] << " " << phrase_origin_utf8 << std::endl;
  
  Lima::LimaString phrase_utf16 = Lima::Common::Misc::utf8stdstring2limastring(phrase_origin_utf8);
  
  std::cerr << "phrase_utf16 = " << Lima::Common::Misc::limastring2utf8stdstring(phrase_utf16) << std::endl;
  
  string phrase_dest_utf8;
  uint64_t max_size = atoi(argv[3]);
  phrase_dest_utf8 = limastring2utf8stdstring( phrase_utf16,  max_size );
  
  ofstream fout(argv[2], std::ofstream::binary);
  fout << phrase_dest_utf8;
  
}
