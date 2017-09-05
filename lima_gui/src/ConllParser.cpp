/**
 * \file    ConllParser.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "ConllParser.h"

#include "common/LimaCommon.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <array>
#include <cstdio>
#include <iomanip>

#include <boost/algorithm/string/replace.hpp>

#define CONLLPARSERLOGINIT LOGINIT("Lima::Gui::ConllParser");

namespace Lima 
{
namespace Gui 
{


///
/// Contains all kinds of function to parse text and files

CONLL_Line::CONLL_Line(const std::string& str) 
{
  raw = str;
  tokens = split(str, '\t');
}

const std::string& CONLL_Line::operator[](unsigned int i) const 
{
  if (i < tokens.size()) 
  {
    return tokens[i];
  }
  else 
  {
    return "";
  }
}

void freeConllList(CONLL_List& cl) 
{
  for (unsigned int i=0;i<cl.size();i++) 
  {
    delete(cl[i]);
  }
}

/// splits a string according to the parameter delimiter
/// \return the list of the resulting tokens
std::vector<std::string> split(const std::string& line, const char& delimiter) 
{
  std::vector<std::string> elements;
  std::stringstream ss(line);
  std::string word;
  while (std::getline(ss,word,delimiter)) {
    elements.push_back(word);
  }
  return elements;
}

/// parse a file and return its content
std::vector<std::string> parseFile(const std::string& filepath) 
{
  std::ifstream myfile;
  myfile.open(filepath.c_str(), std::ifstream::in);
  std::vector<std::string> content;
  std::string line;
  while (!myfile.eof() && std::getline(myfile,line)) {
    content.push_back(line);
  }
  myfile.close();
  return content;
}

std::vector<CONLL_Line*> getConllData(const std::string& filepath) 
{
  return textToConll(parse_conll(filepath));
}

std::vector<CONLL_Line*> textToConll(const std::string& text) 
{
  std::vector<CONLL_Line*> conll_lines;

  // retrieve the file's content
  std::vector<std::string> data = into_lines(text);

  // create structures
  for (unsigned int i=0; i<data.size(); i++) 
  {
      if (!data[i].empty())
      conll_lines.push_back(new CONLL_Line(data[i]));
  }

  return conll_lines;
}

/// removes the unneeded text at the start of the output
void clean_up(std::string& str) 
{
    str.erase(0,str.find('\'')+1);
    str.erase(0,str.find('\'')+1);
}

/// runs LIMA's analyzeText on parameter file and returns the output
/// \return the raw text content of the file
std::string parse_conll(const std::string& file) 
{
    std::string unit = "analyzeText";
    std::string unit_path = "~/Lima/Dist/lima-gui/debug/bin/";
//     chdir(unit_path.c_str());
    std::string cmd = unit + " -l fre " + file;
    //cmd = "cat " + file;
    std::array<char, 128> buffer;
    std::string result;
    
    // open pipe to run another process
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe)
    {
      throw std::runtime_error("Couldn't open a pipe.");
    }
    
    while (!feof(pipe)) 
    {
        if (fgets(buffer.data(), 128, pipe) != NULL) 
        {
            result += buffer.data();
        }
    }
    
    int return_value = pclose(pipe);
    if (return_value) 
    {
      CONLLPARSERLOGINIT;
      LINFO << "pipe returned " << return_value;
    }
    
    clean_up(result);
    return result;
}

/// gets lines from raw text
/// \return the list of the resulting lines
std::vector<std::string> into_lines(const std::string& str) 
{
    std::stringstream ss(str);
    std::string line;
    std::vector<std::string> lines;
    
    while (std::getline(ss,line,'\n')) {
        lines.push_back(line);
    }
    
    return lines;
}

/// \brief Displays the conll file content
void displayAsColumns(const CONLL_List& lines) 
{
    
    std::vector<std::string> headers;
    headers.push_back("token_num");
    headers.push_back("token");
    headers.push_back("norm_token");
    headers.push_back("token_type");
    headers.push_back("token_what");
    headers.push_back("named_entity");
    headers.push_back("?");
    headers.push_back("target_token");
    headers.push_back("relationship");
    
    int offset = 20;
    for (unsigned int i=0;i<headers.size();i++) 
    {
        std::cout << std::setw(offset) << headers[i];
    }
    std::cout << std::endl;
    for (unsigned int i=0;i<lines.size();i++) for (int j=0;j<offset;j++) std::cout << "-";
    std::cout << std::endl;
    for (unsigned int i=0;i<lines.size();i++) 
    {
        for (unsigned int j=0;j<lines[i]->tokens.size();j++) 
        {
            std::cout << std::setw(offset) << (lines[i]->tokens[j] != "_"?lines[i]->tokens[j]:" ");
        }
        std::cout << std::endl;
    }
}

/// \brief This will extract named entities from the conll output
std::map<std::string, std::vector<std::string> > getNamedEntitiesFromConll(const std::string& text) 
{
  std::map<std::string, std::vector<std::string> > disa;
  CONLL_List content = textToConll(text);
  for (auto & cl : content) 
  {
    CONLL_Line& line = *cl;
    if (cl && line[5] != "_") 
    {
      disa[line[5]].push_back(line[1]);
    }
  }

  freeConllList(content);
  return disa;
}

/// \brief creates a html markup with content and style
std::string markup(const std::string& content, const std::string& markup, const std::string& style) 
{
  return "<" + markup  + (style.length() ? " style=\"" + style + "\"" : "") + ">" + content + "</" + markup + ">";
}

/// \return the text with html highlighted named entities
/// \param text : the raw text
/// \param types is the map obtained by the above function
/// \param is the colors : <named_entity_type_name>:<color>
std::string highlightNamedEntities(
    const std::string& raw,
    std::map<std::string, std::vector<std::string>>& types,
    std::map<std::string, std::string>& colors)
{
  std::string text = raw;
  for (auto& type : types)
  {
    for (auto& entity : type.second) 
    {
      boost::replace_all(text, entity, markup(entity,"strong","color:"+colors[type.first]));
    }
  }

  return text;
}

/// \brief takes a raw conll formated text and splits it into CONLL_Line structures
CONLL_List conllRawToLines(const std::string& conll)
{
  CONLL_List list;

  std::vector<std::string> data = into_lines(conll);

  for (auto& line : data) 
  {
    if (line.length()) list.push_back(new CONLL_Line(line));
  }

  return list;
}

} // End namespace Gui
} // End namespace Lima
