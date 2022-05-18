// Copyright 2004-2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

bool EasySourceHandler::startElement(const QString & , const QString & localname, const QString & qname, const QXmlAttributes & attrs)
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

bool EasySourceHandler::endElement(const QString &, const QString & localname, const QString & qname)
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
