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
#include "TextFeaturesDumper.h"

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
//using namespace boost;
using namespace boost::tuples;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit,TextFeaturesDumper> textFeaturesDumperFactory(TEXTFEATURESDUMPER_CLASSID);

TextFeaturesDumper::TextFeaturesDumper():
AbstractTextualAnalysisDumper(),
m_graph("PosGraph"),
m_sep(" "),
m_sepReplace("_"),
m_sepPOS("#"),
m_features()
{}


TextFeaturesDumper::~TextFeaturesDumper()
{}

void TextFeaturesDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  m_language=manager->getInitializationParameters().media;
  try
  {
    m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value

  try { 
    m_sep=unitConfiguration.getParamsValueAtKey("sep"); 
  }
  catch (NoSuchParam& ) {} // keep default value
  
  try { 
    m_sepReplace=unitConfiguration.getParamsValueAtKey("sep_replace"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try { 
    m_sepPOS=unitConfiguration.getParamsValueAtKey("sepPOS"); 
  }
  catch (NoSuchParam& ) {} // keep default value

  try { 
    std::deque<string> featureList=unitConfiguration.getListsValueAtKey("features"); 
    // initialize feature access
    m_features.setLanguage(m_language);
    m_features.initialize(featureList);
  }
  catch (NoSuchList& ) { // keep default value
    LOGINIT("LP::Dumper");
    LERROR << "Warning: no features selected in TextFeaturesDumper: output will be empty";
  }

}

LimaStatusCode TextFeaturesDumper::process(
  AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0) {
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  DumperStream* dstream=initialize(analysis);

  map<Token*,LinguisticGraphVertex,lTokenPosition > categoriesMapping;

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graph));
  if (anagraph==0) {
    LERROR << "graph " << m_graph << " has not been produced: check pipeline";
    return MISSING_DATA;
  }
  LinguisticGraph* graph=anagraph->getGraph();
  // const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_language);

  // instead of looking to all vertices, follow the graph (in
  // morphological graph, some vertices are not related to main graph:
  // idiomatic expressions parts and named entity parts)
  
  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;
  toVisit.push(anagraph->firstVertex());
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (v == anagraph->lastVertex()) {
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
        categoriesMapping[ft]=v;
      }
  }
  
  for (map<Token*,LinguisticGraphVertex,lTokenPosition >::const_iterator ftItr=categoriesMapping.begin();
       ftItr!=categoriesMapping.end();
       ftItr++)
  {
    outputVertex(dstream->out(),anagraph,ftItr->second,metadata->getStartOffset());
  }

  delete dstream;
  return SUCCESS_ID;
}


void TextFeaturesDumper::
outputVertex(std::ostream& out, 
             const LinguisticAnalysisStructure::AnalysisGraph* graph,
             LinguisticGraphVertex v,
             uint64_t /*offset*/) const
{
  //TODO : use offset
  bool first=true;
  for (WordFeatures::const_iterator it=m_features.begin(),it_end=m_features.end();
       it!=it_end; it++) 
  {
    if (first) { first=false; }
    else {
      out << m_sep;
    }
    // take only first morphosyntactic data
    string str=(*it)->getValue(graph,v);
    boost::replace_all(str,m_sep,m_sepReplace);
    out << str;
  }
  out << endl;
}

} // end namespace
} // end namespace
} // end namespace
