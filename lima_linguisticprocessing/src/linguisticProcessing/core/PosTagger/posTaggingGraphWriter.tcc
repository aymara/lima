// Copyright 2002-2020 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
       auto token = get(vertex_token,*m_graph, v);
       if (m_display.size()==0)
       {
         out << token->stringForm().toStdString()
              << " ["
              << microManager.getPropertyAccessor().readValue(data->begin()->properties)
              << "] ";
       }
       else
       {
          for (auto it = m_display.cbegin(); it != m_display.cend(); it++)
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
              auto ls = token->stringForm();
              int index=ls.indexOf('"');
              if (index!=-1)
              {
                ls[index] = QChar('\'');
              }
              out << ls.toStdString();
            }
            else if (*it == "inflectedform" )
            {
              const auto& forms = data->allInflectedForms();
              for (auto formItr = forms.cbegin(); formItr != forms.cend();
                   formItr++)
              {
                if (formItr != forms.begin())
                {
                  out << "#";
                }
                auto ls = stringspool[*formItr];
                int index = ls.indexOf('"');
                if (index!=-1)
                {
                  ls[index] = QChar('\'');
                }
                out << ls.toStdString();
              }
            }
            else if (*it == "lemme" )
            {
              const auto& forms = data->allLemma();
              for (auto formItr = forms.cbegin(); formItr != forms.cend();
                   formItr++)
              {
                if (formItr!=forms.begin())
                {
                  out << "#";
                }
                auto ls = stringspool[*formItr];
                auto index = ls.indexOf('"');
                if (index!=-1)
                {
                  ls[index] = QChar('\'');
                }
                out << ls.toStdString();
              }
            }
            else if (*it == "symbolicmicrocategory" )
            {
              std::set<LinguisticCode> micros;
              data->allValues(microManager.getPropertyAccessor(), micros);
              for (auto it = micros.cbegin(); it != micros.cend(); it++)
              {
                if (it != micros.begin())
                {
                  out << "#";
                }
                out << microManager.getPropertySymbolicValue(*it);
             }
            }
            else if (*it == "numericmicrocategory" )
            {
              std::set<LinguisticCode> micros;
              data->allValues(microManager.getPropertyAccessor(), micros);
              for (auto it = micros.cbegin(); it != micros.cend(); it++)
              {
                if (it!=micros.begin())
                {
                  out << "#";
                }
                out << it->toString();
              }
            }
            else if (*it == "symbolicmacrocategory" )
            {
              std::set<LinguisticCode> macros;
              data->allValues(macroManager.getPropertyAccessor(), macros);
              for (auto it = macros.cbegin(); it != macros.cend(); it++)
              {
                if (it!=macros.begin()) out << "#";
                out << macroManager.getPropertySymbolicValue(*it);
              }
            }
            else if (*it == "numericmacrocategory" )
            {
              std::set<LinguisticCode> macros;
              data->allValues(macroManager.getPropertyAccessor(), macros);
              for (auto it = macros.cbegin(); it != macros.cend(); it++)
              {
                if (it!=macros.begin()) out << "#";
                out << it->toString();
              }
            }
            else if (*it == "genders" )
            {
              const auto& genderManager = m_propertyCodeManager->getPropertyManager("GENDER");
              std::set< LinguisticCode > genders;
              data->allValues(genderManager.getPropertyAccessor(), genders);
              auto itgenders = genders.cbegin();
              auto itgenders_end = genders.cend();
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
              const auto& numberManager = m_propertyCodeManager->getPropertyManager("NUMBER");
              std::set< LinguisticCode > numbers;
              data->allValues(numberManager.getPropertyAccessor(), numbers);
              auto itnumbers = numbers.cbegin();
              auto itnumbers_end = numbers.cend();
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
              const auto& syntaxManager = m_propertyCodeManager->getPropertyManager("SYNTAX");
              std::set< LinguisticCode > syntax;
              data->allValues(syntaxManager.getPropertyAccessor(), syntax);
              auto itn = syntax.cbegin();
              auto itn_end = syntax.cend();
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

template <typename Graph, typename Edge> class PosTaggingEdgeWriter
{
public:
  PosTaggingEdgeWriter(
    MediaId language,
    const Graph* graph,
    const Lima::LinguisticProcessing::PosTagger::TrigramMatrix* trigramMatrix,
    const Lima::LinguisticProcessing::PosTagger::BigramMatrix* bigramMatrix) :
    m_graph(graph),
    m_trigramMatrix(trigramMatrix),
    m_bigramMatrix(bigramMatrix)
  {
    m_propertyCodeManager=&(static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager());
  }

  void operator()(std::ostream& out, const Edge& e) const
  {
    const auto& microManager = m_propertyCodeManager->getPropertyManager("MICRO");

    bool hasNonValidTrigram = false;
    bool hasNonValidBigram = false;

    std::vector<std::vector<LinguisticCode> > gramNotFound;
    std::vector<LinguisticCode> gram;

    LinguisticCode cat1, cat2, cat3;
    auto d3 = get(vertex_data,*m_graph,target(e,*m_graph));
    if (d3 != nullptr && !d3->empty())
    {
      cat3 = microManager.getPropertyAccessor().readValue(d3->begin()->properties);
    }
    auto d2 = get(vertex_data,*m_graph,source(e,*m_graph));
    if (d2 !=0 && !d2->empty())
    {
      cat2 = microManager.getPropertyAccessor().readValue(d2->begin()->properties);
    }
    gram.push_back(cat2);
    gram.push_back(cat3);

    LinguisticGraphInEdgeIt inItr, inItrEnd;
    for(boost::tie(inItr,inItrEnd) = in_edges(source(e,*m_graph),*m_graph);
        inItr!=inItrEnd;
        inItr++)
    {
      auto d1 = get(vertex_data,*m_graph,source(*inItr,*m_graph));
      if (d1 != 0 && !d1->empty())
      {
        cat1 = microManager.getPropertyAccessor().readValue(d1->begin()->properties);
      }
      if (!m_trigramMatrix->exists(cat1, cat2, cat3))
      {
        hasNonValidTrigram = true;
        gramNotFound.push_back(gram);
        gramNotFound.back().insert(gramNotFound.back().begin(), cat1);
      }

    }
    if (hasNonValidTrigram)
    {
      // check if bigram is valid
      if (!m_bigramMatrix->exists(cat2, cat3))
      {
        gramNotFound.push_back(gram);
        hasNonValidBigram = true;
      }
    }

    out << "[label=\"";
    for (auto it = gramNotFound.begin(); it != gramNotFound.end(); it++)
    {
      copy(it->begin(), it->end(),
           std::ostream_iterator<LinguisticCode>(out,","));
      out << "\\l";
    }
    out << "\",style=";
    if (hasNonValidBigram)
    {
      out << "dotted";
    }
    else if (hasNonValidTrigram)
    {
      out << "dashed";
    }
    else
    {
      out << "solid";
    }
    out << ",fontname=\"Monospace\",fontsize=\"9\"]";
  }


  private:
    const Graph* m_graph;
    const Lima::LinguisticProcessing::PosTagger::TrigramMatrix* m_trigramMatrix;
    const Lima::LinguisticProcessing::PosTagger::BigramMatrix* m_bigramMatrix;
    const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;
 };
