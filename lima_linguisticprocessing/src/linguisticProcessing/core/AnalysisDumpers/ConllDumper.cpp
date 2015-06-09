/*
    Copyright 2002-2014 CEA LIST

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

#include "ConllDumper.h"
#include "common/MediaProcessors/DumperStream.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationGraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "common/misc/AbstractAccessByString.h"
#include "linguisticProcessing/core/AnalysisDumpers/EasyXmlDumper/ConstituantAndRelationExtractor.h"
#include "linguisticProcessing/core/AnalysisDumpers/EasyXmlDumper/relation.h"
#include "linguisticProcessing/core/SemanticAnalysis/LimaConllTokenIdMapping.h"

#include <QQueue>
#include <QSet>
#include <QStringList>

#include <fstream>

using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<MediaProcessUnit,ConllDumper> conllDumperFactory(CONLLDUMPER_CLASSID);

class ConllDumperPrivate
{
  friend class ConllDumper;
  ConllDumperPrivate();

  virtual ~ConllDumperPrivate();

  /**
   * @brief Collect all annotation tokens corresponding to a predicate of the
   * sentence starting at @ref sentenceBegin and finishing at @ref sentenceEnd
   */
  QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex>  collectPredicateTokens(
                                  Lima::AnalysisContent& analysis, LinguisticGraphVertex sentenceBegin, LinguisticGraphVertex sentenceEnd);

  MediaId m_language;
  std::string m_property;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  const Common::PropertyCode::PropertyManager* m_propertyManager;
  const Common::PropertyCode::PropertyManager* m_timeManager; //Ajout
  const Common::PropertyCode::PropertyAccessor* m_timeAccessor; //Ajout

  std::string m_graph;
  std::string m_sep;
  std::string m_sepPOS;
  std::string m_verbTenseFlag; //Ajout
  QMap<QString, QString> m_conllLimaDepMapping;
  std::string m_suffix;
};


ConllDumperPrivate::ConllDumperPrivate():
m_language(0),
m_property("MICRO"),
m_propertyAccessor(0),
m_propertyManager(0),
m_graph("PosGraph"),
m_sep(" "),
m_sepPOS("#"),
m_conllLimaDepMapping(),
m_suffix(".conll")
{
}

ConllDumperPrivate::~ConllDumperPrivate()
{}

ConllDumper::ConllDumper():
AbstractTextualAnalysisDumper(),
m_d(new ConllDumperPrivate())
{
}

ConllDumper::~ConllDumper()
{
  delete m_d;
}

void ConllDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)
{
  DUMPERLOGINIT;
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  m_d->m_language=manager->getInitializationParameters().media;
  try
  {
    m_d->m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value
  const Common::PropertyCode::PropertyCodeManager& codeManager=static_cast<const Common::MediaticData     ::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager();
  m_d->m_propertyAccessor=&codeManager.getPropertyAccessor("MICRO");

  try
  {
    m_d->m_verbTenseFlag=unitConfiguration.getParamsValueAtKey("verbTenseFlag");
  }
  catch (NoSuchParam& ) {
    m_d->m_verbTenseFlag=std::string("False");
  } // keep default value

  try
  {
    m_d->m_sep=unitConfiguration.getParamsValueAtKey("sep");
  }
  catch (NoSuchParam& ) {} // keep default value

  try
  {
    m_d->m_sepPOS=unitConfiguration.getParamsValueAtKey("sepPOS");
  }
  catch (NoSuchParam& ) {} // keep default value

  try
  {
    m_d->m_property=unitConfiguration.getParamsValueAtKey("property");
  }
  catch (NoSuchParam& ) {} // keep default value
  try
  {
    m_d->m_suffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
  catch (NoSuchParam& ) {} // keep default value
  m_d->m_propertyManager=&codeManager.getPropertyManager(m_d->m_property);

  m_d->m_timeManager=&codeManager.getPropertyManager("TIME");
  m_d->m_timeAccessor=&codeManager.getPropertyAccessor("TIME");
  
  try {
    std::string resourcePath = Common::MediaticData::MediaticData::single().getResourcesPath();
    std::string mappingFile = resourcePath + "/" + unitConfiguration.getParamsValueAtKey("mappingFile");
    std::ifstream ifs(mappingFile, std::ifstream::binary);
    if (!ifs.good())
    {
      LERROR << "ERROR: cannot open"+ mappingFile;
      throw InvalidConfiguration();
    }
    while (ifs.good() && !ifs.eof())
    {
      std::string line;
      while(getline(ifs, line))  // as long as we can put the line on "line"
      {
        QStringList strs = QString::fromUtf8(line.c_str()).split('\t');
        m_d->m_conllLimaDepMapping.insert(strs[0],strs[1]);
      }
    }

  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no parameter 'mappingFile' in ConllDumper group" << " !";
    throw InvalidConfiguration();
  }
}

LimaStatusCode ConllDumper::process(AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
  LDEBUG << "ConllDumper::process";

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "ConllDumper::process no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  if (annotationData == 0) {
      LERROR << "ConllDumper::process no AnnotationData ! abort";
      return MISSING_DATA;
  }
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_d->m_graph));//est de type PosGraph et non pas AnalysisGraph
  if (tokenList==0) {
    LERROR << "ConllDumper::process graph " << m_d->m_graph << " has not been produced: check pipeline";
    return MISSING_DATA;
  }
  LinguisticGraph* graph=tokenList->getGraph();
  SegmentationData* sd=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sd==0) {
    LERROR << "ConllDumper::process no SentenceBoundaries! abort";
    return MISSING_DATA;
  }

  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    syntacticData=new SyntacticData(tokenList,0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }
  const DependencyGraph* depGraph = syntacticData-> dependencyGraph();

  QScopedPointer<DumperStream> dstream(initialize(analysis));

  std::map< LinguisticGraphVertex, std::pair<LinguisticGraphVertex, std::string> > vertexDependencyInformations;

  std::vector<Segment>::iterator sbItr=(sd->getSegments().begin());
  uint64_t nbSentences((sd->getSegments()).size());
  LDEBUG << "ConllDumper::process There are "<< nbSentences << " sentences";
  LinguisticGraphVertex sentenceBegin = sbItr->getFirstVertex();
  LinguisticGraphVertex sentenceEnd = sbItr->getLastVertex();


    const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_d->m_language);
//   for (auto im=m_d->m_conllLimaDepMapping.begin();im!=m_d->m_conllLimaDepMapping.end();im++)
//   {
//     LDEBUG << "("<< (*im).first<< "," << (*im).second << ")" << endl;
//   }

  LimaConllTokenIdMapping* limaConllTokenIdMapping = static_cast<LimaConllTokenIdMapping*>(analysis.getData("LimaConllTokenIdMapping"));
  if (limaConllTokenIdMapping == 0)
  {
    limaConllTokenIdMapping = new LimaConllTokenIdMapping();
    analysis.setData("LimaConllTokenIdMapping", limaConllTokenIdMapping);
  }
  int sentenceNb=0;

  while (sbItr != sd->getSegments().end() ) //for each sentence
  {
    sentenceNb++;
    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();
    std::map<LinguisticGraphVertex,int>segmentationMapping;//mapping the two types of segmentations (Lima and conll)
    std::map<int,LinguisticGraphVertex>segmentationMappingReverse;

    LDEBUG << "ConllDumper::process begin - end: " << sentenceBegin << " - " << sentenceEnd;
    //LinguisticGraphOutEdgeIt outItr,outItrEnd;
    QQueue<LinguisticGraphVertex> toVisit;
    QSet<LinguisticGraphVertex> visited;
    toVisit.enqueue(sentenceBegin);
    int tokenId = 0;
    LinguisticGraphVertex v = 0;
    while (v != sentenceEnd && !toVisit.empty())

    {
      v = toVisit.dequeue();
      LDEBUG << "ConllDumper::process Vertex index : " << v;
      visited.insert(v);
      segmentationMapping.insert(std::make_pair(v,tokenId));
      segmentationMappingReverse.insert(std::make_pair(tokenId,v));
      LDEBUG << "ConllDumper::process conll id : " << tokenId << " Lima id : " << v;
      DependencyGraphVertex dcurrent = syntacticData->depVertexForTokenVertex(v);
      DependencyGraphOutEdgeIt dit, dit_end;
      boost::tie(dit,dit_end) = boost::out_edges(dcurrent,*depGraph);
      for (; dit != dit_end; dit++)
      {
        LDEBUG << "ConllDumper::process Dumping dependency edge " << (*dit).m_source << " -> " << (*dit).m_target;
        try
        {
          CEdgeDepRelTypePropertyMap typeMap = get(edge_deprel_type, *depGraph);
          SyntacticRelationId type = typeMap[*dit];
          std::string syntRelName=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getSyntacticRelationName(type);
          LDEBUG << "ConllDumper::process relation = " << syntRelName;
          LDEBUG << "ConllDumper::process Src  : Dep vertex= " << boost::source(*dit, *depGraph);
          LinguisticGraphVertex src = syntacticData->tokenVertexForDepVertex(boost::source(*dit, *depGraph));
          LDEBUG << "ConllDumper::process Src  : Morph vertex= " << src;
          LDEBUG << "ConllDumper::process Targ : Dep vertex= " << boost::target(*dit, *depGraph);
          LinguisticGraphVertex dest = syntacticData->tokenVertexForDepVertex(boost::target(*dit, *depGraph));
          LDEBUG << "ConllDumper::process Targ : Morph vertex= " << dest;
          if (syntRelName!="")
          {
            LDEBUG << "ConllDumper::process saving target for" << v << ":" << dest << syntRelName;
            vertexDependencyInformations.insert(std::make_pair(v, std::make_pair(dest,syntRelName)));
          }
        }
        catch (const std::range_error& )
        {
        }
        catch (...)
        {
          LDEBUG << "ConllDumper::process: catch others.....";
        throw;
        }
      }
      if (v == sentenceEnd)
      {
        continue;
      }
      LinguisticGraphOutEdgeIt outItr,outItrEnd;
      for (boost::tie(outItr,outItrEnd)=boost::out_edges(v,*graph); outItr!=outItrEnd; outItr++)
      {
        LinguisticGraphVertex next=boost::target(*outItr,*graph);
        if (!visited.contains(next) && next != tokenList->lastVertex())
        {
          toVisit.enqueue(next);
        }
      }
      ++tokenId;
    }

  // instead of looking to all vertices, follow the graph (in
  // morphological graph, some vertices are not related to main graph:
  // idiomatic expressions parts and named entity parts)

    toVisit.clear();
    visited.clear();

    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();

    // get the list of predicates for the current sentence
    QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex > predicates = m_d->collectPredicateTokens( analysis, sentenceBegin, sentenceEnd );
    LDEBUG << "ConllDumper::process predicates for sentence between" << sentenceBegin << "and" << sentenceEnd << "are:" << predicates;

    toVisit.enqueue(sentenceBegin);
    tokenId=0;
    v=0;
    while (!toVisit.empty() && v!=sentenceEnd)
    { //as long as there are vertices in the sentence
      v = toVisit.dequeue();

      Token* ft=get(vertex_token,*graph,v);
      MorphoSyntacticData* morphoData=get(vertex_data,*graph, v);
      LDEBUG << "ConllDumper::process PosGraph token" << v;
      if( morphoData!=0 )
      {
        const QString graphTag=QString::fromUtf8(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertySymbolicValue(morphoData->firstValue(*m_d->m_propertyAccessor)).c_str());

        std::string inflectedToken=ft->stringForm().toUtf8().constData();
        std::string lemmatizedToken;
        if (morphoData != 0 && !morphoData->empty())
        {
          lemmatizedToken=sp[(*morphoData)[0].lemma].toUtf8().constData();
        }

        QString conllRelName = "-";
        int targetConllId = 0;
        if (vertexDependencyInformations.count(v)!=0)
        {
          LinguisticGraphVertex target=vertexDependencyInformations.find(v)->second.first;
          LDEBUG << "ConllDumper::process target saved for" << v << "is" << target;
          targetConllId=segmentationMapping.find(target)->second;
          LDEBUG << "ConllDumper::process conll target saved for " << tokenId << " is " << targetConllId;
          QString relName = QString::fromUtf8(vertexDependencyInformations.find(v)->second.second.c_str());
          LDEBUG << "ConllDumper::process the lima dependency tag for "
                 << ft->stringForm()<< " is " << relName;
          if (m_d->m_conllLimaDepMapping.contains(relName))
          {
            conllRelName=m_d->m_conllLimaDepMapping[relName];
          }
          else
          {
            LERROR << "ConllDumper::process" << relName << "not found in mapping";
          }
        }
        QString targetConllIdString = targetConllId > 0 ? QString("%1").arg(targetConllId) : "-";
        dstream->out() << tokenId << "\t"<< inflectedToken << "\t"
                        << lemmatizedToken << "\t" << graphTag  << "\t"
                        << graphTag << "\t" << "-" << "\t" << targetConllIdString << "\t"
                        << conllRelName.toUtf8().constData() << "\t-\t-";
        if (!predicates.isEmpty())
        {
          dstream->out() << "\t";
          LDEBUG << "ConllDumper::process output the predicate if any";
          if (!predicates.contains(v))
          {
            // No predicate for this token
            dstream->out() << "-";
          }
          else
          {
            // This token is a predicate, output it
            QString predicateAnnotation = annotationData->stringAnnotation(predicates.value(v),"Predicate");
            dstream->out() << predicateAnnotation;
          }
          // Now output the roles supported by the current PoS graph token

          LDEBUG << "ConllDumper::process output the roles for the" << predicates.keys().size() << "predicates";
          for (int i = 0; i < predicates.keys().size(); i++)
          {
            // There will be one column for each predicate. Output the
            // separator right now
            dstream->out() << "\t";
            AnnotationGraphVertex predicateVertex = predicates.value(predicates.keys()[i]);

            std::set< AnnotationGraphVertex > vMatches = annotationData->matches("PosGraph", v, "annot");
            if (vMatches.empty())
            {
              LDEBUG << "ConllDumper::process no node matching PoS graph vertex" << v << "in the annotation graph. Output '-'.";
              dstream->out() << "-";
            }
            else
            {
              LDEBUG << "ConllDumper::process there is"<<vMatches.size()<<"nodes matching PoS graph vertex" << v << "in the annotation graph.";
              QString roleAnnotation = "-";
              for (auto it = vMatches.begin(); it != vMatches.end(); it++)
              {
                AnnotationGraphVertex vMatch = *it;
                AnnotationGraphInEdgeIt vMatchInEdgesIt, vMatchInEdgesIt_end;
                boost::tie(vMatchInEdgesIt, vMatchInEdgesIt_end) = boost::in_edges(vMatch,annotationData->getGraph());
                for (; vMatchInEdgesIt != vMatchInEdgesIt_end; vMatchInEdgesIt++)
                {
                  AnnotationGraphVertex inVertex = boost::source(*vMatchInEdgesIt, annotationData->getGraph());
                  std::set< LinguisticGraphVertex > inVertexAnnotPosGraphMatches = annotationData->matches("annot",inVertex,"PosGraph");
                  if (inVertex == predicateVertex && !inVertexAnnotPosGraphMatches.empty())
                  {
                    // Current edge is holding a role of the current predicate
                    roleAnnotation = annotationData->stringAnnotation(*vMatchInEdgesIt,"SemanticRole");
                    break;
                  }
                  else
                  {
                    // Current edge does not hold a role of the current predicate
//                     dstream->out() << "-";
                  }
                }
                if (roleAnnotation != "-") break;
              }
              dstream->out() << roleAnnotation.toUtf8().constData();
            }
          }
        }
        dstream->out() << std::endl;
      }

      if (v == sentenceEnd)
      {
        continue;
      }
      LDEBUG << "ConllDumper::process look at out edges of" << v;
      LinguisticGraphOutEdgeIt outIter,outIterEnd;
      for (boost::tie(outIter,outIterEnd) = boost::out_edges(v,*graph); outIter!=outIterEnd; outIter++)
      {
        LinguisticGraphVertex next = boost::target(*outIter,*graph);
        LDEBUG << "ConllDumper::process looking out vertex" << next;
        if (!visited.contains(next))
        {
          LDEBUG << "ConllDumper::process enqueuing" << next;
          visited.insert(next);
          toVisit.enqueue(next);
        }
      }
      tokenId++;
    }
    dstream->out() << std::endl;
    limaConllTokenIdMapping->insert(std::make_pair(sentenceNb, segmentationMappingReverse));
    sbItr++;
  }

  return SUCCESS_ID;

}

QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex> ConllDumperPrivate::collectPredicateTokens(
                                  Lima::AnalysisContent& analysis, LinguisticGraphVertex sentenceBegin, LinguisticGraphVertex sentenceEnd)
{
  DUMPERLOGINIT;
 QMap<LinguisticGraphVertex, AnnotationGraphVertex> result;

  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (tokenList==0) {
    LERROR << "graph " << m_graph << " has not been produced: check pipeline";
    return result;
  }
  LinguisticGraph* graph=tokenList->getGraph();


  QQueue<LinguisticGraphVertex> toVisit;
  QSet<LinguisticGraphVertex> visited;
  toVisit.enqueue(sentenceBegin);
  LinguisticGraphVertex v = 0;
  while (v!=sentenceEnd && !toVisit.empty())
  {
    v = toVisit.dequeue();
    LDEBUG << "ConllDumperPrivate::collectPredicateTokens vertex:" << v;
    visited.insert(v);

    std::set< AnnotationGraphVertex > vMatches = annotationData->matches("PosGraph", v, "annot");
    for (auto it = vMatches.begin(); it != vMatches.end(); it++)
    {
      AnnotationGraphVertex vMatch = *it;
      if (annotationData->hasStringAnnotation(vMatch,"Predicate"))
      {
        LDEBUG << "ConllDumperPrivate::collectPredicateTokens insert" << v <<    vMatch;
        result.insert(v, vMatch);
      }
    }
    LinguisticGraphOutEdgeIt outItr,outItrEnd;bool newSentence(const QString & line);
    for (boost::tie(outItr,outItrEnd)=boost::out_edges(v,*graph); outItr!=outItrEnd; outItr++)
    {
      LinguisticGraphVertex next=boost::target(*outItr,*graph);
      if (!visited.contains(next) && next != tokenList->lastVertex())
      {
        toVisit.enqueue(next);
      }
    }
  }
  return result;
}

} // end namespace
} // end namespace
} // end namespace
