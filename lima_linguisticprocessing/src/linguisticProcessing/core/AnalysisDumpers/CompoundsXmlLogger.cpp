/*
    Copyright 2002-2013 CEA LIST

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
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "CompoundsXmlLogger.h"
#include "BowGeneration.h"

#include "common/time/timeUtilsController.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include <iostream>
#include <fstream>
#include <queue>

using namespace boost;
using namespace boost::tuples;

typedef color_traits<default_color_type> Color;

using namespace Lima::Common;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::Automaton;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDumpers;

namespace Lima
{
namespace LinguisticProcessing
{
namespace Compounds
{

SimpleFactory<MediaProcessUnit,CompoundsXmlLogger> compoundsXmlLoggerFactory(COMPOUNDSXMLLOGGER_CLASSID);

CompoundsXmlLogger::CompoundsXmlLogger():
AbstractLinguisticLogger(".compounds.xml")
{
  m_bowGenerator = new BowGenerator();
}


CompoundsXmlLogger::~CompoundsXmlLogger()
{
  delete m_bowGenerator;
}

void CompoundsXmlLogger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractLinguisticLogger::init(unitConfiguration,manager);
  m_language=manager->getInitializationParameters().media;
  m_bowGenerator->init(unitConfiguration, m_language);
}

LimaStatusCode CompoundsXmlLogger::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("CompoundsXmlLogger");

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    COMPOUNDSLOGINIT;
    LERROR << "no LinguisticMetaData ! abort" << LENDL;
    return MISSING_DATA;
  }

  std::ofstream outputStream;
  if (!openLogFile(outputStream,metadata->getMetaData("FileName"))) {
    SALOGINIT;
    LERROR << "Error: cannot open log file" << LENDL;
    return CANNOT_OPEN_FILE_ERROR;
  }
  SALOGINIT;

  const SyntacticData* syntacticData=static_cast<const SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    LERROR << "no SyntacticData ! abort" << LENDL;
    return MISSING_DATA;
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anagraph==0)
  {
    LERROR << "no AnalysisGraph ! abort" << LENDL;
    return MISSING_DATA;
  }
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "no PosGraph ! abort" << LENDL;
    return MISSING_DATA;
  }
  SegmentationData* sb=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sb==0)
  {
    LERROR << "no SentenceBounds ! abort" << LENDL;
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !" << LENDL;
    return MISSING_DATA;
  }

  std::set< std::pair<size_t, size_t> > alreadyDumped;

  outputStream << "<?xml version='1.0' encoding='UTF-8'?>" << std::endl;
  outputStream << "<compounds_dump>" << std::endl;


  //LinguisticGraphVertex sentenceBegin=sb->getStartVertex();
  // ??OME2 SegmentationData::iterator sbItr=sb->begin();
  std::vector<Segment>::iterator sbItr=(sb->getSegments()).begin();

  uint64_t sentNum = 1;
  // ??OME2 while (sbItr!=sb->end())
  while (sbItr!=(sb->getSegments()).end())
  {
    LinguisticGraphVertex beginSentence=sbItr->getFirstVertex();
    LinguisticGraphVertex endSentence=sbItr->getLastVertex();

    dumpLimaData(outputStream,
                  sentNum,
                  beginSentence,
                  endSentence,
                  anagraph,
                  posgraph,
                  syntacticData,
                  annotationData);

    sbItr++;
    sentNum++;
  }

  outputStream << "</compounds_dump>" << std::endl;
  outputStream.close();

  return SUCCESS_ID;
}


//***********************************************************************
// main function for outputing the graph
//***********************************************************************
void CompoundsXmlLogger::dumpLimaData(
  std::ostream& os,
  uint64_t sentNum,
  const LinguisticGraphVertex begin,
  const LinguisticGraphVertex end,
                                        const AnalysisGraph* anagraph,
                                        const AnalysisGraph* posgraph,
                                        const SyntacticData* syntacticData,
  const Common::AnnotationGraphs::AnnotationData* annotationData,
  const uint64_t offsetBegin) const
{
//   COMPOUNDSLOGINIT;
  //  LinguisticGraph* graph = const_cast< LinguisticGraph* >(posgraph->getGraph());
  // go through the graph, add BoWTokens that are not in complex terms

  os << "<sentence num=\"" << sentNum << "\" >" << std::endl;
  // dump compounds
  os << "<compounds>" << std::endl;
  LinguisticGraphVertex firstVx = posgraph->firstVertex();
  LinguisticGraphVertex lastVx = posgraph->lastVertex();

  const LinguisticGraph& lanagraph=*(anagraph->getGraph());
  const LinguisticGraph& lposgraph=*(posgraph->getGraph());
  std::set< std::string > alreadyStored;
  std::set<LinguisticGraphVertex> visited;
  std::queue<LinguisticGraphVertex> toVisit;
  toVisit.push(begin);

  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  while (!toVisit.empty())
  {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (v == end) {
      continue;
    }

    for (boost::tie(outItr,outItrEnd)=out_edges(v,lposgraph);
         outItr!=outItrEnd;
         outItr++)
    {
      LinguisticGraphVertex next=target(*outItr,lposgraph);
      if (visited.find(next)==visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }

    if (v != firstVx && v != lastVx)
    {
      /// @todo replace v in LDEBUGs below by matching annotation vertices
//     LDEBUG << "hasAnnotation("<<v<<", CompoundTokenAnnotation): "
//         << annotationData->hasAnnotation(v, Common::Misc::utf8stdstring2limastring("CompoundTokenAnnotation")) << LENDL;
      //std::set< uint64_t > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead"); portage 32 64
      std::set< AnnotationGraphVertex > cpdsHeads = annotationData->matches("PosGraph", v, "cpdHead");
      if (!cpdsHeads.empty())
      {
        std::set< AnnotationGraphVertex >::const_iterator cpdsHeadsIt, cpdsHeadsIt_end;
        cpdsHeadsIt = cpdsHeads.begin(); cpdsHeadsIt_end = cpdsHeads.end();
        for (; cpdsHeadsIt != cpdsHeadsIt_end; cpdsHeadsIt++)
        {
          AnnotationGraphVertex agv  = *cpdsHeadsIt;
          std::vector<std::pair<BoWRelation*, BoWToken*> > bowTokens = 
            m_bowGenerator->buildTermFor(agv, agv, lanagraph, lposgraph, offsetBegin, 
                                         syntacticData, annotationData, visited);
          for (std::vector<std::pair<BoWRelation*, BoWToken*> >::const_iterator bowItr=bowTokens.begin();
               bowItr!=bowTokens.end();
               bowItr++)
          {
            std::string elem = (*bowItr).second->getIdUTF8String();
            if (alreadyStored.find(elem) != alreadyStored.end())
            { // already stored
              //          LDEBUG << "BuildBoWTokenListVisitor: BoWToken already stored. Skipping it." << LENDL;
              delete (*bowItr).first;
              delete (*bowItr).second;
            }
            else
            {
              outputCompound(os,(*bowItr).second,offsetBegin);
              alreadyStored.insert(elem);
            }
          }
        }
      }
    }
  }
  os << "</compounds>" << std::endl;
  os << "</sentence>" << std::endl;
}

//***********************************************************************
// output functions
//***********************************************************************


uint64_t CompoundsXmlLogger::getPosition(const uint64_t position,
    const uint64_t offsetBegin) const
{
  return (offsetBegin+position);
}


void CompoundsXmlLogger::outputCompound(
  std::ostream& os,
  const BoWToken* compound,
  const uint64_t offsetBegin) const
{
  COMPOUNDSLOGINIT;
  LDEBUG << "Outputing compound: " << compound << LENDL;
  uint64_t pos = getPosition(compound->getPosition(),offsetBegin);
  LimaString form = compound->getLemma();
  std::string cat = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(compound->getCategory());
  os << "<compound pos=\"" << pos << "\" form=\"" << form
  << "\" cat=\"" << cat << "\" >" << std::endl;
  const BoWComplexToken* term = dynamic_cast<const BoWComplexToken*>(compound);
  if (term != 0)
  {
    uint64_t headId = term->getHead();
    std::deque< BoWComplexToken::Part >::const_iterator partIt, partIt_end;
    partIt = term->getParts().begin();
    partIt_end = term->getParts().end();
    for (uint64_t partId=0; partIt != partIt_end; partIt++,partId++)
    {
      const BoWToken* partTok = (*partIt).get<1>();
      //        bool head = (*partIt).second;
      os << "<part head=\"" << std::boolalpha << (partId == headId) << "\" >" << std::endl;
      if (dynamic_cast<const BoWComplexToken*>(partTok))
      {
        outputCompound(os, const_cast<BoWToken*>(partTok), offsetBegin);
      }
      else
      {
        LDEBUG << "    part: " << partTok << LENDL;
        uint64_t partPos = getPosition(partTok->getPosition(),offsetBegin);
        LimaString partForm = partTok->getLemma();
        std::string partCat = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(partTok->getCategory()) ;
        os << "<word pos=\"" << partPos << "\" form=\"" << partForm
        << "\" cat=\"" << partCat << "\" />" << std::endl;
      }
      os << "</part>" << std::endl;
    }
  }
  os << "</compound>" << std::endl;
}

} // SyntacticAnalysis
} // LinguisticProcessing
} // Lima
