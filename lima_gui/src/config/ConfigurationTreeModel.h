#ifndef CONFIGURATIONTREEMODEL_H
#define CONFIGURATIONTREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>

#include <memory>

/// Simple tree view example :
/// http://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html
///
/// Editable tree view example :
/// http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html

namespace Lima {
namespace Gui {
namespace Config {
  
class ConfigurationNode;
class ConfigurationTree;

class ConfigurationTreeModelNode {
  
public:
  ConfigurationTreeModelNode(ConfigurationTreeModelNode* p = 0);
  ConfigurationTreeModelNode(ConfigurationNode*, ConfigurationTreeModelNode* p = 0);
  void fromConfigurationNode(ConfigurationNode*);
  
  /// REIMPLEMENTED METHODS
  
  //  int rowCount(const QModelIndex & parent = QModelIndex()) const;
  //  QHash<int, QByteArray> roleNames() const;
  

  ConfigurationTreeModelNode* child(int ind);
  int childCount() const;
  int columnCount() const;
  QVariant data(int col) const;
  void addChild(ConfigurationTreeModelNode* node);

  int row() const;
  ConfigurationTreeModelNode* parent();

  ~ConfigurationTreeModelNode();
  
private:
  ConfigurationNode*                                  m_node = nullptr;
  QList<ConfigurationTreeModelNode*>                  m_children;
  QList<QVariant>                                     m_data;
  ConfigurationTreeModelNode*                         m_parent = nullptr;
  
  enum {
    ID = Qt::UserRole + 1,
    NAME,
    CHECKED,
    CONTENTS
  };
};


class ConfigurationTreeModel : public QAbstractItemModel {
  Q_OBJECT
  
public:
  
  ConfigurationTreeModel(QObject* parent = 0);
  ConfigurationTreeModel(const ConfigurationTree& tree, QObject* parent = 0);

  ~ConfigurationTreeModel();
  
  QVariant      data
                (const QModelIndex& index,
                 int role)
                const;

  Qt::ItemFlags flags
                (const QModelIndex &index)
                const;

  QVariant      headerData
                (int section,
                 Qt::Orientation orientation,
                 int role = Qt::DisplayRole)
                const;

  QModelIndex   index
                (int row,
                 int column,
                 const QModelIndex &parent = QModelIndex())
                const;

  QModelIndex   parent(const QModelIndex &index) const;

  int           rowCount(const QModelIndex &parent = QModelIndex()) const;

  int           columnCount(const QModelIndex &parent = QModelIndex()) const;
  
private:
  
  ConfigurationTreeModelNode* m_rootNode;
};

} // end namespace Config
} // end namespace Gui
} // end namespace Lima

#endif // CONFIGURATIONTREEMODEL_H
