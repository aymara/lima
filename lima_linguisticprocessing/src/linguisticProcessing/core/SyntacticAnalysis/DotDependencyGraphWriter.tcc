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
template <typename Graph, typename Vertex> 
class DependencyGraphVertexWriter 
{
public:
  DependencyGraphVertexWriter(const Graph* graph,
                              PosTagger::PosTaggingVertexWriter<LinguisticGraph,LinguisticGraphVertex> vertexWriter):
    m_graph(graph),
    m_postaggingVertexWriter(vertexWriter)
  {}
  
  void operator()(std::ostream& out, const Vertex& v) const
  {
//    LinguisticGraphVertex pv=static_cast<LinguisticGraphVertex>(v);
    m_postaggingVertexWriter(out,v);
  }
  
private:
  const Graph* m_graph;
  PosTagger::PosTaggingVertexWriter<LinguisticGraph,LinguisticGraphVertex> m_postaggingVertexWriter; 
};

// write only edges of dependency graph
template <typename Graph, typename Edge> class DependencyGraphEdgeWriter 
{
public:
  DependencyGraphEdgeWriter(const Graph* graph,
                            MediaId language):
    m_graph(graph),
    m_language(language)
  {
  }

  void operator()(std::ostream& out, const Edge& e) const 
  {
    CEdgeDepRelTypePropertyMap typeMap = 
      get(edge_deprel_type, *m_graph);
    std::string relName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(static_cast<Common::MediaticData::SyntacticRelationId>(typeMap[e]));
    SALOGINIT;
    LDEBUG << "Got name="<<relName<<" for rel="<<source(e,*m_graph)<<","<<target(e,*m_graph)<<"," << typeMap[e] << LENDL;
    if (relName.empty()) {
      relName="UNKNOWN";
    }
    out << "[label=\"" << relName
        << "\",fontname=\"Monospace\",fontsize=\"9\"]";
  }
private:
  const Graph* m_graph;
  MediaId m_language;
};


// write edges of morphological graph + edges of dependency graph
template <typename Graph, typename Edge> 
class PosTaggingDepGraphEdgeWriter
{
public:
  PosTaggingDepGraphEdgeWriter(const Graph* graph,
                               MediaId language,
                               const DependencyGraph* depGraph,
                               const SyntacticData* syntacticData):
    m_graph(graph),
    m_language(language),
    m_depGraph(depGraph),
    m_processedVertices(0),
    m_syntacticData(syntacticData)
  {
    m_processedVertices=new std::set<LinguisticGraphVertex>();
  }

  PosTaggingDepGraphEdgeWriter(const PosTaggingDepGraphEdgeWriter& w):
    m_graph(w.m_graph),
    m_language(w.m_language),
    m_depGraph(w.m_depGraph),
    m_processedVertices(0)
  {
    if (w.m_processedVertices!=0) {
      m_processedVertices=new
        std::set<LinguisticGraphVertex>(*(w.m_processedVertices));
    }
  }

  ~PosTaggingDepGraphEdgeWriter() { delete m_processedVertices; }

  void operator()(std::ostream& out, const Edge& e) const
  {
    //morphological path => gray
    out << "[color=darkgray]";
    
    //take the edges in depGraph attached to source vertex
    LinguisticGraphVertex v=source(e,*m_graph);
//     LinguisticGraphVertex t=target(e,*m_graph);
    if (v == 0) {
      return; // first vertex (hard-coded...)
    }
    if (m_processedVertices->find(v)!=m_processedVertices->end()) {
      return; // vertex has already been treated
    }
    m_processedVertices->insert(v);
    
//     DependencyGraphVertex depv=static_cast<LinguisticGraphVertex>(v);
    DependencyGraphVertex depv=m_syntacticData->depVertexForTokenVertex(v);
//     DependencyGraphVertex dept=m_syntacticData->depVertexForTokenVertex(t);

    CEdgeDepRelTypePropertyMap typeMap = get(edge_deprel_type, *m_depGraph);

    DependencyGraphOutEdgeIt it,it_end;
    boost::tie (it,it_end)=out_edges(depv,*m_depGraph);
    for (; it!=it_end; it++) 
    {
      Common::MediaticData::SyntacticRelationId relid = static_cast<Common::MediaticData::SyntacticRelationId>(typeMap[*it]);
      out << ";" << std::endl << depv
          << "->" << m_syntacticData->tokenVertexForDepVertex(target(*it,*m_depGraph))
          << " [color=red,fontcolor=red,weight=0,fontname=\"Monospace\",fontsize=\"9\",label=\"" ;
          std::string relName = static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getSyntacticRelationName(relid);
//       SALOGINIT;
//       LDEBUG << "Got name="<<relName<<" for rel="<<depv<<","<<m_syntacticData->tokenVertexForDepVertex(target(*it,*m_depGraph))<<"," << typeMap[*it] << LENDL;
      if (relName.empty()) {
        relName="UNKNOWN";
      }
      out << relName << "\"]";
    }
  }
private:
  const Graph* m_graph;
  MediaId m_language;
  const DependencyGraph* m_depGraph;
  std::set<LinguisticGraphVertex>* m_processedVertices;
  const SyntacticData* m_syntacticData;
};
