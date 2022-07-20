// Copyright 2004-2014 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by Benoit Mathieu                                  *
 *   mathieub@zoe.cea.fr                                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef EASYPROCESSOREASYSOURCEHANDLER_H
#define EASYPROCESSOREASYSOURCEHANDLER_H


#include <QtXml/QXmlDefaultHandler>

#include <string>
#include <vector>

namespace EasyProcessor
{

struct Enonce
{
  std::string id;
  std::string text;
};

/**
@author Benoit Mathieu
*/
class EasySourceHandler : public QXmlDefaultHandler
{
public:
  EasySourceHandler();

  virtual ~EasySourceHandler();

  bool endElement(const QString & namespaceURI, 
                  const QString & name, 
                  const QString & qName) override;
  
  bool characters(const QString& chars) override;
  
  bool startElement(const QString & namespaceURI, 
                    const QString & name, 
                    const QString & qName, 
                    const QXmlAttributes & attributes) override;
  
  bool startDocument() override;
  
  const std::vector<Enonce>& getEnonces() const;

private:

  std::vector<Enonce> m_enonces;

  Enonce m_currentEnonce;
  bool m_inEnonce;

  std::string getName(const QString& localName, const QString& qName);


};

inline const std::vector<Enonce>& EasySourceHandler::getEnonces() const 
{ 
  return m_enonces;
};


}
; // EasyProcessor

#endif
