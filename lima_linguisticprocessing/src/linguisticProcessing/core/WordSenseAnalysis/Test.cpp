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
#include "Test.h"

int main(int argc,char* argv[])
  boost::regex numre("^\\d+$");
  
  std::string lineString;
  size_t linenum(0);
  getline(ifl, lineString);
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
