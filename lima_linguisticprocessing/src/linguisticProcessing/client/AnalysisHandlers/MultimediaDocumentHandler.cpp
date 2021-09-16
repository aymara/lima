#include "MultimediaDocumentHandler.h"
#include "MultimediaDocumentReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"

#include <sstream>

using namespace Lima;
using namespace Lima::Common;
using namespace Lima::Common::XMLConfigurationFiles;
// using namespace Lima::Common::MediaticData;
using namespace Lima::Common::BagOfWords;
using namespace Lima::Handler;
using namespace std;

MultimediaDocumentHandler::MultimediaDocumentHandler(const QMap< uint64_t,uint64_t >& shiftFrom) :
    AbstractXmlDocumentHandler(shiftFrom), m_out(nullptr)
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "MultimediaDocumentHandler::MultimediaDocumentHandler" << shiftFrom.size();
#endif
  set_LastStructureId ( 0 );
  set_lastNodeId ( 1 );
}

void MultimediaDocumentHandler::setOut(std::ostream* out)
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "MultimediaDocumentHandler::setOut " << out << shiftFrom().size();
#endif
  m_out = out;
  MultimediaBinaryWriter writer(shiftFrom());
//   BoWBinaryWriter writer(shiftFrom());
  writer.writeHeader(*m_out,BOWFILE_SDOCUMENT);
}

void MultimediaDocumentHandler::handle ( const char* buf,int length )
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LTRACE << "MultimediaDocumentHandler::handle writing on" << m_out << QString::fromUtf8(buf,length);
#endif
//    std::cerr << "MultimediaDocumentHandler::handle" << std::endl;
  m_out->write(buf,length);
}

void MultimediaDocumentHandler::startAnalysis()
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "MultimediaDocumentHandler::startAnalysis()" << m_out;
#endif
  Misc::writeOneByteInt(*m_out,Common::BagOfWords::BOW_TEXT_BLOC);
}

void MultimediaDocumentHandler::startAnalysis(const std::string& bloc_type)
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "MultimediaDocumentHandler::startAnalysis(bloc_type) write BLOC " << bloc_type;
#endif

  if (bloc_type.compare("lpFactory")==0)
    Misc::writeOneByteInt(*m_out,Common::BagOfWords::BOW_TEXT_BLOC);
  else if(bloc_type.compare("ImaFactory")==0)
    Misc::writeOneByteInt(*m_out,Common::BagOfWords::BIN_NUM_BLOC);
  else if(bloc_type.compare("VideoFactory")==0)
    Misc::writeOneByteInt(*m_out,Lima::Handler::BIN_VID_BLOC);
  else
    Misc::writeOneByteInt(*m_out,Common::BagOfWords::BOW_TEXT_BLOC);
}

//! @brief called by the document analyzer for a new hierarchy node
// TODO devrait s'appeler startHierarchyNode ?
void MultimediaDocumentHandler::startNode ( const std::string& elementName, bool forIndexing )
{
  NODE_ID nodeId = get_lastNodeId() ;
  set_lastNodeId ( nodeId + 1);
  if (!m_openedNodes.empty())
  {
    //
    set_parentlastOpenedNode ( *m_openedNodes.rbegin() );
  }
  m_openedNodes.push_back ( nodeId );
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "MultimediaDocumentHandler::startNode" << this << elementName << nodeId << m_openedNodes.size();
#endif

  if ( ( get_parentlastOpenedNode() ==1 ) && ( nodeId==1 ) )
      set_parentlastOpenedNode ( 0 );

  if (forIndexing)
    set_LastContentId(get_LastContentId()+1);

  Node newNode("MULTIMEDIA_DOCUMENT_HANDLER0",  // docName
               get_LastStructureId(),           // structId
               nodeId,                          // nodeId
               -1,                              // contentId
               0,                               // indexid
               get_lastUri(),                   // uri
               "xml",                           // nodeMedia
               elementName,                     // nodeType
               "none",                          // descrId
               0,                               // nodeStart
               0,                               // nodeEnd
               0,                               // nodeLength
               get_parentlastOpenedNode()       // nodeParent
              );
  newNode.indexId(0);
//   cout<<" ajout noeud "<<newNode<<endl;
  StructureHandler::add_Node ( newNode );

  if( forIndexing )
  {
#ifdef DEBUG_LP
    LDEBUG << "MultimediaDocumentHandler::startNode write INDEXING_BLOC";
#endif
    Misc::writeOneByteInt(*m_out,Common::BagOfWords::INDEXING_BLOC);
  }
  else
  {
#ifdef DEBUG_LP
    LDEBUG << "MultimediaDocumentHandler::startNode write HIERARCHY_BLOC";
#endif
    Misc::writeOneByteInt(*m_out,Common::BagOfWords::HIERARCHY_BLOC);
  }

#ifdef DEBUG_LP
  LDEBUG << "MultimediaDocumentHandler::startNode write element name: " << elementName;
#endif
  Lima::Common::Misc::writeStringField(*m_out, elementName);

//cerr >>     std::cerr << "MultimediaDocumentHandler::startNode ici" << elementName << std::endl;
}

//! @brief called by the document analyzer at the end of a hierarchy node
// TODO devrait s'appeler endHierarchyNode ?
void MultimediaDocumentHandler::endNode ( const Common::Misc::GenericDocumentProperties& props )
{
  NODE_ID Nid=props.getIntValue ( "NodeId" ).first;
  HANDLERLOGINIT;
  LDEBUG << "MultimediaDocumentHandler::endNode" << this << Nid << m_openedNodes.size();
  CONTENT_ID Cid=props.getIntValue ( "ContentId" ).first;
  //ajouter les propriétés
//   set_LastContentId(get_LastContentId()+1);
//   CONTENT_ID contentId=get_LastContentId();

  Common::Misc::GenericDocumentProperties proposcopy=props;
  ContentHandler<Common::Misc::GenericDocumentProperties>::addContent(Nid,proposcopy);

//   Node newNodeProps(Sid,Nid,contentId,0,get_lastUri(),"xml","props","none",1,0,0,1,0,0,0,get_parentlastOpenedNode() );
//   newNodeProps.m_indexId=0;
//
//   add_Node ( newNodeProps );

//   string docName= props.getStringValue ( "identPrpty" ).first;
  Node* node = StructureHandler::get_Node ( Cid );
  if (node != 0)
  {
    node->nodeStart(props.getIntValue ( "offBegPrpty" ).first);
    node->nodeEnd(props.getIntValue ( "offEndPrpty" ).first);
    node->nodeLength(node->nodeEnd()-node->nodeStart());
  }
  if (!m_openedNodes.empty())
  {
    m_openedNodes.pop_back();
  }
  // TODO Check why the else bellow happens. It could be a bug.
//   else
//   {
//     LERROR << "MultimediaDocumentHandler::endNode m_openedNodes should not be empty for mode id" << Nid;
//     Q_ASSERT(!m_openedNodes.empty());
//   }

  m_lastOpenedNode=m_openedNodes.end();
  m_lastOpenedNode--;

#ifdef DEBUG_LP
  LDEBUG << "MultimediaDocumentHandler::endNode write NODE_PROPERTIES_BLOC on" << m_out;
#endif
  Misc::writeOneByteInt(*m_out,Common::BagOfWords::NODE_PROPERTIES_BLOC);
  props.write(*m_out);
#ifdef DEBUG_LP
  LDEBUG << "MultimediaDocumentHandler::endNode write END_BLOC on" << m_out;
#endif
  Misc::writeOneByteInt(*m_out,Common::BagOfWords::END_BLOC);
  m_out->flush();
}
