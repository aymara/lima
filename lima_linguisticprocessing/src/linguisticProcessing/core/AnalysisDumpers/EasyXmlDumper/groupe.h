// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *
 * @file       groupe.h
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 * Project     s2lp
 *
 * @brief      represents an Easy group
 *
 */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_GROUPE_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_GROUPE_H


#include <limits>
#include <string>
#include <map>
#include <cstdint>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

/**
  * @brief Definition d'un groupe syntaxique. 
  *
  * Les cles sont des positions de formes et les valeurs sont leurs ids
  */
class Groupe : public std::map< uint64_t, uint64_t >
{

public:

  Groupe() : 
      std::map< uint64_t, uint64_t >(),
      m_id(std::numeric_limits<uint64_t>::max()),
      m_type() {};

  Groupe(const Groupe& group) : 
      std::map< uint64_t, uint64_t >(group),
      m_id(group.m_id),
      m_type(group.m_type) {};

  virtual ~Groupe() {};

  inline uint64_t id() { return m_id; };
  inline void id(uint64_t anId) { m_id = anId; };

  inline const std::string& type() const { return m_type; };
  inline void type(const std::string& aType) { m_type = aType; };

private:

  uint64_t m_id;
  std::string m_type;

};

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_GROUPE_H
