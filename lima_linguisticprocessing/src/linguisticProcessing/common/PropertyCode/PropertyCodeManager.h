/*
    Copyright 2002-2020 CEA LIST

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
/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_PROPERTYCODE_PROPERTYCODEMANAGER_H
#define LIMA_COMMON_PROPERTYCODE_PROPERTYCODEMANAGER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "PropertyManager.h"

#include <memory>
#include <set>

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

class PropertyCodeManagerPrivate;
/**
 * Provide tools to parse a property file, and deal with the property coding system.
 * @brief <b>Main Entry Point</b> for property coding, Holds data about a property coding system.
 * @author Benoit Mathieu
 */
class LIMA_PROPERTYCODE_EXPORT PropertyCodeManager
{
  friend class PropertyCodeManagerPrivate;

public:

  /**
   * @brief Default Constructor
   * @return
   */
  PropertyCodeManager();

  ~PropertyCodeManager();

  PropertyCodeManager(const PropertyCodeManager&);
  PropertyCodeManager& operator=(const PropertyCodeManager&);

  /**
   * This functions should be called once, just after creation. It computes
   * all data necessary to perform coding/decoding operations
   * @brief Parse coding data from an XML file.
   * @param filename name of the XML file to parse
   */
  void readFromXmlFile(const std::string& filename);

  /**
   * @brief Get the PropertyAccessor associated to a property
   * @param propertyName Name of the property
   * @return PropertyAccessor
   */
  const PropertyAccessor& getPropertyAccessor(
    const std::string& propertyName) const;

  /**
   * @brief Get the PropertyManager associated to a property
   * @param propertyName Name of the property
   * @return PropertyManager
   */
  const PropertyManager& getPropertyManager(
    const std::string& propertyName) const;

  /**
   * @brief Get a PropertyAccessor associated to several properties
   * @param propertyName Name of the property
   * @return PropertyAccessor
   */
  const std::shared_ptr<PropertyAccessor> getPropertySetAccessor(
    const std::set<std::string>& propertyNames) const;

  /**
   * @brief Get the map of all PropertyManagers
   * @return A map from propery names to associated PropertyManager
   */
  const std::map<std::string,PropertyManager>& getPropertyManagers() const;

  /**
   * Compute the code corresponding to given the properties/values.
   * <b>Warning</b> : No consistency check. If the given map contains a subproperty value
   * and a incoherent parent property value, only the last will be kept.
   * @brief Encode properties
   * @param propValues mapping (property name -> property value) to encode
   * @return coded LinguisticCode
   */
  LinguisticCode encode(const std::map<std::string,std::string>& propValues) const;

  /**
   * Compute conversion of symbolic codes into linguisticCode and append them into
   * the given map. The given is an xml file :
   * &lt;codes&gt;
   *   &lt;code key="Ncgms-"&gt;
   *     &lt;field key="MICRO" value="NC"/&gt;
   *     &lt;field key="GENDER" value="MASC"/&gt;
   *     &lt;field key="NUMBER" value="SING"/&gt;
   *     &lt;field key="PERSON" value=""/&gt;
   *     &lt;field key="TIME" value=""/&gt;
   *     &lt;field key="SYNTAX" value=""/&gt;
   *   &lt;/code&gt;
   *   ...
   * &lt;/codes&gt;
   * @param symbolicCodeFile Name of the file to convert
   * @param conversionTable Map to which append the results
   */
  void convertSymbolicCodes(const std::string& symbolicCodeFile,
                            std::map<std::string,LinguisticCode>& conversionTable) const;

private:
  PropertyCodeManagerPrivate* m_d;
};

} // PropertyCode
} // Common
} // Lima


#endif
