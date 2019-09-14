/*
    Copyright 2002-2019 CEA LIST

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

#ifndef LIMA_TENSORFLOWUNITS_QJSONHELPERS_H
#define LIMA_TENSORFLOWUNITS_QJSONHELPERS_H

#include <vector>
#include <map>
#include <string>

#include <QJsonObject>
#include <QJsonArray>

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Common
{

inline void load_string_array(const QJsonArray& jsa, std::vector<std::string>& v)
{
  v.clear();
  v.reserve(jsa.size());
  for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
  {
    const QJsonValue value = *i;
    QString s = value.toString();
    v.push_back(s.toStdString());
  }
}

inline void load_string_array(const QJsonArray& jsa, std::vector<std::u32string>& v)
{
  v.clear();
  v.reserve(jsa.size());
  for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
  {
    QJsonValue value = *i;
    QString s = value.toString();
    v.push_back(s.toStdU32String());
  }
}

inline void load_string_to_uint_map(const QJsonObject& jso, std::map<std::u32string, unsigned int>& v)
{
  v.clear();
  for (QJsonObject::const_iterator i = jso.begin(); i != jso.end(); ++i)
  {
    if (v.end() != v.find(i.key().toStdU32String()))
      throw std::logic_error(std::string("TensorFlowMorphoSyntax::load_string_to_uint_map: \"")
		             + i.key().toStdString()
			     + std::string("\" already known."));

    v[i.key().toStdU32String()] = i.value().toInt();
  }
}

}
}
}
}

#endif
