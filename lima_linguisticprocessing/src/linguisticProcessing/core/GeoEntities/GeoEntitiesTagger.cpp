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
/**
  *
  * @file        GeoEntitiesTagger.cpp
  * @author      Faiza Gara (faiza.gara@cea.fr) 

  *              Copyright (c) 2010 by CEA
  * @date        Created on January 2010
  *
  */
 
#include "GeoEntitiesTagger.h"
#include "GeoEntityAnnotation.h"

 
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/FlatTokenizer/CharChart.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"


#include <iostream>
#include <queue>
#include <stack>
#include <set>
#include <algorithm>



using namespace std;
using namespace boost;
using namespace boost::tuples;

using namespace soci;

using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;


namespace Lima
{
namespace LinguisticProcessing
{
namespace GeoEntities
{
SimpleFactory<MediaProcessUnit,GeoEntitiesTagger> GeoEntitiesTaggerFactory(GEOENTITIESTAGGERPU_CLASSID);



GeoEntitiesTagger::GeoEntitiesTagger() : 
m_graphId("PosGraph"),
m_language(),
m_field("CLASS_1")
{}

GeoEntitiesTagger::~GeoEntitiesTagger()
{
 free(m_db);
}


void GeoEntitiesTagger::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  GEOENTITIESTAGGERLOGINIT;
  m_language=manager->getInitializationParameters().media;
  LDEBUG << "initialize GeoEntitiesTagger with language "<< (int) m_language << LENDL;
  try
  {
    m_graphId=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    
  } // use default value Posgraph

  try
  {
    string maxEntitylengthS=unitConfiguration.getParamsValueAtKey("maxEntityLength");
    std::istringstream iss(maxEntitylengthS);
    iss >> m_maxlength;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No maxEntityLength defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language << LENDL;
  }
  try
  {
    m_field=unitConfiguration.getParamsValueAtKey("fieldClass");
   
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No fieldClass defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language << LENDL;
  }
   try
  {
    m_db_connect=unitConfiguration.getParamsValueAtKey("dbConnection");
   
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No dbConnection defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language << LENDL;
  }
  LDEBUG << "initialize GeoEntitiesTagger with dbConnection "<<  m_db_connect << LENDL;
   try
  {
    m_dbms=unitConfiguration.getParamsValueAtKey("dbms");
   
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) 
  {
    LERROR << "No dbms defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language << LENDL;
  }
  LDEBUG << "initialize GeoEntitiesTagger with dbms "<<  m_dbms << LENDL;
  
  const Common::PropertyCode::PropertyManager& macroManager=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO");
  const Common::PropertyCode::PropertyManager& microManager=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO");
  m_macroAccessor=&macroManager.getPropertyAccessor();
  m_microAccessor=&static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyAccessor("MICRO");
  std::map<std::string,Common::XMLConfigurationFiles::ItemWithAttributes> triggers;
  std::map<std::string,Common::XMLConfigurationFiles::ItemWithAttributes> endWords;
   try{
      triggers=unitConfiguration.getMapOfItems("Trigger");
      for (std::map<std::string,Common::XMLConfigurationFiles::ItemWithAttributes>::const_iterator it = triggers.begin(); it!=triggers.end(); it++) 
      {
  std::string unlessStatusBefore=(*it).second.getAttribute("unlessSatusBefore");
  std::string unlessMicroBefore=(*it).second.getAttribute("unlessMicroBefore");
  std::string unlessFirstToken=(*it).second.getAttribute("unlessFirstToken");
        bool withoutFirstToken=false;
        if(unlessFirstToken.compare("YES")==0)withoutFirstToken=true;     
  std::set<std::string> status=getSet(unlessStatusBefore);
  std::set<std::string> stringmicros=getSet(unlessMicroBefore);
  std::set<LinguisticCode> micros;
  
  for(std::set<std::string>::const_iterator its=stringmicros.begin();its!=stringmicros.end();its++)
  {
    micros.insert(microManager.getPropertyValue(*its));
  }
  std::string  name= (*it).second.getName();
        boost::tuple<std::set<LinguisticCode>, std::set<std::string> ,bool> p=boost::make_tuple(micros,status,withoutFirstToken);
  if (name.compare("Status")==0)
  {
          
    m_TriggerStatus[(*it).first]=p;
  }
  if (name.compare("Micro")==0)
  {
    m_TriggerMicro[microManager.getPropertyValue((*it).first)]=p;
          LDEBUG << "This Micro code "<<microManager.getPropertyValue((*it).first) <<"is a Trigger"<< LENDL; 
  }
      }
   }
   catch(Common::XMLConfigurationFiles::NoSuchParam& )
   {
     LERROR << "No Trigger defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language << LENDL;
   }
   try{
      endWords=unitConfiguration.getMapOfItems("EndWord");
      for (std::map<std::string,Common::XMLConfigurationFiles::ItemWithAttributes>::const_iterator it = endWords.begin(); it!=endWords.end(); it++) 
      {
  std::string unlessStatusBefore=(*it).second.getAttribute("unlessSatusBefore");
  std::string unlessMicroBefore=(*it).second.getAttribute("unlessMicroBefore");
  std::string unlessFirstToken=(*it).second.getAttribute("unlessFirstToken");      
  std::set<std::string> status=getSet(unlessStatusBefore);
  std::set<std::string> stringmicros=getSet(unlessMicroBefore);
  std::set<LinguisticCode> micros;
        bool withoutFirstToken=false;
        if(unlessFirstToken.compare("YES")==0)withoutFirstToken=true;   
  
  for(std::set<std::string>::const_iterator its=stringmicros.begin();its!=stringmicros.end();its++)
  {
    micros.insert(microManager.getPropertyValue(*its));
  }
  std::string name= (*it).second.getName();
        boost::tuple<std::set<LinguisticCode>, std::set<std::string> ,bool> p=boost::make_tuple(micros,status,withoutFirstToken);
  if (name.compare("Status")==0)
  {
    m_EndWordStatus[(*it).first]=p;
  }
  if (name.compare("Micro")==0)
  {
    m_EndWordMicro[microManager.getPropertyValue((*it).first)]=p;
  }
      }
   }
   catch(Common::XMLConfigurationFiles::NoSuchParam& )
   {
     LERROR << "No Trigger defined in "<<unitConfiguration.getName()<<" configuration group for language " << m_language << LENDL;
   }
 
   
  try
  {
    string dico = unitConfiguration.getParamsValueAtKey("charChart");
    AbstractResource* res = LinguisticResources::single().getResource(manager->getInitializationParameters().media,dico);
    m_charChart = static_cast< FlatTokenizer::CharChart* >(res);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    LERROR << "no param 'charChart' in GeoEntitiesTagger group for language " << m_language << LENDL;
    throw InvalidConfiguration();
  }

   m_db=new GeoDbContext(m_dbms,m_db_connect);
   
   
}

std::set<std::string> GeoEntitiesTagger::getSet(std::string chaine) const
{
  std::string::size_type pos=chaine.find(",");
  std::set<std::string> res;
  std::string::size_type posd=0;
  while ((pos!=string::npos) && posd<chaine.size())
  {
    res.insert(chaine.substr(posd,pos));
    posd=pos+1;
    pos=chaine.find(",",posd);
  }
  if (posd!=string::npos) res.insert(chaine.substr(posd));
  return res;
} 

std::map<std::string,std::vector<uint64_t> ,lTokenMap> GeoEntitiesTagger::getLocationMap(std::string word) const
{
  std::map<std::string,std::vector<uint64_t>, lTokenMap > result;

  GEOENTITIESTAGGERLOGINIT;
  
  std::string word_tiret=word;
  std::string word_blanc=word;
  word_blanc.append(" %");
  word_tiret.append("-%");
  
  
  
  std::string loc_name;

  uint64_t loc_id;
  LDEBUG << "Before Statement" <<  LENDL;
  
  if (! m_db->isConnected()) 
   {
     LERROR << "Problème de connexion à la base" << LENDL;
     return result;
   }
   else
   {  
    LDEBUG << "Mdb is connected, word to find=" <<word<< LENDL;
    

  statement m_BlobManipStatement ( m_db->getDb2() );
  m_BlobManipStatement.alloc();
  m_BlobManipStatement.prepare ( "select GAZETIKI_ID, NAME  from LOCATION_NAMES where NAME =:word OR NAME like :word_tiret OR NAME like :word_blanc" );
  m_BlobManipStatement.exchange ( into ( loc_id ) );
  m_BlobManipStatement.exchange ( into ( loc_name ) );
  m_BlobManipStatement.exchange ( use ( word ) );
  m_BlobManipStatement.exchange ( use ( word_tiret ) );
  m_BlobManipStatement.exchange ( use ( word_blanc ) );
  m_BlobManipStatement.define_and_bind();
  m_BlobManipStatement.execute ( false );
 

  while ( m_BlobManipStatement.fetch() )
    {
       LDEBUG << "LOC_NAME" <<loc_name<< LENDL;
       std::string loc_name_normalized=Normalize(loc_name);     
       LDEBUG << "Normalized LOC_NAME" <<loc_name_normalized<< LENDL;
       if(result.find(loc_name_normalized) == result.end())
       {
         std::vector<uint64_t> vect;
         vect.push_back(loc_id);
         result[loc_name_normalized]=vect;
       }
       else
         result[loc_name_normalized].push_back(loc_id);
    }
  return result;
  }
}

std::vector<boost::tuple<std::string,std::string,std::string> >GeoEntitiesTagger::getAllLocationClass(uint64_t id) const
{
  std::vector<boost::tuple<std::string,std::string,std::string> >  result;
  std::string classe1; 
  std::string classe2;
  std::string classe3;
  
  //GEOENTITIESTAGGERLOGINIT;

  //uint64_t loc_id;


  statement m_BlobManipStatement ( m_db->getDb2() );
  m_BlobManipStatement.alloc();
  m_BlobManipStatement.prepare ( "select CLASS_1, CLASS_2, CLASS_3  from LOCATION where GAZETIKI_ID = :id" );
  m_BlobManipStatement.exchange ( into ( classe1 ) );
  m_BlobManipStatement.exchange ( into ( classe2 ) );
  m_BlobManipStatement.exchange ( into ( classe3 ) );
  m_BlobManipStatement.exchange ( use ( id ) );
  m_BlobManipStatement.define_and_bind();
  m_BlobManipStatement.execute ( false );

  while ( m_BlobManipStatement.fetch() )
    {
       boost::tuple<std::string,std::string,std::string> locationclasses;
       locationclasses.get<0>()=classe1;
       locationclasses.get<1>()=classe2;
       locationclasses.get<2>()=classe3;
       result.push_back(locationclasses);
    }
  return result;
}

std::vector<std::string> GeoEntitiesTagger::getLocationField(uint64_t id,std::string field) const
{
  std::vector<std::string>  result;
  std::string classe1; 
 
  
  //GEOENTITIESTAGGERLOGINIT;

  //uint64_t loc_id;


  statement m_BlobManipStatement ( m_db->getDb2() );
  m_BlobManipStatement.alloc();
  std::string query="select ";
  query.append(field);
  query.append(" from LOCATION where GAZETIKI_ID = :id");
  m_BlobManipStatement.prepare (query);
  m_BlobManipStatement.exchange ( into ( classe1 ) );
  m_BlobManipStatement.exchange ( use ( id ) );
  m_BlobManipStatement.define_and_bind();
  m_BlobManipStatement.execute ( false );
  //int rien ( 1 );


  while ( m_BlobManipStatement.fetch() )
    {
       result.push_back(classe1);
    }
  return result;
}


/**
 * 
 * @param analysis 
 * @return 
 */
LimaStatusCode GeoEntitiesTagger::process(
  AnalysisContent& analysis) const
{
  GEOENTITIESTAGGERLOGINIT;
  TimeUtils::updateCurrentTime();
  LDEBUG << "start GeoEntitiesTagger" << LENDL;
  
  // ici normalement on peut prendre soit analysis graph soit le prostgraph, cela doit être paramétré
  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData(m_graphId));
  if (anagraph==0)
  {
    LERROR << "no "<< m_graphId << " ! abort" << LENDL;
    return MISSING_DATA;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no AnnotationData ! abort" << LENDL;
    return MISSING_DATA;
  }
  
  compute_entities(anagraph->getGraph(), anagraph,   annotationData);

  TimeUtils::logElapsedTime("GeoEntitiesTagger");
  return SUCCESS_ID;
}

bool GeoEntitiesTagger::isTrigger(LinguisticGraphVertex v,LinguisticGraph* graph, LinguisticGraphVertex vprev, AnalysisGraph* anagraph) const
{
  GEOENTITIESTAGGERLOGINIT;
  
  bool result=false;
  Token* token = get(vertex_token, *graph, v);  
  //Token* previous_token = get(vertex_token, *graph, vprev);  
  LDEBUG << "isTrigger Token=" << Lima::Common::Misc::limastring2utf8stdstring(token->stringForm()) << LENDL;
  std::string statusKey=Lima::Common::Misc::limastring2utf8stdstring(token->status()->defaultKey());
  std::map< std::string, boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> ,bool> >::const_iterator it;
  it=m_TriggerStatus.find(statusKey);
  if (it!=m_TriggerStatus.end())
  { 
    boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> ,bool> exclude;
    exclude=(*it).second;
    if (vprev == anagraph->firstVertex() && !exclude.get<2>() ) return true;
    else if (vprev == anagraph->firstVertex() && exclude.get<2>() ) return false;
    else
    {    
      if (hasStatus(vprev,graph,exclude.get<1>())) return false;
      else if (hasMicro(vprev,graph,exclude.get<0>())) return false;
      else return true;
    }
  }
  else
  {
    bool micro_found=false;
    MorphoSyntacticData* data = get(vertex_data, *graph, v);
    if (data ==0 || data->empty())
      return false;
    else
    {
      std::set<LinguisticCode> categ=data->allValues(*m_microAccessor);
      std::set<LinguisticCode>::const_iterator sit (categ.begin()), send(categ.end());
      while(sit!=send && !micro_found )  
      {
        micro_found=(m_TriggerMicro.find(*sit) != m_TriggerMicro.end());
        if (!micro_found) sit++;
        LDEBUG << "Current Micro=" << *sit << LENDL;
      }
      LDEBUG << "Micro_Found ="<<micro_found << LENDL;
      if (! micro_found) return false;
      else
      {
        std::map< Lima::LinguisticCode, boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> ,bool> >::const_iterator itm;
  itm=m_TriggerMicro.find(*sit);
  boost::tuple<std::set<Lima::LinguisticCode>, std::set<std::string> ,bool> exclude;
  exclude=(*itm).second;
        if (vprev == anagraph->firstVertex() && !exclude.get<2>() ) return true;
        else if (vprev == anagraph->firstVertex() && exclude.get<2>() ) return false;
        else
        {  
    if (hasStatus(vprev,graph,exclude.get<1>())) return false;
    else if(hasMicro(vprev,graph,exclude.get<0>())) return false;
    else return true;
        }
      }
    }
  }
  return result;
}




bool GeoEntitiesTagger::hasMicro(LinguisticGraphVertex v,LinguisticGraph* graph, std::set< Lima::LinguisticCode> setofmicro) const
{
  bool result=false;
  MorphoSyntacticData* data = get(vertex_data, *graph, v);
    if (data ==0 || data->empty())
      result=false;
    else
    {
      std::set<LinguisticCode> categ=data->allValues(*m_microAccessor);
      std::set<LinguisticCode>::const_iterator sit (categ.begin()), send(categ.end());
      for(;sit!=send && !result;++sit)  result=(setofmicro.find(*sit) != setofmicro.end());
    }
  return result;
}


bool GeoEntitiesTagger::hasStatus(LinguisticGraphVertex v,LinguisticGraph* graph, std::set< std::string > setofstatus) const
{
  Token* token = get(vertex_token, *graph, v);  
  std::string statusKey=Lima::Common::Misc::limastring2utf8stdstring(token->status()->defaultKey());
  return(setofstatus.find(statusKey)!=setofstatus.end());
}

void GeoEntitiesTagger::compute_entities(LinguisticGraph* graph,AnalysisGraph* anagraph, Common::AnnotationGraphs::AnnotationData* annotationData) const
{
  GEOENTITIESTAGGERLOGINIT;
  
  if (annotationData->dumpFunction("GeoEntity") == 0)
  {
    annotationData->dumpFunction("GeoEntity", new DumpGeoEntityAnnotation());
  }

  //   LinguisticGraphVertex v;
  
  const LinguisticGraphVertex firstVx = anagraph->firstVertex();
  const LinguisticGraphVertex lastVx = anagraph->lastVertex();
  
  std::set<LinguisticGraphVertex> visited;
  std::queue<LinguisticGraphVertex> toVisit;
  toVisit.push(firstVx);
  
  LinguisticGraphOutEdgeIt outItr,outItrEnd;
  
  
    
  Token* token=0;
  Token* previous_token=0;
  LinguisticGraphVertex v,previous_v;
  previous_v=firstVx;
//  uint64_t position;
//   uint64_t length=0;
  
  
  bool success=false;
  while (!toVisit.empty())
  {
    v=toVisit.front();
    toVisit.pop();
    if (v != lastVx) {
    

      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph);
         outItr!=outItrEnd;
         outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }
    }
    if (v != firstVx && v != lastVx)
    {
      token = get(vertex_token, *graph, v);  
      if (isTrigger(v,graph,previous_v,anagraph))
      {
        LDEBUG << "It s a trigger " << LENDL;
        bool lower_match=false;;
        std::map<std::string,std::vector<uint64_t>, lTokenMap > results;
        
        std::string word = Normalize(token->stringForm());
        results= getLocationMap(word);

        std::map<std::string,std::vector<uint64_t>, lTokenMap >::iterator iTl;
        iTl=results.lower_bound(word);
        success=(results.find(word)!=results.end());
        lower_match=(iTl!=results.end() && (iTl->first.find(word)!=string::npos));
       
                
        std::string last_found_word;
        
        
        
        
        unsigned short length=1;
        
        if (success || lower_match)
        {
          std::vector<LinguisticGraphVertex> exact_match_vertexs;
          std::vector<LinguisticGraphVertex> lower_match_vertexs;
          if (success)
          {
            exact_match_vertexs.push_back(v);
            last_found_word=word;
          }
          else
            lower_match_vertexs.push_back(v);
          
          //LinguisticGraphVertex vfirst=v;
         
          while ((success || lower_match) && length < m_maxlength && v != lastVx)
    {
      LinguisticGraphVertex vne=toVisit.front();
            if (vne == lastVx) {
              break;
            }
      Token *tok=get(vertex_token, *graph, vne);
            if (tok==0) {
              LERROR << "Error: no token found for vertex " << vne << LENDL;
              v=vne;
              toVisit.pop();
              continue;
            }
            word.append(" ");
      word.append(Normalize(tok->stringForm()));
            
            
            
            success=(results.find(word)!=results.end());
            iTl=results.lower_bound(word);
            
            lower_match=(iTl!=results.end() && (iTl->first.find(word)!=string::npos));
            
        
        
      if (success || lower_match)
      {
              if (success)
              {
                exact_match_vertexs.insert(exact_match_vertexs.end(),lower_match_vertexs.begin(),lower_match_vertexs.end());
                exact_match_vertexs.push_back(vne);
                lower_match_vertexs.clear();
                last_found_word=word;
              } 
              else 
                lower_match_vertexs.push_back(vne);
        length=length+1;
        v=vne;
        toVisit.pop();
        if (v != lastVx)
        {
      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph);
      outItr!=outItrEnd;
      outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }
                  // mettre le type de V à MIDDLE
        }
              
      }
    }// end while
          if ( lower_match_vertexs.size()!=0)
          {
            v=lower_match_vertexs.front();
            lower_match_vertexs.clear();
          }
          bool first=true;
          LDEBUG << "found word " << last_found_word << "  found_vertexs.size()=" << exact_match_vertexs.size() << LENDL;
          for(std::vector<LinguisticGraphVertex>::const_iterator it=exact_match_vertexs.begin();it!=exact_match_vertexs.end();)
          {
            std::set< AnnotationGraphVertex > matches = annotationData->matches(anagraph->getGraphId(),*it,"annot");
            
            PositionType t;
            if (first)
              t=FIRST;
            else
              t=MIDDLE;
            std::set<std::string> all_classes;
            std::vector<uint64_t> vectId=results[last_found_word];
            for(std::vector<uint64_t>::iterator iTid=vectId.begin();iTid!=vectId.end();iTid++)
            {
              std::vector<std::string> classes=getLocationField(*iTid,m_field);
              for (uint64_t i=0; i< classes.size(); i++) all_classes.insert(classes[i]);  
            } 
            (it++);
            if (it==exact_match_vertexs.end() && first)  t=UNIQ;
            if (it==exact_match_vertexs.end() && !first) t=LAST;
            
            GeoEntityAnnotation geoEntity(t,all_classes);
            GenericAnnotation ga(geoEntity);
            annotationData->annotate(*matches.begin(),Common::Misc::utf8stdstring2limastring("GeoEntity"), ga);
            first=false;
          }
        }// end success pour le trigger
      }// if Token is a Trigger    
    }
    else
    {
      if (v == lastVx)
      {
          
        
      }
    }
    previous_token=token;
    previous_v=v;
  }
  
}

std::string GeoEntitiesTagger::Normalize (const LimaString  tokenstring) const
{
 //GEOENTITIESTAGGERLOGINIT;

 LimaString token=LimaString(tokenstring);
 string::size_type pos = token.indexOf(LimaChar(L'-'), 0);
  while (pos != -1)
  {
    token[pos] = LimaChar(L' ');
    pos = token.indexOf(LimaChar(L'-'), pos+1);
  }
 std::string word= Lima::Common::Misc::limastring2utf8stdstring(m_charChart->toLower(token));
 return (word);
}

std::string GeoEntitiesTagger::Normalize (const   std::string tokenstring) const
{
 //GEOENTITIESTAGGERLOGINIT;

 LimaString token=Lima::Common::Misc::utf8stdstring2limastring(tokenstring);
 string::size_type pos = token.indexOf(LimaChar(L'-'), 0);
  while (pos != -1)
  {
    token[pos] = LimaChar(L' ');
    pos = token.find(LimaChar(L'-'), pos+1);
  }
 std::string word= Lima::Common::Misc::limastring2utf8stdstring(m_charChart->toLower(token));
 return (word);
}


bool GeoEntitiesTagger::DichoFind (std::string,std::vector<string>) const
{

 return (true);
}



} // closing namespace GeoEntities
} // closing namespace LinguisticProcessing
} // closing namespace Lima
