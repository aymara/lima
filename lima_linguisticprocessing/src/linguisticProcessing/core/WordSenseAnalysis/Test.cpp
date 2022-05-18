// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "Test.h"

int main(int argc,char* argv[])
  boost::regex numre("^\\d+$");
  
  std::string lineString;
  size_t linenum(0);
  lineString = Lima::Common::Misc::readLine(ifl);
  while (ifl.good() && !ifl.eof())
  {
    Common::Misc::chomp(lineString);
    linenum++;

    LinguisticCode bigram[2];
    std::string sbigram[2];
    float proba;
    

    std::string::const_iterator start, end; 
    start = lineString.begin(); 
    end = lineString.end(); 
    boost::match_results<std::string::const_iterator> what; 
//    boost::match_flag_type flags = boost::match_default; 
    regex_search(start, end, what, linere); 
    
    return 0;
  }
