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
#include "common/tools/FileUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/misc/AbstractAccessByString.h"
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
#include "linguisticProcessing/core/AnalysisDumpers/EasyXmlDumper/ConstituantAndRelationExtractor.h"
#include "linguisticProcessing/core/AnalysisDumpers/EasyXmlDumper/relation.h"
#include "linguisticProcessing/core/SemanticAnalysis/LimaConllTokenIdMapping.h"

#include <QQueue>
#include <QSet>
#include <QStringList>

#include <fstream>
#include <QJsonDocument>
#include <QJsonObject>

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
  std::string m_outputFormat;
};

class OutputFormatter
{
public:
  virtual std::string getHeader()=0;
  virtual void pushAttr(const char* name,const std::string& value, bool first = false )=0;
  virtual std::string getTokenOutput()=0;
  virtual std::string getTokenSeparator()=0;
  virtual std::string getTokenTerminator()=0;
  virtual std::string getfooter()=0;
};

class OutputCsvFormatter: public OutputFormatter
{
public:
  std::string getHeader() override;
  void pushAttr(const char* name,const std::string& value, bool first ) override;
  std::string getTokenOutput() override;
  std::string getTokenSeparator() override;
  std::string getTokenTerminator() override;
  virtual std::string getfooter() override;
private:
  std::stringstream m_out;
};

class OutputJsonFormatter: public OutputFormatter
{
public:
  std::string getHeader() override;
  void pushAttr(const char* name,const std::string& value, bool first ) override;
  std::string getTokenOutput() override;
  std::string getTokenSeparator() override;
  std::string getTokenTerminator() override;
  virtual std::string getfooter() override;
private:
  QJsonObject m_jsonToken;
  std::string m_id;
};

std::string OutputCsvFormatter::getHeader() {
  return std::string("");
}

void OutputCsvFormatter::pushAttr(const char* ,const std::string& value, bool first ){
  if( first ) {
    m_out.str("");
  }
  else {
    m_out << "\t";
  }
  m_out << value;
}

std::string OutputCsvFormatter::getTokenOutput() {
  return m_out.str();
}

std::string OutputCsvFormatter::getTokenSeparator() {
  return std::string("");
}

std::string  OutputCsvFormatter::getTokenTerminator() {
  return std::string("\n");
}

std::string OutputCsvFormatter::getfooter() {
  return std::string("");
}

std::string OutputJsonFormatter::getHeader() {
  return std::string("{ \"tokens\": [\n");
}

void OutputJsonFormatter::pushAttr(const char* name,const std::string& value, bool first) {
  if( first ) {
    m_jsonToken = QJsonObject();
    m_id = value;
  }
  m_jsonToken[QString(name)]=QString(value.c_str());
}

std::string  OutputJsonFormatter::getTokenOutput() {
  QJsonDocument doc(m_jsonToken);
  std::string result(doc.toJson().data());
  return result;
}

std::string OutputJsonFormatter::getTokenSeparator() {
  return std::string(",");
}

std::string OutputJsonFormatter::getTokenTerminator() {
  return std::string("");
}

std::string OutputJsonFormatter::getfooter() {
  return std::string("]\n}");
}

ConllDumperPrivate::ConllDumperPrivate():
m_language(0),
m_property("MICRO"),
m_propertyAccessor(0),
m_propertyManager(0),
m_graph("PosGraph"),
m_sep(" "),
m_sepPOS("#"),
m_conllLimaDepMapping(),
m_outputFormat("csv")
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
  const Common::PropertyCode::PropertyCodeManager& codeManager=static_cast<const Common::MediaticData     ::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager();
  m_d->m_propertyAccessor=&codeManager.getPropertyAccessor("MICRO");
  try
  {
    m_d->m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value
  try
  {
    m_d->m_outputFormat=unitConfiguration.getParamsValueAtKey("outputFormat");
  }
  catch (NoSuchParam& ) {} // keep default value = "csv"

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
  m_d->m_propertyManager=&codeManager.getPropertyManager(m_d->m_property);

  m_d->m_timeManager=&codeManager.getPropertyManager("TIME");
  m_d->m_timeAccessor=&codeManager.getPropertyAccessor("TIME");
  
  try {
    std::string resourcePath = Common::MediaticData::MediaticData::single().getResourcesPath();
    QString mappingFile =  Common::Misc::findFileInPaths(resourcePath.c_str(), unitConfiguration.getParamsValueAtKey("mappingFile").c_str());
    std::ifstream ifs(mappingFile.toUtf8().constData(), std::ifstream::binary);
    if (!ifs.good())
    {
      LERROR << "ERROR: cannot open" << mappingFile;
      throw InvalidConfiguration();
    }
    while (ifs.good() && !ifs.eof())
    {
      std::string line = Lima::Common::Misc::readLine(ifs);
      QStringList strs = QString::fromUtf8(line.c_str()).split('\t');
      if (strs.size() == 2)
      {
        m_d->m_conllLimaDepMapping.insert(strs[0],strs[1]);
      }
    }

  } catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LINFO << "no parameter 'mappingFile' in ConllDumper group" << " !";
//     throw InvalidConfiguration();
  }
}

LimaStatusCode ConllDumper::process(AnalysisContent& analysis) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
  LDEBUG << "ConllDumper::process";
#endif

  LinguisticMetaData* metadata = static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) 
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));
  if (annotationData == 0) 
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process no AnnotationData ! abort";
    return MISSING_DATA;
  }
  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_d->m_graph));//est de type PosGraph et non pas AnalysisGraph
  if (tokenList==0) 
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process graph " << m_d->m_graph << " has not been produced: check pipeline";
    return MISSING_DATA;
  }
  LinguisticGraph* graph=tokenList->getGraph();
  SegmentationData* sd=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  if (sd==0) 
  {
    DUMPERLOGINIT;
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
  // stringstream where output string is built for each vertex before being output to dstream->out
  std::stringstream dstreamOutBuff;
  OutputFormatter* outputFormatter=0;
  if( m_d->m_outputFormat.compare("csv") == 0 )
    outputFormatter = new OutputCsvFormatter();
  else
    outputFormatter = new OutputJsonFormatter();

  
  std::map< LinguisticGraphVertex, std::pair<LinguisticGraphVertex, std::string> > vertexDependencyInformations;

  uint64_t nbSentences((sd->getSegments()).size());
  if (nbSentences == 0)
  {
    DUMPERLOGINIT;
    LERROR << "ConllDumper::process 0 sentence to process";
    return SUCCESS_ID;
  }
  
  std::vector<Segment>::iterator sbItr=(sd->getSegments().begin());
#ifdef DEBUG_LP
  LDEBUG << "ConllDumper::process There are "<< nbSentences << " sentences";
#endif
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

#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process begin - end: " << sentenceBegin << " - " << sentenceEnd;
#endif
    //LinguisticGraphOutEdgeIt outItr,outItrEnd;
    QQueue<LinguisticGraphVertex> toVisit;
    QSet<LinguisticGraphVertex> visited;
    toVisit.enqueue(sentenceBegin);
    int tokenId = 0;
    LinguisticGraphVertex v = 0;
    while (v != sentenceEnd && !toVisit.empty())

    {
      v = toVisit.dequeue();
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process Vertex index : " << v;
#endif
      visited.insert(v);
      segmentationMapping.insert(std::make_pair(v,tokenId));
      segmentationMappingReverse.insert(std::make_pair(tokenId,v));
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process conll id : " << tokenId << " Lima id : " << v;
#endif
      DependencyGraphVertex dcurrent = syntacticData->depVertexForTokenVertex(v);
      DependencyGraphOutEdgeIt dit, dit_end;
      boost::tie(dit,dit_end) = boost::out_edges(dcurrent,*depGraph);
      for (; dit != dit_end; dit++)
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process Dumping dependency edge " << (*dit).m_source << " -> " << (*dit).m_target;
#endif
        try
        {
          CEdgeDepRelTypePropertyMap typeMap = get(edge_deprel_type, *depGraph);
          SyntacticRelationId type = typeMap[*dit];
          std::string syntRelName=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getSyntacticRelationName(type);
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process relation = " << syntRelName;
          LDEBUG << "ConllDumper::process Src  : Dep vertex= " << boost::source(*dit, *depGraph);
          LinguisticGraphVertex src = syntacticData->tokenVertexForDepVertex(boost::source(*dit, *depGraph));
          LDEBUG << "ConllDumper::process Src  : Morph vertex= " << src;
          LDEBUG << "ConllDumper::process Targ : Dep vertex= " << boost::target(*dit, *depGraph);
#endif
          LinguisticGraphVertex dest = syntacticData->tokenVertexForDepVertex(boost::target(*dit, *depGraph));
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process Targ : Morph vertex= " << dest;
#endif
          if (syntRelName!="")
          {
#ifdef DEBUG_LP
            LDEBUG << "ConllDumper::process saving target for" << v << ":" << dest << syntRelName;
#endif
            vertexDependencyInformations.insert(std::make_pair(v, std::make_pair(dest,syntRelName)));
          }
        }
        catch (const std::range_error& )
        {
        }
        catch (...)
        {
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process: catch others.....";
#endif
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
#ifdef DEBUG_LP
    LDEBUG << "ConllDumper::process predicates for sentence between" << sentenceBegin << "and" << sentenceEnd << "are:" << predicates;
#endif
    QList< LinguisticGraphVertex > keys = predicates.keys();

    toVisit.enqueue(sentenceBegin);
    tokenId=0;
    v=0;
    bool firstToken = true;
    dstream->out() << outputFormatter->getHeader();

    while (!toVisit.empty() && v!=sentenceEnd)
    { //as long as there are vertices in the sentence
      v = toVisit.dequeue();

      Token* ft=get(vertex_token,*graph,v);
      MorphoSyntacticData* morphoData=get(vertex_data,*graph, v);
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process PosGraph token" << v;
#endif
      if( morphoData!=0 && !morphoData->empty() && ft != 0)
      {
        const QString macro=QString::fromUtf8(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertySymbolicValue(morphoData->firstValue(*m_d->m_propertyAccessor)).c_str());
        const QString micro=QString::fromUtf8(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertySymbolicValue(morphoData->firstValue(*m_d->m_propertyAccessor)).c_str());
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process graphTag:" << micro;
#endif

        std::string inflectedToken=ft->stringForm().toUtf8().constData();
        std::string lemmatizedToken;
        if (morphoData != 0 && !morphoData->empty())
        {
          lemmatizedToken=sp[(*morphoData)[0].lemma].toUtf8().constData();
        }

        QString neType = QString::fromUtf8("_") ;
        std::set< AnnotationGraphVertex > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph");
        // note: anaVertices size should be 0 or 1
        for (std::set< AnnotationGraphVertex >::const_iterator anaVerticesIt = anaVertices.begin();
            anaVerticesIt != anaVertices.end(); anaVerticesIt++)
        {
          std::set< AnnotationGraphVertex > matches = annotationData->matches("AnalysisGraph",*anaVerticesIt,"annot");
          for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
              it != matches.end(); it++)
          {
            AnnotationGraphVertex vx=*it;
            if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
            {
              const SpecificEntityAnnotation* se =
                annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
                pointerValue<SpecificEntityAnnotation>();
              neType = Common::MediaticData::MediaticData::single().getEntityName(se->getType());
              break;
            }
          }
          if (neType != "_") break;
        }
        QString conllRelName = "_";
        int targetConllId = 0;
        if (vertexDependencyInformations.count(v)!=0)
        {
          LinguisticGraphVertex target=vertexDependencyInformations.find(v)->second.first;
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process target saved for" << v << "is" << target;
#endif
          if (segmentationMapping.find(target) != segmentationMapping.end())
          {
            targetConllId=segmentationMapping.find(target)->second;
          }
          else
          {
            DUMPERLOGINIT;
            LERROR << "ConllDumper::process target" << target << "not found in segmentation mapping";
          }
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process conll target saved for " << tokenId << " is " << targetConllId;
#endif
          QString relName = QString::fromUtf8(vertexDependencyInformations.find(v)->second.second.c_str());
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process the lima dependency tag for "
                 << ft->stringForm()<< " is " << relName;
#endif
          if (m_d->m_conllLimaDepMapping.contains(relName))
          {
            conllRelName=m_d->m_conllLimaDepMapping[relName];
          }
          else
          {
            conllRelName= relName;
//             LERROR << "ConllDumper::process" << relName << "not found in mapping";
          }
        }
        // Modified CONLL-X format with an extra named entity type column
        // http://ilk.uvt.nl/conll/#dataformat
        // 1   ID  Token counter, starting at 1 for each new sentence.
        // 2   FORM  Word form or punctuation symbol.
        // 3   LEMMA   Lemma or stem (depending on particular data set) of word form, or an underscore if not available.
        // 4   CPOSTAG   Coarse-grained part-of-speech tag, where tagset depends on the language.
        // 5   POSTAG  Fine-grained part-of-speech tag, where the tagset depends on the language, or identical to the coarse-grained part-of-speech tag if not available.
        // 6   NER Extra column: Named entity type
        // 7   FEATS   Unordered set of syntactic and/or morphological features (depending on the particular language), separated by a vertical bar (|), or an underscore if not available.
        // 8   HEAD  Head of the current token, which is either a value of ID or zero ('0'). Note that depending on the original treebank annotation, there may be multiple tokens with an ID of zero.
        // 9   DEPREL  Dependency relation to the HEAD. The set of dependency relations depends on the particular language. Note that depending on the original treebank annotation, the dependency relation may be meaningfull or simply 'ROOT'.
        // 10  PHEAD   Projective head of current token, which is either a value of ID or zero ('0'), or an underscore if not available. Note that depending on the original treebank annotation, there may be multiple tokens an with ID of zero. The dependency structure resulting from the PHEAD column is guaranteed to be projective (but is not available for all languages), whereas the structures resulting from the HEAD column will be non-projective for some sentences of some languages (but is always available).
        // 11  PDEPREL   Dependency relation to the PHEAD, or an underscore if not available. The set of dependency relations depends on the particular language. Note that depending on the original treebank annotation, the dependency relation may be meaningfull or simply 'ROOT'.

        QString targetConllIdString = targetConllId > 0 ? QString(QLatin1String("%1")).arg(targetConllId) : "_";
        if (!firstToken) {
          dstream->out() << outputFormatter->getTokenSeparator();
        }
        firstToken=false;
        outputFormatter->pushAttr("ID",std::to_string(tokenId), true );
        outputFormatter->pushAttr("FORM",inflectedToken);
        outputFormatter->pushAttr("LEMMA",lemmatizedToken);
        outputFormatter->pushAttr("CPOSTAG",macro.toUtf8().constData());
        outputFormatter->pushAttr("POSTAG",micro.toUtf8().constData());
        outputFormatter->pushAttr("NE",neType.toUtf8().constData());
        outputFormatter->pushAttr("FEATS","_");
        outputFormatter->pushAttr("HEAD",targetConllIdString.toUtf8().constData());
        outputFormatter->pushAttr("DEPREL",conllRelName.toUtf8().constData());
        outputFormatter->pushAttr("PHEAD","_");
        outputFormatter->pushAttr("PDEPREL","_");
        if (!predicates.isEmpty())
        {
//           LDEBUG << "ConllDumper::process output the predicate if any";
          if (!predicates.contains(v))
          {
            // No predicate for this token
            outputFormatter->pushAttr("PRED","_");
          }
          else
          {
            // This token is a predicate, output it
            QString predicateAnnotation = annotationData->stringAnnotation(predicates.value(v),"Predicate");
            outputFormatter->pushAttr("PRED",predicateAnnotation.toStdString());
          }

          // Now output the roles supported by the current PoS graph token
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process output the roles for the" << keys.size() << "predicates";
#endif
          for (int i = 0; i < keys.size(); i++)
          {
            // There will be one column for each predicate.
            AnnotationGraphVertex predicateVertex = predicates.value(keys[keys.size()-1-i]);

            std::set< AnnotationGraphVertex > vMatches = annotationData->matches("PosGraph", v, "annot");
            if (vMatches.empty())
            {
#ifdef DEBUG_LP
              LDEBUG << "ConllDumper::process no node matching PoS graph vertex" << v << "in the annotation graph. Output '_'.";
#endif
              outputFormatter->pushAttr("ROLE","_");
            }
            else
            {
#ifdef DEBUG_LP
              LDEBUG << "ConllDumper::process there is"<<vMatches.size()<<"nodes matching PoS graph vertex" << v << "in the annotation graph.";
#endif
              QString roleAnnotation = "_";
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
//                     dstream->out() << "_";
                  }
                }
                if (roleAnnotation != "_") break;
              }
              outputFormatter->pushAttr("ROLE",roleAnnotation.toUtf8().constData());
            }
          }
        }
        dstream->out() << outputFormatter->getTokenOutput().c_str();
        dstream->out() << outputFormatter->getTokenTerminator().c_str();
      }

      if (v == sentenceEnd)
      {
        continue;
      }
#ifdef DEBUG_LP
      LDEBUG << "ConllDumper::process look at out edges of" << v;
#endif
      LinguisticGraphOutEdgeIt outIter,outIterEnd;
      for (boost::tie(outIter,outIterEnd) = boost::out_edges(v,*graph); outIter!=outIterEnd; outIter++)
      {
        LinguisticGraphVertex next = boost::target(*outIter,*graph);
#ifdef DEBUG_LP
        LDEBUG << "ConllDumper::process looking out vertex" << next;
#endif
        if (!visited.contains(next))
        {
#ifdef DEBUG_LP
          LDEBUG << "ConllDumper::process enqueuing" << next;
#endif
          visited.insert(next);
          toVisit.enqueue(next);
        }
      }
      tokenId++;
    }
    dstream->out() << outputFormatter->getfooter();

    limaConllTokenIdMapping->insert(std::make_pair(sentenceNb, segmentationMappingReverse));
    sbItr++;
  }
  delete outputFormatter;

  return SUCCESS_ID;

}

QMultiMap<LinguisticGraphVertex, AnnotationGraphVertex> ConllDumperPrivate::collectPredicateTokens(
                                  Lima::AnalysisContent& analysis, LinguisticGraphVertex sentenceBegin, LinguisticGraphVertex sentenceEnd)
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
#endif
  QMap<LinguisticGraphVertex, AnnotationGraphVertex> result;

  AnnotationData* annotationData = static_cast<AnnotationData*>(analysis.getData("AnnotationData"));

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (tokenList==0) {
    DUMPERLOGINIT;
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
#ifdef DEBUG_LP
    LDEBUG << "ConllDumperPrivate::collectPredicateTokens vertex:" << v;
#endif
    visited.insert(v);

    std::set< AnnotationGraphVertex > vMatches = annotationData->matches("PosGraph", v, "annot");
    for (auto it = vMatches.begin(); it != vMatches.end(); it++)
    {
      AnnotationGraphVertex vMatch = *it;
      if (annotationData->hasStringAnnotation(vMatch,"Predicate"))
      {
#ifdef DEBUG_LP
        LDEBUG << "ConllDumperPrivate::collectPredicateTokens insert" << v <<    vMatch;
#endif
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
