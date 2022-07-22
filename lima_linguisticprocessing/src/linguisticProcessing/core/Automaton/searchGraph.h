// Copyright 2002-2018 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file       searchGraph.h
 * @author     besancon (besanconr@zoe.cea.fr)
 * @date       Tue May 24 2005
 * @version    $Id$
 * copyright   Copyright (C) 2005-2018 by CEA LIST
 * Project     Automaton
 *
 * @brief classes to abstract the sense of search in the graph
 * (forward/backward)
 *
 ***********************************************************************/
#ifndef SEARCHGRAPH_H
#define SEARCHGRAPH_H

#include "AutomatonExport.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include <deque>

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

/**
 * enumerated type to indicate in which sense the automaton should
 * be built or searched
 *
 */
enum SearchGraphSense {
    FORWARDSEARCH, /**< forward search in the graph */
    BACKWARDSEARCH  /**< backward search in the graph */
};

class LIMA_AUTOMATON_EXPORT SearchGraph {
public:
    SearchGraph() {}
    virtual ~SearchGraph() {}
    virtual void findNextVertices(const LinguisticGraph* graph,
                                  const LinguisticGraphVertex& current) = 0;
    virtual bool getNextVertex(const LinguisticGraph* graph,
                               LinguisticGraphVertex& next) = 0;
    virtual SearchGraph* createNew() const = 0;
    virtual LinguisticGraphVertex endOfGraph(const LinguisticAnalysisStructure::AnalysisGraph& graph) = 0;
    virtual void clear() = 0;
    virtual void reinit() = 0;
};

#ifdef DEBUG_LP
LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const SearchGraph *x, const LinguisticGraph* graph);
#endif

class LIMA_AUTOMATON_EXPORT BackwardSearch :
            public SearchGraph
{
public:
    BackwardSearch();
    ~BackwardSearch() {}
    void findNextVertices(const LinguisticGraph* graph,
                          const LinguisticGraphVertex& current) override;
    bool getNextVertex(const LinguisticGraph* graph,
                       LinguisticGraphVertex& next) override;
    SearchGraph* createNew() const override;
    LinguisticGraphVertex endOfGraph(const LinguisticAnalysisStructure::AnalysisGraph& graph) override;
    void clear()  override{
        m_current.pop_back();
    }
    void reinit()  override{
        m_current.clear();
    }

    typedef std::pair<LinguisticGraphVertex,std::pair<LinguisticGraphInEdgeIt,LinguisticGraphInEdgeIt> > Vertex2EdgePair;

private:
    // use a deque for a stack so that can be clear()ed
    std::deque<Vertex2EdgePair> m_current;

#ifdef DEBUG_LP
    friend LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const BackwardSearch::Vertex2EdgePair& x, const LinguisticGraph* graph);
    friend LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const BackwardSearch& x, const LinguisticGraph* graph);
#endif
};

class LIMA_AUTOMATON_EXPORT ForwardSearch :
            public SearchGraph
{
public:
    ForwardSearch();
    ~ForwardSearch() {}
    void findNextVertices(const LinguisticGraph* graph,
                          const LinguisticGraphVertex& current) override;
    bool getNextVertex(const LinguisticGraph* graph,
                       LinguisticGraphVertex& next) override;
    SearchGraph* createNew() const override;
    LinguisticGraphVertex endOfGraph(const LinguisticAnalysisStructure::AnalysisGraph& graph) override;
    void clear() override {
        m_current.pop_back();
    }
    void reinit() override {
        m_current.clear();
    }

    typedef std::pair<LinguisticGraphVertex,std::pair<LinguisticGraphOutEdgeIt,LinguisticGraphOutEdgeIt> > Vertex2EdgePair;

private:
    // use a deque for a stack so that can be clear()ed
    std::deque<Vertex2EdgePair> m_current;

#ifdef DEBUG_LP
    friend LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const ForwardSearch::Vertex2EdgePair& x, const LinguisticGraph* graph);
    friend LIMA_AUTOMATON_EXPORT std::ostream& output(std::ostream& os, const ForwardSearch& x, const LinguisticGraph* graph);
#endif
};

} // end namespace
} // end namespace
} // end namespace

#endif
