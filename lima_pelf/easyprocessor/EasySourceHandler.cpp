/*
 *    Copyright 2004-2014 CEA LIST
 * 
 *    This file is part of LIMA.
 * 
 *    LIMA is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU Affero General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 * 
 *    LIMA is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU Affero General Public License for more details.
 * 
 *    You should have received a copy of the GNU Affero General Public License
 *    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
 */
/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#include "EasySourceHandler.h"

#include <iostream>

using namespace std;

namespace EasyProcessor
{

EasySourceHandler::EasySourceHandler() : QXmlDefaultHandler()
{}


EasySourceHandler::~EasySourceHandler()
{}


bool EasySourceHandler::startDocument()
{
  m_enonces.clear();
  m_inEnonce=false;
  return true;
}

bool EasySourceHandler::startElement(const QString & uri, const QString & localname, const QString & qname, const QXmlAttributes & attrs)
{
  string name=getName(localname,qname);
  if (name=="E")
  {
    m_currentEnonce=Enonce();
    m_currentEnonce.id=attrs.value("id").toUtf8().constData();
    m_inEnonce=true;
  }
  return true;
}

bool EasySourceHandler::characters(const QString& chars)
{
  if (m_inEnonce)
  {
    m_currentEnonce.text=chars.toUtf8().constData();
//    QDebug() << "read text : " << m_currentEnonce.text << endl;
  }
  return true;
}

bool EasySourceHandler::endElement(const QString & uri, const QString & localname, const QString & qname)
{
  string name=getName(localname,qname);
  if (name=="E")
  {
    m_inEnonce=false;
    m_enonces.push_back(m_currentEnonce);
  }
  return true;
}

std::string EasySourceHandler::getName(const QString& localName, const QString& qName)
{
  if (localName.isEmpty())
  {
    return qName.toUtf8().constData();
  }
  return localName.toUtf8().constData();
}


}
