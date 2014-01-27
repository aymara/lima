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
/************************************************************************
 * @file     Events.cpp
 * @author   Olivier MESNARD
 * @date     March 2010
 * @version   $Id:
 * copyright Copyright (C) 2010 by CEA LIST
 * 
 ***********************************************************************/


#include "Events.h"

#include "common/Data/readwritetools.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"


using namespace std;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima
{
namespace LinguisticProcessing
{
namespace EventAnalysis
{ 

void Events::read(std::istream& file)
{
  int size=Common::Misc::readCodedInt(file);
  for (int i=0;i<size;i++)
  {
    Event * e=new Event();
    e->read(file);
    push_back(e);
  }
}

void Events::write(std::ostream& file) const
{
  LOGINIT("LP::EventAnalysis");
  LDEBUG << "Events::write()..."<< LENDL; 
  Common::Misc::writeCodedInt(file,size());
  for(std::vector<Event*>::const_iterator iT=begin(); iT !=end();iT++)
  {
    (*iT)->write(file);
  }
}

std::string Events::toString(std::string parentURI) const
{
  std::cerr << "Events::toString(...)" << std::endl;

  std::string out;
  uint64_t eventindex=1;
  for(std::vector<Event*>::const_iterator iT=begin(); iT !=end();iT++)
  {
    out.append((*iT)->toString(parentURI,eventindex));
    eventindex++;
    std::cerr << out << std::endl;
  }
  return out;
}

} // namespace 
} // namespace
} // namespace Lima
