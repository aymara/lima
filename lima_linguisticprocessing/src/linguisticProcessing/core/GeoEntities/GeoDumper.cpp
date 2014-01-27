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
#include "GeoDumper.h"
// #include "linguisticProcessing/core/LinguisticProcessors/HandlerStreamBuf.h"
#include "common/MediaProcessors/HandlerStreamBuf.h"

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
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/client/AnalysisHandlers/AbstractTextualAnalysisHandler.h"
#include "GeoEntityAnnotation.h"

#include <set>

using namespace std;
using namespace boost;
using namespace boost::tuples;
using namespace Lima::Common::Misc;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::GeoEntities;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<MediaProcessUnit,GeoDumper> geoDumperFactory(GEODUMPER_CLASSID);

GeoDumper::GeoDumper() :
    MediaProcessUnit(),
    m_language(),
    m_propertyAccessor(0),
    m_propertyManager(0),
    m_graph("PosGraph"),
    m_handler(),
    m_sep("\t"),
    m_sepPOS("#"),
    m_printPosition(false),
    m_property("MICRO")
{}


GeoDumper::~GeoDumper()
{}

void GeoDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)

{
  m_language=manager->getInitializationParameters().media;
  std::string m_graph;  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) { } // optional (default value exists)
  // read handler in conf file
  try
  {
    m_handler=unitConfiguration.getParamsValueAtKey("handler");
  }
  catch (NoSuchParam& )
  {
    DUMPERLOGINIT;
    LERROR << "GeoDumper::init: Missing parameter handler in BowDumper configuration" << LENDL;
    throw InvalidConfiguration();
  }

  //optional parameters to configure output format
  try { 
    m_sep=unitConfiguration.getParamsValueAtKey("separator"); 
  } 
  catch (NoSuchParam) { } 
  try { 
    m_sepPOS=unitConfiguration.getParamsValueAtKey("separatorPOS"); 
  } 
  catch (NoSuchParam) { }
  try { 
    std::string str=unitConfiguration.getParamsValueAtKey("printPosition"); 
    if (str=="1" || str=="yes") { m_printPosition=true; }
  }
  catch (NoSuchParam&) { }
  try { 
    m_property=unitConfiguration.getParamsValueAtKey("property"); 
  }
  catch (NoSuchParam&) { }
  

m_propertyAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor(m_property);
  m_propertyManager=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager(m_property);  

}

LimaStatusCode GeoDumper::process(
  AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
  LDEBUG << "Process GeoDumper " << LENDL;
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "GeoDumper::process: no LinguisticMetaData ! abort" << LENDL;
      return MISSING_DATA;
  }
  /*AnalysisHandlerContainer* handlerContainer=static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
  if (handlerContainer == 0) {
    LERROR << "GeoDumper::process: no handler in analysisContent ! abort" << LENDL;
    return MISSING_DATA;
  }*/

  Lima::Common::AnnotationGraphs::AnnotationData* annotationData = static_cast< Lima::Common::AnnotationGraphs::AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "GeoDumper::process: no AnnotationData ! abort" << LENDL;
    return MISSING_DATA;
  }

  //AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(handlerContainer->getHandler());
  LDEBUG << "handler will be: " << m_handler << LENDL;
  //MediaId langid = static_cast<const  Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(metadata->getMetaData("Lang"))).getMedia();
  AnalysisHandlerContainer* h = static_cast<AnalysisHandlerContainer*>(analysis.getData("AnalysisHandlerContainer"));
  AbstractTextualAnalysisHandler* handler = static_cast<AbstractTextualAnalysisHandler*>(h->getHandler(m_handler));
  if (handler==0)
  {
    LERROR << "GeoDumper::process: handler " << m_handler << " has not been given to the core client" << LENDL;
    return MISSING_DATA;
  }
  //handler->setOut(&std::cout);
  
  handler->startAnalysis();
  HandlerStreamBuf hsb(handler);
  std::ostream out(&hsb);

  map<Token*, pair<LinguisticGraphVertex,vector<MorphoSyntacticData*> >, lTokenPosition > categoriesMapping;

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  LinguisticGraph* graph=anagraph->getGraph();
  ltNormProperty sorter(m_propertyAccessor);
  //const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  LinguisticGraphVertexIt vxItr,vxItrEnd;
  boost::tie(vxItr,vxItrEnd) = vertices(*graph);
  for (;vxItr!=vxItrEnd;vxItr++)
  {
    Token* ft=get(vertex_token,*graph,*vxItr);
    if( ft!=0)
    {
      std::pair<LinguisticGraphVertex,vector<MorphoSyntacticData*> > element =categoriesMapping[ft];
      element.second.push_back(get(vertex_data,*graph,*vxItr));
      element.first=*vxItr;
      categoriesMapping[ft]=element;
    }
  }

  for (map<Token*, pair<LinguisticGraphVertex,vector<MorphoSyntacticData*> >, lTokenPosition >::const_iterator ftItr=categoriesMapping.begin();
       ftItr!=categoriesMapping.end();
       ftItr++)
  {

    Token* ft=ftItr->first;
    std::ostringstream os;
    // get position
    uint64_t position=ft->position() + metadata->getStartOffset();
    // get string
    std::string str=Common::Misc::limastring2utf8stdstring(ft->stringForm());
    // replace separator in string by '_'
    string::size_type sepLen=m_sep.size();
    string::size_type p=0;
    while ( (p = str.find(m_sep, p)) != string::npos ) {
      str.replace( p, sepLen, "_");
      p++;
    }
    
    // newlines (paragraphes) => print empty line
    //if (str=="\n") {
    //  os << str; continue;
    //}
    if (m_printPosition) {
      os << position << m_sep;
    }
    os << str << m_sep;
    // POS
    std::set<LinguisticCode> props;
    vector<MorphoSyntacticData*> vt=ftItr->second.second;
    for (vector<MorphoSyntacticData*>::const_iterator dataItr=vt.begin();
           dataItr!=vt.end();
           dataItr++)
    {
        MorphoSyntacticData* data=*dataItr;
        sort(data->begin(),data->end(),sorter);
        //StringsPoolIndex norm(0),curNorm(0);
        LinguisticCode prop(0);
    
        // output first 
        MorphoSyntacticData::const_iterator elemIt=data->begin(),elemIt_end=data->end();
        //norm=elemIt->normalizedForm;
        if(elemIt != elemIt_end)
        {
          prop=m_propertyAccessor->readValue(elemIt->properties);
          os << m_propertyManager->getPropertySymbolicValue(prop);
          props.insert(prop);
        
          // output rest, with separator
          for ( elemIt++; elemIt!=elemIt_end; elemIt++)
          {
            //curNorm=elemIt->normalizedForm;
            prop=m_propertyAccessor->readValue(elemIt->properties);
            //if ((curNorm != norm) || (curProp != prop)) {
            //  norm=curNorm;
            //  prop=curProp;
            if (props.find(prop)==props.end()) {
              os << m_sepPOS << m_propertyManager->getPropertySymbolicValue(prop);
              props.insert(prop);
            }
          }
        }
    }
    std::set< AnnotationGraphVertex > matches = annotationData->matches(anagraph->getGraphId(),(ftItr->second).first,"annot");
    if (annotationData->hasAnnotation(*matches.begin(),utf8stdstring2limastring("GeoEntity")))
    {
      os << m_sep;
      os << annotationData->annotation(*matches.begin(),Common::Misc::utf8stdstring2limastring("GeoEntity"))
        .pointerValue<GeoEntityAnnotation>()->getPosition();
      os << m_sep;
      std::set<std::string> classes=annotationData->annotation(*matches.begin(),Common::Misc::utf8stdstring2limastring("GeoEntity"))
        .pointerValue<GeoEntityAnnotation>()->getGeoClasses();
      for (std::set<std::string>::iterator iT = classes.begin(); iT!=classes.end();)
      {
       os << *iT;
       iT++;
       if (iT!=classes.end()) os << " "; 
       }
    }
    out << os.str();
    out << endl;
  }

  out.flush();
  handler->endAnalysis();

  return SUCCESS_ID;
}



}

}

}
