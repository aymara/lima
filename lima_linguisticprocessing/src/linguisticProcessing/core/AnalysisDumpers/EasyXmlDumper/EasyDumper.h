// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *
 * @file       EasyDumper.h
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 * Project     s2lp
 *
 * @brief      dump the content of the analysis graph in Easy XML format
 *
 */

#ifndef LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_EASYDUMPER_H
#define LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_EASYDUMPER_H

#include "ConstituantAndRelationExtractor.h"

#include <string>
#include <map>
#include <set>

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

class EasyDumper
{

public:

  EasyDumper(const ConstituantAndRelationExtractor& care,
             const std::map<std::string,std::string>& relationTypeMapping,
             const std::map<std::string,std::string>& srcVxTag,
             const std::map<std::string,std::string>& tgtVxTag,
             const std::string& enonceId);

  ~EasyDumper();

  void dump(std::ostream& out);

private:

  typedef std::pair<Chaine,std::vector<Forme> > Constituant;

  std::string m_enonceId;
  std::map<std::string,std::string> m_relationTypeMapping;
  std::map<std::string,std::string> m_srcVxTag;
  std::map<std::string,std::string> m_tgtVxTag;

  std::map<Chaine,std::vector<uint64_t> > m_chaines;
  std::map<uint64_t,uint64_t> m_vertexToFormeIds;
  std::map<uint64_t,Forme*> m_formesIndex;
  std::vector<Relation*> m_relations;
  std::vector<Relation*> m_inRelations;
  std::vector<Constituant> m_constituants;

  std::map<Chaine,std::string> m_chaineIds;
  std::map<uint64_t,std::string> m_formesIds;
  std::map<Relation*,std::string> m_relationsIds;

  /** Les cles sont des positions, ce qui donne l'ordre des groupes dans la phrase */
  std::map<uint64_t, Groupe> m_groupes;

  std::map<uint64_t, std::string> m_groupeIdGroupStr;

  /** form id -> group id */
  std::map<uint64_t, Groupe*> m_formesIdsGroupsIds;

  std::map< uint64_t, uint64_t > m_positionsFormsIds;
  std::set< uint64_t > m_inGroupFormsPositions;

  void dumpConstituants(std::ostream& out);
  void dumpRelations(std::ostream& out);
  void computeFormeIds();

};

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima

#endif // LIMA_LINGUISTICPROCESSINGS_ANALYSISDUMPERS_EASYXMLDUMPER_EASYDUMPER_H
