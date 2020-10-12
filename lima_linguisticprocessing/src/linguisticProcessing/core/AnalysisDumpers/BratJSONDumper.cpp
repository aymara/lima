/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "BratJSONDumper.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

#include "common/MediaticData/mediaticData.h"
#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"
#include "common/time/traceUtils.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/AbstractResource.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/SemanticAnalysis/SemanticAnnotation.h"

#include <boost/algorithm/string.hpp>
#include <fstream>
#include <queue>
#include <tuple>

using namespace std;
using namespace Lima::Common::Misc;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::EventAnalysis;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;
using namespace boost;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit,BratJSONDumper> bratJSONDumperFactory(BRATJSONDUMPER_CLASSID);

BratJSONDumper::BratJSONDumper() :
AbstractIEDumper(),
m_firstEntity(true),
m_firstRelation(true),
m_firstEvent(true),
m_attributes(),
m_norms()
{
  DUMPERLOGINIT;
  LDEBUG << "BratJSONDumper::BratJSONDumper()";
}


BratJSONDumper::~BratJSONDumper()
{}

void BratJSONDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DUMPERLOGINIT;
  LDEBUG << "BratJSONDumper::init";
  AbstractIEDumper::init(unitConfiguration,manager);
  // here, specific parameters for this dumper if different from AbstractIEDumper ones

}

// headers and footers 
void BratJSONDumper::outputGlobalHeader(std::ostream& os, const std::string& /*sourceFile*/,
                                        const LimaStringText& originalText) const
{
  os << "{" << endl << "\"text\": \"" << limastring2utf8stdstring(originalText) << "\"," << endl;
}
void BratJSONDumper::outputGlobalFooter(std::ostream& os) const
{
  if (m_attributes.size()) {
    os << "\"attributes\": [" << endl;
    for (const auto& a: m_attributes) {
      os << a << "," << endl;
    }
    os << "]" << endl;
  }
  if (m_norms.size()) {
    os << "\"normalizations\": [" << endl;
    for (const auto& n: m_norms) {
      os << n << "," << endl;
    }
    os << "]" << endl;
  }
  os << "}" << endl;
}
void BratJSONDumper::outputEntitiesHeader(std::ostream& os) const
{
  os << "\"entities\": [";
}
void BratJSONDumper::outputEntitiesFooter(std::ostream& os) const
{
  os << "]," << endl;
}
void BratJSONDumper::outputRelationsHeader(std::ostream& os) const
{
  os << "\"relations\": [";
}
void BratJSONDumper::outputRelationsFooter(std::ostream& os) const
{
  os << "]" << endl;
}
void BratJSONDumper::outputEventsHeader(std::ostream& os) const
{
  os << "\"events\": [";
}
void BratJSONDumper::outputEventsFooter(std::ostream& os) const
{
  os << "]," << endl;
}

  
void BratJSONDumper::
outputEntityString(ostream& out,
                   unsigned int entityId,
                   const std::string& entityType,
                   const std::string& entityString,
                   const vector<pair<uint64_t,uint64_t> >& positions,
                   const Automaton::EntityFeatures& /*entityFeatures*/, bool /*noNorm*/) const
{
  if (m_firstEntity) { m_firstEntity=false; out << endl; } else { out << "," << endl; }
  
  out << "[ \"T" << entityId << "\", \"" <<  entityType << "\", ";
  if (positions.size()==0) {
    // use simple positions
    DUMPERLOGINIT;
    LERROR << "BratJSONDumper: no positions given for entity" << entityString;
    out << "[[0,0]]";
  }
  else {
    vector<pair<uint64_t,uint64_t> >::const_iterator posIt=positions.begin();
    out << "[ [" << (*posIt).first << "," << (*posIt).second << "]";
    for (posIt++; posIt!=positions.end(); posIt++) {
      out << ", [" << (*posIt).first << "," << (*posIt).second << "]";
    }
    out << " ]";
  }
  out << " ]";
}

void BratJSONDumper::
outputAttributesString(std::ostream& /*out*/,
                        unsigned int entityId,
                        std::map <std::tuple <std::size_t, std::string , std::string >,  std::size_t >& mapAttributes) const
{
  for(auto iterAttribute = mapAttributes.begin();
        iterAttribute!=mapAttributes.end();
        iterAttribute++)
  {
    auto feature = iterAttribute->first;
    std::size_t eId = std::get<0>(feature);
    if( eId == entityId )
    {
      std::size_t attributeId = iterAttribute->second;
      std::string featName = std::get<1>(feature);
      std::string featValue = std::get<2>(feature);
      ostringstream oss;
      oss << "[ \"" << attributeId << "\", \"" << featName << "\", \"" << entityId << "\"";
      if(featValue!=""){
        oss << ", \"" << featValue << "\"";
      }
      oss << "]";
      m_attributes.push_back(oss.str());
    }
  }
}

void BratJSONDumper::
outputEntityNorm(ostream& /*out*/,
                 unsigned int entityId,
                 const std::string& entityNorm) const
{
  ostringstream oss;
  // format is id normType target refdb refid reftext;
  oss << "[ \"N" << entityId << "\", \"Reference\", \"T" << entityId << ", \"X\", \"Y\", \""
      << entityNorm << "\" ]";
  m_norms.push_back(oss.str());
}

unsigned int BratJSONDumper::
outputRelationString(ostream& out,
                     unsigned int relationId,
                     const std::string& relationType,
                     const std::string& sourceArgString,
                     const std::string& targetArgString) const
{
  QString relation = QString::fromUtf8(relationType.c_str());
  QStringList relations = relation.split(',');
  unsigned int i = 0;
  for (auto& rel : relations)
  {
    if (rel.isEmpty())
    {
      DUMPERLOGINIT;
      LERROR << "BratJSONDumper::outputRelationString Ignoring relation with empty type:"
              << relationId << relationType << sourceArgString
              << targetArgString;
      continue;
    }
    if (sourceArgString.empty())
    {
      DUMPERLOGINIT;
      LERROR << "BratJSONDumper::outputRelationString Ignoring relation with empty source:"
              << relationId << relationType << sourceArgString
              << targetArgString;
      continue;
    }
    if (targetArgString.empty())
    {
      DUMPERLOGINIT;
      LERROR << "BratJSONDumper::outputRelationString Ignoring relation with empty target:"
              << relationId << relationType << sourceArgString
              << targetArgString;
      continue;
    }
    
    if (m_firstRelation) { m_firstRelation=false; out << endl; } else { out << "," << endl; }
    
    out << "[ \"R" << (relationId+i) << "\", \"" <<  rel.toUtf8().constData() << "\", [\""
        << "[\"Arg1\", \"T" << sourceArgString << "\"], "
        << "[\"Arg2\", \"T" << targetArgString << "\"]] ]" << endl;
    i++;
  }
  return i;
}

void BratJSONDumper::
outputEventString(ostream& out,
                  unsigned int eventId,
                  unsigned int eventMentionId,
                  const std::string& /*eventMentionType*/,
                  const std::vector<unsigned int>& eventRoleId,
                  const std::vector<std::string>& eventRoleType) const
{
  if (m_firstEvent) { m_firstEvent=false; out << endl; } else { out << "," << endl; }
  
    out << "[ \"E" << eventId << "\", ";
  // mention type is now one of the roles
  if (eventMentionId!=0) {
    //out << eventMentionType << "T" << eventMentionId;
    out << "\"T" << eventMentionId << "\", [";
  }
  if (eventRoleId.size()==0) {out << endl; return; }
//   out << eventRoleType[0] << ":T" << eventRoleId[0]; // first one without space before
//   for (unsigned int i(1), n=eventRoleId.size(); i<n; i++) {
  for (unsigned int i(0), n=eventRoleId.size(); i<n; i++) {
    out << "[\"" << eventRoleType[i] << "\", \"" << eventRoleId[i] << "\"";
  }
  out << "] ]" << endl;
}

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima