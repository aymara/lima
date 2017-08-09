#ifndef NAMEDENTITIESPARSER_H
#define NAMEDENTITIESPARSER_H

#include "LimaGuiExport.h"

#include <QObject>
#include <QString>

namespace Lima {
namespace Gui {

struct LIMA_GUI_EXPORT EntityItem {

  EntityItem(std::string name, std::string color, const std::vector<std::string>& occ) :
  name(name), color(color), occurences(occ)
  {}

  std::string name;
  std::string color;
  std::vector<std::string> occurences;
};

class LIMA_GUI_EXPORT NamedEntitiesParser : public QObject
{
  Q_OBJECT

  Q_PROPERTY(QString highlightedText READ getHighlightedText)
  Q_PROPERTY(QStringList entityTypes READ getEntityTypes)
public:
  NamedEntitiesParser(QObject* p = 0);

  Q_INVOKABLE void parse(const QString& rawText, const QString& conllText);

  Q_INVOKABLE QStringList getEntityTypes();
  Q_INVOKABLE QString getHighlightedText();

  EntityItem* findEntity(const std::string& name);

private:
  std::vector<EntityItem> entities;
  QString rawText;
  QString conllText;
};

} // namespace Gui
} // namespace Lima

#endif // NAMEDENTITIESPARSER_H
