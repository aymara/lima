/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                              *
 *                                                                         *
 ***************************************************************************/
#include "XmlIEDumper.h"
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

SimpleFactory<MediaProcessUnit,XmlIEDumper> bvDumperFactory(XmlIEDUMPER_CLASSID);

XmlIEDumper::XmlIEDumper() : AbstractIEDumper()
{}

XmlIEDumper::~XmlIEDumper()
{}

void XmlIEDumper::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  AbstractIEDumper::init(unitConfiguration,manager);
  // other specific parameters
  //...
}

void XmlIEDumper::outputGlobalHeader(std::ostream& out, const std::string& sourceFile) const
{
  out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  out << "<Doc>" << endl;
  if (!sourceFile.empty())
  {
    out << "<sourceFile>" << sourceFile << "</sourceFile>" << endl;
  }
}
void XmlIEDumper::outputGlobalFooter(std::ostream& out) const
{
  out << "</Doc>" << endl;
}
void XmlIEDumper::outputEntitiesHeader(std::ostream& out) const
{
  out << "<specific_entities>" << endl;
}
void XmlIEDumper::outputEntitiesFooter(std::ostream& out) const
{
  out << "</specific_entities>" << endl;
}
void XmlIEDumper::outputRelationsHeader(std::ostream& out) const
{
  out << "<relations>" << endl;
}
void XmlIEDumper::outputRelationsFooter(std::ostream& out) const
{
  out << "</relations>" << endl;
}
void XmlIEDumper::outputEventsHeader(std::ostream& out) const
{
  out << "<events>" << endl;
}
void XmlIEDumper::outputEventsFooter(std::ostream& out) const
{
  out << "</events>" << endl;
}

void XmlIEDumper::
outputEntityString(std::ostream& out,
                   unsigned int entityId,
                   const std::string& entityType,
                   const std::string& entityString,
                   const std::vector<std::pair<uint64_t,uint64_t> >& positions,
                   const Automaton::EntityFeatures& entityFeatures) const
{
    out << "<entity>"
        << "<id>" << "T" << entityId  << "</id>"
        << "<type>" <<  entityType << "</type>";
    // do not need all detailed positions (around carriage returns): get only global extent
    if (positions.size()==0) {
      LOGINIT("LP::AnalysisDumpers");
      LERROR << "XmlIEDumper: missing position for entity" << entityString;
    }
    else {
      out << "<posD>"  << positions.front().first << "</posD>";
      out << "<posF>" << positions.back().second << "</posF>";
    }
    // replace multiple spaces with only one space : from http://stackoverflow.com/questions/8362094/replace-multiple-spaces-with-one-space-in-a-string
    string str(entityString);
    std::string::iterator new_end =
        std::unique(str.begin(), str.end(), [=](char lhs, char rhs){ return (lhs == rhs) && (lhs == ' '); } );
    str.erase(new_end, str.end());
    out << "<string>"
        << Common::Misc::transcodeToXmlEntities(QString::fromStdString(str)).toStdString()
        << "</string>";
    out << "<norm>";

    for (Automaton::EntityFeatures::const_iterator
           featureItr=entityFeatures.begin(),features_end=entityFeatures.end();
         featureItr!=features_end; featureItr++)
    {
      out << "<" << featureItr->getName() << ">"
          << Common::Misc::transcodeToXmlEntities(QString::fromStdString(featureItr->getValueString())).toStdString()
          << "</" << featureItr->getName() << ">";
    }
    out << "</norm>";
    out << "</entity>\n";
}

void XmlIEDumper::
outputEntityNorm([[maybe_unused]] std::ostream& out,
                   [[maybe_unused]] unsigned int entityId,
                   [[maybe_unused]] const std::string& entityNorm) const
{
}

void XmlIEDumper::
outputAttributesString(std::ostream& out,
                        unsigned int entityId,
                        std::map <std::tuple <std::size_t, std::string , std::string >,  std::size_t >& mapAttributes) const
{
}

unsigned int XmlIEDumper::
outputRelationString(std::ostream& out,
                     unsigned int relationId,
                     const std::string& relationType,
                     const std::string& sourceArgString,
                     const std::string& targetArgString) const
{
  out << "<relation><id>R" << relationId << "</id><type>" << relationType <<"</type>";
  out << "<source>T" << sourceArgString << "</source>";
  out << "<target>T" << targetArgString << "</target>";
  out << "</relation>" << endl;
  return 1;
}

void XmlIEDumper::
outputEventString(std::ostream& out,
                  unsigned int eventId,
                  unsigned int eventMentionId,
                  const std::string& eventMentionType,
                  const std::vector<unsigned int>& eventRoleId,
                  const std::vector<std::string>& eventRoleType) const
{
  out << "<event><id>E" << eventId << "</id>" << endl
      << "<eventElements>" << endl;

  out <<"<element><role>"<< eventMentionType <<"</role><id>T" << eventMentionId << "</id></element>" << endl;

  for (unsigned int i(0), n=eventRoleId.size(); i<n; i++) {
    out <<"<element><role>"<< eventRoleType[i] <<"</role><id>T" << eventRoleId[i] << "</id></element>" << endl;
  }
  out << "</eventElements>" << endl
      << "</event>" << endl;
}

} // AnalysisDumpers
} // LinguisticProcessing
} // Lima
