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
