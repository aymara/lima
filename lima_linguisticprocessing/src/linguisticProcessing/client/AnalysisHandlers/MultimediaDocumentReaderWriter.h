#include "AnalysisHandlersExport.h"

#include "linguisticProcessing/common/BagOfWords/bowBinaryReaderWriter.h"
#include "linguisticProcessing/common/BagOfWords/bowXMLWriter.h"

namespace Lima {
namespace Handler {

class MultimediaXMLWriterPrivate;
class LIMA_ANALYSISHANDLERS_EXPORT MultimediaXMLWriter: public Lima::Common::BagOfWords::BoWXMLWriter
{
public:
  MultimediaXMLWriter(std::ostream& os);
  virtual ~MultimediaXMLWriter();

  void writeMultimediaDocumentsHeader();

  void writeMultimediaDocumentsFooter();

  void processSBinNum(std::vector<float> vect,bool useIterator);

  void openSNode( const Lima::Common::Misc::GenericDocumentProperties* /*unused properties*/,const std::string& elementName ) override;

  void openSIndexingNode(const Lima::Common::Misc::GenericDocumentProperties* /*unused properties*/,const std::string& elementName ) override;

  void closeSNode() override;

  void processSContent( const Lima::Common::Misc::GenericDocumentProperties* properties ) override;

  void closeSContent() override;

  void setSpaces(const std::string& s);

  void incIndent();
  void decIndent();

private:
  MultimediaXMLWriterPrivate* m_dmult;
};

class LIMA_ANALYSISHANDLERS_EXPORT MultimediaBinaryReader:public  Lima::Common::BagOfWords::BoWBinaryReader
{
 public:
  MultimediaBinaryReader();
  ~MultimediaBinaryReader();

  void readMultimediaDocumentBlock(std::istream& file, Lima::Common::BagOfWords::BoWDocument& document, Lima::Handler::MultimediaXMLWriter& handler, bool useIterator, bool useIndexIterator);

  void readBinNum(std::istream& file,unsigned int& cid,std::string& path,std::string& name,std::vector<float>& vect);

  private:

    unsigned int m_content_id;
};


class LIMA_ANALYSISHANDLERS_EXPORT MultimediaBinaryWriter:public Lima::Common::BagOfWords::BoWBinaryWriter
{
 public:
  MultimediaBinaryWriter(const QMap< uint64_t, uint64_t >& shiftFrom = QMap< uint64_t, uint64_t >()): Lima::Common::BagOfWords::BoWBinaryWriter(shiftFrom) {};
  ~MultimediaBinaryWriter(){};


 private:

};



}
}
