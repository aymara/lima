#include "XmlDocumentParser.h"

#include "common/AbstractFactoryPattern/Lima::SimpleFactory.h"

#include <string>
#include <iostream>


using namespace Lima::Common::XMLConfigurationFiles;
using namespace  FrCeaLic2m;
using namespace std;


namespace Lima {
namespace XmlReader{


Lima::SimpleFactory<MediaProcessUnit,XmlDocumentParser> moduleFactory ( MODULE_CLASSID );

XmlDocumentParser::XmlDocumentParser()  {}

XmlDocumentParser::~XmlDocumentParser()
{           }

void XmlDocumentParser::init (
    Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
    Manager* manager )
throw ( InvalidConfiguration,std::exception )
{

}

LimaStatusCode XmlDocumentParser::process (
    AnalysisContent& analysis ) const
{
    
    cout << "*** XmlDocumentParser::process( AnalysisContent& analysis ) " << endl;

    return SUCCESS_ID;
}

} //namespace XmlDocumentParser
} // namespace Lima

