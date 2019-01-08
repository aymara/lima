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
#ifndef LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEANNOTATION_H
#define LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEANNOTATION_H

#include "WordSenseAnalysisExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"

#include <map>
#include <set>
#include <string>
#include "WordUnit.h"
#include "KnnSearcher.h"




namespace Lima
{
namespace Common {
namespace AnnotationGraphs {
    class AnnotationData;
}
}
namespace LinguisticProcessing
{
namespace SyntacticAnalysis {
class SyntacticData;
}
namespace WordSenseDisambiguation
{ 

class LIMA_WORDSENSEANALYSIS_EXPORT WordSenseAnnotation
{
  /**
  *
  * @file        WordSenseAnnotation.h
  * @author      Claire Mouton (Claire.Mouton@cea.fr) 

  *              Copyright (c) 2010 by CEA
  * @date        Created on August, 17 2010  
  *
  */


public:

  /*
typedef std::set<WordSenseAnnotation*> Vertices;
typedef std::map<WordSenseAnnotation*,std::set<WordSenseAnnotation*> > VerticesRelation;
typedef std::map< WordSenseAnnotation*,std::map <WordSenseAnnotation*,float> > WeightedVerticesRelation;
*/
  WordSenseAnnotation() {}

  WordSenseAnnotation(const Mode mode,
                      const Mapping mapping,
                      const float confidence,
                      LinguisticGraphVertex v) : 
      m_wsu(),
      m_mode(mode),
      m_mapping(mapping),
      m_confidence(confidence),
      m_morphVertex(v),
      m_senseTag()
      {}

  WordSenseAnnotation(const Mode mode,
                      const Mapping mapping,
                      LinguisticGraphVertex v) : 
      m_wsu(),
      m_mode(mode),
      m_mapping(mapping),
      m_confidence(-1),
      m_morphVertex(v),
      m_senseTag()
      {}

  WordSenseAnnotation(const WordSenseAnnotation& wsa) : 
  m_wsu(wsa.m_wsu),
  m_mode(wsa.m_mode),
  m_mapping(wsa.m_mapping),
  m_confidence(wsa.m_confidence),
  m_morphVertex(wsa.m_morphVertex),
  m_senseTag(wsa.m_senseTag)
  {}
      
  WordSenseAnnotation& operator=(const WordSenseAnnotation& wsa)
  { 
    m_mode = wsa.m_mode;
    m_mapping = wsa.m_mapping;
    m_confidence = wsa.m_confidence;
    m_morphVertex = wsa.m_morphVertex;
    m_wsu = wsa.m_wsu;
    m_senseTag = wsa.m_senseTag;
    return *this;
  };
  friend LIMA_WORDSENSEANALYSIS_EXPORT std::ostream& operator << (std::ostream& os, const WordSenseAnnotation& wsa);

  virtual ~WordSenseAnnotation() {}

  inline uint64_t senseId();
  inline uint64_t senseId() const;
  inline std::string senseTag();
  inline std::string senseTag() const;
  inline std::string mode();
  inline std::string mapping();
  inline const std::string mode() const;
  inline const std::string mapping() const;
  inline float confidence() const;
  inline LinguisticGraphVertex morphVertex();
  inline LinguisticGraphVertex morphVertex() const;
  inline AnnotationGraphVertex av();
  inline AnnotationGraphVertex av() const; 
  inline void senseId(uint64_t newSenseId);
  inline void confidence(const float newConfidence);
  inline void morphVertex(LinguisticGraphVertex v);
  inline void av(AnnotationGraphVertex av);
  inline void dump(std::ostream& os);

  /** general test functions */
  bool isDisambiguated() const;
  

  /** main functions of the global algorithm (called by WordSenseDisambiguator) */
/*
  int classify(
    LinguisticGraph* graph,
    SyntacticAnalysis::SyntacticData* sd,
    const Common::PropertyCode::PropertyAccessor* macroAccessor,
    const Common::PropertyCode::PropertyAccessor* microAccessor,
    const std::map<std::string,LinguisticCode>& tagLocalDef,
    const std::map<std::string, std::deque<std::string> >& relLocalDef,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    MediaId language) ;
*/

  bool disambiguate(const WordUnit& wu);
  bool disambiguate(KnnSearcher* searcher,
        const WordUnit& wu, 
        const SemanticContext& context,
        double thres, 
        char method);

  AnnotationGraphVertex writeAnnotation(  
    Common::AnnotationGraphs::AnnotationData* ad) const;

  virtual void outputXml(std::ostream& xmlStream,
    const LinguisticGraph& g) const;



private:

protected:  
  /* internal computation functions called by classifKnnMRD*/
  
  std::set<WordSenseUnit>::iterator classify(const WordUnit& wu,
               const SemanticContext& context,
               const std::map<int, std::map<std::string, double> >& cvListRep,
               const double thres,
               const char method) ;
               
  int computeConfidenceVector(const WordUnit& wu,
               const NNList& knns, 
               const SemanticContext::const_iterator itContext,
               std::map<int, std::map<std::string, double> >& cvListRep);
  
  
  /**
  * cv_cluster_rep:
  * @cv_list_rep:  la matrice 'cv_list_rep' (voir fonction précédente)
  * @nb_clusters:  nombre de clusters découverts pour le mot
  * @id_relation:  numero d'une relation
  * @id_cluster:   numero d'un cluster
  *
  * Return:        le vecteur de confiance de 'id_cluster' dans 'id_relation'
  */
  double cvCluster(const std::map<int, std::map<std::string, double> >& cvListRep, 
       const std::string& relation,
       int id_cluster);

  /**
  * sum_cv_cluster_rep:
  * @cv_list_rep: la matrice 'cv_list_rep' (voir fonction précédente)
  * @nb_clusters: nombre de clusters découverts pour le mot
  * @id_relation: numero d'une relation
  *
  * Return:       la somme des vecteurs de confiance de chacun des clusters dans 'id_relation'
  */
  double sumCvCluster(std::map<int, std::map<std::string, double> >& cvListRep,
          std::string relation);
  //(F)
  double sumLogCv(const std::map<int, std::map<std::string, double> >& cvListRep, 
      const std::string& relation);
  //(G)
  double sumLogCvList(const std::map<int, std::map<std::string, double> >& cvListRep);  
  /* end internal computation functions */
  
  
  void retrieveSense(std::set<WordSenseUnit>::iterator itSenses);
  float classifKnnMRD(KnnSearcher* searcher,
          const WordUnit& wu,
          const SemanticContext& context,
          double thres, 
          char method);
  
  WordSenseUnit m_wsu;
  Mode m_mode;
  Mapping m_mapping;
  float m_confidence;   
  LinguisticGraphVertex m_morphVertex;
  std::string m_senseTag;
  
//  AnnotationGraphVertex m_av;

};


inline uint64_t WordSenseAnnotation::senseId()
{
  return m_wsu.senseId();
}
inline uint64_t WordSenseAnnotation::senseId() const
{
  return m_wsu.senseId();
}
inline std::string WordSenseAnnotation::senseTag()
{
  /*if (m_wsu.senseTag() !="") 
    return m_wsu.senseTag();*/
  return m_senseTag;
}
inline std::string WordSenseAnnotation::senseTag() const
{
  return m_wsu.senseTag();
}
inline std::string WordSenseAnnotation::mode()
{
  switch (m_mode) {
    case B_MOST_FREQUENT :
      return "b_Most_Frequent";
    case B_ROMANSEVAL_MOST_FREQUENT :
      return "b_Romanseval_Most_Frequent";
    case B_JAWS_MOST_FREQUENT :
      return "b_Jaws_Most_Frequent";
    case S_WSI_MRD :
      return "s_Wsi_mrd";
    case S_WSI_DS :
      return "s_Wsi_Dempster_Schaffer";
    case S_UNKNOWN :
      return "Unknown";
    default :
      break;
  }
  return "";
}
inline const std::string WordSenseAnnotation::mode() const
{
  switch (m_mode) {
    case B_MOST_FREQUENT :
      return "b_Most_Frequent";
   case B_ROMANSEVAL_MOST_FREQUENT :
      return "b_Romanseval_Most_Frequent";
    case B_JAWS_MOST_FREQUENT :
      return "b_Jaws_Most_Frequent";
    case S_WSI_MRD :
      return "s_Wsi_mrd";
    case S_WSI_DS :
      return "s_Wsi_Dempster_Schaffer";
    case S_UNKNOWN :
      return "Unknown";
    default :
      break;
  }
  return "";  
}
inline std::string WordSenseAnnotation::mapping()
{  
  switch (m_mapping) {
    case M_ROMANSEVAL_SENSES :
      return "m_Romanseval_Senses";
    case M_JAWS_SENSES :
      return "m_Jaws_Senses";
    case M_UNKNOWN :
      return "Unknown";
    default :
      break;
  }
  return "";
}
inline const std::string WordSenseAnnotation::mapping() const
{
  switch (m_mapping) {
    case M_ROMANSEVAL_SENSES :
      return "m_Romanseval_Senses";
    case M_JAWS_SENSES :
      return "m_Jaws_Senses";
    case M_UNKNOWN :
      return "Unknown";
    default :
      break;
  }
  return "";
}
inline float WordSenseAnnotation::confidence() const
{
  return m_confidence;
}

inline LinguisticGraphVertex WordSenseAnnotation::morphVertex()
{
  return m_morphVertex;
}
inline LinguisticGraphVertex WordSenseAnnotation::morphVertex() const
{
  return m_morphVertex;
}
/*
inline void WordSenseAnnotation::senseId(uint64_t newSenseId)
{
  m_senseId = newSenseId;
}*/
inline void WordSenseAnnotation::confidence(const float newConfidence)
{
  m_confidence = newConfidence;
}
inline void WordSenseAnnotation::morphVertex(LinguisticGraphVertex v)
{
  m_morphVertex = v;
}
inline void WordSenseAnnotation::dump(std::ostream& os)
{
  os << "#" << m_wsu.senseId() << ";" << m_mode<< ";" << /*"V:" << m_morphVertex <<*/ "\n";
}




/** @brief Definition of a function suitable to be used as a dumper for WordSense
  * Annotations of an Annotation graph 
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump the point 
  * @param @b ga <I>GenericAnnotation&amp;</I> the coreferent Annotation to be 
  *        dumped. 
  * @return <I>int</I> If succesful, SUCCESS_ID is returned. An error is 
  *         displayed in the case where the given Annotation is not a Point 
  *         Annotation and UNKNOWN_ERROR is returned .
  */
class DumpWordSense : public Common::AnnotationGraphs::AnnotationData::Dumper
{
  public:
    virtual int dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const override;
};




} // closing namespace WordSenseDisambiguation
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_WORDSENSEDISAMBIGUATION_WORDSENSEANNOTATION_H
