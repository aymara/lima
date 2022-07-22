// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 *
 * @file       EasyDumper.cpp
 * @author     Damien Nouvel <Damien.Nouvel@cea.fr> 

 *             Copyright (C) 2004 by CEA LIST
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 * @author     Benoit Mathieu <mathieub@zoe.cea.fr>
 * @date       Mon Oct 07 2008
 *
 * @brief      dump the content of the analysis graph in Easy XML format
 *
 */

#include "EasyDumper.h"
#include "easyXmlDumper.h"

#include "common/time/traceUtils.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

#include <iostream>
#include <sstream>

using namespace Lima::Common;
using namespace Lima::Common::Misc;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {
namespace EasyXmlDumper {

EasyDumper::EasyDumper(
  const ConstituantAndRelationExtractor& care,
  const std::map<std::string,std::string>& relationTypeMapping,
  const std::map<std::string,std::string>& srcVxTag,
  const std::map<std::string,std::string>& tgtVxTag,
  const std::string& enonceId) :
    m_enonceId(enonceId),
    m_relationTypeMapping(relationTypeMapping),
    m_srcVxTag(srcVxTag),
    m_tgtVxTag(tgtVxTag),
    m_chaines(care.getChaines()),
    m_vertexToFormeIds(care.getVertexToFormeIds()),
    m_formesIndex(care.getFormesIndex()),
    m_relations(care.getRelations()),
    m_inRelations(care.getInRelations()),
    m_groupes(care.getGroupes()),
    m_positionsFormsIds(care.positionsFormsIds()),
    m_inGroupFormsPositions(care.inGroupFormsPositions())
{
  std::map<uint64_t, Groupe>::iterator it, it_end;
  it = m_groupes.begin(); it_end = m_groupes.end();
  for (; it != it_end; it++)
  {
    Groupe& group = (*it).second;
    Groupe::iterator git, git_end;
    git = group.begin(); git_end = group.end();
    for (; git != git_end; git++)
    {
      DUMPERLOGINIT;
      LDEBUG << "EasyDumper:: setting " << group.id() << " as group of " << git->second;
      m_formesIdsGroupsIds[git->second]=&group;
    }
  }
}


EasyDumper::~EasyDumper()
{
  for (std::vector<Relation*>::iterator it =  m_relations.begin(); it != m_relations.end(); it++)
  {
    delete *it;
  }
}


void EasyDumper::dump(std::ostream& out)
{
//   computeFormeIds();
  dumpConstituants(out);
  dumpRelations(out);
}

void EasyDumper::dumpConstituants(std::ostream& out)
{
  std::map< uint64_t, uint64_t >::const_iterator posIt, posIt_end;
  posIt = m_positionsFormsIds.begin();
  posIt_end = m_positionsFormsIds.end();
  uint64_t groupId = 1;
  uint64_t formId = 1;
  std::set< uint64_t > alreadyDumpedForms;
  for (; posIt != posIt_end; posIt++)
  {
    uint64_t position = (*posIt).first;
    uint64_t forme = (*posIt).second;
    DUMPERLOGINIT;
    if (m_formesIndex[forme] == 0)
    {
      LWARN << "EasyDumper:: form not found " << forme;
      continue;
    }
    if (alreadyDumpedForms.find(position) != alreadyDumpedForms.end())
    {
      LDEBUG << "EasyDumper:: already dumped " << m_formesIndex[forme]->forme;
      continue;
    }
    LDEBUG << "EasyDumper:: dump " << m_formesIndex[forme]->forme;
    if (m_inGroupFormsPositions.find(position) == m_inGroupFormsPositions.end())
    {
      std::string sforme = limastring2utf8stdstring(transcodeToXmlEntities(utf8stdstring2limastring((m_formesIndex[forme])->forme)));

      while (sforme.find('_') != std::string::npos)
      {
        LDEBUG << "EasyDumper:: remove _ in " << sforme;
        sforme[sforme.find('_')] = ' ';
      }

      std::ostringstream oss;
      oss << m_enonceId  << "F"<< formId;
      out << "  <F id=\"" << oss.str() << "\">" << sforme << "</F>" << std::endl;
      LDEBUG << "EasyDumper:: adding to m_formesIds1: "<<m_formesIndex[forme]->forme<<" -> " << oss.str();
      m_formesIds.insert(std::make_pair(forme, oss.str()));
      alreadyDumpedForms.insert((m_formesIndex[forme])->poslong.position);
      formId++;
    }
    else
    {
      std::ostringstream sout;
      if (m_groupes.find(position) == m_groupes.end())
      {
        continue;
      }
      Groupe& groupe = m_groupes[position];
      Groupe::const_iterator groupeIt, groupeIt_end;
      groupeIt = groupe.begin(); groupeIt_end = groupe.end();
      // compute the chaine id here
      std::ostringstream oss;
      oss << m_enonceId << "G" << groupId;
      groupe.id(groupId);
      sout << "  <Groupe type=\"" << groupe.type() << "\" id=\"" << oss.str() << "\">" << std::endl;
      m_groupeIdGroupStr[groupe.id()] = oss.str();

      groupeIt = groupe.begin(); groupeIt_end = groupe.end();
      DUMPERLOGINIT;
      for (; groupeIt != groupeIt_end; groupeIt++)
      {
        std::ostringstream oss;
        oss << m_enonceId << "F" << formId;
        std::string sforme = limastring2utf8stdstring(transcodeToXmlEntities((utf8stdstring2limastring(m_formesIndex[(*groupeIt).second]->forme))));

        while (sforme.find('_') != std::string::npos)
        {
          LDEBUG << "EasyDumper:: remove _ in " << sforme;
          sforme[sforme.find('_')] = ' ';
        }
        sout << "    <F id=\"" << oss.str() << "\">" << sforme << "</F>" << std::endl;
        m_formesIds.insert(std::make_pair((*groupeIt).second, oss.str()));
        if (m_formesIndex.find((*groupeIt).second) != m_formesIndex.end())
        {
          alreadyDumpedForms.insert((m_formesIndex[(*groupeIt).second])->poslong.position);
        }
        else
        {
          LERROR << "Error at " << __FILE__ << ", line " << __LINE__;
        }
        formId++;
      }
      sout << "  </Groupe>" << std::endl;
      out << sout.str();
      LDEBUG << "EasyDumper:: " << sout.str();
      groupId++;
    }
  }
}



void EasyDumper::dumpRelations(std::ostream& out)
{
  int numRel=1;
  // in easy2/passage, the relations tag can be avoided if there is no relation
  if (m_relations.empty()) 
  {
    return;
  }
  out << "  <relations>" << std::endl;
  // to handle three-value relations (COORD)
  bool notClosed(false);
  std::string previousRelType(""), previousSrcId("");
  std::string relType("");
  for (std::vector<Relation*>::const_iterator it=m_relations.begin();
       it!=m_relations.end();
       it++)
  {

    relType=m_relationTypeMapping[(*it)->type];
    DUMPERLOGINIT;
    LDEBUG << "EasyDumper:: relation '"<<relType<<"'";
    if (relType=="") continue;

    // possible complement
    std::string complement;
    
    std::string::size_type i=relType.find(",");
    if (i!=std::string::npos)
    {
      complement=std::string(relType,i+1);
      relType.erase(i);
      // add possible quotes around arguments
      i=0;
      std::string::size_type j;
      while ((i=complement.find("=",i)) != std::string::npos
              && complement[i+1] != '"')
      {
        complement.insert(i+1,"\"");
        j=complement.find(" ",i+1);
        if (j == std::string::npos)
        {
          complement.insert(complement.end(),'"');
          break;
        }
        else
        {
          complement.insert(j,"\"");
          i=j+1;
        }
      }
    }
    
    std::ostringstream oss;
    oss << m_enonceId << "R" << numRel++;
    m_relationsIds[*it]=oss.str();
    
    std::string srcFormeId=m_formesIds[m_vertexToFormeIds[(*it)->srcVertex]];
    if (srcFormeId == "")
    {
      LWARN << "EasyDumper:: Warning: empty source form id in " << m_enonceId << " for vertex " << (*it)->srcVertex;
      srcFormeId = m_enonceId + "F0";
    }
    std::string tgtFormeId=m_formesIds[m_vertexToFormeIds[(*it)->tgtVertex]];
    if (tgtFormeId == "")
    {
      LWARN << "EasyDumper:: Warning: empty target form id in " << m_enonceId << " for vertex " << (*it)->tgtVertex;
      tgtFormeId = m_enonceId + "F0";
    }

    // to handle three-value relations
    std::string tgtVxTag=m_tgtVxTag[relType];
    LDEBUG << "EasyDumper:: tgtVxTag=" << tgtVxTag;
    std::string::size_type k=tgtVxTag.find(",");
    if (k!=std::string::npos)
    {
      LDEBUG << "EasyDumper:: 3-ary relation";
      if (!notClosed)
      { //first part
        LDEBUG << "EasyDumper:: first part: " << tgtVxTag;
        tgtVxTag.erase(k);
        previousRelType=relType;
        previousSrcId=srcFormeId;
        notClosed=true;
      }
      else
      { // second part
        notClosed=false;
        tgtVxTag.erase(0,k+1);
        LDEBUG << "EasyDumper:: second part: " << tgtVxTag << " ; " << relType <<"/" << previousRelType << " ; " << srcFormeId << "/" << previousSrcId << "/" << tgtFormeId;
        if (relType==previousRelType && tgtFormeId==previousSrcId)
        {
          std::string href =  m_enonceId + srcFormeId;
          if (href == m_enonceId)
          {
            href += "F0";
          }
          out << "      <" << tgtVxTag << " xlink:type=\"locator\" xlink:href=\"" << href << "\"/>" << std::endl;
          out << "    </relation>" << std::endl;
          continue;
        }
        else
        {
          std::string href =  m_enonceId + "F0";
          out << "      <" << tgtVxTag << " xlink:type=\"locator\" xlink:href=\""<<href<<"\"/>" << std::endl;
          LWARN << "EasyDumper:: Warning: relation " << previousRelType << " not continued";
          out << "    </relation>" << std::endl;
        }
      }
    }
    else
    {
      if (notClosed)
      {
        // @TODO check why this happens
        LWARN << "EasyDumper:: Warning: non 3-ary relation marked as not closed for relation " << previousRelType;
        if (relType == "COORD")
        {
          std::string href =  m_enonceId + "F0";
          out << "      <coord-d xlink:type=\"locator\" xlink:href=\""<<href<<"\"/>" << std::endl;
        }
        out << "    </relation>" << std::endl;
        notClosed=false;
      }
    }

    bool coordgOutputed = false;
    bool coorddOutputed = false;
    out << "    <relation xlink:type=\"extended\" type=\"" << relType << "\" id=\"" <<  oss.str() << "\">" << std::endl;

    if (relType == "JUXT")
    {
      if (tgtVxTag == "coord-g") coordgOutputed = true;
      if (tgtVxTag == "coord-d") coorddOutputed = true;
      if (m_srcVxTag[relType] == "coord-g") coordgOutputed = true;
      if (m_srcVxTag[relType] == "coord-d") coorddOutputed = true;
      out << "      <" << tgtVxTag << " xlink:type=\"locator\" xlink:href=\"" << tgtFormeId << "\"/>" << std::endl;
      out << "      <" << m_srcVxTag[relType] << " xlink:type=\"locator\" xlink:href=\"" << srcFormeId << "\"/>" << std::endl;
    }
    else
    {
      /*
      DUMPERLOGINIT;
      LDEBUG << "EasyDumper:: searching " << m_vertexToFormeIds[(*it)->srcVertex] << " in groups";
      if ( (relType == "CPL-V" || relType == "SUJ-V" || relType == "COD-V"
            || relType == "CPL-V" || relType == "MOD-V" || relType == "ATB-SO" || relType == "MOD-N")
            && m_formesIdsGroupsIds.find(m_vertexToFormeIds[(*it)->srcVertex]) != m_formesIdsGroupsIds.end())
      {
        uint64_t gid = m_formesIdsGroupsIds[m_vertexToFormeIds[(*it)->srcVertex]]->id();
        std::string gsid = m_groupeIdGroupStr[gid];
        out << "      <" << m_srcVxTag[relType] << " xlink:type=\"locator\" xlink:href=\"" << gsid << "\"/>" << std::endl;
      }
      else
      */
      {
        if (m_srcVxTag[relType] == "coord-g") coordgOutputed = true;
        if (m_srcVxTag[relType] == "coord-d") coorddOutputed = true;
        out << "      <" << m_srcVxTag[relType] << " xlink:type=\"locator\" xlink:href=\"" << srcFormeId << "\"/>" << std::endl;
      }
      if (tgtVxTag == "coord-g") coordgOutputed = true;
      if (tgtVxTag == "coord-d") coorddOutputed = true;
      out << "      <" << tgtVxTag << " xlink:type=\"locator\" xlink:href=\"" << tgtFormeId << "\"/>" << std::endl;
    }
    if ((*it)->secondaryVertex != 0)
    {
      std::string href =  m_formesIds[m_vertexToFormeIds[(*it)->secondaryVertex]];
      if (href == "")
      {
        LWARN << "EasyDumper:: Warning: form id not found for " << (*it)->secondaryVertex;
        href = m_enonceId + "F0";
      }
      coorddOutputed = true;
      out << "      <coord-d xlink:type=\"locator\" xlink:href=\"" << href << "\"/>" << std::endl;
    }
    if (coordgOutputed && !coorddOutputed)
    {
      std::string href = m_enonceId + "F0";
      out << "      <coord-d xlink:type=\"locator\" xlink:href=\"" << href << "\"/>" << std::endl;
      coorddOutputed = true;
    }
    
    if (! complement.empty())
    {
      out << "      <" << complement << "/>" << std::endl;
    }
    
    if (!notClosed)
    {
      out << "    </relation>" << std::endl;
    }
  }
  
  if (notClosed)
  {
    DUMPERLOGINIT;
    LWARN << "EasyDumper:: Warning: relation " << relType << " not continued";
    out << "    </relation>" << std::endl;
    notClosed=false;
  }
  out << "  </relations>" << std::endl;
}


void EasyDumper::computeFormeIds()
{
  DUMPERLOGINIT;
  LDEBUG << "EasyDumper:: computeFormeIds";
  int i=1;
  for (std::map<uint64_t,Forme*>::const_iterator it=m_formesIndex.begin();
       it!=m_formesIndex.end();
       it++, i++)
  {
    std::ostringstream oss;
    oss << "F" << i++;
    LDEBUG << "EasyDumper:: adding to m_formesIds3: "<<(*it).second->forme<<" ("<<it->first<<") -> " << oss.str();
    m_formesIds[it->first]=oss.str();
  }
  LDEBUG << "EasyDumper:: DONE computeFormeIds";
}

} // end namespace EasyXmlDumper
} // end namespace AnalysisDumpers
} // end namespace LinguisticProcessings
} // end namespace Lima
