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
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include <boost/algorithm/string/replace.hpp>

#include <fstream>
#include <queue>
#include <sstream>

using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::SpecificEntities;

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
m_NEgraph("AnalysisGraph"),
m_sep(" "),
m_sepPOS("#"),
m_followGraph(false)
{}


ConllDumper::~ConllDumper()
{}

void ConllDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)
{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  
  m_language=manager->getInitializationParameters().media;
  try
  {
    m_NEgraph = unitConfiguration.getParamsValueAtKey("NEgraph");
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value

  if (m_graph=="AnalysisGraph")
  {
    // change default for followGraph
    m_followGraph=true;
  }
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
    std::string str=unitConfiguration.getParamsValueAtKey("followGraph"); 
    if (str=="1" || str=="true" || str=="yes") {
      m_followGraph=true;
    }
    else {
      m_followGraph=false;
    }
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

 
  AnalysisGraph* NE = static_cast<AnalysisGraph*>(analysis.getData(m_NEgraph));
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));

  std::map<Token*,std::vector<MorphoSyntacticData*>,lTokenPosition > categoriesMapping;
  std::map<Token*,LinguisticGraphVertex> nodesMapping;

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (tokenList==0) {
    LERROR << "graph " << m_graph << " has not been produced: check pipeline" << LENDL;
    return MISSING_DATA;
  }
  LinguisticGraph* graph=tokenList->getGraph();
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  if (m_followGraph) {
    // instead of looking to all vertices, follow the graph (in
    // morphological graph, some vertices are not related to main graph:
    // idiomatic expressions parts and named entity parts)
    
    std::queue<LinguisticGraphVertex> toVisit;
    std::set<LinguisticGraphVertex> visited;
    toVisit.push(tokenList->firstVertex());

    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    while (!toVisit.empty()) {
      LinguisticGraphVertex v=toVisit.front();
      toVisit.pop();
      if (v == tokenList->lastVertex()) {
        continue;
      }
      
      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph); outItr!=outItrEnd; outItr++) 
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }
      
      Token* ft=get(vertex_token,*graph,v);
      if( ft!=0) {
        categoriesMapping[ft].push_back(get(vertex_data,*graph,v));
        nodesMapping[ft]  = v;
      }
    }
  }

  std::string previousNE = "";
  for (std::map<Token*,std::vector<MorphoSyntacticData*>,lTokenPosition >::const_iterator ftItr=categoriesMapping.begin();
       ftItr!=categoriesMapping.end();
       ftItr++)
  {
    if (!ftItr->second.empty())
    {
      previousNE = outputVertex(dstream->out(),
                  ftItr->first,
                  ftItr->second,
                  sp,
                  nodesMapping[ftItr->first],
                  annotationData,
                  NE,
                  metadata->getStartOffset(),
                  previousNE,
                  positions);
    }
    else
    {
      std::cerr << "ERROR: vertex " << ftItr->first->stringForm() << " at " << ftItr->first->position() << " has no morphosyntactic data" << std::endl;
    }
  }

  delete dstream;
  return SUCCESS_ID;
}


std::string ConllDumper::outputVertex(std::ostream& out,
                              const Token* ft,
                              const std::vector<MorphoSyntacticData*>& vdata,
                              const FsaStringsPool& sp,
                              LinguisticGraphVertex v,
                              const AnnotationData* annotationData,
                              AnalysisGraph* anagraph,
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
          std::string neType = outputSpecificEntity(out,se,vdata,anagraph->getGraph(),sp,offset, previousNE, positions);
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
                          const LinguisticGraph* graph,
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
  for (; itse != itse_end; itse++)
  {
    LinguisticGraphVertex v = *itse;
    Token* ft=get(vertex_token,*graph,v);
    outputString(out,Common::Misc::limastring2utf8stdstring(ft->stringForm()));
    out << m_sep << microCategory << m_sep;
    out << "CHUNK" << m_sep;
    if (positions.find(ft->position()-1) != positions.end())
    {
      out << (*positions.find(ft->position()-1)).second << m_sep;
    }
    else
    {
      std::cerr << "ERROR: no such position " << ft->position()-1 << " in positions map!" << std::endl;
      out << "O" << m_sep;
    }
    if (entityTypesMapping.find(Common::Misc::limastring2utf8stdstring(Common::MediaticData::MediaticData::single().getEntityName(se->getType()))) != entityTypesMapping.end())
    {
      neType = entityTypesMapping[Common::Misc::limastring2utf8stdstring(Common::MediaticData::MediaticData::single().getEntityName(se->getType()))];
    }
    else
    {
      neType = "MISC";
    }
   /* if (itse == se->begin() && previousNE == neType)
      out << "B-";
    else
      out << "I-"; */

    LDEBUG << "ConllDumper::outputSpecificEntity neType=" << neType << LENDL;
    out << "I-"<< neType << std::endl;
  }
  if (microCategory == "PONCTU_FORTE")
  {
    out << std::endl;
  }
  return neType;
}


} // end namespace
} // end namespace
} // end namespace
