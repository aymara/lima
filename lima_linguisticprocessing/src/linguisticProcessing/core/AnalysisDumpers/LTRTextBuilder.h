// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/** =========================================================================
    @file       LTRTextBuilder.h

    @version    $Id$
    @date       created       jun 28, 2005
    @date       last revised  dec 31, 2010

    @author     Olivier Ferret
    @brief      build a LTRText from the graph resulting from the linguistic
                analysis of a document

    Copyright (C) 2005-2010 by CEA LIST

    ========================================================================= */

#ifndef LIMA_LINGUISTICPROCESSING_LTRTEXTBUILDER_H
#define LIMA_LINGUISTICPROCESSING_LTRTEXTBUILDER_H

#include "AnalysisDumpersExport.h"
#include "StopList.h"

//#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/linearTextRepresentation/ltrToken.h"
#include "linguisticProcessing/common/linearTextRepresentation/ltrText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"

#include <list>
#include <set>
#include <deque>


namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {


/**
  * @class  LTRTextBuilder
  * @brief  build a LTR_Text with all the tokens from the analysis
  */

class LIMA_ANALYSISDUMPERS_EXPORT LTRTextBuilder {

// type definitions
// ----------------
private:
    typedef std::list<LinguisticGraphVertex> VERTICES_TO_EXPLORE_T;
    typedef std::set<LinguisticGraphVertex> EXPLORED_VERTICES_T;


// data
// ----
private:
    VERTICES_TO_EXPLORE_T m_verticesToExplore;
    EXPLORED_VERTICES_T m_exploredVertices;
    uint64_t m_currentOffset;
    MediaId m_language;
    std::shared_ptr<StopList> m_stopList;
    const Common::PropertyCode::PropertyAccessor* m_macroAccessor;
    const Common::PropertyCode::PropertyAccessor* m_microAccessor;


// methods
// -------
public:
    LTRTextBuilder(
    const MediaId& language,
        std::shared_ptr<StopList> stopList);
    /** @brief build a LTRText representation of the analyzed text
     * @param graph the linguistic graph containing the analyzed text
     * @param sb a pointer on the sentence boundaries segmentation data: if zero, sentence boundaries are ignored, all text is treated as a single segment
     * @param graphFirstVertex the first vertex of the text in the linguistic graph (needed when sb==0)
     * @param graphLastVertex the last vertex of the text in the linguistic graph (for last segment)
     * @param textRep the LTRText built 
     * @param offset the offset of the text in the document (to have a global correct position)
     */
    void buildLTRTextFrom(
        const LinguisticGraph& graph,
        Lima::LinguisticProcessing::SegmentationData* sb,
        const LinguisticGraphVertex& graphFirstVertex,
        const LinguisticGraphVertex& graphLastVertex,
        Lima::Common::BagOfWords::LTR_Text* textRep,
        uint64_t offset);

    /// **** @name  word selection
    //@{
    bool isWordToSelect(
        const Lima::LimaString& lemma,
        LinguisticCode macroCategory) const;
    bool isWordToSelect(
        const Lima::LimaString& lemma,
        LinguisticCode macroCategory,
        LinguisticCode microCategory) const;
    //}@

private:
    void addTokensToLTRTextFrom(
        const LinguisticGraph& graph,
        const LinguisticGraphVertex& firstVertex,
        const LinguisticGraphVertex& lastVertex,
        const LinguisticGraphVertex& graphLastVertex,
        Lima::Common::BagOfWords::LTR_Text* textRep,
        uint64_t offset,
        uint64_t* tokenCounter);
    void exploreVerticesFrom(
        const LinguisticGraphVertex& vertex,
        const LinguisticGraphVertex& graphLastVertex,
        const LinguisticGraph& graph);
    void updateLTR_TokenFromVertex(
        const LinguisticGraphVertex& vertex,
        const LinguisticGraph& graph,
        Lima::Common::BagOfWords::LTR_Token* tokenRep,
        uint64_t offset) const;
};


} // AnalysisDumpers
} // LinguisticProcessing
} // Lima



#endif  // LIMA_LINGUISTICPROCESSING_LTRTEXTBUILDER_H
