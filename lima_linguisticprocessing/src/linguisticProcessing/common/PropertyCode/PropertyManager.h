// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004 by CEA LIST                                        *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_COMMON_PROPERTYCODE_PROPERTYMANAGER_H
#define LIMA_COMMON_PROPERTYCODE_PROPERTYMANAGER_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "PropertyAccessor.h"

#include <map>

namespace Lima
{
namespace Common
{
namespace PropertyCode
{

class PropertyManagerPrivate;
/**
 * Provide tools to manage a specific property. The PropertyManager provide the PropertyAccessor
 * for read/write operation. It also offers convertion function from symbolic values to numeric values.
 * @brief Holds data about coding of a specific Property
 * @author Benoit Mathieu
 */
class LIMA_PROPERTYCODE_EXPORT PropertyManager
{
  friend class PropertyManagerPrivate;
public:

  /**
   * @brief constructor
   * @param name Name of the property
   * @param mask Mask of the property
   * @param emptyMask Mask for emptyness of the property
   * @param symbol2code Mapping from symbolicValues to values for this properties
   */
  PropertyManager(const std::string& name,
                  const LinguisticCode& mask,
                  const LinguisticCode& emptyNessMask,
                  const std::map<std::string,LinguisticCode> symbol2code);

  ~PropertyManager();

  PropertyManager(const PropertyManager&);
  PropertyManager& operator=(const PropertyManager&);

  /**
   * @brief give the corresponding PropertyAccessor
   * @return PropertyAccessor
   */
  const PropertyAccessor& getPropertyAccessor() const;

  /**
   * @brief Get the property mask
   * @return uint64_t
   */
  LinguisticCode getMask() const;

  /**
   * The emptyNessMask is the mask use to check if property is empty. In the case of
   * a 'property', emptyNessMask and mask are the same. But in the case of a
   * 'subproperty', emptyNessMask is the part of the mask used to code the subproperty itself,
   * so the mask is the union of emptyNessMask and parent property mask.
   * @brief Get the property emptyness mask
   * @return uint64_t
   */
  LinguisticCode getEmptyNessMask() const;

  /**
   * @brief Get the coded property value from the symbolic value
   * @param symbolicValue symbolic value
   * @return coded value
   */
  LinguisticCode getPropertyValue(const std::string& symbolicValue) const;

  /**
   * The coded property value can hold several property data.
   * @brief Get the symbolic value for a coded property value
   * @param value coded property value. This coded value can code several properties.
   * @return symbolic value for this property
   */
  const std::string& getPropertySymbolicValue(const LinguisticCode& value) const;

  /**
   * @brief Get the Code -> Symbol mapping
   * @return code2symbol map
   */
  const std::map<LinguisticCode,std::string>& getCode2Symbol() const;

  /**
   * @brief Get the Symbol -> Code mapping
   * @return symbol2code map
   */
  const std::map<std::string,LinguisticCode>& getSymbol2Code() const;

  /**
   * @brief give the number of values for this property
   * @return the number of values
   */
  size_t getNbValues() const;

private:
  std::unique_ptr<PropertyManagerPrivate> m_d;
};

#define GET_PROPERTY_MANAGER(language, property) \
    static_cast<const Lima::Common::MediaticData::LanguageData&>(\
      Lima::Common::MediaticData::MediaticData::single().mediaData( language ))\
        .getPropertyCodeManager().getPropertyManager( property );

#define GET_PROPERTY_ACCESSOR(language, property) \
    static_cast<const Lima::Common::MediaticData::LanguageData&>(\
      Lima::Common::MediaticData::MediaticData::single().mediaData( language ))\
      .getPropertyCodeManager().getPropertyAccessor( property )
} // PropertyCode
} // Common
} // Lima


#endif
