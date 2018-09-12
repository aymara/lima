// includes from project
#include "nerUtils.h"

// includes system
#include <boost/program_options.hpp>
#include <QDir>
#include <chrono>
#include <QRegularExpression>
#include <QStringList>
#include <memory>
#include <qt5/QtCore/QTextStream>

namespace po = boost::program_options;

// declaration of using namespace
using namespace tensorflow;

int main(int argc, char* argv[]) {
  
  std::string language,strResourcesPath,fileGraph,textToEvaluate,outputFileWithTags,outputFileWithoutTags;
  po::options_description desc("Usage");
  desc.add_options()
  ("help,h", "Display this help message")
  ("language,l,lang", po::value< std::string>(&language),"Specify the language between french as fr and english as eng")
  ("resources-dir", po::value<std::string>(&strResourcesPath),"Vocabularies' directory to use")
  ("output-graph", po::value<std::string>(&fileGraph),"Graph for the model"  )
  ("output-file-without-tags", po::value< std::string>(&outputFileWithoutTags),"File in which the result will be saved with the format : Token Id")
  ("output-file-with-tags", po::value< std::string>(&outputFileWithTags),"File in which the result will be saved with the format : Token Tag")
  ("input-file", po::value< std::string>(&textToEvaluate),"Raw text to evaluate");
  
  po::positional_options_description p;
  p.add("input-file", -1);
  po::variables_map vm;
  
  try 
  {
  //   po::store(po::parse_command_line(ac, av, desc), vm);
    po::store(po::command_line_parser(argc, argv).
      options(desc).positional(p).run(), vm);
    po::notify(vm);
  }
  catch (const boost::program_options::unknown_option& e) 
  {
    std::cerr << e.what() << std::endl;
    return 1;
  }
  
  if (vm.count("help")) {
      std::cout << desc << "\n";
      return 0;
  }
  if (!vm.count("language")) {
    std::cerr << "language level was not set.\n";
    return 1;
  } 
  if (!vm.count("output-graph")) {
    std::cerr << "output-graph level was not set. Needed to run the graph.\n";
    return 1;
  } 
  if (!vm.count("resources-dir")) {
    std::cerr << "resources-dir level was not set. Needed to format data.\n";
    return 1;
  } 
  if (!vm.count("input-file")) {
    std::cerr << "input-file level was not set. Nothing to analyse.\n";
    return 1;
  } 
  
  std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

  // Initialize a tensorflow session
  Session* session = nullptr;
  std::shared_ptr<Status> status(new Status(NewSession(SessionOptions(), &session)));
  if (!status->ok()) {
    std::cerr << status->ToString() << "\n";
    return 1;
  }
  
  // Read in the protobuf graph we have exported
  GraphDef graphDef;
  *status = ReadBinaryProto(Env::Default(),fileGraph, &graphDef);
  if (!status->ok()) {
    std::cerr << status->ToString() << "\n";
    return 1;
  }

  // Add the graph to the session
  *status = session->Create(graphDef);
  if (!status->ok()) {
    std::cerr << status->ToString() << "\n";                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                      
    return 1;
  }
  
  //Calculate the time for loading the graph
  std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
  auto d1 = std::chrono::duration_cast<std::chrono::milliseconds>( t2 - t1 ).count();

  //Load vocabulary 
  std::map<QString,int> vocabWords;
  std::map<QChar,int> vocabChars;
  std::map<unsigned int,QString> vocabTags;
  
  std::string fileWords=strResourcesPath+"words.txt";
  std::string fileChars=strResourcesPath+"chars.txt";
  std::string fileTags=strResourcesPath+"tags.txt";
  try{
    vocabWords= loadFileWords(fileWords.c_str());
    if(vocabWords.empty()){
      return 1;
    }    
    vocabChars= loadFileChars(fileChars.c_str());
    if(vocabChars.empty()){
      return 1;
    }    
    vocabTags = loadFileTags(fileTags.c_str());
    if(vocabTags.empty()){
    return 1;
  }    
  }
  catch(const BadFileException& e){
    std::cerr<<e.what()<<std::endl;
    return 1;
  }
  
  QTextStream qtIn;
  QFile inputFile;
  
  try{
    if(loadTextToEvaluate(qtIn,inputFile,textToEvaluate)==NERStatusCode::EMPTY_FILE){
      return 1;
    }
  }
  catch(const BadFileException& e){
    std::cerr<<e.what()<<std::endl;
    return 1;
  }
  
  /*Initialization of the output file 
  The result will be formated the same as for the dataset original format :
  - one line represents a token and its entity
  - two sentences are seperated by a vertical space*/
  std::shared_ptr<QTextStream> out;
  std::shared_ptr<QTextStream> out2;
  QFile outputWithoutTags,outputWithTags;
  if(!outputFileWithoutTags.empty())
  {
    outputWithoutTags.setFileName(outputFileWithoutTags.c_str()); 
    outputWithoutTags.open(QIODevice::WriteOnly | QIODevice::Truncate);
    out.reset(new QTextStream(&outputWithoutTags));
  }

  if(!outputFileWithTags.empty())
  {
    outputWithTags.setFileName(outputFileWithTags.c_str()); 
    outputWithTags.open(QIODevice::WriteOnly | QIODevice::Truncate);
    out2.reset(new QTextStream(&outputWithTags));
  }
  
  QString line; 
  
  //Minibatching (group of max 20 sentences of different size) is used in order to amortize the cost of loading the network weights from CPU/GPU memory across many inputs.
  //and to take advantage from parallelism.
  
  int batchSizeMax = 20;
  
  std::chrono::high_resolution_clock::time_point t3 = std::chrono::high_resolution_clock::now();
  std::vector<Eigen::MatrixXi> result;
  
  while (!qtIn.atEnd()) { 
    
    std::vector<std::vector<std::pair<std::vector<int>,int>>> textConverted(batchSizeMax);
    std::vector<std::vector<int>> wordIds(batchSizeMax);
    std::vector<std::vector<std::vector<int>>> charIds(batchSizeMax);
    std::vector<QStringList> sentencesByBatch;
    sentencesByBatch.reserve(batchSizeMax);
    int batchSize =0;
    
    while(batchSize<batchSizeMax && !qtIn.atEnd()){
      //Iterate over the text until batchSizeMax is reached or end of file
      
      //1. Transform sentence to a list of words
      qtIn.readLineInto(&line);
      if(line.isEmpty()){
        std::cerr<<"Text is empty.\n";
        return 1;
      }
      line= line.simplified(); //remove whitespaces, equivalent to strip() method
      QStringList wordsRaw = line.split(" ");
      if(wordsRaw.isEmpty()){
        std::cerr<<"The sentence was not split.\n";
        return 1;
      }
      sentencesByBatch.push_back(wordsRaw);
      
      //2. Transform words into ids and split all the characters and identify them
      textConverted[batchSize].reserve(wordsRaw.size());
    
      for(auto it=wordsRaw.cbegin();it!=wordsRaw.cend();++it){
        try{
          textConverted[batchSize].push_back(getProcessingWord(*it, vocabWords, vocabChars, true, true));
          if(!std::get<1>(textConverted[batchSize].back())){
            return 1;
          }
        }
        catch(const UnknownWordClassException& e){
          std::cerr<<e.what()<<std::endl;
        }
      }
    
      //3. Gather ids of words and ids of sequences of characters according to the order of words

      wordIds[batchSize].resize(wordsRaw.size());
      charIds[batchSize].resize(wordsRaw.size());
      for(auto i=0;i<textConverted[batchSize].size();++i)
      {
        charIds[batchSize][i].resize(textConverted[batchSize][i].first.size());
        charIds[batchSize][i]=textConverted[batchSize][i].first;
        wordIds[batchSize][i]=textConverted[batchSize][i].second;
      }
      ++batchSize;  
    }
    
    //4.Resize data if current batch size is fewer than batchSizeMax
    
    if(batchSize<batchSizeMax){
      wordIds.resize(batchSize);
      charIds.resize(batchSize);
      sentencesByBatch.resize(batchSize);
    }   
    
    //5. Predict tags
    result.resize(result.size()+batchSize);
    if(predictBatch(status, session, batchSize, charIds, wordIds, result)==NERStatusCode::MISSING_DATA){
      return 1;
    }
    
    if(out)
    {
      for(auto k=0;k<batchSize;++k){
        for(auto i=0;i<result[result.size()-batchSize+k].size();++i){
          (*out) <<sentencesByBatch[k][i]<<" "<<result[result.size()-batchSize+k](i)<<"\n";
        }
        (*out)<<"\n";
      }
    }
    
    if(out2)
    {
      for(auto k=0;k<batchSize;++k){
        for(auto i=0;i<result[result.size()-batchSize+k].size();++i){
          (*out2) <<sentencesByBatch[k][i]<<" "<<vocabTags[result[result.size()-batchSize+k](i)]<<"\n";
        }
        (*out2)<<"\n";
      }
    }
    
    //Continue to the next batch 
  }
  
  //6. Free any resources used by the session
  *status=session->Close();

  if (!status->ok()) {
    std::cerr << status->ToString() << "\n";
    return 1;
  }

  delete session;
  
  //Calculate executated time for applying the model
  std::chrono::high_resolution_clock::time_point t4 = std::chrono::high_resolution_clock::now();
  auto d2 = std::chrono::duration_cast<std::chrono::milliseconds>( t4 - t3 ).count();
  std::cout<<"Execution time: \n- Loading model: "<<d1<<" ms\n- Apply model on text: "<<d2<<" ms\n";
  return 0;
  
}
