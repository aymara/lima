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
 * \file    ConfigurationTree.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#ifndef CONFIGURATIONTREE_H
#define CONFIGURATIONTREE_H

#include "LimaGuiExport.h"

#include <vector>
#include <deque>
#include <map>
#include <memory>

namespace Lima 
{
  namespace Common 
  {
    namespace XMLConfigurationFiles 
    {
      class XMLConfigurationFileParser;
      class ConfigurationStructure;
      class ModuleConfigurationStructure;
      class GroupConfigurationStructure;
    }
  }

namespace Gui
{
namespace Config
{

enum CONFIGURATION_NODE_TYPE
{
  NONE,
  FILE,
  MODULE,
  LIST,
  GROUP,
  MAP,
  ITEM,
  ENTRY,
  PARAM
};

std::string typeName(CONFIGURATION_NODE_TYPE);

/**
 * @brief A configuration structure stored in a tree
 *
 * Each node has a boolean property that will indicate whether or not to include it
 * when converting into the new custom configuration structure. It will be modified
 * by the user inside a treeView.
 */
class LIMA_GUI_EXPORT ConfigurationNode
{
public:
  static int pid;
  
  ConfigurationNode();
  ConfigurationNode(const Lima::Common::XMLConfigurationFiles::ConfigurationStructure&);
  ConfigurationNode(const Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure&);
  ConfigurationNode(const Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure&);
  
  void addAttribute(const std::string& key, const std::string& value);
  
  bool hasAttribute(const std::string& key);
  std::string name();
  std::string getAttribute(const std::string& key);
  // std::map<std::string, std::deque<std::string>> attributesChoices;
  
  void addChild(ConfigurationNode* cn);
  
  /// \brief Not every node is avaible to the user to disable.
  bool checkable();

  /// \brief Whether the user can actually see and alter the node in the user interface.
  bool visible();

  bool checked();

  void setChecked(bool);
  
  /// \brief The idea of this function was to ease the communication between the interface
  /// and the logic by allowing to set a node's status (checked) directly by its id, rather
  /// than searching in the configuration tree for this specific node.
  bool toggleById(int);
  
  void fromMap(const std::string& name, const std::map<std::string, std::string>& map);
  void fromEntry(const std::string& key, const std::string& value);
  void fromList(const std::string& name, const std::deque<std::string>& list);
  void fromItem(const std::string& item);
  void fromParam(const std::string& key, const std::string& value);
  
  /// \brief The goal here is to 'mask' a configuration structure with another.
  /// The idea was to use a configurationstructure with every 'possible' option
  /// (for example, you'd have a configuration file with every possible pipeline
  /// unit, uncommented
  /// and you'd 'mask' the resulting configuration structure with the default
  /// configuration file that would have only a few process units enabled,
  /// thus you know which nodes to display as checked beforehand to represent
  /// the latter file.
  void mask(const Lima::Common::XMLConfigurationFiles::ConfigurationStructure& cstruct);
  
  /// \brief reconvert to ConfigurationStructure
  Lima::Common::XMLConfigurationFiles::ConfigurationStructure toConfigurationStructure();
  
  friend class ConfigurationModelNode;

  std::vector<std::shared_ptr<ConfigurationNode>>& children();
  
private:
  
  CONFIGURATION_NODE_TYPE m_type;
  int m_id;
  bool m_checked;
  std::map<std::string, std::string> m_attributes;
  std::vector<std::shared_ptr<ConfigurationNode>> m_children;
};

/// \brief Not sure if this is actually useful.
/// For now, the root pointer is destroyed on
/// ConfigurationTree object destructor call.
class LIMA_GUI_EXPORT ConfigurationTree 
{
public:
  ConfigurationTree();
  ConfigurationTree(ConfigurationNode* root);

  ConfigurationNode* root() const;
  void setRoot(ConfigurationNode* root);

  ~ConfigurationTree();
  
private:
  ConfigurationNode* m_root;
  
};

} // end namespace Config
} // end namespace Gui
} // end namespace Lima

#endif // CONFIGURATIONTREE_H
