/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "BratDumper.h"
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
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::SpecificEntities;
using namespace Lima::LinguisticProcessing::EventAnalysis;
using namespace Lima::LinguisticProcessing::SemanticAnalysis;
using namespace boost;

namespace Lima {
namespace LinguisticProcessing {
namespace AnalysisDumpers {

SimpleFactory<MediaProcessUnit,BratDumper> bratDumperFactory(BRATDUMPER_CLASSID);

BratDumper::BratDumper() :
AbstractIEDumper(),
m_useNormalizedForms(false)
{
  DUMPERLOGINIT;
  LDEBUG << "BratDumper::BratDumper()";
}


BratDumper::~BratDumper()
{}

void BratDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  DUMPERLOGINIT;
  LDEBUG << "BratDumper::init";
  AbstractIEDumper::init(unitConfiguration,manager);
  // here, specific parameters for this dumper if different from AbstractIEDumper ones
  try
  {
    string str=unitConfiguration.getParamsValueAtKey("useNorms");
    if (str=="yes" || str=="1" || str=="true") {
      m_useNormalizedForms=true;
    }
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& )  { } // optional keep default

}

void BratDumper::
outputEntityString(ostream& out,
                   unsigned int entityId,
                   const std::string& entityType,
                   const std::string& entityString,
                   const vector<pair<uint64_t,uint64_t> >& positions,
                   const Automaton::EntityFeatures& entityFeatures, bool noNorm) const
{
  out << "T" << entityId << "\t" << entityType << " ";
  if (positions.size()==0) {
    // use simple positions
    DUMPERLOGINIT;
    LERROR << "BratDumper: no positions given for entity" << entityString;
    out << 0 << " " << 0;
  }
  else {
    vector<pair<uint64_t,uint64_t> >::const_iterator posIt=positions.begin();
    out << (*posIt).first << " " << (*posIt).second;
    for (posIt++; posIt!=positions.end(); posIt++) {
      out << ";" << (*posIt).first << " " << (*posIt).second;
    }
  }
  out << "\t" << entityString << "\n";
  if (m_useNormalizedForms && !noNorm) {
    bool done(false);
    for (Automaton::EntityFeatures::const_iterator
      featureItr=entityFeatures.begin(),features_end=entityFeatures.end();
    featureItr!=features_end; featureItr++)
    {
      if (featureItr->getName()=="value") {
        string valuestr=featureItr->getValueString();
        // if only whitespaces, do not print it (error in brat format)
        if (valuestr.find_first_not_of(' ') != std::string::npos) {
          out << "N" << entityId << "\tReference T" << entityId << " LIMA:1\t" << valuestr << "\n";
          done=true;
          break;
        }
      }
    }
    if (! done) { // backoff to surface form if norm is not found
        out << "N" << entityId << "\tReference T" << entityId << " LIMA:1\t" << entityString << "\n";
    }
  }
}

void BratDumper::
outputAttributesString(std::ostream& out,
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
      out << "A" << attributeId << "\t" << featName << " T" << entityId;
      if(featValue!=""){
        out << " " << featValue;
      }
      out << "\n";
    }
  }
}

void BratDumper::
outputEntityNorm(ostream& out,
                   unsigned int entityId,
                   const std::string& entityNorm) const
{
  out << "N" << entityId << "\tReference T" << entityId << " X:Y\t"
      << entityNorm << "\n";
}

unsigned int BratDumper::
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
      LERROR << "BratDumper::outputRelationString Ignoring relation with empty type:"
              << relationId << relationType << sourceArgString
              << targetArgString;
      continue;
    }
    if (sourceArgString.empty())
    {
      DUMPERLOGINIT;
      LERROR << "BratDumper::outputRelationString Ignoring relation with empty source:"
              << relationId << relationType << sourceArgString
              << targetArgString;
      continue;
    }
    if (targetArgString.empty())
    {
      DUMPERLOGINIT;
      LERROR << "BratDumper::outputRelationString Ignoring relation with empty target:"
              << relationId << relationType << sourceArgString
              << targetArgString;
      continue;
    }
    // filter group name if present
    string relstr=rel.toUtf8().constData();
    if (relstr.find(".")!=string::npos) {
      relstr=relstr.substr(relstr.find("."));
    }
    out << "R" << (relationId+i) << "\t" << relstr  << " "
        << "Arg1:T" << sourceArgString
        << " Arg2:T" << targetArgString << endl;
    i++;
  }
  return i;
}

void BratDumper::
outputEventString(ostream& out,
                  unsigned int eventId,
                  unsigned int eventMentionId,
                  const std::string& eventMentionType,
                  const std::vector<unsigned int>& eventRoleId,
                  const std::vector<std::string>& eventRoleType) const
{
  out << "E" << eventId << "\t";
  // mention type is now one of the roles
  if (eventMentionId!=0) {
    out << eventMentionType << ":T" << eventMentionId;
    for (unsigned int i(0), n=eventRoleId.size(); i<n; i++) {
      out << " " << eventRoleType[i] << ":T" << eventRoleId[i];
    }
  }
  else {
    // first event role is the event mention: no space before
    if (eventRoleId.size()==0) {out << endl; return; }
    out << eventRoleType[0] << ":T" << eventRoleId[0]; 
    for (unsigned int i(1), n=eventRoleId.size(); i<n; i++) {
      out << " " << eventRoleType[i] << ":T" << eventRoleId[i];
    }
  }
  out << endl;
}

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima
