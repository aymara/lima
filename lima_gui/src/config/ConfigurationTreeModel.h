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
 * \file    ConfigurationTreeModel.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef CONFIGURATIONTREEMODEL_H
#define CONFIGURATIONTREEMODEL_H

#include "LimaGuiExport.h"

#include <QObject>
#include <QAbstractItemModel>

#include <memory>

/// Simple tree view example :
/// http://doc.qt.io/qt-5/qtwidgets-itemviews-simpletreemodel-example.html
///
/// Editable tree view example :
/// http://doc.qt.io/qt-5/qtwidgets-itemviews-editabletreemodel-example.html


// Comments:
/*
 * This class is supposed to serve as a model for a (Q)TreeView.
 * The example did not work with the roleNames method though.
 * 
 */

namespace Lima 
{
namespace Gui 
{
namespace Config 
{
  
class ConfigurationNode;
class ConfigurationTree;

/// \brief This class is the 'TreeItem' of the Treeview model.
/// 
/// For each Node in the 'ConfigurationTree', there is a twin node
/// for the qt modeL.
class ConfigurationTreeModelNode 
{
  
public:
  ConfigurationTreeModelNode(ConfigurationTreeModelNode* p = 0);
  ConfigurationTreeModelNode(ConfigurationNode*, ConfigurationTreeModelNode* p = 0);
  ~ConfigurationTreeModelNode();
  
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

  
private:
  ConfigurationNode*                                  m_node = nullptr;
  QList<ConfigurationTreeModelNode*>                  m_children;
  QList<QVariant>                                     m_data;
  ConfigurationTreeModelNode*                         m_parent = nullptr;
  
};


/// \brief This is the TreeModel for the Qt model pattern.
/// 
/// It relies on a regular configuration tree to generate its nodes.
class ConfigurationTreeModel : public QAbstractItemModel 
{
  Q_OBJECT

  enum 
  {
    ID = Qt::UserRole + 1,
    NAME,
    CHECKED,
    CONTENTS
  };
  
public:
  
  ConfigurationTreeModel(QObject* parent = 0);
  ConfigurationTreeModel(const ConfigurationTree& tree, QObject* parent = 0);

  ~ConfigurationTreeModel();
  
  QHash<int, QByteArray> roleNames() const override;

  QVariant data(const QModelIndex& index, int role) const override;

  Qt::ItemFlags flags(const QModelIndex &index) const override;

  QVariant headerData(int section, Qt::Orientation orientation,
                      int role = Qt::DisplayRole) const override;

  QModelIndex index(int row, int column, 
                    const QModelIndex &parent = QModelIndex()) const override;

  QModelIndex parent(const QModelIndex &index) const override;

  int rowCount(const QModelIndex &parent = QModelIndex()) const override;

  int columnCount(const QModelIndex &parent = QModelIndex()) const override;
  
private:
  
  ConfigurationTreeModelNode* m_rootNode;
};

} // end namespace Config
} // end namespace Gui
} // end namespace Lima

#endif // CONFIGURATIONTREEMODEL_H
