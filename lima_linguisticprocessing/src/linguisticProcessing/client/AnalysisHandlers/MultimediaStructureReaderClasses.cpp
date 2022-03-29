#include "MultimediaStructureReaderClasses.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"
#include <sstream>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Handler;

using namespace std;

MultimediaXMLStructureReader::MultimediaXMLStructureReader():
    StructureHandler(),
    ContentHandler<Common::Misc::GenericDocumentProperties>(),
    ContentHandler< std::vector<float> >(),
    ContentHandler< Common::BagOfWords::BoWText >(),
    m_lastNode(0),
    m_openedNodes(),
    m_metadata(),
    m_mapNodes()
{
  StructureHandler::set_LastContentId(1);
}

MultimediaXMLStructureReader::~MultimediaXMLStructureReader()
{
  m_openedNodes.clear();
  m_mapNodes.clear();
  ContentHandler< BoWText >::resetHandler();
  ContentHandler< std::vector<float> >::resetHandler();
  ContentHandler<Misc::GenericDocumentProperties>::resetHandler();
  StructureHandler::resetStructure();
}

void MultimediaXMLStructureReader::openSNode(
    const Misc::GenericDocumentProperties* /*unused properties*/,
    const std::string& elementName )
{
  LIMA_UNUSED(elementName);
  m_lastNode++;
  m_openedNodes.push_back(m_lastNode);
}

void MultimediaXMLStructureReader::openSIndexingNode(
    const Misc::GenericDocumentProperties* /*unused properties*/,
    const std::string& elementName )
{
  LIMA_UNUSED(elementName);
  m_lastNode++;
  m_openedNodes.push_back(m_lastNode);
}

void MultimediaXMLStructureReader::closeSNode()
{
}
//***********************************************************************
// reader
MultimediaBinaryStructureReader::MultimediaBinaryStructureReader() : BoWBinaryReader()
{
}

MultimediaBinaryStructureReader::~MultimediaBinaryStructureReader()
{
}

void MultimediaBinaryStructureReader::readBinNum(istream& file,
                                                 uint64_t& cid,
                                                 string& path,
                                                 string& name,
                                                 std::vector< float >& vect)
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

void MultimediaXMLStructureReader::closePropertiesNode(const Misc::GenericDocumentProperties* prop)
{

#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultimediaXMLStructureReader::closePropertiesNode()";
#endif
  int NodeId = -1;
  int StructId = -1;
  int offBegPrpty = -1, offEndPrpty = -1;
  string encodPrpty, langPrpty, srcePrpty;
  string indexDatePrpty;
  // get values of properties whose type is int
  auto beginEnd = prop->getIntProperties();
  for(auto elmtIt = beginEnd.first; elmtIt != beginEnd.second; elmtIt++ )
  {
    const auto& elmt = *elmtIt;
    if (elmt.first == "StructureId")
      StructId = elmt.second ;
    else if (elmt.first == "offBegPrpty")
      offBegPrpty = elmt.second ;
    else if (elmt.first == "offEndPrpty")
      offEndPrpty = elmt.second ;
    else if (elmt.first == "NodeId")
      NodeId = elmt.second ;
  }
  // get values of properties whose type is string
  auto beginEndS = prop->getStringProperties();
  for(auto elmtIt = beginEndS.first; elmtIt != beginEndS.second; elmtIt++)
  {
    const auto& elmt = *elmtIt;
    if (elmt.first == "encodPrpty")
        encodPrpty=elmt.second ;
    else if (elmt.first == "langPrpty")
        langPrpty=elmt.second ;
    else if (elmt.first == "srcePrpty")
        srcePrpty=elmt.second ;
  }
  // get values of properties whose type is date
  auto beginEndD = prop->getDateProperties();
  for(auto elmtIt = beginEndD.first; elmtIt != beginEndD.second; elmtIt++)
  {
    const auto& elmt = *elmtIt;
    if (elmt.first == "indexDatePrpty")
        indexDatePrpty = elmt.second.toString(Qt::ISODate).toUtf8().data();
  }

  // openedNodes.pop_back();

  const std::map<uint64_t,Node> Nodes=m_mapNodes[StructId][NodeId];
  // cout<<"ici   "<<StructId<<"       "<<NodeId<<endl;
  if (!m_openedNodes.empty())
    m_openedNodes.pop_back();
  else
    m_lastNode=0;

  for (auto ItrNodes = Nodes.cbegin(); ItrNodes!=Nodes.cend(); ItrNodes++)
  {
    const Node& laNode = ItrNodes->second;
    const auto& uri = laNode.uri();
    if (uri.length()!=0)
      srcePrpty = uri;

    // TODO: replace MULTIMEDIA_DOCUMENT_READER0 with consistent value
    Node node("MULTIMEDIA_DOCUMENT_READER0", // docName
              StructId,                      // structId
              NodeId,                        // nodeId
              laNode.get_ContentId(),        // contentId
              laNode.indexId(),              // indexid
              srcePrpty,                     // uri
              "",                            // nodeMedia
              "",                            // nodeType
              laNode.descrId(),              // descrId
              offBegPrpty,                   // nodeStart
              offEndPrpty,                   // nodeEnd
              offEndPrpty-offBegPrpty,       // nodeLength
              m_lastNode                     // nodeParent
             );
    // cout<<"la "<<StructId<<"insertion dans la structure "<<node<<endl;
    StructureHandler::add_Node(node);
  }
  if (!m_openedNodes.empty())
  {
    m_openedNodes.pop_back();
  }
}

void MultimediaXMLStructureReader::addContentNode(const Node& noeud)
{
  Node node = noeud;
  node.set_NodeId(m_lastNode);
//   cout<<"ici ---0--- "<<m_lastNode<<"   "<<m_mapNodes.size() <<"    "<<node.get_StructId()<<"    Node "<<node<<endl;

  m_mapNodes[node.get_StructId()][m_lastNode][noeud.get_ContentId()] = node;

//   cout<<"insertion du noeid "<<node<<"     "<<node.get_StructId()<<"    "<<m_lastNode<<"   "<<noeud.get_ContentId()<<endl;
}

uint64_t MultimediaXMLStructureReader::lastNode() const
{
  return m_lastNode;
}

void MultimediaXMLStructureReader::lastNode(uint64_t n)
{
  m_lastNode = n;
}

void MultimediaBinaryStructureReader::readMultimediaDocumentBlock(
    std::istream& file,
    BoWDocument& document,
    MultimediaXMLStructureReader* handler,
    bool useIterator)
{
  LIMA_UNUSED(useIterator);
  BoWBlocType blocType = static_cast<BoWBlocType>(Misc::readOneByteInt(file));
#ifdef DEBUG_LP
  BOWLOGINIT;
  LDEBUG << "MultimediaBinaryReaderStructureReader::readBoWDocumentBlock: read blocType"
          << (int)blocType;
#endif
  // new format
  switch ( blocType )
  {
    case HIERARCHY_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "HIERARCHY_BLOC";
#endif

      std::string elementName;
      Misc::readStringField(file, elementName);
// 	cout<<"ici ----1--- "<<handler->StructureHandler::get_LastStructureId()<<endl;
      handler->StructureHandler::set_LastStructureId(handler->StructureHandler::get_LastStructureId()+1);
// 	cout<<"ici ----1--- "<<handler->StructureHandler::get_LastStructureId()<<endl;
      handler->lastNode(0);

      Node node("MULTIMEDIA_DOCUMENT_READER0",                    // docName
                handler->StructureHandler::get_LastStructureId(), // structId
                0,                                                // nodeId
                0,                                                // contentId
                0,                                                // indexid
                "",                                               // uri
                "xml",                                            // nodeMedia
                "",                                               // nodeType
                "hierarchy",                                      // descrId
                0,                                                // nodeStart
                0,                                                // nodeEnd
                0,                                                // nodeLength
                0                                                 // nodeParent
               );
      handler->openSNode(&document, elementName);
      node.set_NodeId(handler->lastNode());
      handler->addContentNode(node);

//         m_content_id++;
      break;
    }
    case INDEXING_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "INDEXING_BLOC";
#endif
      std::string elementName;
      Misc::readStringField(file, elementName);
      handler->openSIndexingNode(&document, elementName);
      break;
    }
    case BIN_NUM_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "BIN_NUM_BLOC";
#endif
      document.clear();
      uint64_t cid;
      string name,path;
      std::vector<float> vect;

      readBinNum(file, cid, path, name,vect);
      document.setStringValue("type", "numerical_feature");
      document.setIntValue("ContentId",
                           handler->StructureHandler::get_LastContentId());
      document.setStringValue("PathImage", path);
      document.setStringValue("name", name);
      handler->processSContent(&document);
      //TODO: trouver un contentId
      handler->processSBinNum(vect, name,
                              handler->StructureHandler::get_LastContentId());

      Node node("MULTIMEDIA_DOCUMENT_READER2",                    // docName
                handler->StructureHandler::get_LastStructureId(), // structId
                0,                                                // nodeId
                handler->StructureHandler::get_LastContentId(),   // contentId
                2,                                                // indexid
                path,                                             // uri
                "ima",                                            // nodeMedia
                "",                                               // nodeType
                name,                                             // descrId
                0,                                                // nodeStart
                0,                                                // nodeEnd
                0,                                                // nodeLength
                0                                                 // nodeParent
               );
      handler->addContentNode(node);
      handler->StructureHandler::set_LastContentId(handler->StructureHandler::get_LastContentId()+1);
      break;
    }
    case BOW_TEXT_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "BOW_TEXT_BLOC";
#endif
      document.clear();
      readBoWText(file, document);
      document.setStringValue("type", "tokens");
      document.setIntValue("ContentId",
                           handler->StructureHandler::get_LastContentId());
      handler->processSContent(&document);
      //TODO: donner un nouveau contentId
      handler->processBoWText(&document,
                              handler->StructureHandler::get_LastContentId());

      Node node("MULTIMEDIA_DOCUMENT_READER3",                    // docName
                handler->StructureHandler::get_LastStructureId(), // structId
                0,                                                // nodeId
                handler->StructureHandler::get_LastContentId(),   // contentId
                1,                                                // indexid
                "",                                               // uri
                "text",                                           // nodeMedia
                "",                                               // nodeType
                "bow",                                            // descrId
                0,                                                // nodeStart
                0,                                                // nodeEnd
                0,                                                // nodeLength
                0                                                 // nodeParent
               );
      handler->addContentNode(node);
      handler->StructureHandler::set_LastContentId(handler->StructureHandler::get_LastContentId()+1);
      break;
    }
    case NODE_PROPERTIES_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "NODE_PROPERTIES_BLOC";
#endif
      document.Misc::GenericDocumentProperties::read(file);
      handler->closePropertiesNode(&document);//TODO:ici important
      handler->ContentHandler <Common::Misc::GenericDocumentProperties >::addContent(handler->lastNode(),
                                                                                     document);
      break;
    }
    case END_BLOC:
    {
#ifdef DEBUG_LP
      LDEBUG << "END_BLOC";
#endif
      handler->closeSNode();
      break;
    }
    case DOCUMENT_PROPERTIES_BLOC:
    { // do nothing ?
#ifdef DEBUG_LP
      LDEBUG << "DOCUMENT_PROPERTIES_BLOC";
#endif
      break;
    }
    case ST_BLOC:
    { // do nothing ?
#ifdef DEBUG_LP
      LDEBUG << "ST_BLOC";
#endif
      break;
    }
    default:;
  }
}

void MultimediaXMLStructureReader::processSContent(
    const Misc::GenericDocumentProperties *properties)
{
  LIMA_UNUSED(properties);
}

void MultimediaXMLStructureReader::closeSContent()
{
}


void MultimediaXMLStructureReader::
processBoWText(const BoWText* text,int cId)
{
//   cout<<"ajout d'un contenu linguistique "<<cId<<" "<<*text<<endl;
  ContentHandler<BoWText>::addContent(cId, *text);
}

void MultimediaXMLStructureReader::processSBinNum(
    const std::vector< float >& vect,
    const string& SID,
    int CID)
{
  LIMA_UNUSED(SID);
//   m_dmult->writeMultimediaFeature(vect);
  ContentHandler<std::vector <float > >::addContent ( CID , vect );
}

