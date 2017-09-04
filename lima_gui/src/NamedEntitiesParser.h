#ifndef NAMEDENTITIESPARSER_H
#define NAMEDENTITIESPARSER_H

#include "LimaGuiExport.h"

#include <QObject>
#include <QString>

namespace Lima {
namespace Gui {

///
/// \brief A simple struct to store the important stuff about a given entity
///
struct LIMA_GUI_EXPORT EntityItem {

  EntityItem(std::string name, std::string color, const std::vector<std::string>& occ) :
  m_name(name), m_color(color), m_occurences(occ)
  {}

  std::string m_name;
  std::string m_color;
  std::vector<std::string> m_occurences;
};

///
/// \brief Parse the CONLL output and extract the named entities
///
class LIMA_GUI_EXPORT NamedEntitiesParser : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString highlightedText READ getHighlightedText)
  Q_PROPERTY(QStringList entityTypes READ getEntityTypes)
public:
  NamedEntitiesParser(QObject* p = 0);

  /// \brief parse the conll content
  Q_INVOKABLE void parse(const QString& rawText, const QString& conllText);

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
  QString m_rawText;
  QString m_conllText;
};

} // namespace Gui
} // namespace Lima

#endif // NAMEDENTITIESPARSER_H
