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
#include "ConllDumper.h"
// #include "linguisticProcessing/core/LinguisticProcessors/DumperStream.h"
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
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <boost/algorithm/string/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>
#include <queue>
#include <sstream>
#include <map>

using namespace boost;
using namespace boost::tuples;
using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;
using namespace Lima::LinguisticProcessing::AnalysisDumpers;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<MediaProcessUnit,ConllDumper> conllDumperFactory(CONLLDUMPER_CLASSID);

ConllDumper::ConllDumper():
AbstractTextualAnalysisDumper(),
m_language(0),
m_property("MICRO"),
m_propertyAccessor(0),
m_propertyManager(0),
m_graph("PosGraph"),
m_sep(" "),
m_sepPOS("#"),
m_conllLimaDepMapping(),
m_suffix(".conll")
{DUMPERLOGINIT;
  std::string mappingFile="Projets/Lima/lima/lima_linguisticdata/SRLIntegration/lima_conll_dependency_tag_mapping/mapping_conll_Lima.txt";
  std::ifstream ifs(mappingFile, std::ifstream::binary);
  if (!ifs.good())
  {
    LERROR << "ERROR: cannot open"+ mappingFile << LENDL;
  }
  while (ifs.good() && !ifs.eof())
  {
    std::vector<std::string> strs;
    string line;
    while(getline(ifs, line))  // as long as we can put the line on "line"
    {
      boost::split(strs, line, boost::is_any_of("\t"));
      m_conllLimaDepMapping.insert(make_pair(strs[0],strs[1]));
    }
  }
}

ConllDumper::~ConllDumper()
{}

void ConllDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)
{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  m_language=manager->getInitializationParameters().media;
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");

  }
  catch (NoSuchParam& ) {} // keep default value

  //Ajout
  try
  {
    m_verbTenseFlag=unitConfiguration.getParamsValueAtKey("verbTenseFlag"); 
  }
  catch (NoSuchParam& ) {
    m_verbTenseFlag=std::string("False");
  } // keep default value
  //---

  try
  {
    m_sep=unitConfiguration.getParamsValueAtKey("sep"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try
  {
    m_sepPOS=unitConfiguration.getParamsValueAtKey("sepPOS"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try
  {
    m_property=unitConfiguration.getParamsValueAtKey("property"); 
  }
  catch (NoSuchParam& ) {} // keep default value
  try
  {
    m_suffix=unitConfiguration.getParamsValueAtKey("outputSuffix");
  }
  catch (NoSuchParam& ) {} // keep default value
  const Common::PropertyCode::PropertyCodeManager& codeManager=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
  m_propertyAccessor=&codeManager.getPropertyAccessor(m_property);
  m_propertyManager=&codeManager.getPropertyManager(m_property);
   /*ajout*/
  m_timeManager=&codeManager.getPropertyManager("TIME");
  m_timeAccessor=&codeManager.getPropertyAccessor("TIME");
  /*fin ajout*/
}

LimaStatusCode ConllDumper::process(AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "no LinguisticMetaData ! abort" << LENDL;
      return MISSING_DATA;
  }
  // read positions files previously obtained from reference data. it gives the position for each token of the reference data in the obtained text file
  std::map<int, std::string> positions;
  std::ifstream ifs((metadata->getMetaData("FileName")+".positions.txt").c_str(), std::ifstream::binary);
  if (!ifs.good())
  {
    std::cerr << "ERROR: cannot open " + metadata->getMetaData("FileName") + ".positions.txt" << std::endl;
  }
  std::ofstream ofs((metadata->getMetaData("FileName")+m_suffix), std::ofstream::binary);

  while (ifs.good() && !ifs.eof())
  {
    std::string line = Lima::Common::Misc::readLine(ifs);
    std::istringstream iss(line);
    int position;
    std::string netype;
    iss >> position >> netype;
    positions.insert(std::make_pair(position,netype));
    //     std::cerr << "storing " << position << ", " << netype << std::endl;
  }
  ifs.close();

  DumperStream* dstream=initialize(analysis);


  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));//est de type PosGraph et non pas AnalysisGraph
  if (tokenList==0) {
    LERROR << "graph " << m_graph << " has not been produced: check pipeline" << LENDL;
    return MISSING_DATA;
  }
  LinguisticGraph* graph=tokenList->getGraph();

  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);
  SyntacticData* syntacticData=static_cast<SyntacticData*>(analysis.getData("SyntacticData"));
  if (syntacticData==0)
  {
    syntacticData=new SyntacticData(tokenList,0);
    syntacticData->setupDependencyGraph();
    analysis.setData("SyntacticData",syntacticData);
  }
  const DependencyGraph* depGraph = syntacticData-> dependencyGraph();
  std::map<LinguisticGraphVertex, pair<LinguisticGraphVertex, string>>vertexDependencyInformations;

  SegmentationData* sd=static_cast<SegmentationData*>(analysis.getData("SentenceBoundaries"));
  std::vector<Segment>::iterator sbItr=(sd->getSegments().begin());
  uint64_t nbSentences((sd->getSegments()).size());
  LDEBUG << "\n There are "<< nbSentences << " sentences"<< LENDL;
  LinguisticGraphVertex sentenceBegin=sbItr->getFirstVertex();
  LinguisticGraphVertex sentenceEnd=sbItr->getLastVertex();


  map<std::string, std::string>::const_iterator im;
  for (im=m_conllLimaDepMapping.begin();im!=m_conllLimaDepMapping.end();im++){
    LDEBUG << "("<< (*im).first<< "," << (*im).second << ")" << endl;
  }

  while (sbItr!=(sd->getSegments().end()))//for each sentence
  {
    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();
    map<LinguisticGraphVertex,int>segmentationMapping;//mapping the two types of segmentations (global graphe and Conll segmentation)

    LDEBUG << "begin - end: " << sentenceBegin << " - " << sentenceEnd << LENDL;
    //LinguisticGraphOutEdgeIt outItr,outItrEnd;
    std::queue<LinguisticGraphVertex> toVisit;
    std::set<LinguisticGraphVertex> visited;
    toVisit.push(sentenceBegin);
    int tokenId = 0;
    for (LinguisticGraphVertex v=toVisit.front();v!=sentenceEnd;v=toVisit.front()) {
      toVisit.pop();
      LDEBUG << "Vertex index : " << v;
      visited.insert(v);
      segmentationMapping.insert(make_pair(v,tokenId));
      DependencyGraphVertex dcurrent = syntacticData->depVertexForTokenVertex(v);
      DependencyGraphOutEdgeIt dit, dit_end;
      std::vector<DependencyGraphEdge> edges;
      boost::tie(dit,dit_end)=out_edges(dcurrent,*depGraph);
      for (; dit != dit_end; dit++)
      {
        edges.push_back(*dit);
      }
      // Token* ft=get(vertex_token,*graph,v);
      std::vector<DependencyGraphEdge>::const_iterator it, it_end;
      it = edges.begin(); it_end = edges.end();
      std::set<std::string> m_relation_names;
      for (; it != it_end; it++)
      {
        LDEBUG << "Dumping dependency edge " << (*it).m_source << " -> " << (*it).m_target;
        try
        {
          LDEBUG << "DepTripleDumper::dumpDependencyRelations";
          CEdgeDepRelTypePropertyMap typeMap = get(edge_deprel_type, *depGraph);
          SyntacticRelationId type = typeMap[*it];
          string SyntRelName=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(type);
          LDEBUG << "DepTripleDumper::dumpDependencyRelations relation = " << SyntRelName;
          std::set<std::string>::const_iterator relationPos =
          m_relation_names.find(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(type));
  //      if( relationPos != m_relation_names.end() )
  //      {
          LDEBUG << "Src  : Dep vertex= " << source(*it, *depGraph);
          LinguisticGraphVertex src = syntacticData->tokenVertexForDepVertex(source(*it, *depGraph));
          LDEBUG << "Src  : Morph vertex= " << src << LENDL;
          LDEBUG << "Targ : Dep vertex= " << target(*it, *depGraph);
          LinguisticGraphVertex dest = syntacticData->tokenVertexForDepVertex(target(*it, *depGraph));
          LDEBUG << "Targ : Morph vertex= " << dest;
          if (SyntRelName!=""){
            vertexDependencyInformations.insert(make_pair(v,make_pair(dest,SyntRelName)));
          }
          LDEBUG << "target saved for " << v << " is " << dest;
        }
        catch (const std::range_error& )
        {
        }
        catch (...)
        {
          LDEBUG << "DepTripleDumper::dumpDependencyRelations: catch others....." << LENDL;
        throw;
        }
      }
      if (v == sentenceEnd)
      {
        continue;
      }
      LinguisticGraphOutEdgeIt outItr,outItrEnd;
      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end() && next != tokenList->lastVertex())
        {
          toVisit.push(next);
        }
      }
      ++tokenId;
    }

  // instead of looking to all vertices, follow the graph (in
  // morphological graph, some vertices are not related to main graph:
  // idiomatic expressions parts and named entity parts)

    toVisit = std::queue<LinguisticGraphVertex>();
    visited.clear();

    sentenceBegin=sbItr->getFirstVertex();
    sentenceEnd=sbItr->getLastVertex();
    toVisit.push(sentenceBegin);
    tokenId=0;
    for (LinguisticGraphVertex v=toVisit.front();!toVisit.empty() && v!=sentenceEnd;v=toVisit.front()) {//as long as there are vertices in the sentence
      toVisit.pop();
      Token* ft=get(vertex_token,*graph,v);
      MorphoSyntacticData* morphoData=get(vertex_data,*graph, v);
      LDEBUG << v << "th token in the global graph" <<endl;
      if( morphoData!=0 && v != sentenceBegin) {
        const Common::PropertyCode::PropertyCodeManager& codeManager=static_cast<const Common::MediaticData     ::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager();
        const Common::PropertyCode::PropertyAccessor m_propertyAccessor=codeManager.getPropertyAccessor("MICRO");

        const QString graphTag=QString::fromStdString(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertySymbolicValue(morphoData->firstValue(m_propertyAccessor)));


        std::string unLemmatizedToken=ft->stringForm().toStdString();
        std::string lemmatizedToken=sp[(*morphoData)[0].lemma].toUtf8().data();

        if (vertexDependencyInformations.count(v)!=0){
          LinguisticGraphVertex source=vertexDependencyInformations.find(v)->second.first;
          LinguisticGraphVertex sourceConllId=segmentationMapping.find(source)->second;
          LDEBUG << "source saved for " << tokenId << " is " << sourceConllId<<endl; 
          std::string relName=vertexDependencyInformations.find(v)->second.second;
          std::map< std::string, std::string >::const_iterator m_conllLimaDepMappingIt = m_conllLimaDepMapping.find(relName);
          std::string conllRelName = "-";
          if (m_conllLimaDepMappingIt != m_conllLimaDepMapping.end())
          {
            conllRelName=m_conllLimaDepMappingIt->second;
          }
          else
          {
            LERROR << relName << "not found in mapping";
          }

          ofs << tokenId << "\t"<< unLemmatizedToken << "\t" << lemmatizedToken << "\t" << graphTag  << "\t" << graphTag << "\t" << "-" << "\t" << sourceConllId << "\t" << conllRelName << "\t-\t-" << endl;
          LDEBUG << "the lima dependency tag for " << ft->stringForm().toStdString() << " is " << relName<< endl;
        }
        else{
          ofs << tokenId << "\t"<< unLemmatizedToken << "\t" << lemmatizedToken << "\t" << graphTag  << "\t" << graphTag << "\t-\t-\t-\t-\t-" << endl;
        }
      }

      if (v == sentenceEnd)
      {
        continue;
      }
      LinguisticGraphOutEdgeIt outIter,outIterEnd;
      for (boost::tie(outIter,outIterEnd)=out_edges(v,*graph); outIter!=outIterEnd; outIter++) 
      {
        LinguisticGraphVertex next=target(*outIter,*graph);
        if (visited.find(next)==visited.end())
        {
        visited.insert(next);
        toVisit.push(next);
        }
      }
    ++tokenId;
    }
    ofs << std::endl;
    sbItr++;
  }

  return SUCCESS_ID;

}


std::string ConllDumper::outputVertex(std::ostream& out,
                              const Token* ft,
                              const std::vector<MorphoSyntacticData*>& vdata,
                              const FsaStringsPool& sp,
                              LinguisticGraphVertex v,
                              const AnnotationData* annotationData,
                              uint64_t offset,
                            const std::string& previousNE,
                            const std::map<int, std::string>& positions
) const
{

  ltNormProperty sorter(m_propertyAccessor);

  std::set< LinguisticGraphVertex > anaVertices = annotationData->matches("PosGraph",v,"AnalysisGraph");

  for (std::set< LinguisticGraphVertex >::const_iterator anaVerticesIt = anaVertices.begin();
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
          std::string neType = outputSpecificEntity(out,se,vdata,sp,offset, previousNE, positions);
        if (neType != "") {
          return neType;
        }
        else {
          DUMPERLOGINIT;
          LERROR << "failed to output specific entity for vertex " << v << LENDL;
        }
      }
    }
  }
  
//    uint64_t nbmicros=ft->countMicros();

    if ((*vdata.begin())->empty())
    {
          std::cerr << "ERROR: vertex " << ft->stringForm() << " at " << ft->position() << " has no morphosyntactic data" << std::endl;
          return "";
    }
//     uint64_t position=ft->position() + offset;
    std::string microCategory;
    outputString(out,Common::Misc::limastring2utf8stdstring(ft->stringForm()));
    for (std::vector<MorphoSyntacticData*>::const_iterator vdataItr=vdata.begin(),
           vdataItr_end=vdata.end(); vdataItr!=vdataItr_end; vdataItr++)
    {
      MorphoSyntacticData* data=*vdataItr;
      std::sort(data->begin(),data->end(),sorter);
      StringsPoolIndex norm(0),curNorm(0);
      LinguisticCode micro(0),curMicro(0),tense(0),curTense(0);
      for (MorphoSyntacticData::const_iterator elemItr=data->begin();
           elemItr!=data->end();
             elemItr++)
      {
        curNorm=elemItr->normalizedForm;
        curMicro=m_propertyAccessor->readValue(elemItr->properties);
        microCategory = m_propertyManager->getPropertySymbolicValue(curMicro);
        curTense=m_timeAccessor->readValue(elemItr->properties); //ajout
        if ((curNorm != norm) || (curMicro != micro))
        {
          norm=curNorm;
          micro=curMicro;
          tense=curTense; //ajout
          
          std::ostringstream os2;
          os2 << m_sep << m_sepPOS;
          //out << os2.str();
          //outputString(out,Common::Misc::limastring2utf8stdstring(sp[norm]));
          std::ostringstream os3;
          os3 << m_sep << microCategory;
          out << os3.str();

          /*modifications pour afficher le temps des verbes*/
          if(m_verbTenseFlag ==  "True")
          {
            std::ostringstream os4;
            bool wordIsVerb = false;
            std::string::size_type subStrPos = m_propertyManager->getPropertySymbolicValue(curMicro).find("L_VERBE");
            if (subStrPos != std::string::npos){
              wordIsVerb = true;
            }
            if (wordIsVerb == true){
              if (m_timeManager->getPropertySymbolicValue(curTense) != "NONE"){
                os4 << m_sep << m_timeManager->getPropertySymbolicValue(curTense);
              }
              else {
                os4 << m_sep << m_propertyManager->getPropertySymbolicValue(curMicro);
              }
            }
            else{os4 << m_sep << m_timeManager->getPropertySymbolicValue(curTense);}
            out << os4.str();
          }
              /*modifications in order to show the named entity*/
          std::ostringstream os5;
          os5 << m_sep << "CHUNK" << m_sep;
          out << os5.str();
          /*fin des modifications*/
          break;
        }
      }
      break;
    }
    if (positions.find(ft->position()-1) != positions.end())
    {
      out << (*(positions.find(ft->position()-1))).second << m_sep;
    }
    else
    {
      std::cerr << "ERROR: no such position " << ft->position()-1 << " in positions map!" << std::endl;
      out << "O" << m_sep;
    }
    out  << "O" << std::endl;
    if (microCategory == "PONCTU_FORTE")
    {
      out << std::endl;
    }
    return "";
}

// output string: escape endline characters and separator characters
void ConllDumper::outputString(std::ostream& out,const std::string& str) const
{
  std::string newstr(str);
  boost::replace_all(newstr,"\n","\n");
  boost::replace_all(newstr," ","_");
  //boost::replace_all(newstr,m_sep,"\"+m_sep);
  out << newstr;
}

std::string ConllDumper::outputSpecificEntity(std::ostream& out,
                          const SpecificEntities::SpecificEntityAnnotation* se,
                          const std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*>& data,
                          const FsaStringsPool& sp,
                          const uint64_t offset,
                          const std::string& previousNE,
                          const std::map<int, std::string>& positions) const
{
  LIMA_UNUSED(data);
  LIMA_UNUSED(sp);
  LIMA_UNUSED(offset);
  DUMPERLOGINIT;
  LDEBUG << "ConllDumper::outputSpecificEntity previous=" << previousNE << LENDL;

  std::map< std::string, std::string > entityTypesMapping;
  std::string microCategory = "CAT";
  entityTypesMapping.insert(std::make_pair("Person.PERSON", "PER"));
  entityTypesMapping.insert(std::make_pair("Location.LOCATION", "LOC"));
  entityTypesMapping.insert(std::make_pair("Organization.ORGANIZATION", "ORG"));
  std::vector< LinguisticGraphVertex>::const_iterator itse, itse_end;
  itse = se->m_vertices.begin(); itse_end = se->m_vertices.end();
  std::string neType = "";

  //if (microCategory == "PONCTU_FORTE")
 // {
    //out << std::endl;
 // }
  //return neType;
}


} // end namespace
} // end namespace
} // end namespace
