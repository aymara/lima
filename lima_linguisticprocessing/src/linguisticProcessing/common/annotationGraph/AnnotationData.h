// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @brief This file is the main header file for the data related to annotation
 *        graphs
 * @file        AnnotationData.h
 * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 *              Copyright (c) 2004-2014 by CEA LIST
 * @date        Created on Nov, 8 2004
 */

#ifndef LIMA_ANNOTATIONGRAPHS_ANNOTATIONDATA_H
#define LIMA_ANNOTATIONGRAPHS_ANNOTATIONDATA_H

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/misc/stringspool.h"

#include "AnnotationGraph.h"
#include "common/ProcessUnitFramework/AnalysisContent.h"

namespace Lima {
namespace Common {
namespace AnnotationGraphs {

class AnnotationDataPrivate;

/**
 * @brief Holds an annotation graph and gives an API to manipulate it
 * @author      Gael de Chalendar <Gael.de-Chalendar@cea.fr>
 */
class LIMA_ANNOTATIONGRAPH_EXPORT AnnotationData : public AnalysisData
{
public:

  class Dumper {
    public:
      virtual ~Dumper() {}
      virtual int dump(std::ostream&, GenericAnnotation&) const = 0;
  };

  /** @brief Default constructor */
  AnnotationData();

  /** @brief Default destructor */
  virtual ~AnnotationData();

  /** @brief Creates a new annotation vertex in the graph */
  AnnotationGraphVertex createAnnotationVertex();

  /** @brief Creates a new annotation edge in the graph */
  AnnotationGraphEdge createAnnotationEdge(AnnotationGraphVertex s, AnnotationGraphVertex t);

  /** @defgroup VertexFeatIdAnnot annotation of a vertex
   *
   * for annotation by feature name, the name and its id are created if needed
   @{ */
  void annotate(AnnotationGraphVertex v, const LimaString& annot, uint64_t value);
  void annotate(AnnotationGraphVertex v, const LimaString& annot, const LimaString& value);
  void annotate(AnnotationGraphVertex v, const LimaString& annot, const GenericAnnotation& value);
  /** @} */

  /** @defgroup BoundsEdgeFeatName Annot annotation of an edge designed by its two bounds vertices; if needed, the
    * edge is created
   @{ */
  void annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, const LimaString& annot, uint64_t value);
  void annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, const LimaString& annot, const LimaString& value);
  void annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, const LimaString& annot, const GenericAnnotation& value);
  /** @} */

  /** @defgroup EdgeFeatNameAnnot annotation of an edge
   @{ */
  void annotate(AnnotationGraphEdge e, const LimaString& annot, uint64_t value);
  void annotate(AnnotationGraphEdge e, const LimaString& annot, const LimaString& value);
  void annotate(AnnotationGraphEdge e, const LimaString& annot, const GenericAnnotation& value);
  /** @} */

  uint64_t intAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const;
  const LimaString& stringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const;
  const GenericAnnotation& annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const;
  GenericAnnotation& annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot);

  /** @defgroup FeatNameCAssocAccess access to int, string and generic association by feature name, const
    * versions
   @{ */
  uint64_t intAnnotation(AnnotationGraphVertex v, const LimaString& annot) const;
  const LimaString& stringAnnotation(AnnotationGraphVertex v, const LimaString& annot) const;
  const GenericAnnotation& annotation(AnnotationGraphVertex v, const LimaString& annot) const;
  GenericAnnotation& annotation(AnnotationGraphVertex v, const LimaString& annot);
  /** @} */

  /** @defgroup FeatNameNCAssocAccess access to int, string and generic association by feature name, non-const
    * versions
   @{ */
  uint64_t intAnnotation(AnnotationGraphEdge e, const LimaString& annot);
  const LimaString& stringAnnotation(AnnotationGraphEdge e, const LimaString& annot);
  const GenericAnnotation& annotation(AnnotationGraphEdge e, const LimaString& annot) const;
  GenericAnnotation& annotation(AnnotationGraphEdge e, const LimaString& annot);
  /** @} */

  /** @defgroup VertexFeatIdAnnot annotation of a vertex
   *
   * for annotation by feature id, the id have to already exist with a name
  @{ */
  void annotate(AnnotationGraphVertex v, uint64_t annot, uint64_t value);
  void annotate(AnnotationGraphVertex v, uint64_t annot, const LimaString& value);
  void annotate(AnnotationGraphVertex v, uint64_t annot, const GenericAnnotation& value);
  /** @} */

  /** @defgroup BoundsEdgeFeatIdAnnot annotation of an edge designed by its two bounds vertices; if needed, the
    * edge is created
   @{ */
  void annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, uint64_t annot, uint64_t value);
  void annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, uint64_t annot, const LimaString& value);
  void annotate(AnnotationGraphVertex vs, AnnotationGraphVertex vt, uint64_t annot, const GenericAnnotation& value);
  /** @} */

  /** @defgroup EdgeFeatIdAnnot annotation of an edge functions
    @{ */
  void annotate(AnnotationGraphEdge e, uint64_t annot, uint64_t value);
  void annotate(AnnotationGraphEdge e, uint64_t annot, const LimaString& value);
  void annotate(AnnotationGraphEdge e, uint64_t annot, const GenericAnnotation& value);
  /** @} */

  /** @defgroup FeatIdCAssocAccess access to int, string and generic association by feature id, const
    * versions
   @{ */
  uint64_t intAnnotation(AnnotationGraphVertex v, uint64_t annot) const;
  const LimaString& stringAnnotation(AnnotationGraphVertex v, uint64_t annot) const;
  const GenericAnnotation& annotation(AnnotationGraphVertex v, uint64_t annot) const;
  GenericAnnotation& annotation(AnnotationGraphVertex v, uint64_t annot);
  /** @} */

  /** @defgroup FeatIdCEBAssocAccess access to int, string and generic annotation
   * of an edge designed bi its bounds by feature id, const versions
  @{ */
  uint64_t intAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const;
  const LimaString& stringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const;
  const GenericAnnotation& annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const;
  GenericAnnotation& annotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot);
  /** @} */


  /** @defgroup FeatIdNCAssocAccess access to int, string and generic annotation
   * of an edge by feature id, non-const versions
  @{ */
  uint64_t intAnnotation(AnnotationGraphEdge e, uint64_t annot);
  LimaString& stringAnnotation(AnnotationGraphEdge e, uint64_t annot);
  GenericAnnotation& annotation(AnnotationGraphEdge e, uint64_t annot);
  /** @} */

  /** @defgroup FeatIdCEAssocAccess access to int, string and generic annotation
   * of an edge by feature id, const versions
  @{ */
  uint64_t intAnnotation(AnnotationGraphEdge e, uint64_t annot) const;
  const LimaString& stringAnnotation(AnnotationGraphEdge e, uint64_t annot) const;
  const GenericAnnotation& annotation(AnnotationGraphEdge e, uint64_t annot) const;
  /** @} */

  /** @defgroup FeatNameCAssocExistence test if int, string or generic
   * association exists by feature name, const versions
  @{ */
  bool hasIntAnnotation(AnnotationGraphVertex v, const LimaString& annot) const;
  bool hasStringAnnotation(AnnotationGraphVertex v, const LimaString& annot) const;
  bool hasAnnotation(AnnotationGraphVertex v, const LimaString& annot) const;
  bool hasIntAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const;
  bool hasStringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const;
  bool hasAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, const LimaString& annot) const;
  bool hasIntAnnotation(AnnotationGraphEdge e, const LimaString& annot) const;
  bool hasStringAnnotation(AnnotationGraphEdge e, const LimaString& annot) const;
  bool hasAnnotation(AnnotationGraphEdge e, const LimaString& annot) const;
  /** @} */


  /** @defgroup FeatIdNCAssocExistence test if int, string or generic
   * association exists by feature id, const versions
  @{ */
  bool hasIntAnnotation(AnnotationGraphVertex v, uint64_t annot) const;
  bool hasStringAnnotation(AnnotationGraphVertex v, uint64_t annot) const;
  bool hasAnnotation(AnnotationGraphVertex v, uint64_t annot) const;
  bool hasIntAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const;
  bool hasStringAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const;
  bool hasAnnotation(AnnotationGraphVertex v1, AnnotationGraphVertex v2, uint64_t annot) const;
  bool hasIntAnnotation(AnnotationGraphEdge e, uint64_t annot) const;
  bool hasStringAnnotation(AnnotationGraphEdge e, uint64_t annot) const;
  bool hasAnnotation(AnnotationGraphEdge e, uint64_t annot) const;
  /** @} */


  /** @defgroup AGAccess access to the underlying annotation graph
   @{ */
  AnnotationGraph& getGraph();
  const AnnotationGraph& getGraph() const;
  /** @} */

  /** @brief Gives the name of an annotation id */
  const LimaString& annotationName(uint64_t annotationId) const;

  /** @brief Gives the id of an annotation name */
  uint64_t annotationId(const LimaString& s);

  /** @defgroup AccessDump Accessors for dumpFunctions
   *
   * These functions gives various manners to access to the annotations dump functions
   * WARNING : AnnotationData is responsible for deleting Dumper objects
   @{ */
  void dumpFunction(const std::string& annot, const Dumper* f);
  const Dumper* dumpFunction(const std::string& annot);
  const Dumper* dumpFunction(uint64_t annot);
  /** @} */

  /** @brief Adds a symetric matching between two vertices of two graphs
   * identified by the two string parameters
   */
  void addMatching(const std::string& first, AnnotationGraphVertex firstVx,
                   const std::string& second, AnnotationGraphVertex secondVx);

  /** @brief Gets the set of vertices matched in the second graph by the given
   * vertex of the first graph
   */
  std::set< AnnotationGraphVertex > matches(const std::string& first, AnnotationGraphVertex firstVx,
                               const std::string& second) const;

  /** @brief Tests if the two given vertices are matching in the two given graphs
   */
  bool isMatching(const std::string& first, AnnotationGraphVertex firstVx,
                  const std::string& second, AnnotationGraphVertex secondVx) const;

  /** @brief Adds a symetric matching between two vertices of two graphs
    * identified by the two string parameters
   */
   void addMatching(const StringsPoolIndex& first, AnnotationGraphVertex firstVx,
                   const StringsPoolIndex& second, AnnotationGraphVertex secondVx);

  /** @brief Gets the set of vertices matched in the second graph by the given
    * vertex of the first graph
   */
  std::set< AnnotationGraphVertex > matches(const StringsPoolIndex& first, AnnotationGraphVertex firstVx,
                               const StringsPoolIndex& second) const;

  /** @brief Tests if the two given vertices are matching in the two given graphs
   */
  bool isMatching(const StringsPoolIndex& first, AnnotationGraphVertex firstVx,
                  const StringsPoolIndex& second, AnnotationGraphVertex secondVx) const;

  /** @brief Adds a symetric matching between two vertices of two graphs
    * identified by the two string parameters
    */
    void addMatching(const StringsPoolIndex& direction, AnnotationGraphVertex firstVx,
                   AnnotationGraphVertex secondVx);

  /** @brief Gets the set of vertices matched in the second graph by the given
    * vertex of the first graph
    */
  std::set< AnnotationGraphVertex > matches(const StringsPoolIndex& direction,
                               AnnotationGraphVertex firstVx) const;

  /** @brief Tests if the two given vertices are matching in the two given graphs
   */
  bool isMatching(const StringsPoolIndex& direction, AnnotationGraphVertex firstVx,
                  AnnotationGraphVertex secondVx) const;

  /** @briefs copies all the annotations of the source annotation vertex onto
   * the target annotation vertex, except those listed
  */
  void cloneAnnotations(AnnotationGraphVertex src, AnnotationGraphVertex tgt,
                        std::set< std::string >& excepted);

  /** @brief Access to the underlying raw matching structure */
  const std::map<StringsPoolIndex, std::multimap<AnnotationGraphVertex, AnnotationGraphVertex> >& matchings() const;

private:
  AnnotationData(const AnnotationData&);
  AnnotationData& operator=(const AnnotationData&);


  AnnotationDataPrivate* m_d;
};

} // closing namespace AnnotationGraphs
} // closing namespace Common
} // closing namespace Lima


#endif // LIMA_ANNOTATIONGRAPH_ANNOTATIONDATA_H
