#include "XmlBowDocumentHandler.h"
#include "common/Data/DataTypes.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowDocument.h"
#include "common/Data/strwstrtools.h"
#include "common/Data/readwritetools.h"
#include "common/MediaticData/mediaticData.h"
#include "common/MediaProcessors/MediaProcessors.h"

#include <sstream>

using namespace Lima;
using namespace Lima;
using namespace Lima::Common;
using namespace std;
using namespace Lima::Common::XMLConfigurationFiles;

XmlBowDocumentHandler::XmlBowDocumentHandler(std::shared_ptr<const ShiftFrom> shiftFrom) : AbstractXmlDocumentHandler(shiftFrom)
{
    set_LastStructureId ( 0 );
    set_lastNodeId ( 1 );

};

void XmlBowDocumentHandler::setOut(std::ostream* out)
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "XmlBowDocumentHandler::setOut";
#endif
//    std::cerr << "XmlBowDocumentHandler::setOut" << std::endl;
  m_out = out;
  Common::BagOfWords::BoWBinaryWriter writer;
  writer.writeHeader(*m_out,Common::BagOfWords::BOWFILE_SDOCUMENT);
}

void XmlBowDocumentHandler::handle ( const char* buf,int length )
{
//    std::cerr << "XmlBowDocumentHandler::handle" << std::endl;
  m_out->write(buf,length);
}

void XmlBowDocumentHandler::startAnalysis()
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "XmlBowDocumentHandler::startAnalysis write BOW_TEXT_BLOC";
#endif
  Lima::Common::Misc::writeOneByteInt(*m_out,Common::BagOfWords::BOW_TEXT_BLOC);
}

//! @brief called by the document analyzer for a new hierarchy node
// TODO devrait s'appeler startHierarchyNode ?
void XmlBowDocumentHandler::startNode ( const string& elementName, bool forIndexing )
{
#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "xmlDocumentHandler::startNode " << elementName;
#endif
  NODE_ID nodeId = get_lastNodeId() + 1;
  set_lastNodeId ( nodeId );
  if (!m_openedNodes.empty())
  {
    set_parentlastOpenedNode ( *(m_openedNodes.end() - 1) );
  }
  m_openedNodes.push_back ( nodeId );

  if ( ( get_parentlastOpenedNode() ==1 ) && ( nodeId==1 ) )
      set_parentlastOpenedNode ( 0 );

  set_LastContentId(get_LastContentId()+1);
  CONTENT_ID contentId=get_LastContentId();

  Node newNode("XML_BOW_DOCUMENT_HANDLER0", // docName
               get_LastStructureId(),       // structId
               *m_openedNodes.rbegin(),     // nodeId
               contentId,                   // contentId
               0,                           // indexid
               get_lastUri(),               // uri
               "xml",                       // nodeMedia
               elementName,                 // nodeType
               "none",                      // descrId
               0,                           // nodeStart
               0,                           // nodeEnd
               0,                           // nodeLength
               get_parentlastOpenedNode()   // nodeParent
              );
  newNode.indexId(0);
  add_Node ( newNode );

  if( forIndexing )
  {
#ifdef DEBUG_LP
    LDEBUG << "XmlBowDocumentHandler::startNode write INDEXING_BLOC";
#endif
    Misc::writeOneByteInt(*m_out,Common::BagOfWords::INDEXING_BLOC);
  }
  else
  {
#ifdef DEBUG_LP
    LDEBUG << "XmlBowDocumentHandler::startNode write HIERARCHY_BLOC";
#endif
    Misc::writeOneByteInt(*m_out,Common::BagOfWords::HIERARCHY_BLOC);
  }

#ifdef DEBUG_LP
  LDEBUG << "XmlBowDocumentHandler::startNode write element name: " << elementName;
#endif
  Lima::Common::Misc::writeStringField(*m_out, elementName);

//     std::cerr << "XmlBowDocumentHandler::startNode ici" << elementName << std::endl;
}

//! @brief called by the document analyzer at the end of a hierarchy node
// TODO devrait s'appeler endHierarchyNode ?
void XmlBowDocumentHandler::endNode ( const Common::Misc::GenericDocumentProperties& props )
{
//    std::cerr << "XmlBowDocumentHandler::endNode" << std::endl;
  STRUCT_ID Sid=props.getIntValue ( "StructureId" ).first;
  NODE_ID Nid=props.getIntValue ( "NodeId" ).first;
  CONTENT_ID Cid=props.getIntValue ( "ContentId" ).first;
  //ajouter les propriétés
  set_LastContentId(get_LastContentId()+1);
  CONTENT_ID contentId=get_LastContentId();

  Common::Misc::GenericDocumentProperties proposcopy=props;
  ContentHandler<Common::Misc::GenericDocumentProperties>::addContent(Nid,proposcopy);

  Node newNodeProps("XML_BOW_DOCUMENT_HANDLER1",  // docName
                    Sid,                          // structId
                    Nid,                          // nodeId
                    contentId,                    // contentId
                    0,                            // indexid
                    get_lastUri(),                // uri
                    "xml",                        // nodeMedia
                    "props",                      // nodeType
                    "none",                       // descrId
                    0,                            // nodeStart
                    0,                            // nodeEnd
                    0,                            // nodeLength
                    get_parentlastOpenedNode()    // nodeParent
                   );
  newNodeProps.indexId(0);

  add_Node ( newNodeProps );

//   string docName= props.getStringValue ( "identPrpty" ).first;

  Node* node = get_Node ( Cid );
  node->nodeStart(props.getIntValue ( "offBegPrpty" ).first);
  node->nodeEnd(props.getIntValue ( "offEndPrpty" ).first);
  node->nodeLength(node->nodeEnd()-node->nodeStart());
  m_openedNodes.pop_back();

#ifdef DEBUG_LP
  HANDLERLOGINIT;
  LDEBUG << "XmlBowDocumentHandler::endNode write NODE_PROPERTIES_BLOC";
#endif
  Misc::writeOneByteInt(*m_out,Common::BagOfWords::NODE_PROPERTIES_BLOC);
  props.write(*m_out);
#ifdef DEBUG_LP
  LDEBUG << "XmlBowDocumentHandler::endNode write END_BLOC";
#endif
  Misc::writeOneByteInt(*m_out,Common::BagOfWords::END_BLOC);
  m_out->flush();
}
