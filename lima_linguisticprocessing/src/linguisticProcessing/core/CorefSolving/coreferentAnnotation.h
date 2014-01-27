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
  * @file        coreferentAnnotation.h
  * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

  *              Copyright (c) 2004 by CEA
  * @date        Created on Dec, 3 2004
  * @version     $Id$
  *
  */

#ifndef LIMA_COREFSOLVING_COREFERENTANNOTATION_H
#define LIMA_COREFSOLVING_COREFERENTANNOTATION_H

#include "CorefSolvingExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/SyntacticAnalysis/DependencyGraph.h"

#include <string>



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
namespace Coreferences
{ 

class LIMA_COREFSOLVING_EXPORT CoreferentAnnotation
{


public:

typedef std::set<CoreferentAnnotation*> Vertices;
typedef std::map<CoreferentAnnotation*,std::set<CoreferentAnnotation*> > VerticesRelation;
typedef std::map< CoreferentAnnotation*,std::map <CoreferentAnnotation*,float> > WeightedVerticesRelation;

  CoreferentAnnotation() {}

  CoreferentAnnotation(uint64_t id, const std::string& categ, const float salience, LinguisticGraphVertex v) : 
       m_id(id), m_categ(categ), m_salience(salience),  /*m_av(v),*/ m_bindingSalience(0), m_morphVertex(v), m_newerRef(this) {}

  CoreferentAnnotation(const CoreferentAnnotation& ca) : 
      m_id(ca.m_id), m_categ(ca.m_categ), m_salience(ca.m_salience), /*m_av(ca.m_av), */ m_bindingSalience(ca.m_bindingSalience), m_morphVertex(ca.m_morphVertex), m_newerRef(ca.m_newerRef) {}

  CoreferentAnnotation(uint64_t id,
           LinguisticGraphVertex v) : 
      m_id(id), m_categ("not initialized"), m_salience(-1), m_bindingSalience(0), m_morphVertex(v), m_newerRef(this) {}


  CoreferentAnnotation& operator=(const CoreferentAnnotation& ca)
  { 
    m_id = ca.m_id;
    m_categ = ca.m_categ;
    m_salience = ca.m_salience;
//    m_av = ca.m_av;
    m_bindingSalience = ca.m_bindingSalience;
    m_morphVertex = ca.m_morphVertex;
    m_newerRef = ca.m_newerRef;
    return *this;
  };


  virtual ~CoreferentAnnotation() {}

  inline uint64_t id();
  inline uint64_t id() const;
  inline std::string& categ();
  inline const std::string& categ() const;
  inline float salience() const;
  inline float bindingSalience() const;
  inline LinguisticGraphVertex morphVertex();
  inline LinguisticGraphVertex morphVertex() const;
  inline AnnotationGraphVertex av();
  inline AnnotationGraphVertex av() const;
  inline CoreferentAnnotation* newerRef();
  inline CoreferentAnnotation* newerRef() const; 
  inline void id(uint64_t newId);
  inline void categ(const std::string newType);
  inline void salience(const float newSalience);
  inline void bindingSalience(const float newBindingSalience);
  inline void morphVertex(LinguisticGraphVertex v);
  inline void av(AnnotationGraphVertex av);
  inline void newerRef(CoreferentAnnotation* newerRef);
  inline void dump(std::ostream& os);
  inline bool hasNewerRef(/*std::deque<Vertices>* npCandidates*/);

  /** general test functions */

  bool isIncludedInNounPhrase(
    const LinguisticGraph* g,
    MediaId language,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    AnalysisContent& ac,
    const std::set< LinguisticCode >& inNpCategs,
    const Common::PropertyCode::PropertyAccessor* microAccessor) const;

  LinguisticGraphVertex npHeadVertex(
    const SyntacticAnalysis::SyntacticData* sd,
    MediaId language,
    std::set<DependencyGraphVertex>* alreadyProcessed ) const;

  bool isTaggedAsOneOfThese(
    const LinguisticGraph* g,
    const std::set< LinguisticCode >& categs,
    const Common::PropertyCode::PropertyAccessor* microAccessor) const;


  /** test functions for the initial classification */

  bool isDefinite(const SyntacticAnalysis::SyntacticData* sd,
      const std::string& definiteRel,
      const std::set< LinguisticCode >& definiteCategs,
      const Common::PropertyCode::PropertyAccessor* microAccessor,
                  MediaId language) const ;
  bool isPleonastic(const SyntacticAnalysis::SyntacticData* sd,
        const std::string& m_pleonRel,
        MediaId language) const;

  bool isPronoun(const LinguisticGraph* g,
        const Common::PropertyCode::PropertyAccessor* macroAccessor,
        const LinguisticCode& L_PRON) const ;
/*
  bool isVerb(const LinguisticGraph* g,
        const Common::PropertyCode::PropertyAccessor* macroAccessor,
        const LinguisticCode& L_V) const ;*/

  bool isConjCoord(
        const Common::PropertyCode::PropertyAccessor* microAccessor,
        const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
        const LinguisticCode& conjCoord) const;

  std::string referentType(
                           const SyntacticAnalysis::SyntacticData* sd,
                           const LinguisticGraph* g,
                           const Common::PropertyCode::PropertyAccessor* macroAccessor,
                           const Common::PropertyCode::PropertyAccessor* microAccessor,
                           const std::map<std::string,LinguisticCode>& tagLocalDef,
                           const std::map<std::string, std::deque<std::string> >& relLocalDef,
                           const std::set< LinguisticCode >& definiteCategs,
                           const std::set< LinguisticCode >& reflexiveReciprocalCategs,
                           const std::set< LinguisticCode >& undefPronounsCategs,
                           const std::set< LinguisticCode >& possPronounsCategs,
                           const Common::PropertyCode::PropertyAccessor* personAccessor,
                           const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
                           MediaId language) const;
 
bool isN3PPronoun(
  const Common::PropertyCode::PropertyAccessor* personAccessor,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph) const; 

  /** test functions for the salience weighting */

  bool sentenceIsRecent() const;
  bool beginWithColon(
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    LinguisticGraphVertex& beginSentence) const;
  bool endWithColon(
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    LinguisticGraphVertex& endSentence) const;
  bool isInAppos(
    const SyntacticAnalysis::SyntacticData* sd,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    AnalysisContent& ac,
    MediaId language,
    std::set<DependencyGraphVertex>* alreadyProcessed) const;
  bool isInQuantA(
    const SyntacticAnalysis::SyntacticData* sd,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph) const;
  bool isInSubordinate(
    const SyntacticAnalysis::SyntacticData* sd,
    const Common::PropertyCode::PropertyAccessor* macroAccessor,
    const std::map<std::string,LinguisticCode>& tagLocalDef,
    MediaId language,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    AnalysisContent& ac,
    std::set<LinguisticGraphVertex>* alreadyProcessed) const;
  bool isFunctionMasterOf(
    const SyntacticAnalysis::SyntacticData* sd,
                          const std::deque<std::string>& macroDependencyRelation,
                          MediaId language) const;


  /** test functions for the syntactic filter and the reflexive binding algorithm */

  
  bool isVerb(
    const Common::PropertyCode::PropertyAccessor* macroAccessor,
    const std::map<std::string,LinguisticCode>& tagLocalDef,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph) const;

  bool isGovernedByMasculineCoordinate(
    const SyntacticAnalysis::SyntacticData* sd,
    const Common::PropertyCode::PropertyAccessor* genderAccessor,
    MediaId language,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    AnalysisContent& ac) const ;

  // P and N have incompatible agreement features. (gender, number)
  bool isAgreementCompatibleWith(
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* genderAccessor,
  const Common::PropertyCode::PropertyAccessor* personAccessor,
  const Common::PropertyCode::PropertyAccessor* numberAccessor,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const ;

  // P is in the Argument Domain of N <=> P and N are both argument of the same head
  bool isInTheArgumentDomainOf(
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const LinguisticCode& conjCoord,
  std::set<LinguisticGraphVertex>* alreadyProcessed) const ;

  // P is in the Argument Domain of N <=> P and N are both argument of the same head
  bool isInTheArgumentDomainOf2(
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const LinguisticCode& conjCoord,
  std::set<LinguisticGraphVertex>* alreadyProcessed) const ;

  // P is in the Adjunct Domain of N <=> N is an argument of a head H, P is the object of a preposition PREP, PREP is an adjunct of H
  bool isInTheAdjunctDomainOf(
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  const LinguisticGraph* graph,
    //const Common::PropertyCode::PropertyManager& microManager,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string, std::deque<std::string> >& relLocalDef,
  MediaId language) const;

// P is the object of a preposition PREP, 
// and PREP is an adjunct of Q
bool isInThePrepAdjunctNP(
  const DependencyGraphVertex& qv,
  const SyntacticAnalysis::SyntacticData* sd,
  const LinguisticGraph* graph,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string, std::deque<std::string> >& relLocalDef,
                          MediaId language) const;

  // P is an argument of a head H, N is not a pronoun,
  // and N is contained in H.
  bool sf4(  
  const CoreferentAnnotation& ca,  
  const SyntacticAnalysis::SyntacticData* sd,
  //const LinguisticGraph* g,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const LinguisticCode& L_PRON,  
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const;

  // P is in the NP domain of N <=> N is a determiner of a noun Q, and:
  //    (i) P is an argument of Q.
  // or (ii) P is an object of a preposition PREP and PREP is an adjunct of Q.
  bool isInTheNpDomainOf(  
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
        const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const;

  // P is contained in a phrase Q <=> 
  //    (i) P is either an argument or an adjunct of Q (ie. P is immediately contained in Q.
  // or (ii) P is immediately contained in some phrase R, and R is contained in Q.
  bool isContainedIn(
  const DependencyGraphVertex& dv,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const;

  bool isDeterminer(
  DependencyGraphVertex* qv,
  const SyntacticAnalysis::SyntacticData* sd,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const;

  // P is a determiner of a noun Q, 
  // and N is contained in Q.
  bool sf6(
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  MediaId language,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac) const;

  // N is an argument of a verb V,
  // there is an NP Q in the argument domain of N such that Q has no noun determiner,
  // and
  //    (i) A is an argument of Q.
  // or (ii) A is an argument of a preposition PREP and PREP is an adjunct of Q.
  bool aba4(
  const CoreferentAnnotation& ca,
  //Common::AnnotationGraphs::AnnotationData* ad,
  const SyntacticAnalysis::SyntacticData* sd,
  const std::map<std::string,LinguisticCode >& tagLocalDef,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  MediaId language,
  const std::set< LinguisticCode >& inNpCategs,
  LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const LinguisticCode& conjCoord,
  std::deque<Vertices>* npCandidates) const;

// [higher slot] subj > agent > obj > (iobj|pobj) [lower slot]
int getSlotValue(
  const SyntacticAnalysis::SyntacticData* sd,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  const std::map<std::string, int>& slotValues,
                 MediaId language) const;

  // A is a determiner of a noun Q, 
  // and
  //    (i) Q is in the argument domain of N,
  //        and N fills a higher argument slot than Q.
  // or (ii) Q is in the adjunct domain of N.
  bool aba5(
  const CoreferentAnnotation& ca,
  const SyntacticAnalysis::SyntacticData* sd,
  const std::map<std::string,LinguisticCode>& tagLocalDef,
  const std::map<std::string,std::deque<std::string> >& relLocalDef,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const Common::PropertyCode::PropertyAccessor* microAccessor,
  MediaId language,
  LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  AnalysisContent& ac,
  const std::map<std::string, int>& slotValues,
  const LinguisticCode& conjCoord) const;
  

  /** main functions of the global algorithm (called by Corefsolver) */

  int classify(
    LinguisticGraph* graph,
    SyntacticAnalysis::SyntacticData* sd,
    const Common::PropertyCode::PropertyAccessor* macroAccessor,
    const Common::PropertyCode::PropertyAccessor* microAccessor,
    const std::map<std::string,LinguisticCode>& tagLocalDef,
    const std::map<std::string, std::deque<std::string> >& relLocalDef,
    const std::set< LinguisticCode >& definiteCategs,
    const std::set< LinguisticCode >& reflexiveReciprocalCategs,
    const std::set< LinguisticCode >& undefPronouns,
    const std::set< LinguisticCode >& possPronouns,
    const bool& resolveDefinites,
    const bool& resolveN3PPronouns,
    const Common::PropertyCode::PropertyAccessor* personAccessor,
    const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
    MediaId language) ;


  float salienceWeighting(
  const std::map<std::string,float>& weights,
  const SyntacticAnalysis::SyntacticData* sd,
  const Common::PropertyCode::PropertyAccessor* macroAccessor,
  const LinguisticAnalysisStructure::AnalysisGraph* anagraph,
  MediaId language,
  const std::map< std::string,LinguisticCode>& tagLocalDef,
  const std::map< std::string,std::deque<std::string> >& relLocalDef,
  AnalysisContent& ac,
  LinguisticGraphVertex& beginSentence,
  LinguisticGraphVertex& endSentence) const;

  AnnotationGraphVertex writeAnnotation(
  Common::AnnotationGraphs::AnnotationData* ad,
  CoreferentAnnotation& antecedent) const;

    virtual void outputXml(std::ostream& xmlStream,
    const LinguisticGraph& g,
    const Common::AnnotationGraphs::AnnotationData* ad) const;


private:

protected:
  uint64_t m_id;
  std::string m_categ;
  float m_salience; 
  float m_bindingSalience;
  LinguisticGraphVertex m_morphVertex;
//  AnnotationGraphVertex m_av;

  CoreferentAnnotation* m_newerRef;
};

inline uint64_t CoreferentAnnotation::id()
{
  return m_id;
}
inline uint64_t CoreferentAnnotation::id() const
{
  return m_id;
}
inline std::string& CoreferentAnnotation::categ()
{
  return m_categ;
}
inline const std::string& CoreferentAnnotation::categ() const
{
  return m_categ;
}
inline float CoreferentAnnotation::salience() const
{
  return m_salience;
}
inline float CoreferentAnnotation::bindingSalience() const
{
  return m_bindingSalience;
}
inline LinguisticGraphVertex CoreferentAnnotation::morphVertex()
{
  return m_morphVertex;
}
inline LinguisticGraphVertex CoreferentAnnotation::morphVertex() const
{
  return m_morphVertex;
}
// inline AnnotationGraphVertex CoreferentAnnotation::av()
// {
//   return m_av;
// }
// inline AnnotationGraphVertex CoreferentAnnotation::av() const
// {
//   return m_av;
// }
inline CoreferentAnnotation* CoreferentAnnotation::newerRef()
{
  return m_newerRef;
}
inline CoreferentAnnotation* CoreferentAnnotation::newerRef() const
{
  return m_newerRef;
}
inline void CoreferentAnnotation::id(uint64_t newId)
{
  m_id = newId;
}
inline void CoreferentAnnotation::categ(const std::string newType)
{
  m_categ = newType;
}
inline void CoreferentAnnotation::salience(const float newSalience)
{
  m_salience = newSalience;
}
inline void CoreferentAnnotation::bindingSalience(const float newBindingSalience)
{
  m_bindingSalience = newBindingSalience;
}
inline void CoreferentAnnotation::morphVertex(LinguisticGraphVertex v)
{
  m_morphVertex = v;
}
// inline void CoreferentAnnotation::av(AnnotationGraphVertex av)
// {
//   m_av = av;
// }
inline void CoreferentAnnotation::newerRef(CoreferentAnnotation* newerRef)
{
  m_newerRef = newerRef;
}
inline void CoreferentAnnotation::dump(std::ostream& os)
{
  os << "#" << m_id << ";" << m_categ<< ";" << /*"V:" << m_morphVertex <<*/ "\n";
}
inline bool CoreferentAnnotation::hasNewerRef()
{
return (newerRef()!=this);
}




/** @brief Definition of a function suitable to be used as a dumper for Coreferent
  * Annotations of an Annotation graph 
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump the point 
  * @param @b ga <I>GenericAnnotation&amp;</I> the coreferent Annotation to be 
  *        dumped. 
  * @return <I>int</I> If succesful, SUCCESS_ID is returned. An error is 
  *         displayed in the case where the given Annotation is not a Point 
  *         Annotation and UNKNOWN_ERROR is returned .
  */
class DumpCoreferent : public Common::AnnotationGraphs::AnnotationData::Dumper
{
  public:
    virtual int dump(std::ostream& os, Common::AnnotationGraphs::GenericAnnotation& ga) const;
};


} // closing namespace Coreferences
} // closing namespace LinguisticProcessing
} // closing namespace Lima

#endif // LIMA_COREFSOLVING_COREFSOLVER_H
