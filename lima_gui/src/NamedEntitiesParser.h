/*
    Copyright 2017 CEA LIST

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
/**
 * \file    NamedEntitiesParser.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef NAMEDENTITIESPARSER_H
#define NAMEDENTITIESPARSER_H

#include "LimaGuiExport.h"

#include <QObject>
#include <QString>

namespace Lima 
{
namespace Gui 
{

///
/// \brief A simple struct to store the important stuff about a given entity
///
struct LIMA_GUI_EXPORT EntityItem 
{

  EntityItem(std::string name, std::string color, const std::vector<std::string>& occ) :
  m_name(name), m_color(color), m_occurences(occ)
  {}

  std::string m_name;
  std::string m_color;
  std::vector<std::string> m_occurences;
};

///
/// \brief Parse the CONLL output and extract the named entities.
/// The method getHighlightedText returns HTML formatted text. It is meant
/// to be used by the RichText property of the TextArea Qml Element.
///
class LIMA_GUI_EXPORT NamedEntitiesParser : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString highlightedText READ getHighlightedText)
  Q_PROPERTY(QStringList entityTypes READ getEntityTypes)
public:
  NamedEntitiesParser(QObject* p = 0);

  /// \brief parse the conll content
  Q_INVOKABLE void parse(const QString& conllText);

  /// \brief returns the list of entity types that were extracted
  /// with the format <entityName>:<entityColor>;
  /// a 'split' function is called in the QML to get those two
  Q_INVOKABLE QStringList getEntityTypes();

  /// \brief returns the html formatted text with tags for each
  /// recognized named entity
  Q_INVOKABLE QString getHighlightedText();

  /// \brief look whether an entity with this names already exists
  /// \return a reference on the entity type OR nullptr
  EntityItem* findEntity(const std::string& name);

private:
  std::vector<EntityItem> m_entities;
  QString m_conllText;
};

} // namespace Gui
} // namespace Lima

#endif // NAMEDENTITIESPARSER_H
