// includes from project
#include "Test.h"
#include "linguisticProcessing/core/TensorflowSpecificEntities/nerUtils.h"

// includes system
#include <QtTest/QTest>
#include <QRegularExpression>
#include <QStringList>
#include <memory>

// declaration of using namespace

QTEST_MAIN(Test);

void Test::initTestCase()
{
  // Called before the first testfunction is executed
}

void Test::cleanupTestCase()
{
  // Called after the last testfunction was executed
}

void Test::init()
{
  // Called before each testfunction is executed
}

void Test::cleanup()
{
  // Called after every testfunction
}

//Unit testing

void Test::loadTags()
{
  //Test with a file that doesn't exist
  QVERIFY_EXCEPTION_THROWN(loadFileTags("test.txt"),BadFileException);

  //Test if the file has been loaded successfully into the map
  auto vocabChars=loadFileTags("data/IOB1/eng/tags.txt");
  QVERIFY2(static_cast<int>(vocabChars.size())==8,
           "The file has not been loaded successfully into the map.");
}

void Test::loadChars()
{
  //Test with a file that doesn't exist
  QVERIFY_EXCEPTION_THROWN(loadFileChars("test.txt"),BadFileException);

  //Test if the file has been loaded successfully into the map
  auto vocabChars=loadFileChars("data/IOB1/eng/chars.txt");
  QVERIFY2(static_cast<int>(vocabChars.size())==84,
           "The file has not been loaded successfully into the map.");
}

void Test::loadWords()
{
  //Test with a file that doesn't exist
  QVERIFY_EXCEPTION_THROWN(loadFileWords("test.txt"),BadFileException);

  //Test if the file has been loaded successfully into the map
  auto vocabChars=loadFileWords("data/IOB1/eng/words.txt");
  QVERIFY2(static_cast<int>(vocabChars.size())==22217,
           "The file has not been loaded into the map.");
}

void Test::loadText()
{
  QTextStream qtIn;
  QFile inputFile;

  //Test with a file that doesn't exist
  QVERIFY_EXCEPTION_THROWN(loadTextToEvaluate(qtIn,inputFile,"test.txt"),
                           BadFileException);

  //Test if the file has been opened successfully
  QVERIFY2(loadTextToEvaluate(qtIn,inputFile,
                              "data/coNLL/eng.test")==NERStatusCode::SUCCESS,
           "The file has not been opened.");
}

void Test::testProcessingWords()
{
  //Test with an empty vocabulary
  QVERIFY_EXCEPTION_THROWN(
    getProcessingWord("hello",std::map<QString,int>(),
                      std::map<QChar,int>(),true,true),
    std::logic_error
  );

  auto vocabWords= loadFileWords("data/IOB1/eng/words.txt");
  auto vocabChars= loadFileChars("data/IOB1/eng/chars.txt");

  //Test with an empty string
  QVERIFY_EXCEPTION_THROWN(
    getProcessingWord("",vocabWords,vocabChars,true,true),
    std::logic_error
  );

  //Test with an unknown word and allowUnk set to false
  QVERIFY_EXCEPTION_THROWN(getProcessingWord("hello",vocabWords,
                                             vocabChars,true,false),
                           UnknownWordClassException);

  //Test with a unknown word (not belonging to the vocabulary) and allowUnk
  //set to true

  QString str="hello";
  auto textConverted = getProcessingWord(str,vocabWords,vocabChars,true,true);
  QVERIFY2(!std::get<0>(textConverted).empty(),
           "The word has not been converted.");
  //Checks that each character is linked to the good identifier
  QVERIFY(std::get<0>(textConverted).size()==str.size());
  for(auto i=0;i<std::get<0>(textConverted).size();++i)
  {
    QVERIFY(std::get<0>(textConverted)[i]==vocabChars[str[i]]);
  }
  //Checks that the word is linked to the good identifier
  QVERIFY(std::get<1>(textConverted)==vocabWords["$UNK$"]);

  //Test with a known word
  str="structured";
  textConverted = getProcessingWord(str,vocabWords,vocabChars,true,true);
  QVERIFY2(!std::get<0>(textConverted).empty(),
           "The word has not been converted.");
  //Checks that each character is linked to the good identifier
  //warning : some characters may not be associated to an identifier
  QVERIFY(std::get<0>(textConverted).size()==str.size());
  for(auto i=0;i<std::get<0>(textConverted).size();++i)
  {
    QVERIFY(std::get<0>(textConverted)[i]==vocabChars[str[i]]);
  }
  //Checks that the word is linked to the good identifier
  QVERIFY(std::get<1>(textConverted)==vocabWords["structured"]);

  //Test that the identifier of the word is the same with a lowercase word
  //and uppercase word when lowercase parameter is set to true
  //warning : chars are not set to lowercase
  str="Structured";
  auto textConvertedUpperCase = getProcessingWord(str,vocabWords,
                                                  vocabChars,true,true);
  QVERIFY(std::get<0>(textConvertedUpperCase).size()==str.size());
  QVERIFY(std::get<1>(textConverted)==std::get<1>(textConvertedUpperCase));

  //Test that if lowercase is set to false, word is assimilated to an unknown
  //word because the net has learned with lowercase words
  //warning : chars are not set to lowercase
  textConvertedUpperCase = getProcessingWord(str,vocabWords,
                                             vocabChars,false,true);
  QVERIFY(std::get<0>(textConvertedUpperCase).size()==str.size());
  for(auto i=0;i<std::get<0>(textConvertedUpperCase).size();++i)
  {
    QVERIFY(std::get<0>(textConvertedUpperCase)[i]==vocabChars[str[i]]);
  }
  QVERIFY(std::get<1>(textConvertedUpperCase)==vocabWords["$UNK$"]);
}

void Test::testGetFeedDict()
{
  //Test with empty lists charIds & wordIds
  std::vector<std::pair<std::string,tensorflow::Tensor>> inputs(5);
  std::vector<std::vector<std::vector<int>>> charIds;
  std::vector<std::vector<int>> wordIds;
  tensorflow::Tensor wordIdsT;
  tensorflow::Tensor sequenceLengthsT;
  tensorflow::Tensor wordLengthsT;
  tensorflow::Tensor charIdsT;
  tensorflow::Tensor dropoutT;
  QVERIFY(getFeedDict(inputs,charIds,wordIds,wordIdsT,sequenceLengthsT,
                      charIdsT,wordLengthsT,
                      dropoutT,0,0,0)==NERStatusCode::MISSING_DATA);

  //Test with tensors whose shape is not precised
  auto vocabWords= loadFileWords("data/IOB1/eng/words.txt");
  auto vocabChars= loadFileChars("data/IOB1/eng/chars.txt");
  auto textConverted = getProcessingWord("hello",vocabWords,
                                         vocabChars,true,true);
  charIds.resize(1);
  charIds[0].resize(1);
  charIds[0][0].reserve(std::get<0>(textConverted).size());
  charIds[0].push_back(std::get<0>(textConverted));
  wordIds.resize(1);
  wordIds[0].reserve(1);
  wordIds[0].push_back(std::get<1>(textConverted));
  QVERIFY(getFeedDict(inputs,charIds,wordIds,wordIdsT,sequenceLengthsT,
                      charIdsT,wordLengthsT,dropoutT,1,1,
                      std::get<0>(textConverted).size())==NERStatusCode::MISSING_DATA);
}

void Test::testViterbiDecodeWithoutInputs()
{
  //Test with empty parameters
  Eigen::MatrixXi test=viterbiDecode(Eigen::MatrixXf(),Eigen::MatrixXf());
  QVERIFY(test.size()==0);
}

//Test the main function

void Test::testPredictBatch1()
{
//   Initialize a tensorflow session
  tensorflow::Session* session = nullptr;
  std::shared_ptr<tensorflow::Status> status(new tensorflow::Status(NewSession(tensorflow::SessionOptions(),
                                                       &session)));
  QVERIFY(status->ok());

//   Read in the protobuf graph we have exported
  tensorflow::GraphDef graphDef;
  *status = ReadBinaryProto(tensorflow::Env::Default(),
                            "results/eng_IOB1/output_graph.pb",
                            &graphDef);
  QVERIFY(status->ok());

//   Add the graph to the session
  *status = session->Create(graphDef);
  QVERIFY(status->ok());

//   Test with empty lists charIds & wordIds & sentencesByBatch & size of the
//    batch
  auto vocabWords= loadFileWords("data/IOB1/eng/words.txt");
  auto vocabChars= loadFileChars("data/IOB1/eng/chars.txt");
  auto vocabTags= loadFileTags("data/IOB1/eng/tags.txt");
  std::vector<std::vector<std::vector<int>>> charIds;
  std::vector<std::vector< int >> wordIds;
  std::vector<QStringList> sentencesByBatch;
  std::vector<Eigen::MatrixXi> result;
  QVERIFY(predictBatch(status, session, 0,
                       charIds, wordIds, result)==NERStatusCode::MISSING_DATA);

//   Test with empty session and status
  sentencesByBatch.resize(1);
  sentencesByBatch[0]=QStringList("hello");
  auto textConverted = getProcessingWord(sentencesByBatch[0][0],
                                         vocabWords,vocabChars,true,true);
  charIds.resize(1);
  charIds[0].resize(1);
  charIds[0][0].reserve(std::get<0>(textConverted).size());
  charIds[0].push_back(std::get<0>(textConverted));
  wordIds.resize(1);
  wordIds[0].reserve(1);
  wordIds[0].push_back(std::get<1>(textConverted));
  result.resize(1);
  QVERIFY(predictBatch(nullptr,nullptr,1,
                       charIds,wordIds,result)==NERStatusCode::MISSING_DATA);

//   Test with one word no entity
//   Here the size of the batch is equal to 1.

  QVERIFY(predictBatch(status, session, 1,
                       charIds, wordIds, result)==NERStatusCode::SUCCESS);
  QVERIFY(vocabTags[result[0](0)]=="O");
  QVERIFY(session->Close().ok());
  delete session;
}

// void Test::testPredictBatch2()
// {
// //   Initialize a tensorflow session
//
//   tensorflow::Session* session;
//   std::shared_ptr<tensorflow::Status> status(new tensorflow::Status(NewSession(tensorflow::SessionOptions(),
//                                                        &session)));
//   QVERIFY(status->ok());
//
// //   Read in the protobuf graph we have exported
//
//   tensorflow::GraphDef graphDef;
//   *status = ReadBinaryProto(tensorflow::Env::Default(),
//                             "results/eng_IOB1/output_graph.pb", &graphDef);
//   QVERIFY(status->ok());
//
// //   Add the graph to the session
//
//   *status = session->Create(graphDef);
//   QVERIFY(status->ok());
//
// //   Load vocabulary
//
//   auto vocabWords= loadFileWords("data/IOB1/eng/words.txt");
//   auto vocabChars= loadFileChars("data/IOB1/eng/chars.txt");
//   auto vocabTags= loadFileTags("data/IOB1/eng/tags.txt");
//
//   //Here the size of the batch is equal to 1.
//
//   std::vector<QStringList> sentencesByBatch(1);
//   sentencesByBatch[0]=QStringList({"I","love","Paris"});
//
//   // Transform words into ids and split all the characters and identify them
//
//   std::vector<std::pair<std::vector<int>,int>> textConverted(sentencesByBatch[0].size());
//   for(auto it=0;it<sentencesByBatch[0].size();++it)
//   {
//     textConverted[it]=getProcessingWord(sentencesByBatch[0][it],
//                                         vocabWords, vocabChars, true, true);
//   }
//
//   // Gather ids of words and ids of sequences of characters according to the order of words
//
//   std::vector<std::vector< int >> wordIds(1);
//   wordIds[0].resize(sentencesByBatch[0].size());
//   std::vector<std::vector<std::vector<int>>> charIds(1);
//   charIds[0].resize(sentencesByBatch[0].size());
//   for(std::size_t i=0;i<textConverted.size();++i)
//   {
//     charIds[0][i].resize(textConverted[i].first.size());
//     charIds[0][i]=textConverted[i].first;
//     wordIds[0][i]=textConverted[i].second;
//   }
//   std::vector<Eigen::MatrixXi> result(1);
//
//   //Run the session with the predictBatch function
//
//   QVERIFY(predictBatch(status, session, 1, charIds,
//                        wordIds, result)==NERStatusCode::SUCCESS);
//
//   //Check that the results are correct
//   QVERIFY(vocabTags[result[0](0)]=="O");
//   QVERIFY(vocabTags[result[0](1)]=="O");
//   QVERIFY(vocabTags[result[0](2)]=="I-LOC");
//   QVERIFY(session->Close().ok());
//   delete session;
// }
//
// void Test::testPredictBatch3()
// {
// //   Initialize a tensorflow session
//   tensorflow::Session* session;
//   std::shared_ptr<tensorflow::Status> status(new tensorflow::Status(NewSession(tensorflow::SessionOptions(),
//                                                        &session)));
//   QVERIFY(status->ok());
//
// //   Read in the protobuf graph we have exported
//   tensorflow::GraphDef graphDef;
//   *status = ReadBinaryProto(tensorflow::Env::Default(),
//                             "results/eng_IOB1/output_graph.pb", &graphDef);
//   QVERIFY(status->ok());
//
// //   Add the graph to the session
//   *status = session->Create(graphDef);
//   QVERIFY(status->ok());
//
// //   Load vocabulary
//   auto vocabWords= loadFileWords("data/IOB1/eng/words.txt");
//   auto vocabChars= loadFileChars("data/IOB1/eng/chars.txt");
//   auto vocabTags= loadFileTags("data/IOB1/eng/tags.txt");
//
//   int batchSize =2;
//
//   std::vector<std::vector<std::pair<std::vector<int>,int>>> textConverted(batchSize);
//   std::vector<std::vector<int>> wordIds(batchSize);
//   std::vector<std::vector<std::vector<int>>> charIds(batchSize);
//   std::vector<QStringList> sentencesByBatch;
//   sentencesByBatch.reserve(batchSize);
//
// //   Analyze one sentence with entities
//   QString sentence("Japan began the defence of their Asian Cup title with a "
//                    "lucky 2-1 win against Syria in a Group C championship "
//                    "match on Friday .");
//   sentencesByBatch.push_back(sentence.split(" "));
//
// //   Analyze one sentence without entities
//   sentence="He added that, \" overhauling the fragile state of our computing "
//            "education \" would require an ambitious, multipronged approach.";
//   sentencesByBatch.push_back(sentence.split(" "));
//
//   // Transform words into ids and split all the characters and identify them
//   for(auto k=0;k<batchSize;++k)
//   {
//     textConverted[k].reserve(sentencesByBatch[k].size());
//     for(auto it=sentencesByBatch[k].cbegin();
//         it!=sentencesByBatch[k].cend();++it)
//     {
//       textConverted[k].push_back(getProcessingWord(*it, vocabWords,
//                                                    vocabChars, true, true));
//     }
//     // Gather ids of words and ids of sequences of characters according to
//     // the order of words
//     wordIds[k].resize(sentencesByBatch[k].size());
//     charIds[k].resize(sentencesByBatch[k].size());
//     for(std::size_t i=0;i<textConverted[k].size();++i)
//     {
//       charIds[k][i].resize(textConverted[k][i].first.size());
//       charIds[k][i]=textConverted[k][i].first;
//       wordIds[k][i]=textConverted[k][i].second;
//     }
//   }
//
//   //Run the session with the predictBatch function
//   std::vector<Eigen::MatrixXi> result(2);
//   QVERIFY(predictBatch(status, session, batchSize, charIds,
//                        wordIds, result)==NERStatusCode::SUCCESS);
//
//   // Analyze the first sentence's result
//   std::vector<QString> lstEntities(result[0].rows());
//   unsigned int nbEntities=0;
//   for(auto i=0;i<result[0].rows();++i)
//   {
//     if(vocabTags[result[0](i)]!="O")
//     {
//       lstEntities[nbEntities]=vocabTags[result[0](i)];
//       ++nbEntities;
//     }
//     std::cout <<sentencesByBatch[0][i].toStdString()<<" "
//         <<vocabTags[result[0](i)].toStdString()<<"\n";
//   }
//   // Check the number of entities and the sequence : six entities have to be
//   // found
//   QVERIFY(nbEntities==6);
//   std::vector<QString> lstCorrectEntities={"I-LOC","I-MISC","I-MISC",
//                                             "I-LOC","I-MISC","I-MISC"};
//   for(std::size_t i=0;i<lstCorrectEntities.size();++i)
//   {
//     QVERIFY(lstCorrectEntities[i]==lstEntities[i]);
//   }
//
//   // Analyze the second sentence's result : any entities have to be found
//   for(auto i=0;i<result[1].rows();++i)
//   {
//     QVERIFY(vocabTags[result[1](i)]=="O");
//   }
//
//   QVERIFY(session->Close().ok());
// }
