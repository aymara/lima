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

#include "greedyPosTagger.h"

#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"

#include <iostream>
#include <iterator>
#include <set>
#include <map>
#include <algorithm>

//using namespace boost;
using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::MediaticData;

namespace Lima
{
namespace LinguisticProcessing
{
namespace PosTagger
{

SimpleFactory<MediaProcessUnit,GreedyPosTagger> greedyPosTaggerFactory(GREEDYPOSTAGGER_CLASSID);

void GreedyPosTagger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  /** @addtogroup ProcessUnitConfiguration
   * - <b>&lt;group name="..." class="GreedyPosTagger"&gt;</b>
   *    -  trigramMatrix : TrigramMatrix resource
   *    -  bigramMatrix : BigramMatrix resource
   *    -  ponctuforte : micro category for ponctu forte. default : 'PONCTU_FORTE'
   */
  PTLOGINIT;
  m_language=manager->getInitializationParameters().media;
  m_macroAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MACRO"));
  m_microAccessor=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO"));
  try
  {
    string trigrams=unitConfiguration.getParamsValueAtKey("trigramMatrix");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,trigrams);
    m_trigramMatrix=static_cast<TrigramMatrix*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "No param 'trigramMatrix' in GreedyPosTagger group for language " << (int)m_language;
    throw InvalidConfiguration();
  }

  try
  {
    string bigrams=unitConfiguration.getParamsValueAtKey("bigramMatrix");
    AbstractResource* res=LinguisticResources::single().getResource(m_language,bigrams);
    m_bigramMatrix=static_cast<BigramMatrix*>(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No param 'bigramMatrix' in GreedyPosTagger group for language " << (int)m_language;
    throw InvalidConfiguration();
  }

  try
  {
    std::string id=unitConfiguration.getParamsValueAtKey("ponctuforte");
    m_microCatPonctuForte=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertyValue(id);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LWARN << "No ponctuforte microcateg category for GreedyPosTagger ! use category PONCTU_FORTE";
    m_microCatPonctuForte=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertyValue("PONCTU_FORTE");
  }

}

LimaStatusCode GreedyPosTagger::process(
  AnalysisContent& analysis) const
{

  // start postagging here !
  TimeUtils::updateCurrentTime();
  PTLOGINIT;
  LINFO << "start greedy posTagging";

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));

  AnalysisGraph* posgraph=new AnalysisGraph("PosGraph",m_language,false,true,*anagraph);

  // walk on the vertex but don't process a vertex if one
  // of its predecessor hasn't been processed.
  LinguisticGraph* graph=posgraph->getGraph();
  LinguisticGraphVertex endVx=posgraph->lastVertex();

  map<LinguisticGraphVertex,uint64_t> processed;
  set<LinguisticGraphVertex> toProcess;
  toProcess.insert(anagraph->firstVertex());
  set<LinguisticGraphVertex> nextToProcess;
  set<LinguisticGraphVertex>::iterator toProcessItr;
  map<LinguisticGraphVertex,uint64_t>::iterator processedItr;
  LinguisticGraphInEdgeIt inItr,inItrEnd;
  LinguisticGraphOutEdgeIt outItr,outItrEnd;

  while (toProcess.size()!=0)
  {

    //cout << "toProcess is ";
    //        copy(toProcess.begin(),toProcess.end(),ostream_iterator<LinguisticGraphVertex>(cout,","));
    //        cout << endl;

    for (toProcessItr=toProcess.begin();
         toProcessItr!=toProcess.end();
         toProcessItr++)
    {

      // process vertex
      processVertex(*toProcessItr,anagraph);
      processed.insert(make_pair(*toProcessItr,out_degree(*toProcessItr,*graph)));
      //cerr << "processed : insert " << *toProcessItr << " with " << out_degree(*toProcessItr,*graph) << endl;

      // remove processed if necessary
      boost::tie(inItr,inItrEnd)=in_edges(*toProcessItr,*graph);
      for (;inItr!=inItrEnd;inItr++)
      {
        processedItr=processed.find(source(*inItr,*graph));
        processedItr->second--;
        //cerr << "processed : vertex " << processedItr->first << " decremented to " << processedItr->second << endl;
        if (processedItr->second==0)
        {
          //cerr << "processed : remove " << processedItr->first << endl;
          processed.erase(processedItr);
        }
      }

      // check and add nex vertex to process
      boost::tie(outItr,outItrEnd)=out_edges(*toProcessItr,*graph);
      for (;outItr!=outItrEnd;outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (next==endVx)
        {
          continue;
        }
        // check if all in vertices have been processed
        boost::tie(inItr,inItrEnd)=in_edges(next,*graph);
        bool ok=true;
        for (;inItr!=inItrEnd;inItr++)
        {
          if (processed.find(source(*inItr,*graph))==processed.end())
          {
            ok=false;
            break;
          }
        }
        if (ok)
        {
          nextToProcess.insert(next);
        }
      }

    }

    toProcess.clear();
    toProcess.swap(nextToProcess);

  }

  TimeUtils::logElapsedTime("GreedyPosTagger");
  return SUCCESS_ID;
}

void GreedyPosTagger::processVertex(LinguisticGraphVertex vx,AnalysisGraph* anagraph) const
{
  LinguisticGraph* graph=anagraph->getGraph();
  LinguisticGraphVertex startVx=anagraph->firstVertex();
  LinguisticGraphVertex endVx=anagraph->lastVertex();

  PTLOGINIT;
  if (vx==startVx || vx==endVx)
  {
    return;
  }
  MorphoSyntacticData* data=get(vertex_data,*graph,vx);
  Token* token=get(vertex_token,*graph,vx);
  if (data==0)
  {
    LERROR << "MorphoSyntacticData of vertex " << vx << " is NULL !";
    return;
  }
  LDEBUG << "process vertex : " << vx << " : "
    << Common::Misc::limastring2utf8stdstring(token->stringForm());

  MorphoSyntacticData* posdata=new MorphoSyntacticData(*data);
  put(vertex_data,*graph,vx,posdata);

  auto micros = posdata->allValues(*m_microAccessor);
  LinguisticCode selectedMicro;


  if (micros.size()==0)
  {
    LWARN << "Token "
      << Common::Misc::limastring2utf8stdstring(token->stringForm())
      << " has no possible dicowords ! build a DicoWord with category 0";
    selectedMicro=0;
  }
  else if (micros.size()==1)
  {
    // no choice, put this category
    selectedMicro=*(micros.begin());
    LDEBUG << "GreedyPosTagging : only one choice : " << selectedMicro;
  }
  else
  {
    // choose the most probable dicoWord
    set<LinguisticCode>::iterator dwItr,dwMaxTri,dwMaxBi;
    float maxTri=0;
    float maxBi=0;
    LinguisticCode cat1(0),cat2(0);

    LinguisticGraphInEdgeIt inItr,inItrEnd;
    boost::tie(inItr,inItrEnd)=in_edges(vx,*graph);
    for (;inItr!=inItrEnd;inItr++)
    {
      LinguisticGraphVertex predVx=source(*inItr,*graph);
      MorphoSyntacticData* m2=get(vertex_data,*graph,predVx);
      if (predVx==startVx && m2!=0 && !m2->empty())
      {
        cat2=m_microCatPonctuForte;
      }
      else
      {

        cat2=m_microAccessor->readValue(m2->begin()->properties);

        LinguisticGraphInEdgeIt inItr2,inItr2End;
        boost::tie(inItr2,inItr2End)=in_edges(vx,*graph);
        for (;inItr2!=inItr2End;inItr2++)
        {
          LinguisticGraphVertex predpredVx=source(*inItr2,*graph);
          MorphoSyntacticData* m1=get(vertex_data,*graph,predpredVx);
          if (predpredVx==startVx && m1!=0 && !m1->empty())
          {
            cat1=m_microCatPonctuForte;
          }
          else
          {
            cat1=m_microAccessor->readValue(m1->begin()->properties);
          }
          // search better trigram
          for (dwItr=micros.begin();dwItr!=micros.end();dwItr++)
          {
            float p=m_trigramMatrix->freq(cat1,cat2,*dwItr);
            if (p>maxTri)
            {
              maxTri=p;
              dwMaxTri=dwItr;
            }
          }
        }
      }
      if (maxTri==0)
      {
        // no trigram has been found, search bigram
        for (dwItr=micros.begin();dwItr!=micros.end();dwItr++)
        {
          float p=m_bigramMatrix->freq(cat1,*dwItr);
          if (p>maxBi)
          {
            maxBi=p;
            dwMaxBi=dwItr;
          }
        }

      }


    }

    if (maxTri!=0)
    {
      // choose best trigram
      LDEBUG << "found trigram : choose " << *dwMaxTri << " (p=" << maxTri << ")";
      selectedMicro=*dwMaxTri;
    }
    else if (maxBi!=0)
    {
      // choose best bigram
      LDEBUG << "found bigram : choose " << *dwMaxBi << " (p=" << maxBi << ")";
      selectedMicro=*dwMaxBi;
    }
    else
    {
      // no trigram nor bigram has been found
      // choose better probability as source in bigram then as target in bigram
      LWARN << "Found no trigram nor bigram (" << cat1 << "," << cat2 << ") ! try heuristics to find a microcategory";
      for (dwItr=micros.begin();dwItr!=micros.end();dwItr++)
      {
        float p=m_bigramMatrix->freq(m_microCatPonctuForte,*dwItr);
        if (p>maxBi)
        {
          maxBi=p;
          dwMaxBi=dwItr;
        }
      }
      if (maxBi!=0)
      {
        LDEBUG << "found bigram with ponctu forte : choose " << *dwMaxBi <<  " (p=" << maxBi << ")";
        selectedMicro=*dwMaxBi;
      }
      else
      {
        selectedMicro=*(micros.begin());
        LDEBUG << "choose first : " << selectedMicro;
      }
    }
  }

  // filter linguisticelement
  CheckDifferentPropertyPredicate cdpp(*m_microAccessor,selectedMicro);
  posdata->erase(remove_if(posdata->begin(),posdata->end(),cdpp),posdata->end());

}

} // PosTagger
} // LinguisticProcessing
} // Lima
