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

/** =========================================================================
    @file       LTRTextBuilder.cpp

    @version    $Id$
    @date       created       jun 28, 2005
    @date       last revised  jan 4, 2011

    @author     Olivier Ferret
    @brief      build a LTRText from the graph resulting from the linguistic
                analysis of a document

    Copyright (C) 2005-2011 by CEA LIST

    ========================================================================= */

#include "LTRTextBuilder.h"
#include "linearTextRepresentationDumper.h"

#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/BagOfWords/bowToken.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/readwritetools.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"


using namespace Lima::Common::Misc;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Common::MediaticData;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace std;
using namespace boost;

#ifdef WIN32
#undef min
#undef max
#endif

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {


LTRTextBuilder::LTRTextBuilder(
    const MediaId& language,
    StopList* stopList) : m_language(language), m_stopList(stopList) {

    m_macroAccessor =
        &static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MACRO");
    m_microAccessor =
        &static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager().getPropertyAccessor("MICRO");
}


/**
  * @class  LTRTextBuilder
  * @brief  build a LTRText with all the tokens from the analysis
  */

void LTRTextBuilder::buildLTRTextFrom(
    const LinguisticGraph& graph,
    SegmentationData* sb,
    const LinguisticGraphVertex& graphLastVertex,
    LTR_Text* textRep,
    uint64_t offset) {

    // ??OME2 SegmentationData::iterator sbIt = sb->begin();    
    std::vector<Segment>::iterator sbIt = (sb->getSegments()).begin();    
    uint64_t tokenCounter = 0;
    // ??OME2 while (sbIt != sb->end()) {        
    while (sbIt != (sb->getSegments()).end()) {        
      LinguisticGraphVertex sentenceBegin = sbIt->getFirstVertex();    
      LinguisticGraphVertex sentenceEnd = sbIt->getLastVertex();
        this->addTokensToLTRTextFrom(
            graph,
            sentenceBegin,
            sentenceEnd,
            graphLastVertex,
            textRep,
            offset,
            &tokenCounter);
        textRep->addSentenceBound(tokenCounter);
        sbIt ++;
    }
}

void LTRTextBuilder::addTokensToLTRTextFrom(
    const LinguisticGraph& graph,
    const LinguisticGraphVertex& firstVertex,
    const LinguisticGraphVertex& lastVertex,
    const LinguisticGraphVertex& graphLastVertex,
    LTR_Text* textRep,
    uint64_t offset,
    uint64_t* tokenCounter) {

    DUMPERLOGINIT;
    m_verticesToExplore.clear();
    m_exploredVertices.clear();
    this->exploreVerticesFrom(firstVertex, graphLastVertex, graph);
    if (m_verticesToExplore.size() != 0) {
        // find the beginning position of the first word
        LinguisticGraphVertex firstTokenVertex = m_verticesToExplore.front();
        Token* token = get(vertex_token, graph, firstTokenVertex);
        m_currentOffset = token->position() + offset;
        LTR_Token* currentLtrTok = new LTR_Token();
        textRep->addToken(currentLtrTok);
        (*tokenCounter) ++;
        //    LTR_Token* sentenceBegin = currentLtrTok;
        bool endVertexFlag = false;
        while (! endVertexFlag && ! m_verticesToExplore.empty()) {
            // find the closest vertex to the last visited vertex
            LinguisticGraphVertex closestVertex;
            uint64_t smallestPosDiff = std::numeric_limits<uint64_t>::max();
            for (VERTICES_TO_EXPLORE_T::const_iterator itVert = m_verticesToExplore.begin();
                 itVert != m_verticesToExplore.end(); itVert ++) {
                token = get(vertex_token, graph, *itVert);
                uint64_t posDiff = token->position() + offset - m_currentOffset;
                if (posDiff < smallestPosDiff) {
                    smallestPosDiff = posDiff;
                    closestVertex = *itVert;
                }
            }
            // create a new LTR_Token for a new position
            if (smallestPosDiff > 0) {
                currentLtrTok = new LTR_Token();
                m_currentOffset = token->position() + offset;
                textRep->addToken(currentLtrTok);
                (*tokenCounter) ++;
            }
            // update the current LTR_Token with the current vertex
            this->updateLTR_TokenFromVertex(closestVertex, graph,
                                            currentLtrTok, offset);
            // remove the current vertex from the list of vertices to explore
            // and add it to list of explored vertices
            m_verticesToExplore.remove(closestVertex);
            m_exploredVertices.insert(closestVertex);
            // add the vertices that can be reached from the current vertex to the
            // list of vertices to explore
            this->exploreVerticesFrom(closestVertex, graphLastVertex, graph);
            endVertexFlag = (closestVertex == lastVertex);
        }
        if (! m_verticesToExplore.empty()) {
            LWARN << "all vertices not explored between " << firstVertex << " and "
                  << lastVertex;
        }
    }
}

void LTRTextBuilder::exploreVerticesFrom(
    const LinguisticGraphVertex& vertex,
    const LinguisticGraphVertex& graphLastVertex,
    const LinguisticGraph& graph) {

    LinguisticGraphOutEdgeIt it, itEnd;
    boost::tie(it, itEnd) = out_edges(vertex, graph);
    while (it != itEnd) {
        LinguisticGraphVertex outVertex = target(*it, graph);
        if ((outVertex != graphLastVertex) &&
            (m_exploredVertices.find(outVertex) == m_exploredVertices.end())) {
            m_verticesToExplore.push_back(outVertex);
        }
        it ++;
    }
}

void LTRTextBuilder::updateLTR_TokenFromVertex(
    const LinguisticGraphVertex& vertex,
    const LinguisticGraph& graph,
    LTR_Token* tokenRep,
    uint64_t offset) const {

    // get data from the result of the linguistic analysis
    Token* fullToken = get(vertex_token, graph, vertex);
    MorphoSyntacticData* data = get(vertex_data, graph, vertex);
    const FsaStringsPool& sp = (Common::MediaticData::MediaticData::single().stringsPool(m_language));

    sort(data->begin(),data->end(),ltNormProperty(m_macroAccessor));

    StringsPoolIndex norm(0),lastNorm(0);
    LinguisticCode macro(0),lastMacro(0);
    for (MorphoSyntacticData::const_iterator elemItr=data->begin();
         elemItr!=data->end(); elemItr++) {
        norm = elemItr->normalizedForm;
        macro = m_macroAccessor->readValue(elemItr->properties);
        if (norm == lastNorm && macro == lastMacro) {
            continue;
        }
        else {
            lastNorm=norm;
            lastMacro=macro;
            LimaString normStr= sp[norm];
            // test if the same word was not already met at this position
            bool selectionFlag = true;
            LTR_Token::const_iterator itTok = tokenRep->begin();
            while (selectionFlag && (itTok != tokenRep->end())) {
                selectionFlag = (itTok->first->getLemma() != normStr) ||
                                (itTok->first->getCategory() != macro);
                itTok ++;
            }
            if (selectionFlag) {
                // test if the current token is a plain word
                bool plainWordFlag =
                    this->isWordToSelect(normStr, macro,m_microAccessor->readValue(elemItr->properties));
                BoWToken* bowToken = new BoWToken(normStr,macro,
                                                  fullToken->position() + offset,
                                                  fullToken->length());
                bowToken->setInflectedForm(fullToken->stringForm());
                tokenRep->push_back(make_pair(bowToken, plainWordFlag));
            }
        }
    }
}


// -----------------------------------------------------------------------------
// -- word selection
// -----------------------------------------------------------------------------

bool LTRTextBuilder::isWordToSelect(
    const Lima::LimaString& lemma,
    LinguisticCode macroCategory,
    LinguisticCode microCategory) const
{

    if (this->isWordToSelect(lemma, macroCategory)) {
        return ! static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).isAnEmptyMicroCategory(microCategory);
    }
    return false;
}

bool LTRTextBuilder::isWordToSelect(
    const Lima::LimaString& lemma,
    LinguisticCode macroCategory) const {

    if (static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).isAnEmptyMacroCategory(macroCategory)) {
        return false;
    }
    else {
        if (m_stopList && (m_stopList->find(lemma) != m_stopList->end())) {
            return false;
        }
    }
    return true;
}


} // AnalysisDumpers
} // LinguisticProcessing
} // Lima
