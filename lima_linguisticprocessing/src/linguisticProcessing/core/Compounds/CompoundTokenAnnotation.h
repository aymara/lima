/***************************************************************************
 *   Copyright (C) 2005 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/

#ifndef LIMA_LINGUISTICPROCESSING_COMPOUNDS_COMPOUNDTOKENANNOTATION_H
#define LIMA_LINGUISTICPROCESSING_COMPOUNDS_COMPOUNDTOKENANNOTATION_H

#include "CompoundsExport.h"
#include "linguisticProcessing/common/annotationGraph/GenericAnnotation.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/recognizerMatch.h"


#include <vector>

#ifdef WIN32
#undef min
#undef max
#endif
namespace Lima
{

namespace LinguisticProcessing
{

namespace Compounds
{

/** This enumeration lists the types of annotations used to specify the role of
 * an annotation in a compound. It should later be replaced by semantic types as
 * used by the future semantic analysis framework.
*/
//enum SemanticTypes {
//  Determiner, /**< Used for determiners as articles, pronouns, etc. */
//  SemanticModifier, /**< Used for modifiers as adverbs */
//  Complementer /**< Used for complementers like noun complements or adjectives */
//};
  
/**
 * @brief A representation of a concept modifier (adverb, pronoun, etc)
 * A modifier has no structure and thus is represented by a simple annotation.
 * It is a node annotation for adverbs or pronouns and a link annotation for 
 * adjectives
*/
class  ConceptModifier 
{
public:
  
  ConceptModifier() {}
  ConceptModifier(const StringsPoolIndex& realization,
                  const Common::MediaticData::ConceptType& type 
                              = std::numeric_limits< Common::MediaticData::ConceptType >::max(),
                  const std::vector< uint64_t >& references
                              = std::vector< uint64_t>()) : 
      m_realization(realization), 
      m_conceptType(type),
      m_referencedAnnotations(references) {}
  
  virtual ~ConceptModifier() {}
  
  
  /** @brief The functions that dumps a ConceptModifier on an output stream */
  void dump(std::ostream& os,const FsaStringsPool& sp) const {}

  inline StringsPoolIndex getRealization() const;
//   inline void setRealization(StringsPoolIndex realization);
  inline Common::MediaticData::ConceptType getConceptType() const;
  inline void setConceptType(Common::MediaticData::ConceptType type);
  inline const std::vector< uint64_t >& getReferencedAnnotations() const;
  std::vector< uint64_t >& getReferencedAnnotations();
  inline void setReferencedAnnotations(const std::vector< uint64_t >& references);
  inline int32_t getIntensity() const;
  inline void setIntensity(int32_t intensity);
  
private:
  StringsPoolIndex m_realization; /**< The lemma of the annotated token(s) */
  
  Common::MediaticData::ConceptType m_conceptType; /**< extracted from any 
                           * semantic resource. Can represent a determiner, a 
                           * modifier, a complementer, etc. If extracted from a 
                           * type hierarchy, we can have TOP \< DETERMINER \< 
                           * TIME_DETERMINER, etc. */ 
  
  std::vector< uint64_t> m_referencedAnnotations; /**< The ids of annotation
                                                    * vertices linking to the
                                                    * morphologic vertices 
                                                    * realizing this annotation
                                                    * */
  
  int32_t m_intensity; /**< Used to take into account intensity modifications 
                        * like when an adverb is modified by another adverb.
                        * Currently this is just a positive or negative numeric
                        * factor but could later be used as a symbolic data. */
  
  /// @todo It will probably be necessary to store an attribute-value map to
  /// store more semantic or semantic-to-syntactic information
};


class  CompoundTokenAnnotation : public std::vector< ConceptModifier >
{
public: 
  CompoundTokenAnnotation() {}
  virtual ~CompoundTokenAnnotation() {}
  
  /** @brief The functions that dumps a CompoundTokenAnnotation on an output stream */
  void dump(std::ostream& os,const FsaStringsPool& sp);
};

/** @brief Definition of a function suitable to be used as a dumper for a 
  * compound token annotation of an annotation graph 
  * @param @b os <I>std::ostream&amp;</I> the stream on which to dump the annotation 
  * @param @b ga <I>GenericAnnotation&amp;</I> the compound token annotation to 
  *        be dumped. 
  * @return <I>int</I> If succesful, SUCCESS_ID is returned. An error is 
  *         displayed in the case where the given annotation is not a compound 
  *         token annotation and UNKNOWN_ERROR is returned .
  */
class LIMA_COMPOUNDS_EXPORT DumpCompoundTokenAnnotation : public Common::AnnotationGraphs::AnnotationData::Dumper {
public:

  DumpCompoundTokenAnnotation(const FsaStringsPool& sp) : m_sp(sp) {};
  
  virtual int dump(
      std::ostream& os, 
      Common::AnnotationGraphs::GenericAnnotation& ga) const {return SUCCESS_ID;}
      
private:
  const FsaStringsPool& m_sp;
};


inline StringsPoolIndex ConceptModifier::getRealization() const
{
  return m_realization;
}

// inline void ConceptModifier::setRealization(StringsPoolIndex realization)
// {
//   m_realization = realization;
// }

inline Common::MediaticData::ConceptType ConceptModifier::getConceptType() const
{
  return m_conceptType;
}

inline void ConceptModifier::setConceptType(Common::MediaticData::ConceptType type) 
{
  m_conceptType = type;
}

inline const std::vector< uint64_t >& ConceptModifier::getReferencedAnnotations() const
{
  return m_referencedAnnotations;
}

inline std::vector< uint64_t >& ConceptModifier::getReferencedAnnotations()
{
  return m_referencedAnnotations;
}

inline void ConceptModifier::setReferencedAnnotations(const std::vector< uint64_t >& references)
{
  m_referencedAnnotations = references;
}

inline int32_t ConceptModifier::getIntensity() const
{
  return m_intensity;
}

inline void ConceptModifier::setIntensity(int32_t intensity)
{
  m_intensity = intensity;
}

} // Compounds
} // LinguisticProcessing
} // Lima

#endif

