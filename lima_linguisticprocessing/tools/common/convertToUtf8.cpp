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

#include <QtCore/QTextCodec>
#include <QtCore/QCoreApplication>

using namespace std;
using namespace Lima::Common::Misc;

void convertToUtf8(const std::string& inputStr,const std::string& srcEncoding,std::string& outputStr)
{
  QByteArray encodedString(inputStr.c_str());
  QTextCodec *codec = QTextCodec::codecForName(srcEncoding.c_str());
  QString string = codec->toUnicode(encodedString);
  outputStr = string.toUtf8().data();
}


int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  string encoding(argv[2]);
  
  string content;
  ifstream fin(argv[1], std::ifstream::binary);
  readStream(fin,content);
  
  string output;
  convertToUtf8(content,encoding,output);
  
  cout << output;
  
}
