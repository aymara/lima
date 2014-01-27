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

// NAUTITIA
//
// jys 17-JAN-2003
//
// DicoConcatenatedAlternatives is the module which creates split alternatives
// for concatenated expression tokens found into dictionary.
// Rules :
// <each FullToken of the main path is processed. Alternative paths
// are not processed>
// <there are as many created alternative paths as there are concatenated
// entries associated with main Token and orthographic alternative Tokens>
// <each concatenated entry gives FullToken path. Each token has the original
// FullToken localization>
// <if concatenated entry supplies dictionary entry, main Token of the just
// created FullToken takes this entry. Otherwise, a dictionary access is
// performed to find dictionary entry>

#include "DicoConcatenatedAlternatives.h"

#include "common/misc/LimaString.h"
// #include "common/linguisticData/linguisticData.h"
#include "common/misc/traceUtils.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"

#include <list>

using namespace std;
using namespace boost;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{

SimpleFactory<LinguisticProcessUnit,DicoConcatenatedAlternatives> dicoConcatenatedAlternativesFactory(DICOCONCATENATEDALTERNATIVES_CLASSID);

DicoConcatenatedAlternatives::DicoConcatenatedAlternatives()
{}

DicoConcatenatedAlternatives::~DicoConcatenatedAlternatives()
{}


void DicoConcatenatedAlternatives::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure&,
  Manager* manager)
{
  m_language = manager->getInitializationParameters().language;
}

LimaStatusCode DicoConcatenatedAlternatives::process(
  AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  MORPHOLOGINIT;
  LINFO << "starting process DicoConcatenated";

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  LinguisticGraph* g=tokenList->getGraph();
  LinguisticGraphVertexIt it,itEnd;
  VertexDataPropertyMap dataMap=get(vertex_data,*g);
  VertexTokenPropertyMap tokenMap=get(vertex_token,*g);
  boost::tie(it,itEnd)=vertices(*g);
  for (;it!=itEnd;it++)
  {
    MorphoSyntacticData* currentData=dataMap[*it];
    Token* currentToken=tokenMap[*it];
    if (currentToken==0) continue;

    // if no concatenated entries, skip
    DictionaryEntry* entry=currentToken->dictionaryEntry();
    if (entry->hasConcatenated())
    {

      // if some, then insert it in the graph and remove source token
      // retrieve preds and succs vertices
      expandConcatenatedEntries(*it,g,currentToken,entry);

      // delete source token
      clear_vertex(*it,*g);
    }
    else if (!currentToken->orthographicAlternatives().empty())
    {
      const std::vector< Token* >& orthos=currentToken->orthographicAlternatives();
      bool expanded=false;
      for (vector<Token*>::const_iterator tokItr=orthos.begin();
           tokItr!=orthos.end();
           tokItr++)
      {
        DictionaryEntry* entry=(*tokItr)->dictionaryEntry();
        if (entry->hasConcatenated())
        {
          expandConcatenatedEntries(*it,g,currentToken,entry);
          expanded=true;
        }
      }
      // if expanded and no more linguistic info available remove source token
      if (expanded && currentData->empty())
      {
        clear_vertex(*it,*g);
      }
    }
  }
  LINFO << "ending process DicoConcatenated";
  TimeUtils::logElapsedTime("DicoConcatenatedAlternatives");
  return SUCCESS_ID;

}

void DicoConcatenatedAlternatives::expandConcatenatedEntries(
  LinguisticGraphVertex v,
  LinguisticGraph* g,
  Token* token,
  DictionaryEntry* entry) const
{
  MORPHOLOGINIT;
  LDEBUG << "DicoConcatenatedAlternatives::expandConcatenatedEntries" << v;
  list<LinguisticGraphVertex> preds,succs;
  LinguisticGraphInEdgeIt inItr,inItrEnd;
  boost::tie(inItr,inItrEnd)=in_edges(v,*g);
  for (;inItr!=inItrEnd;inItr++)
  {
    preds.push_back(source(*inItr,*g));
  }
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  boost::tie(outItr,outItrEnd)=out_edges(v,*g);
  for (;outItr!=outItrEnd;outItr++)
  {
    succs.push_back(target(*outItr,*g));
  }

  entry->reset();
  if (entry->hasConcatenated())
  {
    StringsPool& sp=Common::LinguisticData::LinguisticData::changeable().stringsPool(m_language);
    ConcatenatedEntry concatEntry=entry->nextConcatenated();
    while (!concatEntry.isEmpty())
    {
      list<LinguisticGraphVertex> localpreds(preds);
      concatEntry.reset();
      SingleConcatenatedEntry singleEntry=concatEntry.nextSingleConcatenated();
      while (!singleEntry.isEmpty())
      {
        Lima::LimaString component = singleEntry.component();
        unsigned char* adr = singleEntry.dictionaryEntryAddress();
        Dictionary::DictionaryEntry* ent=new Dictionary::DictionaryEntry(component, entry->stringStartAddr(), entry->lingPropertiesStartAddr(), adr);

        // create Token
        Token* nft=new Token(*token);
        MorphoSyntacticData* ndata=new MorphoSyntacticData();
        ndata->appendLingInfo(nft->form(),ent,CONCATENATED_ALTERNATIVE,sp);
        nft->setDictionaryEntry(ent);
        // add vertex in graph
        LinguisticGraphVertex nv=add_vertex(*g);
        put(vertex_token,*g,nv,nft);
        put(vertex_data,*g,nv,ndata);
        // link it to predecessors
        for (list<LinguisticGraphVertex>::const_iterator predItr=localpreds.begin();
             predItr!=localpreds.end();
             predItr++)
        {
          add_edge(*predItr,nv,*g);
        }
        // nv is the predecessor for the next vertex
        localpreds.clear();
        localpreds.push_back(nv);
        singleEntry=concatEntry.nextSingleConcatenated();
      }
      // link to successors
      for (list<LinguisticGraphVertex>::const_iterator predItr=localpreds.begin();
           predItr!=localpreds.end();
           predItr++)
      {
        for (list<LinguisticGraphVertex>::const_iterator succItr=succs.begin();
             succItr!=succs.end();
             succItr++)
        {
          add_edge(*predItr,*succItr,*g);
        }
      }
      concatEntry=entry->nextConcatenated();
    }
  }
}


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
