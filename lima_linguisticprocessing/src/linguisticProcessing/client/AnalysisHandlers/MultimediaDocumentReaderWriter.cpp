#include "MultimediaDocumentReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "linguisticProcessing/common/BagOfWords/bowTerm.h"
#include "linguisticProcessing/common/BagOfWords/bowNamedEntity.h"
#include "linguisticProcessing/common/BagOfWords/BoWRelation.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"
#include <sstream>

using namespace Lima;
using namespace Common;
using namespace std;
// using namespace Common::XMLConfigurationFiles;
using namespace Common::MediaticData;
using namespace Common::BagOfWords;
using namespace Lima::Handler;

namespace Lima {
namespace Handler {

class MultimediaXMLWriterPrivate
{
  friend class MultimediaXMLWriter;

  MultimediaXMLWriterPrivate(std::ostream& os): m_outputStream(os){};

  void writeMultimediaFeature(std::vector<float> vect);
  void  writeVectorFloat(std::vector<float> vect);

  std::ostream& m_outputStream;
  std::string m_spaces;         /**< for pretty indentations */

  // for indentations
  void incIndent() { m_spaces+="  "; }
  void decIndent() { m_spaces.erase(m_spaces.size()-2); }
  void setSpaces(const std::string& s){m_spaces=s;}
};
}
}

MultimediaXMLWriter::MultimediaXMLWriter(std::ostream& os):
  BoWXMLWriter(os), m_dmult(new MultimediaXMLWriterPrivate(os))
{

}

MultimediaXMLWriter::~MultimediaXMLWriter()
{
  delete m_dmult;
}

void MultimediaXMLWriter::writeMultimediaDocumentsHeader()
{
  m_dmult->m_outputStream << "<?xml-stylesheet type=\"text/xsl\" href=\"bow.xslt\"?>" << std::endl;
  m_dmult->m_outputStream <<"<MultimediaDocuments>" << std::endl;
  incIndent();
}

void MultimediaXMLWriter::writeMultimediaDocumentsFooter() {
  decIndent();
  m_dmult->m_outputStream <<"</MultimediaDocuments>" << std::endl;
}

void MultimediaXMLWriter::setSpaces(const std::string& s)
{
  Common::BagOfWords::BoWXMLWriter::setSpaces(s);
  m_dmult->setSpaces(s);
}

void MultimediaXMLWriter::incIndent()
{
  m_dmult->incIndent();
  Common::BagOfWords::BoWXMLWriter::incIndent();
}

void MultimediaXMLWriter::decIndent()
{
  m_dmult->decIndent();
  Common::BagOfWords::BoWXMLWriter::decIndent();
}

void MultimediaXMLWriter::openSNode(
    const Common::Misc::GenericDocumentProperties* /*unused properties*/,
    const std::string& elementName )
{
  m_dmult->m_outputStream << m_dmult->m_spaces << "<node elementName=\""
      << elementName << "\">" << std::endl;
  incIndent();
}

void MultimediaXMLWriter::openSIndexingNode(
    const Common::Misc::GenericDocumentProperties* /*unused properties*/,
    const std::string& elementName )
{
  m_dmult->m_outputStream << m_dmult->m_spaces << "<node elementName=\""
      << elementName << "\" indexingNode=\"yes\">" << std::endl;
  incIndent();
}

void MultimediaXMLWriter::closeSNode()
{
  decIndent();
  m_dmult->m_outputStream << m_dmult->m_spaces << "</node>" << std::endl;
}

//***********************************************************************
// reader
MultimediaBinaryReader::MultimediaBinaryReader():
    Common::BagOfWords::BoWBinaryReader()
{
  m_content_id=1;
}

MultimediaBinaryReader::~MultimediaBinaryReader()
{
}


void MultimediaBinaryReader::readBinNum(std::istream& file,
                                        unsigned int& cid,
                                        string& path,
                                        string& name,
                                        std::vector<float>& vect)
{
  unsigned int size;
  unsigned int contentId;
  file.read(( char* ) &contentId,sizeof ( unsigned int ));
  cid=contentId;
  file.read(( char* ) &size,sizeof (unsigned int ));

  char* cstr=new char[size+1]; // + 1 for '\0' (not in file)
  file.read((char *) cstr,size*sizeof(char));
  cstr[size]='\0'; // add the end of string character
  name=string(cstr);
  delete []cstr;

  file.read(( char* ) &size,sizeof (unsigned int));

  char* cstr2=new char[size+1]; // + 1 for '\0' (not in file)
  file.read((char *) cstr2,size*sizeof(char));
  cstr2[size]='\0'; // add the end of string character
  path=string(cstr2);
  delete []cstr2;

  file.read(( char* ) &size,sizeof ( unsigned int ));

  for (unsigned int i=0;i<size;i++)
  {
    float val;
    file.read(( char* ) &val,sizeof ( float ));
    vect.push_back(val);
  }
}

Common::BagOfWords::BoWBlocType MultimediaBinaryReader::readMultimediaDocumentBlock(
    std::istream& file,
    BoWDocument& document,
    MultimediaXMLWriter& handler,
    bool useIterator,
    bool useIndexIterator)
{
  BoWBlocType blocType = static_cast<BoWBlocType>(Misc::readOneByteInt(file));

#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultimediaBinaryReader::readBoWDocumentBlock: read blocType" << blocType;
#endif
  // new format
  switch ( blocType )
  {
    case BoWBlocType::HIERARCHY_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "HIERARCHY_BLOC";
#endif
      std::string elementName;
      Misc::readStringField(file,elementName);
      handler.openSNode(&document, elementName);
//   m_content_id++;
      break;
    }
    case BoWBlocType::INDEXING_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "INDEXING_BLOC";
#endif
      std::string elementName;
      Misc::readStringField(file,elementName);
      handler.openSIndexingNode(&document, elementName);
//   m_content_id++;
      break;
    }
    case BoWBlocType::BIN_NUM_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "BIN_NUM_BLOC";
#endif
      document.clear();
      unsigned int cid;string name,path;std::vector<float> vect;

      readBinNum(file,cid,path,name,vect);
      document.setStringValue("type","numerical_feature");
      document.setIntValue("ContentId",m_content_id);
      document.setStringValue("PathImage",path);
      document.setStringValue("name",name);
      handler.processSContent(&document);
      handler.processSBinNum(vect,useIterator);
      handler.processProperties(&document, useIterator, useIndexIterator);
      handler.closeSContent();
      m_content_id++;
      break;
    }
    case BoWBlocType::BOW_TEXT_BLOC:
    {
  #ifdef DEBUG_LP
      LDEBUG << "BOW_TEXT_BLOC";
  #endif
      document.clear();
      readBoWText(file,document);
      document.setStringValue("type","tokens");
      document.setIntValue("ContentId",m_content_id);
      handler.processSContent(&document);
      handler.processSBoWText(&document, useIterator, useIndexIterator);
      handler.processProperties(&document, useIterator, useIndexIterator);
      handler.closeSContent();
      m_content_id++;
      break;
    }
    case BoWBlocType::NODE_PROPERTIES_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "NODE_PROPERTIES_BLOC";
#endif
      document.Misc::GenericDocumentProperties::read(file);
      handler.processProperties(&document, useIterator, useIndexIterator);
      break;
    }
    case BoWBlocType::END_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "END_BLOC";
#endif
      handler.closeSNode();
      break;
    }
    case BoWBlocType::DOCUMENT_PROPERTIES_BLOC:
    { // do nothing ?
#ifdef DEBUG_LP
      LDEBUG << "DOCUMENT_PROPERTIES_BLOC";
#endif
      break;
    }
    case BoWBlocType::ST_BLOC:
    { // do nothing ?
#ifdef DEBUG_LP
      LDEBUG << "ST_BLOC";
#endif
      break;
    }
    default:;
  }

  file.peek(); // Try to read next byte to force update end-of-file flag on windows
  if (file.eof())
  {
#ifdef DEBUG_LP
    BOWLOGINIT;
    LDEBUG << "MultimediaBinaryReader::readMultimediaDocumentBlock EOF reached but last blockType was" << blocType << getBlocTypeString(blocType);
#endif
    if(blocType!= BoWBlocType::END_BLOC){
      BOWLOGINIT;
      LERROR << "MultimediaBinaryReader::readMultimediaDocumentBlock EOF reached but last blockType was not END_BLOC : " << blocType << getBlocTypeString(blocType);
    }
  }
  return blocType;
}

void MultimediaXMLWriter::processSContent( const Common::Misc::GenericDocumentProperties* properties )
{
  m_dmult->m_outputStream << m_dmult->m_spaces << "<content type=\""
      << properties->getStringValue("type").first << "\">" << std::endl;
  incIndent();
}

void MultimediaXMLWriter::closeSContent()
{
  decIndent();
  m_dmult->m_outputStream <<m_dmult->m_spaces <<"</content>" << std::endl;
}


void MultimediaXMLWriter::processSBinNum(std::vector<float> vect, bool useIterator)
{
  LIMA_UNUSED(useIterator);
  m_dmult->writeMultimediaFeature(vect);
}

void  MultimediaXMLWriterPrivate::writeVectorFloat(std::vector<float> vect)
{
  unsigned int size = vect.size();
  m_outputStream << m_spaces << "<vector type=\"float\" n=\""
      << size << "\">" << std::endl;
  m_outputStream<<m_spaces;
  for (unsigned int i=0;i<size-1;i++)
  {
    m_outputStream<<vect.at(i)<<" ";
  }
  m_outputStream<<vect.at(size-1)<<std::endl;
  m_outputStream<<m_spaces<<"</vector>"<<std::endl ;
}
void  MultimediaXMLWriterPrivate::writeMultimediaFeature(std::vector<float> vect)
{
  incIndent();
  m_outputStream<<m_spaces<< "<feature>"<<std::endl;
  incIndent();
  writeVectorFloat(vect);
  decIndent();
  m_outputStream<<m_spaces <<"</feature>"<<std::endl;
  decIndent();
}
