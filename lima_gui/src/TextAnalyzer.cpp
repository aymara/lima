/// This sould include LIMA at some point

#include "TextAnalyzer.h"
#include "ConllParser.h"

#include <iostream>

void TextAnalyzer::analyzeText() {
  std::cout << m_text.toStdString() << std::endl;
}

void TextAnalyzer::analyzeFile() {
  std::string filepath = split(m_file.toStdString(),':')[1];
  CONLL_List data = ConllParser::getConllData(filepath);
  ConllParser::show_dependencies(data);
}

/////////////// EXPERIMENTS
/////////////// 
/////////////// 
/////////////// 

void TextAnalyzer::tr_analyzeFile() {
  std::string filepath = m_file.toStdString();
}

