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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "TextDumper.h"
// #include "linguisticProcessing/core/LinguisticProcessors/DumperStream.h"
#include "common/MediaProcessors/DumperStream.h"
#include "common/time/traceUtils.h"
#include "common/Data/strwstrtools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/LinguisticGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/MorphoSyntacticDataUtils.h"

#include <boost/algorithm/string/replace.hpp>

#include <fstream>
#include <queue>

using namespace std;
//using namespace boost;
using namespace boost::tuples;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::PropertyCode;
using namespace Lima::Common::XMLConfigurationFiles;

namespace Lima
{

namespace LinguisticProcessing
{

namespace AnalysisDumpers
{

SimpleFactory<MediaProcessUnit,TextDumper> textDumperFactory(TEXTDUMPER_CLASSID);

class TextDumperPrivate
{
  friend class TextDumper;

public:
  TextDumperPrivate();

  virtual ~TextDumperPrivate();

  MediaId m_language;
  std::string m_property;
  const Common::PropertyCode::PropertyAccessor* m_propertyAccessor;
  const Common::PropertyCode::PropertyManager* m_propertyManager;
  const Common::PropertyCode::PropertyManager* m_timeManager; //Ajout
  const Common::PropertyCode::PropertyAccessor* m_timeAccessor; //Ajout

  std::string m_graph;
  std::string m_sep;
  std::string m_sepPOS;
  bool m_followGraph;

  bool m_allFeatures;
  QStringList m_features;

  // private member functions
  void outputVertex(std::ostream& out,
                    const LinguisticAnalysisStructure::Token* ft,
                    const std::vector<LinguisticAnalysisStructure::MorphoSyntacticData*>& data,
                    const FsaStringsPool& sp,
                    uint64_t offset=0) const;

  void outputString(std::ostream& out,const std::string& str) const;
};


TextDumperPrivate::TextDumperPrivate():
m_language(0),
m_property("MICRO"),
m_propertyAccessor(0),
m_propertyManager(0),
m_graph("PosGraph"),
m_sep(" | "),
m_sepPOS("#"),
m_followGraph(false)
{}


TextDumperPrivate::~TextDumperPrivate()
{}

TextDumper::TextDumper():
  AbstractTextualAnalysisDumper(),
  m_d(new TextDumperPrivate())
{
}


TextDumper::~TextDumper()
{
  delete m_d;
}

void TextDumper::init(Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
                      Manager* manager)

{
  AbstractTextualAnalysisDumper::init(unitConfiguration,manager);

  m_d->m_language=manager->getInitializationParameters().media;
  try
  {
    m_d->m_graph=unitConfiguration.getParamsValueAtKey("graph");
  }
  catch (NoSuchParam& ) {} // keep default value

  if (m_d->m_graph=="AnalysisGraph") {
    // change default for followGraph
    m_d->m_followGraph=true;
  }
  try {
    m_d->m_sep=unitConfiguration.getParamsValueAtKey("sep");
  }
  catch (NoSuchParam& ) {} // keep default value

  try {
    m_d->m_sepPOS=unitConfiguration.getParamsValueAtKey("sepPOS");
  }
  catch (NoSuchParam& ) {} // keep default value

  try {
    m_d->m_property=unitConfiguration.getParamsValueAtKey("property");
  }
  catch (NoSuchParam& ) {} // keep default value

  try {
    std::string str=unitConfiguration.getParamsValueAtKey("followGraph");
    if (str=="1" || str=="true" || str=="yes") {
      m_d->m_followGraph=true;
    }
    else {
      m_d->m_followGraph=false;
    }
  }
  catch (NoSuchParam& ) {} // keep default value

  const auto& codeManager = static_cast<const LanguageData&>(
    Lima::Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getPropertyCodeManager();
  m_d->m_propertyAccessor=&codeManager.getPropertyAccessor(m_d->m_property);
  m_d->m_propertyManager=&codeManager.getPropertyManager(m_d->m_property);

  QString timeCode = static_cast<const LanguageData&>(
    Lima::Common::MediaticData::MediaticData::single().mediaData(m_d->m_language)).getLimaToLanguageCodeMappingValue("TIME");
  m_d->m_timeManager=&codeManager.getPropertyManager(timeCode.toUtf8().constData());
  m_d->m_timeAccessor=&codeManager.getPropertyAccessor(timeCode.toUtf8().constData());

  try
  {
    m_d->m_allFeatures = unitConfiguration.getBooleanParameter("allFeatures");
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )
  {
    // Ignored parameters allFeatures and features are optional. Then use only
    // main tag (micro category)
  }
  if (!m_d->m_allFeatures)
  {
    try
    {
      auto features = unitConfiguration.getListsValueAtKey("features");
      for (const auto& feature: features)
      {
        m_d->m_features << QString::fromUtf8(feature.c_str());
      }
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& )
    {
      // Ignored parameters allFeatures and features are optional. Then use only
      // main tag (micro category)
    }
  }

}

LimaStatusCode TextDumper::process(AnalysisContent& analysis) const
{
  DUMPERLOGINIT;
  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  if (metadata == 0)
  {
      LERROR << "no LinguisticMetaData ! abort";
      return MISSING_DATA;
  }

  QScopedPointer<DumperStream> dstream(initialize(analysis));

  std::map<Token*, std::vector<MorphoSyntacticData*>, lTokenPosition > categoriesMapping;

  AnalysisGraph* tokenList=static_cast<AnalysisGraph*>(analysis.getData(m_d->m_graph));
  if (tokenList==0)
  {
    LERROR << "graph " << m_d->m_graph << " has not been produced: check pipeline";
    return MISSING_DATA;
  }
  LinguisticGraph* graph=tokenList->getGraph();
  const FsaStringsPool& sp=Common::MediaticData::MediaticData::single().stringsPool(m_d->m_language);

  if (m_d->m_followGraph)
  {
    // instead of looking to all vertices, follow the graph (in
    // morphological graph, some vertices are not related to main graph:
    // idiomatic expressions parts and named entity parts)

    std::queue<LinguisticGraphVertex> toVisit;
    std::set<LinguisticGraphVertex> visited;
    toVisit.push(tokenList->firstVertex());

    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    while (!toVisit.empty()) {
      LinguisticGraphVertex v=toVisit.front();
      toVisit.pop();
      if (v == tokenList->lastVertex()) {
        continue;
      }

      for (boost::tie(outItr,outItrEnd)=out_edges(v,*graph);
           outItr!=outItrEnd; outItr++)
      {
        LinguisticGraphVertex next=target(*outItr,*graph);
        if (visited.find(next)==visited.end())
        {
          visited.insert(next);
          toVisit.push(next);
        }
      }

      Token* ft=get(vertex_token,*graph,v);
      if( ft!=0)
      {
        categoriesMapping[ft].push_back(get(vertex_data,*graph,v));
      }
    }
  }
  else
  { // output all vertices
    LinguisticGraphVertexIt vxItr,vxItrEnd;
    boost::tie(vxItr,vxItrEnd) = vertices(*graph);
    for (;vxItr!=vxItrEnd;vxItr++)
    {
      Token* ft=get(vertex_token,*graph,*vxItr);
      if( ft!=0)
      {
        categoriesMapping[ft].push_back(get(vertex_data,*graph,*vxItr));
      }
    }
  }

  for (auto ftItr=categoriesMapping.cbegin();
       ftItr!=categoriesMapping.cend();
       ftItr++)
  {
    m_d->outputVertex(dstream->out(),
                 ftItr->first,
                 ftItr->second,sp,
                 metadata->getStartOffset());
  }

  return SUCCESS_ID;
}


void TextDumperPrivate::outputVertex(std::ostream& out,
                              const Token* ft,
                              const vector<MorphoSyntacticData*>& data,
                              const FsaStringsPool& sp,
                              uint64_t offset) const
{
#ifdef DEBUG_LP
  DUMPERLOGINIT;
#endif
  const auto& mediaData = static_cast<const LanguageData&>(
          Lima::Common::MediaticData::MediaticData::single().mediaData(m_language));
  const auto& propertyCodeManager = mediaData.getPropertyCodeManager();
  const auto& microManager = propertyCodeManager.getPropertyManager("MICRO");
  // to add tokens possible tags to the tagger dictionary
  const auto& propertyManagers=propertyCodeManager.getPropertyManagers();


  ltNormProperty sorter(*m_propertyAccessor);

//    uint64_t nbmicros=ft->countMicros();
  std::ostringstream os;
  uint64_t position=ft->position() + offset;
  os << position << m_sep;
  out << os.str();
  outputString(out, Common::Misc::limastring2utf8stdstring(ft->stringForm()));
  for (auto dataItr = data.cbegin(), dataItr_end = data.cend();
       dataItr != dataItr_end; dataItr++)
  {
    MorphoSyntacticData* data=*dataItr;
    sort(data->begin(),data->end(),sorter);
    StringsPoolIndex norm(0),curNorm(0);
    LinguisticCode micro(0),curMicro(0),tense(0),curTense(0);
    for (auto elemItr = data->cbegin(); elemItr != data->cend(); elemItr++)
    {
      curNorm=elemItr->normalizedForm;
      curMicro=m_propertyAccessor->readValue(elemItr->properties);
      curTense=m_timeAccessor->readValue(elemItr->properties); //ajout
      if ((curNorm != norm) || (curMicro != micro))
      {
        norm=curNorm;
        micro=curMicro;
        tense=curTense; //ajout

        std::ostringstream os2;
        os2 << m_sep;
        out << os2.str();
        outputString(out,Common::Misc::limastring2utf8stdstring(sp[norm]));
        std::ostringstream os3;
        out << m_sepPOS;
        LinguisticCode code = elemItr->properties;


#ifdef DEBUG_LP
        LDEBUG << "TextDumper::process taq and features";
#endif
        QString fullTag;
        QTextStream tagStream(&fullTag);
        std::string tag = microManager.getPropertySymbolicValue(code);
        tagStream << QString::fromUtf8(tag.c_str());
        if (m_allFeatures ||!m_features.isEmpty())
        {
          QStringList features;
          for (auto propItr = propertyManagers.cbegin();
              propItr != propertyManagers.cend(); propItr++)
          {
            if (!propItr->second.getPropertyAccessor().empty(code))
            {
              QString property = QString::fromUtf8(propItr->first.c_str());
              QString value = QString::fromUtf8(propItr->second.getPropertySymbolicValue(code).c_str());
              if (property != "MACRO" && property != "MICRO"
                  && (m_allFeatures || m_features.contains(property)))
              {
                features << QString("%1=%2").arg(property).arg(value);
              }
            }
          }
          features.sort();
          if (!features.isEmpty())
          {
            tagStream << "-";
            for (auto it = features.cbegin(); it != features.cend(); it++)
            {
              if (it != features.cbegin())
              {
                tagStream << "|";
              }
              tagStream << *it;
            }
          }
        }
#ifdef DEBUG_LP
        LDEBUG << "TextDumper::process taq and features:" << fullTag;
#endif
        out << fullTag.toUtf8().constData();

      }
    }
  }
  out << endl;
}

// output string: escape endline characters and separator characters
void TextDumperPrivate::outputString(std::ostream& out,const std::string& str) const
{
  string newstr(str);
  boost::replace_all(newstr,"\n","\n");
  boost::replace_all(newstr," ","_");
  //boost::replace_all(newstr,m_sep,"\"+m_sep);
  out << newstr;
}


} // end namespace
} // end namespace
} // end namespace
