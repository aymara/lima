// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
  string encoding(argv[2]);
  
  string content;
  ifstream fin(argv[1], std::ifstream::binary);
  readStream(fin,content);
  
  string output;
  convertToUtf8(content,encoding,output);
  
  cout << output;
  
}
