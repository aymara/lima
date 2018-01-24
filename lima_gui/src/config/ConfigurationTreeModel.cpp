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
 * \file    ConfigurationTreeModel.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "ConfigurationTreeModel.h"
#include "ConfigurationTree.h"
#include "LimaGuiCommon.h"

#include <common/LimaCommon.h>

namespace Lima 
{
namespace Gui 
{
namespace Config 
{

// ConfigurationTreeModelNode::ConfigurationTreeModelNode(QObject* p) : QAbstractListModel(p) {
//   
//   //  std::string configDir = qgetenv("LIMA_CONF").constData();
//   //  std::string lpConfig = "lima-lp-fre.xml";
//   //  Lima::Common::XMLConfigurationFiles::XMLConfigurationFileParser lpconfig(configDir + "/" + lpConfig);
//   
//   //  ConfigurationNode* cn = new ConfigurationNode(lpconfig.getConfiguration());
//   
//   ////  cn->addAttribute("name", "Mothership");
//   
//   ////  ConfigurationNode* cm = new ConfigurationNode();
//   
//   ////  cm->addAttribute("name", "Fighter");
//   
//   ////  cn->addChild(cm);
//   
//   
//   //  fromConfigurationNode(cn);
//   
// }

//ConfigurationTreeModelNode::ConfigurationTreeModelNode(ConfigurationNode* node, QObject* p) : QAbstractListModel(p) {
//  fromConfigurationNode(node);
//}

//void ConfigurationTreeModelNode::fromConfigurationNode(ConfigurationNode* cn) {
//  this->node = cn;

//  for (auto& child : cn->children) {
//    children.push_back(std::shared_ptr<ConfigurationTreeModelNode>(new ConfigurationTreeModelNode(child.get())));
//  }
//}

//int ConfigurationTreeModelNode::rowCount(const QModelIndex & parent) const {
//  return children.size();
//}

//QVariant ConfigurationTreeModelNode::data(const QModelIndex& index, int role) const {
//  if (role > Qt::UserRole) {
//    if (role == ID) { // id
//      return node->id;
//    }
//    if (role == NAME) { // name
//      return QString(node->getName().c_str());
//    }
//    else if (role == CHECKED) { // checked?bool
//      return node->checked;
//    }
//    else if (role == CONTENTS) { // contents
////      return QVariant(children);
//      return 0;
//    }
//    else {
//      return QVariant();
//    }
//  }
//  else {
//    return QVariant();
//  }
//}

ConfigurationTreeModelNode::ConfigurationTreeModelNode(ConfigurationTreeModelNode* p) 
{
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModelNode::ConfigurationTreeModelNode(p)";
  m_parent = p;
}

ConfigurationTreeModelNode::ConfigurationTreeModelNode(ConfigurationNode* node, ConfigurationTreeModelNode *p) : ConfigurationTreeModelNode(p) 
{
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModelNode::ConfigurationTreeModelNode(node,p)";
  fromConfigurationNode(node);
}

void ConfigurationTreeModelNode::fromConfigurationNode(ConfigurationNode* cn) 
{
  this->m_node = cn;

  for (auto& child : cn->children()) 
  {
//    m_children.push_back(std::shared_ptr<ConfigurationTreeModelNode>(new ConfigurationTreeModelNode(child.get())));
    m_children.push_back(new ConfigurationTreeModelNode(child.get(), this));
  }

  m_data << QString(m_node->name().c_str()) << m_node->checked();
}

void ConfigurationTreeModelNode::addChild(ConfigurationTreeModelNode *node) 
{
//   m_children.push_back(std::shared_ptr<ConfigurationTreeModelNode>(node));
  m_children.push_back(node);
}

ConfigurationTreeModelNode* ConfigurationTreeModelNode::child(int ind) 
{
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModelNode::child" << ind << m_children.value(ind);
  return m_children.value(ind);
}

int ConfigurationTreeModelNode::childCount() const 
{
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModelNode::columnCount" <<  m_children.count();
  return m_children.count();
}

int ConfigurationTreeModelNode::columnCount() const 
{
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModelNode::columnCount" <<  m_data.count();
  return m_data.count();
}

ConfigurationTreeModelNode* ConfigurationTreeModelNode::parent() 
{
  return m_parent;
}

int ConfigurationTreeModelNode::row() const 
{
  int result = m_parent ? m_parent->m_children.indexOf(const_cast<ConfigurationTreeModelNode*>(this)) : 0;
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModelNode::row" <<  result;
  return result;
}

QVariant ConfigurationTreeModelNode::data(int col) const 
{
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModelNode::data" <<  col;
  return m_data.value(col);
}

ConfigurationTreeModelNode::~ConfigurationTreeModelNode() 
{
  for (auto& child : m_children) {
    delete child;
  }
}

std::string typeName(CONFIGURATION_NODE_TYPE type) 
{
  switch (type) {
    case CONFIGURATION_NODE_TYPE::MAP:    return "MAP";
    case CONFIGURATION_NODE_TYPE::ITEM:   return "ITEM";
    case CONFIGURATION_NODE_TYPE::LIST:   return "LIST";
    case CONFIGURATION_NODE_TYPE::MODULE: return "MODULE";
    case CONFIGURATION_NODE_TYPE::GROUP:  return "GROUP";
    case CONFIGURATION_NODE_TYPE::FILE:   return "FILE";
    case CONFIGURATION_NODE_TYPE::ENTRY:  return "ENTRY";
    case CONFIGURATION_NODE_TYPE::PARAM:  return "PARAM";
    default:
    case CONFIGURATION_NODE_TYPE::NONE: return "NONE";
  }
}

////////////////////////////////////////////

ConfigurationTreeModel::ConfigurationTreeModel(QObject* parent) : QAbstractItemModel(parent) 
{

}

ConfigurationTreeModel::ConfigurationTreeModel(const ConfigurationTree &tree, QObject *parent) : QAbstractItemModel(parent) 
{
  m_rootNode = new ConfigurationTreeModelNode(tree.root());
}

ConfigurationTreeModel::~ConfigurationTreeModel() 
{
  delete m_rootNode;
}

QModelIndex ConfigurationTreeModel::index(int row, int column, const QModelIndex& parent) const 
{
  CONFLOGINIT;
  LDEBUG << "ConfigurationTreeModel::index" <<  row << column;
  if (!hasIndex(row, column, parent)) 
  {
    return QModelIndex();
  }
  
  ConfigurationTreeModelNode* parentNode;
  
  if (!parent.isValid()) 
  {
    parentNode = m_rootNode;
  }
  else 
  {
    parentNode = static_cast<ConfigurationTreeModelNode*>(parent.internalPointer());
  }
  
  ConfigurationTreeModelNode* childNode = parentNode->child(row);
  if (childNode) 
  {
    return createIndex(row, column, childNode);
  }
  else
  {
    return QModelIndex();
  }
}

QModelIndex ConfigurationTreeModel::parent(const QModelIndex &index) const 
{
  if (!index.isValid()) {
    return QModelIndex();
  }

  ConfigurationTreeModelNode* childItem = static_cast<ConfigurationTreeModelNode*>(index.internalPointer());
  ConfigurationTreeModelNode *parentItem = childItem->parent();

  if (parentItem == m_rootNode) 
  {
    return QModelIndex();
  }

  return createIndex(parentItem->row(), 0, parentItem);
}

int ConfigurationTreeModel::rowCount(const QModelIndex &parent) const
{
    ConfigurationTreeModelNode *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = m_rootNode;
    else
        parentItem = static_cast<ConfigurationTreeModelNode*>(parent.internalPointer());

    return parentItem != nullptr ? parentItem->childCount() : 0;
}

int ConfigurationTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<ConfigurationTreeModelNode*>(parent.internalPointer())->columnCount();
    else
        return m_rootNode->columnCount();
}

QVariant ConfigurationTreeModel::data(const QModelIndex &index, int role) const
{
  LIMA_UNUSED(role)
    CONFLOGINIT;
    LDEBUG << "ConfigurationTreeModel::data";

    if (!index.isValid())
        return QVariant();

//     if (role != Qt::DisplayRole)
//         return QVariant();

    ConfigurationTreeModelNode *item = static_cast<ConfigurationTreeModelNode*>(index.internalPointer());

    return item->data(index.column());
}

Qt::ItemFlags ConfigurationTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
      return 0;
    }

    return QAbstractItemModel::flags(index);
}

QVariant ConfigurationTreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
      return m_rootNode->data(section);
    }

    return QVariant();
}

QHash<int, QByteArray> ConfigurationTreeModel::roleNames() const 
{
 QHash<int, QByteArray> roles;// = QAbstractItemModel::roleNames();
    roles[ID] = "id"; // Those strings are direclty related to the 'TableViewColumn' elements role property
    roles[NAME] = "name";
    roles[CHECKED] = "nodeChecked";
    roles[CONTENTS] = "contents";
 return roles;

}

} // end namespace Config
} // end namespace Gui
} // end namespace Lima
