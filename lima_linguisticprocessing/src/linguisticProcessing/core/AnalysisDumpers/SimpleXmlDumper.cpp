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
 *   Copyright (C) 2010 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "SimpleXmlDumper.h"
#include "TextDumper.h" // for lTokenPosition comparison function to order tokens

// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "common/Handler/AbstractAnalysisHandler.h"

#include <boost/graph/properties.hpp>

#include <fstream>
#include <deque>
#include <queue>
#include <iostream>

// using namespace std;
//using namespace boost;
using namespace boost::tuples;
using namespace Lima::Common;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::SpecificEntities;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit,SimpleXmlDumper> simpleXmlDumperFactory(SIMPLEXMLDUMPER_CLASSID);

SimpleXmlDumper::SimpleXmlDumper():
AbstractTextualAnalysisDumper(),
m_graph("PosGraph"),
m_property("MICRO"),
m_propertyAccessor(0),
m_propertyManager(0),
m_outputVerbTense(false),
m_outputTStatus(false),
m_encapsulatingTag(""),
m_tenseAccessor(0),
m_tenseManager(0)
{
}

SimpleXmlDumper::~SimpleXmlDumper()
{
}

void SimpleXmlDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  DUMPERLOGINIT;
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value

  try {
    m_property=unitConfiguration.getParamsValueAtKey("property");
  }
  catch (NoSuchParam& ) {} // keep default value

  const auto& codeManager=static_cast<const LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
  m_propertyAccessor=&codeManager.getPropertyAccessor(m_property);
  m_propertyManager=&codeManager.getPropertyManager(m_property);

  try {
    std::string str=unitConfiguration.getParamsValueAtKey("outputTStatus");
    if (str=="yes" || str=="1") {
      m_outputTStatus=true;
      LINFO << "activate outputTStatus";
    }
  }
  catch (NoSuchParam& ) {} // keep default value

  try {
    std::string str=unitConfiguration.getParamsValueAtKey("outputVerbTense");
    if (str=="yes" || str=="1") {
      m_outputVerbTense=true;
      QString timeCode = static_cast<const LanguageData&>(
        Common::MediaticData::MediaticData::single().mediaData(m_language)).getLimaToLanguageCodeMappingValue("TIME");
      m_tenseManager=&codeManager.getPropertyManager(timeCode.toUtf8().constData());
      m_tenseAccessor=&codeManager.getPropertyAccessor(timeCode.toUtf8().constData());
      LINFO << "activate outputVerbTense";
    }
  }
 catch (NoSuchParam& ) {} // keep default value

 try {
   std::string str=unitConfiguration.getParamsValueAtKey("encapsulatingTag");
   m_encapsulatingTag=str;
   LINFO << "set encapsulatingTag: "<< m_encapsulatingTag;
 }
 catch (NoSuchParam& ) {
     LDEBUG << "no encapsulatingTag option set. Keep default";
 } // keep default value


}

LimaStatusCode SimpleXmlDumper::
process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  DUMPERLOGINIT;
  LDEBUG << "SimpleXmlDumper::process";

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  if (anagraph==0)
  {
    LERROR << "no graph 'AnaGraph' available !";
    return MISSING_DATA;
  }
  AnalysisGraph* posgraph=static_cast<AnalysisGraph*>(analysis.getData("PosGraph"));
  if (posgraph==0)
  {
    LERROR << "no graph 'PosGraph' available !";
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }

  auto dstream = initialize(analysis);
  xmlOutput(dstream->out(), analysis, anagraph, posgraph, annotationData);

  TimeUtils::logElapsedTime("SimpleXmlDumper");
  return SUCCESS_ID;
}

void SimpleXmlDumper::xmlOutput(
          std::ostream& out,
          AnalysisContent& analysis,
          AnalysisGraph* anagraph,
          AnalysisGraph* posgraph,
          const Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  DUMPERLOGINIT;

  auto metadata = static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));

  auto sb = static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));

  const auto& sp = Common::MediaticData::MediaticData::single().stringsPool(m_language);

  if (m_encapsulatingTag!="" && !m_append) {
      out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\" ?>" << std::endl;
      out << "<"<< m_encapsulatingTag <<">" << std::endl;
  }

  std::string docId("");
  try {
    docId=metadata->getMetaData("FileName");
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: not set in analyzeText (only in analyzeXmlDocuments)
  }


  out << "<DOC id=\""<< docId <<"\">" << std::endl;

  if (sb==nullptr)
  {
    LWARN << "no SentenceBoundaries";
  }

  if (sb==nullptr)
  {
    // no sentence bounds : there can be specific entities,
    // but no compounds (syntactic analysis depend on sentence bounds)
    // dump whole text at once
    xmlOutputVertices(out,
                      anagraph,
                      posgraph,
                      annotationData,
                      anagraph->firstVertex(),
                      anagraph->lastVertex(),
                      sp,
                      metadata->getStartOffset());
  }
  else
  {
    // ??OME2 uint64_t nbSentences(sb->size());
    auto nbSentences = (sb->getSegments()).size();
    LDEBUG << "SimpleXmlDumper: "<< nbSentences << " sentences found";
    for (uint64_t i = 0; i < nbSentences; i++)
    {
      // ??OME2 LinguisticGraphVertex sentenceBegin=(*sb)[i].getFirstVertex();
      // LinguisticGraphVertex sentenceEnd=(*sb)[i].getLastVertex();
      auto sentenceBegin = (sb->getSegments())[i].getFirstVertex();
      auto sentenceEnd = (sb->getSegments())[i].getLastVertex();

      // if (sentenceEnd==posgraph->lastVertex()) {
      //   continue;
      // }

      LDEBUG << "dump sentence between " << sentenceBegin << " and " << sentenceEnd;
      LDEBUG << "dump simple terms for this sentence";

      std::ostringstream oss;
      xmlOutputVertices(oss,
                        anagraph,
                        posgraph,
                        annotationData,
                        sentenceBegin,
                        sentenceEnd,
                        sp,
                        metadata->getStartOffset());
      std::string str = oss.str();
      if (str.empty())
      {
        LDEBUG << "nothing to dump in this sentence";
      }
      else
      {
        out << "<s id=\"" << i << "\">" << std::endl
            << str
            << "</s>" << std::endl;
      }
    }
  }
  out << "</DOC>" << std::endl;
  if(m_encapsulatingTag != "") {
    out << "</"<< m_encapsulatingTag <<">" << std::endl;
  }
}

void SimpleXmlDumper::xmlOutputVertices(
                  std::ostream& out,
                  AnalysisGraph* anagraph,
                  AnalysisGraph* posgraph,
                  const Common::AnnotationGraphs::AnnotationData* annotationData,
                  const LinguisticGraphVertex begin,
                  const LinguisticGraphVertex end,
                  const FsaStringsPool& sp,
                  const uint64_t offset) const
{
  DUMPERLOGINIT;
  LDEBUG << "SimpleXmlDumper: ========================================";
  LDEBUG << "SimpleXmlDumper: outputXml from vertex "  << begin << " to vertex " << end;

  auto graph = posgraph->getGraph();
  auto lastVertex = posgraph->lastVertex();

  std::map<Token*,
           std::vector< std::pair<LinguisticGraphVertex, MorphoSyntacticData*> >,
           lTokenPosition> sortedTokens;

  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;

  LinguisticGraphOutEdgeIt outItr, outItrEnd;

  // output vertices between begin and end,
  // but do not include begin (beginning of text or previous end of sentence) and include end (end of sentence)
  toVisit.push(begin);

  auto first = true;
  auto last = false;
  while (!toVisit.empty())
  {
    auto v = toVisit.front();
    toVisit.pop();
    if (last || v == lastVertex)
    {
      continue;
    }
    if (v == end)
    {
      last = true;
    }

    for (boost::tie(outItr, outItrEnd) = out_edges(v, *graph);
         outItr != outItrEnd; outItr++)
    {
      auto next = target(*outItr,*graph);
      if (visited.find(next) == visited.end())
      {
        visited.insert(next);
        toVisit.push(next);
      }
    }

    if (first)
    {
      first=false;
    }
    else
    {
      auto t = get(vertex_token,*graph,v);
      if( t != 0)
      {
        sortedTokens[t].push_back(make_pair(v, get(vertex_data, *graph, v)));
      }
    }
  }

  for (auto it = sortedTokens.begin(), it_end = sortedTokens.end();
       it!=it_end; it++)
  {
    if ((*it).second.size() == 0)
    {
      continue;
    }

    // if several interpreation of the token (i.e several LinguisticGraphVertex associated),
    // it is not a specific entity, => then just print all interpretations
    else if ((*it).second.size() > 1)
    {
      std::vector<MorphoSyntacticData*> data;
      for (auto d = (*it).second.begin(), d_end = (*it).second.end();
           d != d_end; d++)
      {
        data.push_back((*d).second);
      }
      xmlOutputVertexInfos(out, (*it).first, data, sp, offset);
    }
    else
    {
      xmlOutputVertex(out, (*it).second[0].first, (*it).first, anagraph,
                      posgraph, annotationData, sp, offset);
    }
  }
}

void SimpleXmlDumper::xmlOutputVertex(
                std::ostream& out,
                LinguisticGraphVertex v,
                const Token* ft,
                AnalysisGraph* anagraph,
                AnalysisGraph* posgraph,
                const Common::AnnotationGraphs::AnnotationData* annotationData,
                const FsaStringsPool& sp,
                uint64_t offset) const
{
  auto data = get(vertex_data, *(posgraph->getGraph()), v);

  // first, check if vertex corresponds to a specific entity found before pos
  // tagging (i.e. in analysis graph)
  auto anaVertices = annotationData->matches("PosGraph", v, "AnalysisGraph");
  // note: anaVertices size should be 0 or 1
  for (auto anaVerticesIt = anaVertices.begin();
       anaVerticesIt != anaVertices.end(); anaVerticesIt++)
  {
    auto matches = annotationData->matches("AnalysisGraph",
                                           *anaVerticesIt,
                                           "annot");
    for (auto vx : matches)
    {
      if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
      {
        auto se = annotationData->annotation(
          vx, QString::fromUtf8("SpecificEntity")).
            pointerValue<SpecificEntityAnnotation>();
        if (outputSpecificEntity(out, se, data, anagraph->getGraph(), sp, offset))
        {
          return;
        }
        else
        {
          DUMPERLOGINIT;
          LERROR << "failed to output specific entity for vertex " << v;
        }
      }
    }
  }

  // then check if vertex corresponds to a specific entity found after POS tagging
  auto matches = annotationData->matches("PosGraph", v, "annot");
  for (auto vx : matches)
  {
    if (annotationData->hasAnnotation(vx, QString::fromUtf8("SpecificEntity")))
    {
      //BoWToken* se = createSpecificEntity(v,*it, annotationData, anagraph, posgraph, offsetBegin);
      auto se = annotationData->annotation(vx, QString::fromUtf8("SpecificEntity")).
        pointerValue<SpecificEntityAnnotation>();

      if (outputSpecificEntity(out, se, data, posgraph->getGraph(), sp, offset))
      {
        return;
      }
      else
      {
        DUMPERLOGINIT;
        LERROR << "failed to output specific entity for vertex " << v;
      }
    }
  }

  // if not a specific entity at all, output simple word infos
  xmlOutputVertexInfos(out, ft, std::vector<MorphoSyntacticData*>(1,data), sp, offset);
}

void SimpleXmlDumper::xmlOutputVertexInfos(
                     std::ostream& out,
                     const Token* ft,
                     const std::vector<MorphoSyntacticData*>& data,
                     const FsaStringsPool& sp,
                     uint64_t offset,
                     LinguisticCode category) const
{
  ltNormProperty sorter(m_propertyAccessor);
  auto position = ft->position() + offset;

  auto output = false;

  for (auto dataItr = data.cbegin(), dataItr_end = data.cend();
       dataItr!=dataItr_end; dataItr++)
  {
    auto data = *dataItr;
    sort(data->begin(), data->end(), sorter);
    StringsPoolIndex norm(0),curNorm(0);
    LinguisticCode micro(0),curMicro(0);
    for (auto elemItr = data->cbegin(); elemItr != data->cend(); elemItr++)
    {
      curNorm = elemItr->normalizedForm;
      curMicro = m_propertyAccessor->readValue(elemItr->properties);
      if ((curNorm != norm) || (curMicro != micro))
      {
        norm = curNorm;
        micro = curMicro;

        // if category is specified, output first data (lemma) compatible with this category
        if (category == LinguisticCode(0) || category==curMicro)
        {
          out << "<w p=\"" << position <<  "\""
              << " inf=\"" << xmlString(ft->stringForm().toStdString()) << "\""
              << " pos=\"" << m_propertyManager->getPropertySymbolicValue(curMicro) << "\""
              << " lemma=\"" << xmlString(sp[norm].toStdString()) << "\"";
          if (m_outputTStatus)
          {
            out << " tok=\"" << ft->status().defaultKey() << "\"";
          }
          if (m_outputVerbTense)
          {
            auto tense = m_tenseAccessor->readValue(elemItr->properties);
            if (tense != NONE_1)
            {
              out << " tense=\""
                  << m_tenseManager->getPropertySymbolicValue(tense) << "\"";
            }
          }
          out << "/>" << std::endl;
          output=true;
        }
      }
    }
  }

  // if category is specified and no matching data is found for this category: use first data
  if (category != LinguisticCode(0) && ! output)
  {
    auto norm = data.front()->begin()->normalizedForm;
    out << "<w p=\"" << position <<  "\""
        << " inf=\"" << xmlString(ft->stringForm().toStdString()) << "\""
        << " pos=\"" << m_propertyManager->getPropertySymbolicValue(category) << "\""
        << " lemma=\"" << xmlString(sp[norm].toStdString()) << "\""
        << "/>" << std::endl;
  }
}

bool SimpleXmlDumper::outputSpecificEntity(
                     std::ostream& out,
                     const SpecificEntityAnnotation* se,
                     MorphoSyntacticData* data,
                     const LinguisticGraph* graph,
                     const FsaStringsPool& sp,
                     const uint64_t offset) const
{
  if (se == nullptr)
  {
    DUMPERLOGINIT;
    LERROR << "missing specific entity annotation";
    return false;
  }

  std::string typeName;
  std::string norm;
  try
  {
    auto str = Common::MediaticData::MediaticData::single().getEntityName(se->getType());
    typeName = str.toStdString();
  }
  catch (std::exception& ) {
    DUMPERLOGINIT;
    LERROR << "Undefined entity type " << se->getType();
    return false;
  }
  out
    << "<e type=\"" << typeName << "\""
    << " inf=\"" << xmlString(sp[se->getString()].toStdString()) << "\""
    << " norm=\"" << xmlString(sp[se->getNormalizedForm()].toStdString()) << "\""
    << ">" << std::endl;

  // take as category for parts the category for the named entity
  auto category = m_propertyAccessor->readValue(data->begin()->properties);
  DUMPERLOGINIT;
  LDEBUG << "Using category "
          << m_propertyManager->getPropertySymbolicValue(category)
          << " for specific entity of type " << typeName;

  // get the parts of the named entity match
  // use the category of the named entity for all elements
  for (auto m = se->vertices().cbegin(); m != se->vertices().cend(); m++)
  {
    auto token = get(vertex_token, *graph, *m);
    if (token != nullptr)
    {
      auto vertexData = get(vertex_data, *graph, *m);
      xmlOutputVertexInfos(out, token,
                           std::vector<MorphoSyntacticData*>(1, vertexData),
                           sp,offset,category);
    }
  }
  out << "</e>" << std::endl;
  return true;
}

// string manipulation functions to protect XML entities
std::string SimpleXmlDumper::xmlString(const std::string& inputStr) const
{
  // protect XML entities
  std::string str(inputStr);
  replace(str,"&", "&amp;");
  replace(str,"<", "&lt;");
  replace(str,">", "&gt;");
  replace(str,"\"", "&quot;");
  replace(str,"\n", "\n");
  return str;
}

void SimpleXmlDumper::replace(std::string& str,
                              const std::string& toReplace,
                              const std::string& newValue) const
{
  auto oldLen = toReplace.size();
  auto newLen = newValue.size();
  auto i = str.find(toReplace);
  while (i != std::string::npos)
  {
    str.replace(i, oldLen, newValue);
    i += newLen;
    i = str.find(toReplace, i);
  }
}


} // AnalysisDumper
} // LinguisticProcessing
} // Lima
