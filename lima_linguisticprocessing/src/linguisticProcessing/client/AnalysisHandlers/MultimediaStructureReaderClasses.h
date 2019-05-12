#ifndef MULTIMEDIASTRUCTUREREADERCLASSES_H
#define MULTIMEDIASTRUCTUREREADERCLASSES_H

#include "multimediadocumenthandler_export.h"

#include "common/Handler/structureHandler.h"
#include "common/Handler/contentHandler.h"


#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/genericDocumentProperties.h"

namespace Lima {
namespace Handler {
 

class MULTIMEDIADOCUMENTHANDLER_EXPORT MultimediaXMLStructureReader:
            public Lima::StructureHandler,
            public Lima::ContentHandler<Lima::Common::Misc::GenericDocumentProperties>,
            public Lima::ContentHandler< std::vector<float> >,
            public Lima::ContentHandler< Lima::Common::BagOfWords::BoWText >
{
public:
  
  MultimediaXMLStructureReader();
 
  virtual ~MultimediaXMLStructureReader();

  void processBoWText(const Lima::Common::BagOfWords::BoWText* text,int cId);

  void processSBinNum(const std::vector<float>& vect, const std::string& SID, int CID);

  void openSNode( const Lima::Common::Misc::GenericDocumentProperties* /*unused properties*/,const std::string& elementName );

  void openSIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* /*unused properties*/,const std::string& elementName );

  void closeSNode();

  void processSContent( const Lima::Common::Misc::GenericDocumentProperties* properties );

  void closeSContent();

  void writeMultimediaFeature(const std::vector<float>& vect);

  void  writeVectorFloat(const std::vector<float>& vect);

  void closePropertiesNode(const Lima::Common::Misc::GenericDocumentProperties* prop);

  void addContentNode(const Lima::Node& node);

  uint64_t lastNode() const;
  void lastNode(uint64_t n);

private:
  uint64_t m_lastNode;
  std::vector<uint64_t> m_openedNodes;
  std::map<std::string,std::string> m_metadata;
  std::map<int, std::map<int, std::map< uint64_t , Lima::Node > > > m_mapNodes;

};

class MULTIMEDIADOCUMENTHANDLER_EXPORT MultimediaBinaryStructureReader: public Lima::Common::BagOfWords::BoWBinaryReader
{
 public:
  MultimediaBinaryStructureReader(); 
  virtual ~MultimediaBinaryStructureReader();

  void readMultimediaDocumentBlock(std::istream& file,
                     Lima::Common::BagOfWords::BoWDocument& document,
                     MultimediaXMLStructureReader* handler,
                     bool useIterator);
  
  void readBinNum(std::istream& file,uint64_t& cid,std::string& path,std::string& name,std::vector<float>& vect);

  private:
};

}
}

#endif
