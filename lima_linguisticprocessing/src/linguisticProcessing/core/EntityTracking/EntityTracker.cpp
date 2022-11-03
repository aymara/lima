// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "EntityTracker.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "CoreferenceEngine.h"
#include "CoreferenceData.h"





#include <fstream>
#include <queue>

using namespace std;
using namespace boost;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;


namespace Lima
{
namespace LinguisticProcessing
{
namespace EntityTracking
{

SimpleFactory<MediaProcessUnit,EntityTracker> EntityTrackerFactory(ENTITYTRACKER_CLASSID);

EntityTracker::EntityTracker()
{
}

/*EntityTracker::EntityTracker(const Automaton::RecognizerMatch& entity,
                           FsaStringsPool& sp)
{
}*/
EntityTracker::~EntityTracker() {}

void EntityTracker::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& /*unitConfiguration*/,
            Manager* /*manager*/)
    
{
}

LimaStatusCode EntityTracker::process(AnalysisContent& analysis) const
{
  TimeUtils::updateCurrentTime();
  SELOGINIT;

  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
    LERROR << "no LinguisticMetaData ! abort";
    return MISSING_DATA;
  }

  auto anagraph = std::dynamic_pointer_cast<AnalysisGraph>(analysis.getData("AnalysisGraph"));
  if (anagraph==0)
  {
    LERROR << "no graph 'AnaGraph' available !";
    return MISSING_DATA;
  }

  auto annotationData = std::dynamic_pointer_cast< AnnotationData >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LERROR << "no annotation graph available !";
    return MISSING_DATA;
  }

  // add new data to store co-references
  CoreferenceData* corefData = new CoreferenceData;
  analysis.setData("CoreferenceData",corefData);
  
  CoreferenceEngine ref;
  LinguisticGraph* graph=anagraph->getGraph();
  LinguisticGraphVertex lastVertex=anagraph->lastVertex();
  LinguisticGraphVertex firstVertex=anagraph->firstVertex();

  std::queue<LinguisticGraphVertex> toVisit;
  std::set<LinguisticGraphVertex> visited;

  LinguisticGraphOutEdgeIt outItr,outItrEnd;

  // output vertices between begin and end,
  // but do not include begin (beginning of text or previous end of sentence) and include end (end of sentence)
  toVisit.push(firstVertex);

  bool first=true;
  bool last=false;
  while (!toVisit.empty()) {
    LinguisticGraphVertex v=toVisit.front();
    toVisit.pop();
    if (last || v == lastVertex) {
      continue;
    }
    if (v == lastVertex) {
      last=true;
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

    if (first) {
      first=false;
    }
    else {
   // first, check if vertex corresponds to a specific entity
    std::set< AnnotationGraphVertex > matches = annotationData->matches("AnalysisGraph",v,"annot");
    for (std::set< AnnotationGraphVertex >::const_iterator it = matches.begin();
         it != matches.end(); it++)
    {
      AnnotationGraphVertex vx=*it;
      Token* t=get(vertex_token,*graph,vx);
      /* sauvegarde de tous les vertex */
      if (t != 0)
      {
        //storeAllToken(t);
        //allToken.push_back(t);
        ref.storeAllToken(*t);
      }
      if (annotationData->hasAnnotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")))
      {
        /*const SpecificEntityAnnotation* se =
          annotationData->annotation(vx, Common::Misc::utf8stdstring2limastring("SpecificEntity")).
          pointerValue<SpecificEntityAnnotation>();*/
        //storeSpecificEntity(se);
        //Token* t=get(vertex_token,*graph,vx);
        //storedAnnotations.push_back(*t);
        ref.storeAnnot(*t);
//             std::cout<< "le vertex de nom "<< t->stringForm()<<std::endl;
      }
      }
    }
  }

  /* recherche des coréferences entre les entitées nommées précédemment détectées */

  vector<Token> vectTok;
  vector<Token>::const_iterator it1=ref.getAnnotations().begin(), it1_end=ref.getAnnotations().end();
  for (;
       it1 != it1_end;
       it1++)
  {
//     checkCoreference (*it1,ref);
    vectTok = ref.searchCoreference(*it1);
    if (vectTok.size() > 0)
    {
      corefData->push_back(vectTok);
    }
    ref.searchCoreference(*it1);
  }

  /* get the text */
//   LimaStringText* text=static_cast<LimaStringText*>(analysis.getData("Text"));
  
  return SUCCESS_ID;
}


// bool EntityTracker::checkCoreference(const Token& tok, CoreferenceEngine ref) const
// {
//   //allToken = ref.getToken();
//   vector<Token> vectTok = ref.searchCoreference(tok);
//   if (vectTok.size() > 1)
//   {
//     ref.storeFindedToken(vectTok);
//   }
//   ref.searchCoreference(tok);
// }


/*
void EntityTracker::storeAllToken(const Token* tok)
{
  allToken.push_back(*tok);
}*/
/*
void EntityTracker::storeSpecificEntity (const Lima::LinguisticProcessing::
        SpecificEntities::SpecificEntityAnnotation * se) const
{
    // look at the vertex
      Token* t=get(vertex_token,*graph,v);
      if (t!=0) {
 la condition sera si l'annotation est une personne, un lieu ou bien une organisation
}*/

//   //////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////
// bool EntityTracker::isInclude(const std::string original, const std::string currentWord) const
// {
//   if (currentWord.size() > original.size())
//     return false;
//   
//   uint64_t comptCurrent(0);
//   for (uint64_t i(0); i< original.size(); i++)
//   {
//     if (original[i] == currentWord[comptCurrent])
//     {
//       comptCurrent++;
//     }
//   }
//   if (comptCurrent == currentWord.size())
//     return true;
//   else
//     return false;
// }
// 
// //////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////
// bool EntityTracker::isAcronym(const std::string original, const std::string currentWord) const
// {
//   for (std::vector< std::vector<std::string> >::const_iterator it = Acronyms.begin(), it_end = Acronyms.end();
//        it != it_end;
//        it++)
//   {
//     if (strcmp((*(*it).begin()).c_str(),original.c_str()) == 0)
//     {
//       for (std::vector<string>::const_iterator it_intern = (*it).begin(), it_int_end = (*it).end();
//            it_intern != it_int_end;
//            it_intern++)
//       {
//         if ((strcmp((*it_intern).c_str(),currentWord.c_str()) == 0) ||
//             (isInclude(*it_intern,currentWord)))
//           return true;
//       }
//     }
//   }
//   return false;
// }
// 
// //////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////
// void EntityTracker::addNewForm(const std::string original, const std::string currentWord)
// {
//   for (std::vector< std::vector<std::string> >::iterator it = Acronyms.begin(), it_end = Acronyms.end();
//        it != it_end;
//        it++)
//   {
//     if (strcmp((*(*it).begin()).c_str(),original.c_str()) == 0)
//     {
//       (*it).push_back(currentWord);
//       return;
//     }
//   }
// }
// 
// //////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////
// bool EntityTracker::exist(const std::string mot, const std::vector< std::vector<std::string> > Acronyms) const
// {
//   std::vector<std::string>::const_iterator iterat;
//   for (std::vector< std::vector<std::string> >::const_iterator it = Acronyms.begin(), it_end = Acronyms.end();
//        it != it_end;
//        it++)
//   {
//     iterat = find((*it).begin(),(*it).end(),mot);
//     if (iterat != (*it).end())
//     {
//       return true;
//     }
//   }
//   return false;
// }
// 
// //////////////////////////////////////////////////////////////////////////////////////////
// //////////////////////////////////////////////////////////////////////////////////////////
// void EntityTracker::searchCoreference(const std::string text)
// {
// 
//   string mot;
// 
//   /*
//   LimaStringText* text=static_cast<LimaStringText*>(analysis.getData("Text"));
// 
//   vector<string::size_type> paragraphPositions;
//   string::size_type currentPos=0;
//   string::size_type i=text->find(m_paragraphSeparator,currentPos);
//   while (i!=string::npos) {
//     paragraphPositions.push_back(i);
//     // goto next char that is not a carriage return
//     currentPos=text->find_first_not_of(m_paragraphSeparator,i+1);
//     i=text->find(m_paragraphSeparator,currentPos);
//   }
//   */
// 
//   /* parcourir tous les noeuds */
// 
//     /* Chercher si le type du noeud appartient à l'ensemble des personne, organisation ou bien lieu */
// 
//       /* Si le mot existe dans le vecteur des acronyms, ça ne sert à rien chercher ses acronyms parce qu'ils
//         sont recherchés */
//       if (!exist(mot, Acronyms))
//       {
//         
//       }
//       /* Recherche dans le texte de toutes les formes de mot précédent dans tout le text */
//       
// }



}
}
}
