// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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

inline void load_string_array(const QJsonArray& jsa, std::vector<QString>& v)
{
  v.clear();
  v.reserve(jsa.size());
  for (QJsonArray::const_iterator i = jsa.begin(); i != jsa.end(); ++i)
  {
    QJsonValue value = *i;
    QString s = value.toString();
    v.push_back(s);
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

inline void load_string_to_uint_map(const QJsonObject& jso, std::map<std::string, unsigned int>& v)
{
  v.clear();
  for (QJsonObject::const_iterator i = jso.begin(); i != jso.end(); ++i)
  {
    if (v.end() != v.find(i.key().toStdString()))
      throw std::logic_error(std::string("TensorFlowMorphoSyntax::load_string_to_uint_map: \"")
                     + i.key().toStdString()
                 + std::string("\" already known."));

    v[i.key().toStdString()] = i.value().toInt();
  }
}

}
}
}
}

#endif
