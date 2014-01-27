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
/***************************************************************************
 *   Copyright (C) 2005 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#include "GeoEntityAnnotation.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"
namespace Lima
{

namespace LinguisticProcessing
{

namespace GeoEntities
{

GeoEntityAnnotation::
GeoEntityAnnotation(PositionType pos,std::set<std::string>cl)
{
  //GEOENTITIESTAGGERLOGINIT;
  m_pos=pos;
  m_classes=cl;
}

GeoEntityAnnotation::~GeoEntityAnnotation()
{
}

void GeoEntityAnnotation::dump(std::ostream& os)
{
  //GEOENTITIESTAGGERLOGINIT;
  os << m_pos << "|";
  for (std::set<std::string>::iterator iT = m_classes.begin(); iT!=m_classes.end(); iT++) os << *iT << " "; 
   
}


int DumpGeoEntityAnnotation::dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const
{
  GEOENTITIESTAGGERLOGINIT;
  try
  {
    ga.value<GeoEntityAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& e)
  {
    LERROR << "This annotation is not a GeoEntityAnnotation ; nothing dumped" << LENDL;
    return UNKNOWN_ERROR;
  }
}


} // GeoEntities
} // LinguisticProcessing
} // Lima
