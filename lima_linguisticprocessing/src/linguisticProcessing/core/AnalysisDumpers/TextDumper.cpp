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
#include "TextDumper.h"
// #include "linguisticProcessing/core/LinguisticProcessors/DumperStream.h"
#include "common/MediaProcessors/DumperStream.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include <boost/algorithm/string/replace.hpp>

#include <fstream>
#include <queue>

using namespace std;
using namespace boost;
using namespace boost::tuples;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<MediaProcessUnit,TextDumper> textDumperFactory(TEXTDUMPER_CLASSID);

TextDumper::TextDumper():
AbstractTextualAnalysisDumper(),
m_language(0),
m_property("MICRO"),
m_propertyAccessor(0),
m_propertyManager(0),
m_graph("PosGraph"),
m_sep(" | "),
m_sepPOS("#"),
m_followGraph(false)
{}


TextDumper::~TextDumper()
{}

void TextDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);
  
  m_language=manager->getInitializationParameters().media;
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value

  if (m_graph=="AnalysisGraph") { 
    // change default for followGraph
    m_followGraph=true;
  }
  //Ajout
  try { 
    m_verbTenseFlag=unitConfiguration.getParamsValueAtKey("verbTenseFlag"); 
  }
  catch (NoSuchParam& ) {
    m_verbTenseFlag=string("False");
  } // keep default value
  //---

  try { 
    m_sep=unitConfiguration.getParamsValueAtKey("sep"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try { 
    m_sepPOS=unitConfiguration.getParamsValueAtKey("sepPOS"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try { 
    m_property=unitConfiguration.getParamsValueAtKey("property"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try { 
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

LimaStatusCode TextDumper::process(
  AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  DumperStream* dstream=initialize(analysis);

  map<Token*,vector<MorphoSyntacticData*>,lTokenPosition > categoriesMapping;

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (tokenList==0) {
    LERROR << "graph " << m_graph << " has not been produced: check pipeline";
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
      }
    }
  }
  else { // output all vertices
    LinguisticGraphVertexIt vxItr,vxItrEnd;
    boost::tie(vxItr,vxItrEnd) = vertices(*graph);
    for (;vxItr!=vxItrEnd;vxItr++)
      {
        Token* ft=get(vertex_token,*graph,*vxItr);
        if( ft!=0)
          {
            categoriesMapping[ft].push_back(get(vertex_data,*graph,*vxItr));
          }
      }
  }
  
  for (map<Token*,vector<MorphoSyntacticData*>,lTokenPosition >::const_iterator ftItr=categoriesMapping.begin();
       ftItr!=categoriesMapping.end();
       ftItr++)
  {
    outputVertex(dstream->out(),ftItr->first,ftItr->second,sp,metadata->getStartOffset());
  }

  delete dstream;
  return SUCCESS_ID;
}


void TextDumper::outputVertex(std::ostream& out, 
                              const Token* ft,
                              const vector<MorphoSyntacticData*>& data,
                              const FsaStringsPool& sp,
                              uint64_t offset) const
{

  ltNormProperty sorter(m_propertyAccessor);

//    uint64_t nbmicros=ft->countMicros();
    std::ostringstream os;
    uint64_t position=ft->position() + offset;
    os << position << m_sep;
    out << os.str();
    outputString(out,Common::Misc::limastring2utf8stdstring(ft->stringForm()));
    for (vector<MorphoSyntacticData*>::const_iterator dataItr=data.begin(),
           dataItr_end=data.end(); dataItr!=dataItr_end; dataItr++)
    {
      MorphoSyntacticData* data=*dataItr;
      sort(data->begin(),data->end(),sorter);
      StringsPoolIndex norm(0),curNorm(0);
      LinguisticCode micro(0),curMicro(0),tense(0),curTense(0);
      for (MorphoSyntacticData::const_iterator elemItr=data->begin();
           elemItr!=data->end();
             elemItr++)
      {
        curNorm=elemItr->normalizedForm;
        curMicro=m_propertyAccessor->readValue(elemItr->properties);
  curTense=m_timeAccessor->readValue(elemItr->properties); //ajout
        if ((curNorm != norm) || (curMicro != micro)) {
          norm=curNorm;
          micro=curMicro;
    tense=curTense; //ajout
          
          std::ostringstream os2;
          os2 << m_sep;
          out << os2.str();
          outputString(out,Common::Misc::limastring2utf8stdstring(sp[norm]));
          std::ostringstream os3;
          os3 << m_sepPOS << m_propertyManager->getPropertySymbolicValue(curMicro);
          out << os3.str();
    /*modifications pour afficher le temps des verbes*/
    if(m_verbTenseFlag ==  "True"){
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
    /*fin des modifications*/
        }
      }
    }
    out << endl;
}

// output string: escape endline characters and separator characters
void TextDumper::outputString(std::ostream& out,const std::string& str) const
{
  string newstr(str);
  boost::replace_all(newstr,"\n","\n");
  //boost::replace_all(newstr,m_sep,"\"+m_sep);
  out << newstr;
}


} // end namespace
} // end namespace
} // end namespace
