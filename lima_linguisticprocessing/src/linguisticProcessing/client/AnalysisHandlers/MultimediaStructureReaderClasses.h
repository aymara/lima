#ifndef MULTIMEDIASTRUCTUREREADERCLASSES_H
#define MULTIMEDIASTRUCTUREREADERCLASSES_H

#include "AnalysisHandlersExport.h"

#include "common/Handler/structureHandler.h"
#include "common/Handler/contentHandler.h"


#include "linguisticProcessing/common/BagOfWords/bowText.h"
#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "common/Data/genericDocumentProperties.h"

namespace Lima {
namespace Handler {


class LIMA_ANALYSISHANDLERS_EXPORT MultimediaXMLStructureReader:
            public Lima::StructureHandler,
            public Lima::ContentHandler<Common::Misc::GenericDocumentProperties>,
            public Lima::ContentHandler< std::vector<float> >,
            public Lima::ContentHandler< Common::BagOfWords::BoWText >
{
public:

  MultimediaXMLStructureReader();

  virtual ~MultimediaXMLStructureReader();

  MultimediaXMLStructureReader(const MultimediaXMLStructureReader&) = delete;
  MultimediaXMLStructureReader& operator=(const MultimediaXMLStructureReader&) = delete;

  void processBoWText(const Common::BagOfWords::BoWText* text, int cId);

  void processSBinNum(const std::vector<float>& vect, const std::string& SID, int CID);

  void openSNode(const Common::Misc::GenericDocumentProperties* /*unused properties*/,
                 const std::string& elementName );

  void openSIndexingNode(const Common::Misc::GenericDocumentProperties* /*unused properties*/,
                         const std::string& elementName );

  void closeSNode();

  void processSContent( const Common::Misc::GenericDocumentProperties* properties );

  void closeSContent();

  void writeMultimediaFeature(const std::vector<float>& vect);

  void  writeVectorFloat(const std::vector<float>& vect);

  void closePropertiesNode(const Common::Misc::GenericDocumentProperties* prop);

  void addContentNode(const Lima::Node& node);

  uint64_t lastNode() const;
  void lastNode(uint64_t n);

private:
  uint64_t m_lastNode;
  std::vector<uint64_t> m_openedNodes;
  std::map<std::string,std::string> m_metadata;
  std::map<int, std::map<int, std::map< uint64_t , Lima::Node > > > m_mapNodes;

};

class LIMA_ANALYSISHANDLERS_EXPORT MultimediaBinaryStructureReader:
    public Common::BagOfWords::BoWBinaryReader
{
 public:
  MultimediaBinaryStructureReader();
  virtual ~MultimediaBinaryStructureReader();
  MultimediaBinaryStructureReader(const MultimediaBinaryStructureReader&) = delete;
  MultimediaBinaryStructureReader& operator=(const MultimediaBinaryStructureReader&) = delete;

  void readMultimediaDocumentBlock(std::istream& file,
                                   Common::BagOfWords::BoWDocument& document,
                                   MultimediaXMLStructureReader* handler,
                                   bool useIterator);

  void readBinNum(std::istream& file,
                  uint64_t& cid,
                  std::string& path,
                  std::string& name,
                  std::vector<float>& vect);

  private:
};

}
}

#endif
