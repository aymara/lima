// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
    LERROR << "This annotation is not a GeoEntityAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}


} // GeoEntities
} // LinguisticProcessing
} // Lima
