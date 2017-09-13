/*
    Copyright 2017 CEA LIST

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
 * \file    NamedEntitiesParser.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "NamedEntitiesParser.h"
#include "ConllParser.h"
#include "LimaGuiCommon.h"
#include <common/LimaCommon.h>

namespace Lima 
{
namespace Gui 
{


/// \brief Creates a html tag with subsequent content, metadata and style
std::string markupa(const std::string& content, 
                    const std::string& markup, 
                    const std::string& style, 
                    const std::string& metadata) 
{
  return "<" + markup  + (style.length() ? " style=\"" + style + "\"" : "") 
          + " " + metadata + ">" + content + "</" + markup + ">";
}

/// \brief replace all occurences of X by Y in text
void replace_all(std::string& text, 
                 const std::string& occurence, 
                 const std::string& sub) {
  std::size_t it = -1;
  while (it = text.find(occurence, it + 1), it != std::string::npos) {

    text.replace(it, occurence.length(), sub);
    it += sub.length();
  }
}

/// \brief generates a random integer between a and b
int randint(int a, int b) 
{
  return rand()%(b-a)+a;
}

/// \brief Generates X distinct colors in hexadecimal format (#ABCDEF)
///
/// The range is 8 - F, to have brighter colors
std::vector<std::string> generateDistinctColors(int quantity) 
{

  srand(time(NULL));

  std::vector<std::string> colors;

  for (int i=0; i<quantity; i++) 
  {
      std::string str = "#";

      for (int i=0;i<6;i++) 
      {
        int x = randint(8,15);
        str += (x > 9 ? 'A' - 9:'0') + x;
      }

      colors.push_back(str);
  }

  return colors;
}

///////////////////////////////////////////////////////////////

NamedEntitiesParser::NamedEntitiesParser(QObject* p) : QObject(p)
{

}

void NamedEntitiesParser::parse(const QString& conllText)
{
  m_entities.clear();

  this->m_conllText = conllText;

  auto data = getNamedEntitiesFromConll(conllText.toStdString());
  std::vector<std::string> colors = generateDistinctColors(data.size());
  int i = 0;
  for (auto& pair : data) 
  {
    m_entities.push_back(EntityItem(pair.first, colors[i++], pair.second));
  }
}

QStringList NamedEntitiesParser::getEntityTypes() 
{
  QStringList qsl;
  for (auto& item : m_entities) 
  {
    std::string str = item.m_name + ":" + item.m_color;
    qsl << QString(str.c_str());
  }
  return qsl;
}

EntityItem* NamedEntitiesParser::findEntity(const std::string& name) 
{
  for (auto& entity : m_entities) 
  {
    if (entity.m_name == name) 
    {
      return &entity;
    }
  }
  return nullptr;
}

QString NamedEntitiesParser::getHighlightedText() 
{
  CONLL_List conllList = conllRawToLines(m_conllText.toStdString());

  std::string result = "";
  std::string block = "";

  int i = 0;
  int j = 0;

  for (auto& line : conllList) 
  {
    j = std::stoi(line->at(0));

    if (j - i > 0) {

      std::string hltext = line->at(1);
      EntityItem* entity = nullptr;
      
      if (line->at(5) != "_") 
      {
        entity = findEntity(line->at(5));
      }

      if (entity) 
      {
        std::string style = "";
        style += "background-color: " + entity->m_color + "; ";
        style += "border: 1px solid black; ";
        style += "padding:5px; ";
        style += "border-radius: 5px; ";
        hltext = markupa(hltext, "mark",style , 
                         "name=\"" + entity->m_name + "\"");
      }

      if (line->at(3) != "PONCTU") 
      {
        block += " ";
      }

      block += hltext;

      i = j;
    }
    else 
    {
      std::string style = "";
      result += markupa(block,"p",style + "display:block;","") + "";
      block = "";

      i = j = 0;
    }
  }
  if (!block.empty())
  {
    std::string style = "";
    result += markupa(block,"p",style + "display:block;","") + "";
  }
  freeConllList(conllList);

  return QString::fromUtf8(result.c_str());
}


} // Gui
} // Lima
