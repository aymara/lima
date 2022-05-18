// Copyright 2017 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
#include <QStringList>

namespace Lima 
{
namespace Gui 
{

///
/// \brief A simple struct to store the important stuff about a given entity
///
struct EntityItem 
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
class NamedEntitiesParser : public QObject
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
