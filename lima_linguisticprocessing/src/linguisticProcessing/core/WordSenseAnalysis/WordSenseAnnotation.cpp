// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "WordSenseAnnotation.h"

#include "KnnSearcher.h"
#include "common/Data/LimaString.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/core/SyntacticAnalysis/SyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"


//#include "common/time/traceUtils.h"

#include <iostream>


//using namespace boost;
using namespace std;
#include "common/Data/strwstrtools.h"

using namespace Lima::Common::Misc;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SyntacticAnalysis;


namespace Lima
{
namespace LinguisticProcessing
{
namespace WordSenseDisambiguation
{


int DumpWordSense::dump(ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const
{

  PROCESSORSLOGINIT;
  try
  {
    ga.value<WordSenseAnnotation>().dump(os);
    return SUCCESS_ID;
  }
  catch (const boost::bad_any_cast& e)
  {
    LERROR << "This annotation is not a WordSenseAnnotation ; nothing dumped";
    return UNKNOWN_ERROR;
  }
}

 /** general test functions */


bool WordSenseAnnotation::isDisambiguated() const
{
  return confidence() != -1 ;
}




  /** main functions of the global algorithm (called by WordSenseDisambiguator) */

bool WordSenseAnnotation::disambiguate( const WordUnit& wu)
{
  LOGINIT("WordSenseDisambiguator");
  if (wu.nbSenses()==0 || wu.wordSensesUnits().size()==0)
  {
    return false;
  }
  switch(m_mode)
  {
    case B_MOST_FREQUENT:
    case B_JAWS_MOST_FREQUENT:
    case B_ROMANSEVAL_MOST_FREQUENT:
      for (set<WordSenseUnit>::iterator itSenses = wu.wordSensesUnits().begin();
          itSenses != wu.wordSensesUnits().end();
          itSenses++)
      {
        if (itSenses->senseId()==0 && m_mode == itSenses->mode())
        {
          m_confidence = 0;
          retrieveSense(itSenses);
          break;
        }
      }
      break;
    case S_WSI_MRD:
      LERROR << "Wrong usage";
      // fall through
    default:
      LWARN << "No Disambiguation processing. Bad configuration";
      return false;
  }

  if (m_confidence == -1)
  {
    return false;
  }
  return true;
}


bool WordSenseAnnotation::disambiguate( KnnSearcher* searcher,
          const WordUnit& wu,
          const SemanticContext& context,
          double thres,
          char method)
{
  LOGINIT("WordSenseDisambiguator");
  LDEBUG << "Begin disambiguation of "<< wu.lemma();
  if (wu.nbSenses()==0 || wu.wordSensesUnits().size()==0)
  {
    return false;
  }
  switch(m_mode)
  {
    case S_WSI_MRD:
      classifKnnMRD(searcher, wu, context, thres, method);
      break;
    case B_MOST_FREQUENT:
    case B_JAWS_MOST_FREQUENT:
    case B_ROMANSEVAL_MOST_FREQUENT:
      LERROR << "Wrong usage";
      // fall through
    default:
      LWARN << "No Disambiguation processing. Bad configuration";
      return false;
  }

  if (m_confidence == -1)
  {
    return false;
  }
  return true;
}

float WordSenseAnnotation::classifKnnMRD(KnnSearcher* searcher,
           const WordUnit& wu,
           const SemanticContext& context,
           double thres,
           char method)
{
  LOGINIT("WordSenseDisambiguator");
  float confidence = -1;
  map<int, map<string, double> > cvListRep;
  // Compute needed knn dists
  for (SemanticContext::const_iterator itContext = context.begin();
         itContext!= context.end();
         itContext++) {

    LDEBUG << " Computing knn dists for " << itContext->first ;
    NNList knns;
    searcher->getKNN(wu.lemmaId(), itContext, knns)  ;

    for (NNList::iterator itNN = knns.begin();
          itNN!= knns.end();
          itNN++)
    {
      LDEBUG << itNN->first << ":" << itNN->second;
    }
    if (knns.size()==0 || knns.rbegin()->second==0) {

      LWARN <<  wu.lemmaId() << " : " << wu.lemma() << " has a corrupted sphere. Process may return biased results ";

      for (NNList::iterator itNN = knns.begin();
          itNN!= knns.end();
          itNN++) {
  LDEBUG << itNN->first << ":" << itNN->second;
      }

      continue;
    }
     computeConfidenceVector(wu, knns, itContext, cvListRep);
  }

  // Classsify

  std::set<WordSenseUnit>::iterator bestSense = classify(wu, context, cvListRep, thres, method);
  retrieveSense(bestSense);
  return confidence;
}

/* internal computation functions called by classifKnnMRD */


std::set<WordSenseUnit>::iterator WordSenseAnnotation::classify(const WordUnit& wu,
                 const SemanticContext& context,
                 const map<int, map<string, double> >& cvListRep,
                 const double thres,
                 const char method  )
{
  LOGINIT("WordSenseDisambiguator");
  LDEBUG << "Classifying : " << wu.lemma() << " nbSenses : " << wu.wordSensesUnits().size();
  // prepare confidence vectors
  map<int, double> assignedClasses;
  map<int, double> scoresByClass;
  double max = 0;
  set<WordSenseUnit>::iterator bestSense;
#ifndef WIN32
  for(set<WordSenseUnit>::iterator itSenses = wu.wordSensesUnits().begin();
           itSenses!= wu.wordSensesUnits().end();
           itSenses++ ) {
    LDEBUG << "cluster : " << itSenses->senseId();
    double checkSum = 0;
    double w,
        val_cluster = 0.;
    double entropySum=sumLogCvList(cvListRep);


    /* print debug */
    for (map<int, map<string, double> >::const_iterator itdeb = cvListRep.begin();
              itdeb!= cvListRep.end();
              itdeb++) {
      LDEBUG << "cvlistrepsize : " << itdeb->first  <<  " : " << itdeb->second.size();
      for (map<string, double>::const_iterator itdeb2 = itdeb->second.begin();
           itdeb2 !=itdeb->second.end();
           itdeb2++) {
        LDEBUG << itdeb2->first << " -> " << itdeb2->second;
      }
    }
    /* end print debug */
    for (SemanticContext::const_iterator itContext = context.begin();
           itContext!= context.begin();
           itContext++)  {
      if (cvListRep.find(itSenses->senseId())!=cvListRep.end()
  && cvListRep.find(itSenses->senseId())->second.find(itContext->first)
   ==cvListRep.find(itSenses->senseId())->second.end() ) {
        w=0;
        LDEBUG << itContext->first << " not found for cluster "<< itSenses->senseId();
      } else  {
          w = sumLogCv(cvListRep, itContext->first) / entropySum;
          LDEBUG << itContext->first << " "<< itSenses->senseId() << " w= " << sumLogCv(cvListRep, itContext->first) << "/" << entropySum << " = " << w ;
        }
        val_cluster += w * cvCluster(cvListRep, itContext->first, itSenses->senseId());
        checkSum+=w;
      }
      switch (method) {
      case 'B':
        val_cluster/=itSenses->senseMembersIds().size();
        break;
      case 'C' :
        val_cluster/=log(1+itSenses->senseMembersIds().size());
        break;
      case 'D' :
        val_cluster/=log(10+itSenses->senseMembersIds().size());
        break;
      default:
        break;
      }
      if(val_cluster>max) {
        max=val_cluster;
  bestSense=itSenses;
      }
      scoresByClass[itSenses->senseId()]=val_cluster;
      LDEBUG << "SCORES : " << itSenses->senseId() << " -> " << val_cluster;
  }

  //  cerr << "THRESCUT2 : "<< thresCut << endl;
  for(map<int, double >::iterator itSenses = scoresByClass.begin();
          itSenses!= scoresByClass.end();
          itSenses++ ) {
    LDEBUG << wu.lemma() << " : BEFORE ASSIGN : "  << scoresByClass[itSenses->first] << " - " << thres*max ;
    if (scoresByClass[itSenses->first]>=(thres*max)
        //      && scoresByClass[itcl->first]>= thresCut
        && max!=0) {
      assignedClasses[itSenses->first]=scoresByClass[itSenses->first];
    }
  }
#endif
  return bestSense;
}


int WordSenseAnnotation::computeConfidenceVector(const WordUnit& wu,
               const NNList& knns,
               const SemanticContext::const_iterator itContext,

               map<int, map<string, double> >& cvListRep)
{
#ifndef WIN32
    LOGINIT("WordSenseDisambiguator");
    map<int, double> simByCluster;
    int maxDist = 0;
    if (knns.size()>0)
    {
      maxDist = knns.rbegin()->second;
    }
    //  somme des distances des ppv du mot dans 'id_relation' qui ont été attribués au cluster 'id_cluster'
    for(set<WordSenseUnit>::iterator itSenses = wu.wordSensesUnits().begin();
             itSenses!= wu.wordSensesUnits().end();
             itSenses++ )
    {
      double dist;
      double sim_cluster=0;
      LDEBUG << itSenses->senseTag() << " : " << itSenses->senseMembersIds().size() ;
      for(set<uint64_t>::iterator itMembers =itSenses->senseMembersIds().begin() ;
             itMembers!=itSenses->senseMembersIds().end() ;
             itMembers++ ) {
  LDEBUG << "itMembers "<< *itMembers;
        if (knns.find(*itMembers)!=knns.end()) {
    LDEBUG << "Dist "<< knns.find(*itMembers)->second;
          dist = knns.find(*itMembers)->second;
          double distCos = 1-cos(dist*M_PI/16384);
          if (distCos!=0.) {
          //sim_cluster += (1 / (dist*dist));
            sim_cluster += (1 / (distCos*distCos));
          }
        }
      }


      simByCluster[itSenses->senseId()]=sim_cluster*maxDist*maxDist;
      LDEBUG << sim_cluster << "*" << maxDist <<" = "  <<sim_cluster*maxDist*maxDist;
      LDEBUG << wu.lemmaId()  << " : "<< wu.lemma() << " : similarities : " << itContext->first << " " << itSenses->senseId() << ":" << itSenses->senseTag() << " -> " << simByCluster[itSenses->senseId()];
    }
    //somme des dist totales des clusters de la représentation
    double sim_clusters_rep = 0;
    for (map<int, double>::iterator itdist = simByCluster.begin(); itdist != simByCluster.end(); itdist++) {
      sim_clusters_rep+=itdist->second;
    }
    // compute cv
    if( sim_clusters_rep != 0. ){
      for(set<WordSenseUnit>::iterator itSenses = wu.wordSensesUnits().begin();
            itSenses!= wu.wordSensesUnits().end();
            itSenses++ ) {
        LDEBUG << ": Key : " << wu.lemma() ;
        cvListRep[itSenses->senseId()][itContext->first]=( simByCluster[itSenses->senseId()] / sim_clusters_rep );
        /*TODECIDE
  if (itSenses->senseMembersIds().find(wu.lemmaId())!=itSenses->senseMembersIds().end()) {
          cvListRep[itSenses->senseId()][*itContext]=( simByCluster[itSenses->senseId()] / sim_clusters_rep );
          //          / (itcl->second.size()-1);
        } else {
    cvListRep[itSenses->senseId][*itContext]=( simByCluster[itSenses->senseId] / sim_clusters_rep );
          //        / (itcl->second.size());
  }
        //                     *10/ (10+ log(itcl->second.size()));
  */
        stringstream sscvlist ;
        sscvlist <<  " cvlistrep "<< itSenses->senseId() << " "<< itContext->first <<" : "  << simByCluster[itSenses->senseId()] <<"/"<< sim_clusters_rep << "=" << ( simByCluster[itSenses->senseId()] / sim_clusters_rep )<< endl;
        LDEBUG << sscvlist.str();
      }
    } else {
      // if no intersection between knn of relation and verbes in classes
      LWARN << "No intersection between " << itContext->first
            << " and elements in classes for " << wu.lemmaId();
    }
    return simByCluster.size();
#else
    return 0;
#endif
}








double WordSenseAnnotation::cvCluster(const map<int, map<string, double> >& cvListRep,
              const string& relation,
              int id_cluster)
{
  if (cvListRep.find(id_cluster) != cvListRep.end()
    && cvListRep.find(id_cluster)->second.find(relation)
      !=cvListRep.find(id_cluster)->second.end())
  {
    return cvListRep.find(id_cluster)->second.find(relation)->second;
  }
  return 0;
}

double WordSenseAnnotation::sumCvCluster(map<int, map<string, double> >& cvListRep, string relation)
{
  double sum_cv_rep = 0.;
  for(map<int, map<string, double> >::iterator itcv = cvListRep.begin(); itcv !=cvListRep.end(); itcv++)
  {
    if (itcv->second.find(relation)!=itcv->second.end())
    {
      sum_cv_rep += itcv->second[relation];
    }
  }
  return sum_cv_rep;
}

//(F)
double WordSenseAnnotation::sumLogCv(const map<int, map<string, double> >& cvListRep,
             const string& relation)
{
  double sum = 0.;
  for(map<int, map<string, double> >::const_iterator itcv = cvListRep.begin();
                 itcv !=cvListRep.end();
                 itcv++)
  {
    if (itcv->second.find(relation)!=itcv->second.end()
      && itcv->second.find(relation)->second!= 0 )
    {
      //stringstream ssquicksum;
      sum += itcv->second.find(relation)->second * log( itcv->second.find(relation)->second );
      /*
      ssquicksum << "QuickSum : " << itcv->second.find(relation)->second <<"*"<< log( itcv->second.find(relation)->second )
                 << " = " <<  itcv->second.find(relation)->second * log( itcv->second.find(relation)->second ) << endl
                 << "from : " << itcv->second.find(relation)->second << " (" << relation << ")" << endl;

      cout << ssquicksum.str();
      */
    }
  }
  return (sum+1);
}

//(G)
double WordSenseAnnotation::sumLogCvList(const map<int, map<string, double> >& cvListRep)
{
  double sum_log = 0.;
  if (cvListRep.size()==0)
  {
    return sum_log;
  }
  for(map<string, double>::const_iterator itrel = cvListRep.begin()->second.begin();
                  itrel!= cvListRep.begin()->second.end();
                  itrel++) {
    sum_log += (double)sumLogCv(cvListRep, itrel->first);
  }

 return sum_log;
}

/* end internal computation functions */

void WordSenseAnnotation::retrieveSense(set<WordSenseUnit>::iterator itSenses)
{
  cerr << itSenses->senseId() << " : " << itSenses->senseTag() << endl;
  m_wsu=*itSenses;
  m_senseTag = itSenses->senseTag();
  cerr << m_wsu.senseId() << " : " << m_wsu.senseTag() << endl;
  cerr << "TEST1 : " << senseId() << " : " << senseTag() << endl;
}


AnnotationGraphVertex WordSenseAnnotation::writeAnnotation(
  Common::AnnotationGraphs::AnnotationData* ad
  ) const
{
//   LOGINIT("WordSenseDisambiguator");

  // can have multiple annotations in case of mapping
  /** Creation of an annotation for the object WordSenseAnnotation */
  GenericAnnotation ga(*this);

  /** Creation of a new vertex (a new annotation anchor) in the annotation graph. */
  ad->annotate(morphVertex(), utf8stdstring2limastring("WordSense"), ga);

  return AnnotationGraphVertex(); //unused;
}


void WordSenseAnnotation::outputXml(std::ostream& xmlStream,const LinguisticGraph& g) const
{
//   LOGINIT("WordSenseDisambiguator");

  xmlStream << "<WORDSENSE SENSEID=\"" << senseId() << "\" SENSETAG=\"" << senseTag() << "\" MODE=\"" << mode() << "\" CONFIDENCE=\"" << confidence() << "\">";


  Token* token = get(vertex_token, g, morphVertex());
  if (token != 0)
  {
    xmlStream << limastring2utf8stdstring(token->stringForm());
  }
  xmlStream << "</WORDSENSE>";
}

std::ostream& operator << (std::ostream& os, const WordSenseAnnotation& wsa)
{
  os << wsa.senseId() << "(" << wsa.mode() << "):" << wsa.senseTag() << " - " << wsa.morphVertex();
  return os;
}
} // closing namespace WordSenseDisambiguation
} // closing namespace LinguisticProcessing
} // closing namespace Lima
