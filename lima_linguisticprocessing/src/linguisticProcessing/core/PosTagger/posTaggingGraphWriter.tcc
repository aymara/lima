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

template <typename Graph, typename Vertex> class PosTaggingVertexWriter 
{
public:
  PosTaggingVertexWriter(const Graph* graph,
                          MediaId language,
                          const std::deque<std::string>& display):
    m_graph(graph),
    m_language(language),
    m_display(display)
  {
    m_propertyCodeManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager());
  }

  void operator()(std::ostream& out, const Vertex& v) const
  {

    const Common::PropertyCode::PropertyManager& macroManager = m_propertyCodeManager->getPropertyManager("MACRO");
    const Common::PropertyCode::PropertyManager& microManager = m_propertyCodeManager->getPropertyManager("MICRO");
    const FsaStringsPool& stringspool= Common::MediaticData::MediaticData::single().stringsPool(m_language);
/*    if ( (in_degree(v, *m_graph) == 0) && (out_degree(v, *m_graph) == 0) )
    {
        return;
    }*/
    LinguisticAnalysisStructure::MorphoSyntacticData* data=get(vertex_data,*m_graph,v);

     out << "[label=\"" << v << "\\l";
     if (data == 0)
     {
      out << "no FullToken";
     }
     else 
     {
       LinguisticAnalysisStructure::Token* token=get(vertex_token,*m_graph,v);
       if (m_display.size()==0)
       {
         out << token->stringForm() << " [" << microManager.getPropertyAccessor().readValue(data->begin()->properties) << "] ";
       } 
       else
       {
          for (std::deque<std::string>::const_iterator it=m_display.begin();
              it!=m_display.end(); it++)
          {
            if (it!=m_display.begin())
            {
              out << "\\l";
            }
            if ( *it == "position" )
            {
              out << token->position();
            }
            else if (*it == "length" )
            {
              out << token->length();
            } 
            else if (*it == "tstatus")
            {
              out << Common::Misc::limastring2utf8stdstring(token->status().defaultKey());
            } 
            else if (*it == "text") 
            {
              Lima::LimaString ls=token->stringForm();
              int index=ls.indexOf('"');
              if (index!=-1) 
              {
                ls[index]=Lima::LimaChar('\'');
              }
              out << ls;
            } 
            else if (*it == "inflectedform" )
            {
              std::set<StringsPoolIndex> forms=data->allInflectedForms();
              for (std::set<StringsPoolIndex>::const_iterator formItr=forms.begin();
                   formItr!=forms.end();
                   formItr++)
              {
                if (formItr!=forms.begin()) 
                {
                  out << "#";
                }
                Lima::LimaString ls=stringspool[*formItr];
                int index=ls.indexOf('"');
                if (index!=-1) 
                {
                  ls[index]=Lima::LimaChar('\'');
                }
                out << ls;
              }
            } 
            else if (*it == "lemme" )
            {
              std::set<StringsPoolIndex> forms=data->allLemma();
              for (std::set<StringsPoolIndex>::const_iterator formItr=forms.begin();
                   formItr!=forms.end();
                   formItr++)
              {
                if (formItr!=forms.begin()) 
                {
                  out << "#";
                }
                Lima::LimaString ls=stringspool[*formItr];
                int index=ls.indexOf('"');
                if (index!=-1) 
                {
                  ls[index]=Lima::LimaChar('\'');
                }
                out << ls;
              }
            } 
            else if (*it == "symbolicmicrocategory" )
            {
              std::set<LinguisticCode> micros;
              data->allValues(microManager.getPropertyAccessor(),micros);
              for (std::set<LinguisticCode>::const_iterator it=micros.begin();
                   it!=micros.end();
                   it++)
              {
                if (it!=micros.begin()) 
                {
                  out << "#";
                }
                out << microManager.getPropertySymbolicValue(*it);
             }
            } 
            else if (*it == "numericmicrocategory" )
            {
              std::set<LinguisticCode> micros;
              data->allValues(microManager.getPropertyAccessor(),micros);
              for (std::set<LinguisticCode>::const_iterator it=micros.begin();
                   it!=micros.end();
                   it++)
              {
                if (it!=micros.begin()) 
                {
                  out << "#";
                }
                out << *it;
              }
            } 
            else if (*it == "symbolicmacrocategory" )
            {
              std::set<LinguisticCode> macros;
              data->allValues(macroManager.getPropertyAccessor(),macros);
              for (std::set<LinguisticCode>::const_iterator it=macros.begin();
                   it!=macros.end();
                   it++)
              {
                if (it!=macros.begin()) out << "#";
                out << macroManager.getPropertySymbolicValue(*it);
              }
            } 
            else if (*it == "numericmacrocategory" )
            {
              std::set<LinguisticCode> macros;
              data->allValues(macroManager.getPropertyAccessor(),macros);
              for (std::set<LinguisticCode>::const_iterator it=macros.begin();
                   it!=macros.end();
                   it++)
              {
                if (it!=macros.begin()) out << "#";
                out << *it;
              }
            } 
            else if (*it == "genders" )
            {
              const Common::PropertyCode::PropertyManager& genderManager = m_propertyCodeManager->getPropertyManager("GENDER");
              std::set< LinguisticCode > genders;
              data->allValues(genderManager.getPropertyAccessor(),genders);
              std::set< LinguisticCode >::const_iterator itgenders, itgenders_end;
              itgenders = genders.begin(); itgenders_end = genders.end();
              if ( (itgenders != itgenders_end) && (*itgenders != NONE_1) )
              {
                out << genderManager.getPropertySymbolicValue(*itgenders);
                itgenders++;
              }
              for (; itgenders != itgenders_end; itgenders++)
              {
                if (*itgenders != NONE_1)
                {
                  out << "," << genderManager.getPropertySymbolicValue(*itgenders);
                }
              }
            } 
            else if (*it == "numbers" )
            {
              const Common::PropertyCode::PropertyManager& numberManager = m_propertyCodeManager->getPropertyManager("NUMBER");
              std::set< LinguisticCode > numbers;
              data->allValues(numberManager.getPropertyAccessor(),numbers);
              std::set< LinguisticCode >::const_iterator itnumbers, itnumbers_end;
              itnumbers = numbers.begin(); itnumbers_end = numbers.end();
              if ( (itnumbers != itnumbers_end) && (*itnumbers != NONE_1) )
              {
                out << numberManager.getPropertySymbolicValue(*itnumbers);
                itnumbers++;
              }
              for (; itnumbers != itnumbers_end; itnumbers++)
                if (*itnumbers != NONE_1)
                  out << "," << numberManager.getPropertySymbolicValue(*itnumbers);
            } 
            else if (*it == "syntax" )
            {
              const Common::PropertyCode::PropertyManager& syntaxManager = m_propertyCodeManager->getPropertyManager("SYNTAX");
              std::set< LinguisticCode > syntax;
              data->allValues(syntaxManager.getPropertyAccessor(),syntax);
              std::set< LinguisticCode >::const_iterator itn, itn_end;
              itn= syntax.begin(); itn_end = syntax.end();
              if ( (itn!= itn_end) && (*itn != NONE_1) )
              {
                out << syntaxManager.getPropertySymbolicValue(*itn);
                itn++;
              }
              for (; itn != itn_end; itn++)
              {
                if (*itn != NONE_1)
                {
                  out << ",L_NONE";
                }
              }
            }
          }
          out << "\\l";
       }
     }
     out << "\",fontname=\"Monospace\",fontsize=\"9\"]";
  }
  private:
  const Graph* m_graph;
  const MediaId m_language;
  const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;
  std::deque<std::string> m_display;
};

template <typename Graph, typename Edge> class PosTaggingEdgeWriter {
  public:
    PosTaggingEdgeWriter(
    MediaId language,
  const Graph* graph,
  const Lima::LinguisticProcessing::PosTagger::TrigramMatrix* trigramMatrix,
  const Lima::LinguisticProcessing::PosTagger::BigramMatrix* bigramMatrix) :
  m_graph(graph),
  m_trigramMatrix(trigramMatrix),
  m_bigramMatrix(bigramMatrix) {
      m_propertyCodeManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager());
  }

    void operator()(std::ostream& out, const Edge& e) const {

  const Common::PropertyCode::PropertyManager& microManager = m_propertyCodeManager->getPropertyManager("MICRO");


  bool hasNonValidTrigram=false;
  bool hasNonValidBigram=false;

  std::vector<std::vector<uint64_t> > gramNotFound;
  std::vector<uint64_t> gram;

  LinguisticCode cat1(0),cat2(0),cat3(0);
  LinguisticAnalysisStructure::MorphoSyntacticData* d3=get(vertex_data,*m_graph,target(e,*m_graph));
  if (d3!=0 && !d3->empty()) {
    cat3=microManager.getPropertyAccessor().readValue(d3->begin()->properties);
  }
  LinguisticAnalysisStructure::MorphoSyntacticData* d2=get(vertex_data,*m_graph,source(e,*m_graph));
  if (d2!=0 && !d2->empty()) {
    cat2=microManager.getPropertyAccessor().readValue(d2->begin()->properties);
  }
  gram.push_back(cat2);
  gram.push_back(cat3);

  LinguisticGraphInEdgeIt inItr,inItrEnd;
  for(boost::tie(inItr,inItrEnd)=in_edges(source(e,*m_graph),*m_graph);
      inItr!=inItrEnd;
      inItr++)
      {
    LinguisticAnalysisStructure::MorphoSyntacticData* d1=get(vertex_data,*m_graph,source(*inItr,*m_graph));
    if (d1!=0 && !d1->empty()) {
      cat1=microManager.getPropertyAccessor().readValue(d1->begin()->properties);
    }
    if (!m_trigramMatrix->exists(cat1,cat2,cat3))
    {
        hasNonValidTrigram=true;
        gramNotFound.push_back(gram);
        gramNotFound.back().insert(gramNotFound.back().begin(),cat1);
    }

      }
  if (hasNonValidTrigram) {
      // check if bigram is valid
      if (!m_bigramMatrix->exists(cat2,cat3))
      {
    gramNotFound.push_back(gram);
    hasNonValidBigram=true;
      }
  }

  out << "[label=\"";
  for (std::vector<std::vector<uint64_t> >::iterator it=gramNotFound.begin();
       it!=gramNotFound.end();
       it++)
       {
     copy(it->begin(),it->end(),std::ostream_iterator<uint64_t>(out,","));
     out << "\\l";
       }
  out << "\",style=";
  if (hasNonValidBigram)
  {
      out << "dotted";
  } else if (hasNonValidTrigram)
  {
      out << "dashed";
  } else {
      out << "solid";
  }
      out << ",fontname=\"Monospace\",fontsize=\"9\"]";
  }


  private:
    const Graph* m_graph;
    const Lima::LinguisticProcessing::PosTagger::TrigramMatrix*        m_trigramMatrix;
    const Lima::LinguisticProcessing::PosTagger::BigramMatrix*         m_bigramMatrix;
    const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;
 };
