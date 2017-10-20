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
 * \file    ConfigurationTree.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "ConfigurationTree.h"

#include "common/XMLConfigurationFiles/configurationStructure.h"
#include "common/XMLConfigurationFiles/moduleConfigurationStructure.h"
#include "common/XMLConfigurationFiles/groupConfigurationStructure.h"

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
// using namespace Lima::LinguisticProcessing;

namespace Lima 
{
namespace Gui 
{
namespace Config 
{

Q_GLOBAL_STATIC_WITH_ARGS(int, pid, (0));

bool ConfigurationNode::toggleById(int tid) 
{
  if (this->m_id == tid) 
  {
    if (checkable()) 
    {
      m_checked = !m_checked;
      return true;
    }
  }
  else 
  {
    for (auto& child : m_children) 
    {
      if (child->toggleById(tid)) 
      {
        return true;
      }
    }
  }
  return false;
}

std::string ConfigurationNode::name() 
{
  if (hasAttribute("name")) {
    return m_attributes["name"];
  }
  else if (hasAttribute("key")) {
    return m_attributes["key"];
  }
  else {
    return typeName(m_type);
  }
}

bool ConfigurationNode::checked() 
{
  return m_checked;
}

void ConfigurationNode::setChecked(bool b) 
{
  m_checked = b;
}

void ConfigurationNode::addAttribute(const std::string& key, const std::string& value) 
{
  m_attributes[key] = value;
}

bool ConfigurationNode::hasAttribute(const std::string& key) 
{
  std::map<std::string, std::string>::const_iterator it = m_attributes.find(key);
  return it != m_attributes.end();
}

std::string ConfigurationNode::getAttribute(const std::string& key) 
{
  if (hasAttribute(key)) {
    return m_attributes[key];
  }
  else {
    return "'" + key + "' is no attribute";
  }
}

void ConfigurationNode::addChild(ConfigurationNode *cn) 
{
  m_children.push_back(std::shared_ptr<ConfigurationNode>(cn));
}

bool ConfigurationNode::checkable() 
{
  std::vector<CONFIGURATION_NODE_TYPE> checkables = 
  {
    ITEM,
    ENTRY
  };
  
  bool checkable;
  
  checkable = std::find(checkables.begin(), checkables.end(), m_type) != checkables.end()
  && getAttribute("name") == "processUnitSequence";
  
  return visible() && checkable;
}

bool ConfigurationNode::visible() {
  std::vector<CONFIGURATION_NODE_TYPE> visibles = 
  {
    MODULE, GROUP, LIST, ITEM
  };
  
  std::vector<std::string> authorizedNames = 
  {
    "main",
    "processUnitSequence"
  };
  
  bool visible = std::find(visibles.begin(), visibles.end(), m_type) != visibles.end()
  && std::find(authorizedNames.begin(), authorizedNames.end(), getAttribute("name")) != authorizedNames.end();
  
  return visible;
}

ConfigurationNode::ConfigurationNode() 
{
  m_id = *pid;
  *pid = *pid + 1;
  m_type = CONFIGURATION_NODE_TYPE::NONE;
}

ConfigurationNode::ConfigurationNode(const ConfigurationStructure& p_cstruct) 
{
  ConfigurationStructure cstruct(p_cstruct);
  
  
  m_id = *pid;
  *pid = *pid + 1;
  m_type = CONFIGURATION_NODE_TYPE::FILE;
  
  for (auto& pair : cstruct) {
    addChild(new ConfigurationNode(pair.second));
  }
}

ConfigurationNode::ConfigurationNode(const ModuleConfigurationStructure& p_mstruct) 
{
  
  ModuleConfigurationStructure mstruct(p_mstruct);
  
  m_id = *pid;
  *pid = *pid + 1;
  m_type = CONFIGURATION_NODE_TYPE::MODULE;
  
  addAttribute("name", mstruct.getName());
  
  for (auto& pair : mstruct) {
    addChild(new ConfigurationNode(pair.second));
  }
}

ConfigurationNode::ConfigurationNode(const GroupConfigurationStructure& p_gstruct) 
{
  GroupConfigurationStructure gstruct(p_gstruct);
  
  
  m_id = *pid;
  *pid = *pid + 1;
  m_type = CONFIGURATION_NODE_TYPE::GROUP;
  
  addAttribute("name", gstruct.getName());
  try {
    addAttribute("class", gstruct.getAttribute("class"));
  }
  catch (std::exception& e) {}
  
  for (auto& pair : gstruct.getParams()) 
  {
    ConfigurationNode* cn = new ConfigurationNode();
    cn->fromParam(pair.first, pair.second);
    addChild(cn);
  }
  
  for (auto& pair : gstruct.getLists()) 
  {
    ConfigurationNode* cn = new ConfigurationNode();
    cn->fromList(pair.first, pair.second);
    addChild(cn);
  }
  
  for (auto& pair : gstruct.getMaps()) 
  {
    ConfigurationNode* cn = new ConfigurationNode();
    cn->fromMap(pair.first, pair.second);
    addChild(cn);
  }
}

void ConfigurationNode::fromList(const std::string& name, const std::deque<std::string>& list) 
{
  m_type = CONFIGURATION_NODE_TYPE::LIST;
  
  addAttribute("name", name);
  
  for (auto& item : list) {
    ConfigurationNode* cn = new ConfigurationNode();
    cn->fromItem(item);
    addChild(cn);
  }
}

void ConfigurationNode::fromMap(const std::string& name, const std::map<std::string, std::string>& map) 
{
  m_type = CONFIGURATION_NODE_TYPE::MAP;
  
  addAttribute("name", name);
  
  for (auto& pair : map) {
    ConfigurationNode* cn = new ConfigurationNode();
    cn->fromEntry(pair.first, pair.second);
    addChild(cn);
  }
}

void ConfigurationNode::fromItem(const std::string& item) 
{
  m_type = CONFIGURATION_NODE_TYPE::ITEM;
  
  addAttribute("value", item);
}

void ConfigurationNode::fromEntry(const std::string& key, const std::string& value) 
{
  m_type = CONFIGURATION_NODE_TYPE::ENTRY;
  
  addAttribute("key", key);
  addAttribute("value", value);
}

void ConfigurationNode::fromParam(const std::string& key, const std::string& value) 
{
  m_type = CONFIGURATION_NODE_TYPE::PARAM;
  
  addAttribute("key", key);
  addAttribute("value", value);
}

std::vector<std::shared_ptr<ConfigurationNode>>& ConfigurationNode::children() 
{
  return m_children;
}

// void ConfigurationNode::mask(const ConfigurationStructure& cstruct) 
//{
//   
// }

// ConfigurationStructure ConfigurationNode::toConfigurationStructure() 
//  {
//   ConfigurationStructure cstruct;
//   
//   ConfigurationNode* parent = this;
//   ConfigurationNode* subparent = nullptr;
//   std::string parentName = "root";
//   
//   //  while (parent != nullptr) {
//   
//   //    if (children.size()) {
//   
//   
//   
//   //    }
//   //    else {
//   //      parent = nullptr;
//   //    }
//   
//   //  }
//   
//   return cstruct;
// }

ConfigurationTree::ConfigurationTree() 
{

}

ConfigurationTree::ConfigurationTree(ConfigurationNode* root) : m_root(root)
{

}

ConfigurationNode* ConfigurationTree::root() const 
{
  return m_root;
}

ConfigurationTree::~ConfigurationTree() 
{
  delete m_root;
}

void ConfigurationTree::setRoot(ConfigurationNode *root) 
{
  m_root = root;
}

} // end namespace Config
} // end namespace Gui
} // end namespace Lima
