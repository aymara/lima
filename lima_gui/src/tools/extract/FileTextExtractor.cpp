/**
 * \file    FileTextExtractor.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "FileTextExtractor.h"

#include <algorithm>
#include <iostream>
#include <cstring>

#include <QFile>
#include <QString>
#include <QTextStream>

namespace Lima 
{
namespace Gui 
{
namespace Tools 
{

std::string extractTextFromFile(const std::string &path, const std::string &extension) 
{
  std::vector<std::string> handledTypes = {"txt"};

  bool handled = (std::find(handledTypes.begin(), handledTypes.end(), extension) != handledTypes.end());
  if (!extension.size() || extension == "txt" || handled) 
  {
    QString tump;
    QFile file(QString(path.c_str()));
    if (file.open(QFile::ReadOnly)) {
      QTextStream qts(&file);
      tump = qts.readAll();
      file.close();
    }
    else 
    {
      std::cout << "didn't open : " << path << std::endl;
      std::cout << "Error opening file: " << std::strerror(errno) << std::endl;
    }
    return tump.toStdString();
  }
  else if(handled) 
  {
    // put your formats here
    if (extension == "pdf") {
      std::cout << extension << ": can't handle this file extension" << std::endl;
    }
    else if (extension == "docx") {
      std::cout << extension << ": can't handle this file extension" << std::endl;
    }
  }
}

} // Tools
} // Gui
} // Lima
