// includes from project
#include "nerUtils.h"

// includes system
#include <algorithm>

// declaration of using namespace
using namespace tensorflow;

LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::map<QString,int> loadFileWords(const QString& filepath)
{
  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    throw BadFileException("The file "+filepath.toStdString()+" doesn't exist.");
  }
  if(file.size()==0)
  {
    std::cout<<"The file is empty.";
    return std::map<QString,int>();
  }
  QTextStream in(&file);
  std::map<QString,int> d;
  int i=0;
  while (!in.atEnd())
  {
    QString line = in.readLine();
    line=line.simplified();
    d[line]=i;
    i++;
  }
  return d;
}

LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::map<QChar,int> loadFileChars(const QString& filepath)
{
  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    throw BadFileException("The file "+filepath.toStdString()+" doesn't exist.");
  }
  if(file.size()==0)
  {
    std::cout<<"The file is empty.";
    return std::map<QChar,int>();
  }
  QTextStream in(&file);
  std::map<QChar,int> d;
  int i=0;
  while (!in.atEnd())
  {
    QString line = in.readLine();
    line=line.simplified();
    d[line[0]]=i;
    i++;
  }
  return d;
}

LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::map<unsigned int,QString> loadFileTags(const QString& filepath)
{
  QFile file(filepath);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
    throw BadFileException("The file "+filepath.toStdString()+" doesn't exist.");
  }
  if(file.size()==0)
  {
    std::cout<<"The file is empty.";
    return std::map<unsigned int,QString>();
  }
  QTextStream in(&file);
  std::map<unsigned int,QString> d;
  unsigned int i=0;
  while (!in.atEnd())
  {
    QString line = in.readLine();
    line=line.simplified();
    d[i]=line;
    i++;
  }
  return d;
}

LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT NERStatusCode loadTextToEvaluate(
  QTextStream& qtIn,
  QFile& file,
  const std::string& filepath)
{
  file.setFileName(filepath.c_str());
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
  {
      throw BadFileException("The file "+filepath+" doesn't exist.");
  }
  if(file.size()==0)
  {
    std::cerr<<"The file is empty.";
    return NERStatusCode::EMPTY_FILE;
  }
  qtIn.setDevice(&file);
  return NERStatusCode::SUCCESS;
}


LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT std::pair<std::vector<int>,int> getProcessingWord(
  const QString& wordOriginal,
  const std::map<QString,int>& vocabWords,
  const std::map<QChar,int>& vocabChars,
  bool lowercase, bool allowUnk)
{
  if (!vocabWords.empty()  && !vocabChars.empty())
  {
    if(wordOriginal.isEmpty())
    {
      throw std::logic_error("There is no word to analyze.");
    }

    QString word=wordOriginal;
    int word_id;
    unsigned int chars_found=0;
    std::vector<int> chars_ids;
    chars_ids.reserve(word.size());

    //0. get chars' id of word from the vocabChars map

    for(auto it=word.cbegin();it!=word.cend();++it)
    {
      std::map<QChar,int>::const_iterator found= vocabChars.find(*it);
      //ignore chars out of vocabulary
      if (found !=vocabChars.cend())
      {
        chars_ids.push_back(found->second);
        ++chars_found;
      }
    }
    chars_ids.resize(chars_found);

    //1. preprocess word
    if (lowercase)
    {
      word=word.toLower();
    }
    QString::iterator it = word.begin();
    while(it != word.end() && (*it).isDigit())
    {
      ++it;
    }
    if(it==word.end())
    {
      word="$NUM$";
    }

    //2. get id of word from the vocabWords map
    std::map<QString,int>::const_iterator found= vocabWords.find(word);
    if(found!=vocabWords.cend())
    {
      word_id=found->second;
    }
    else
    {
      if(allowUnk)
      {
        word_id= vocabWords.find("$UNK$")->second;
      }
      else
      {
        throw UnknownWordClassException("Unknow word is not allowed. Check that your vocabulary is correct");
      }
    }

    //3. return a pair {charIds,word_id}
    return make_pair(chars_ids,word_id);
  }
  else
  {
    throw std::logic_error("The vocabulary of chars or words  doesn't exist.");
  }
}

LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT NERStatusCode predictBatch(
  const std::shared_ptr<tensorflow::Status>& status,
  tensorflow::Session* session,
  int batchSize,
  std::vector<std::vector< std::vector< int > >>& charIds,
  std::vector<std::vector< int >>& wordIds,
  std::vector<Eigen::MatrixXi>& result)
{
  if(wordIds.empty() || charIds.empty() || batchSize==0)
  {
    std::cerr<<"There are no sentences to analyze. Check that the vocabulary have been loaded.\n";
    return NERStatusCode::MISSING_DATA;
  }

  if(session==nullptr)
  {
    std::cerr<<"The session hasn't been initialized.\n";
    return NERStatusCode::MISSING_DATA;
  }

  if(status==nullptr)
  {
    std::cerr<<"The status hasn't been initialized.\n";
    return NERStatusCode::MISSING_DATA;
  }

  //1. Feed the Tensor

  //Calculate the size of the longest sentence from the current batch. Mandatory to perform padding.
  int maxSizeSentence=0;
  for(auto it=wordIds.begin();it!=wordIds.end();++it)
  {
    maxSizeSentence = std::max(static_cast<int>((*it).size()),maxSizeSentence);
  }
  //shape = (batch size, max length of sentence in batch)
  Tensor wordIdsT(DT_INT32, TensorShape({batchSize,maxSizeSentence}));
  //shape = (batch size)
  Tensor sequenceLengthsT(DT_INT32, TensorShape({batchSize}));
  //shape = (batch size, max_length of sentence)
  Tensor wordLengthsT(DT_INT32, TensorShape({batchSize,maxSizeSentence}));

  //Calculate the size of the longest word from the current batch. Mandatory to perform padding.
  int maxSizeWord=0;
  for(auto it=charIds.begin();it!=charIds.end();++it)
  {
    for(auto it2=(*it).begin();it2!=(*it).end();++it2)
    {
      maxSizeWord = std::max(static_cast<int>((*it2).size()),maxSizeWord);
    }
  }
  //shape = (batch size, max length of sentence, max length of word)
  Tensor charIdsT(DT_INT32, TensorShape({batchSize,maxSizeSentence,maxSizeWord}));
  Tensor dropoutT(DT_FLOAT,TensorShape());

  std::vector<std::pair<std::string, Tensor>> inputs(5);

  getFeedDict(inputs, charIds, wordIds, wordIdsT, sequenceLengthsT, charIdsT,
              wordLengthsT, dropoutT, batchSize, maxSizeSentence, maxSizeWord);

  //2. Run the session, evaluating our "c" operation from the graph

  std::vector<Tensor> outputs(2);
  *status = session->Run(inputs, {"proj/output_node","transitions"}, {}, &outputs);
  if (!status->ok())
  {
    throw std::runtime_error(status->ToString());
  }

  // Print the results
  //"tensorflow/core/framework/tensor_shape.h"

  //std::cerr << "The results : \n-logits : "<<outputs[0].DebugString() << "\n-transition matrix : "<<outputs[1].DebugString()<<std::endl;

  //   Grab all the outputs and convert the nodes to a matrix representation (Eigen::TensorMap<Eigen::Tensor>)
  auto oLogits = outputs[0].tensor<float, 3>(); //resulting a matrix (3D)
  auto oTransParametersCrf = outputs[1].matrix<float>();
  auto oSequencesLength = std::get<1>(inputs[1]).vec<int>();

//     (Different methods for vectors and matrices here:
//     https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/public/tensor.h)

  //In order to applicate the viterbiDecode algorithm, we have to transform the last outputs in eigen::matrix
  Eigen::MatrixXf transitionParams(outputs[1].dim_size(0),outputs[1].dim_size(1));

  for(auto i=0;i<outputs[1].dim_size(0);++i)
  {
    for(auto j=0;j<outputs[1].dim_size(1);++j)
    {
      transitionParams(i,j)=oTransParametersCrf(i,j);
    }
  }

  //3. Apply viterbiDecode algorithm on each sentence

  for(auto k=0;k<outputs[0].dim_size(0);++k)
  {
    Eigen::MatrixXf logits(oSequencesLength(k),outputs[0].dim_size(2));
    for(auto i=0;i<oSequencesLength(k);++i)
    {
      for(auto j=0;j<outputs[0].dim_size(2);++j)
      {
        logits(i,j)=oLogits(k,i,j);
      }
    }

    result[result.size()-batchSize+k]=viterbiDecode(logits,transitionParams);
    if(result[result.size()-batchSize+k].rows()==0)
    {
      return NERStatusCode::MISSING_DATA;
    }
  }
  return NERStatusCode::SUCCESS;
}

LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT NERStatusCode getFeedDict(
  std::vector< std::pair< std::string, tensorflow::Tensor > >& inputs,
  std::vector<std::vector< std::vector< int > >>& charIds,
  std::vector<std::vector< int >>& wordIds,
  tensorflow::Tensor& wordIdsT,
  tensorflow::Tensor& sequenceLengthsT,
  tensorflow::Tensor& charIdsT,
  tensorflow::Tensor& wordLengthsT,
  tensorflow::Tensor& dropoutT,
  int batchSize,
  int maxSizeSentence, int maxSizeWord)
{
  if(wordIds.empty() || charIds.empty())
  {
    std::cerr<<"NERStatusCode getFeedDict vocabulary is empty: "
              <<wordIds.size() << ", "
              <<charIds.size() << ", "
              <<".\n";
    return NERStatusCode::MISSING_DATA;
  }

  if(wordIdsT.NumElements()==0 || sequenceLengthsT.NumElements()==0 || charIdsT.NumElements()==0 || wordLengthsT.NumElements()==0 || dropoutT.NumElements()==0)
  {
    std::cerr<<"Tensors have not been initialized: "
              <<wordIdsT.NumElements() << ", "
              <<sequenceLengthsT.NumElements() << ", "
              <<charIdsT.NumElements() << ", "
              <<wordLengthsT.NumElements() << ", "
              <<dropoutT.NumElements() << ", "
              <<".\n";
    return NERStatusCode::MISSING_DATA;
  }

  if(maxSizeSentence==0 || maxSizeWord==0 || batchSize==0)
  {
    std::cerr<<"Sizes have not been initialized.\n";
    return NERStatusCode::MISSING_DATA;
  }

  //Transfer data to each input tensor

  auto tSequenceLengths = sequenceLengthsT.vec< int >();
  for(auto j=0;j<batchSize;++j)
  {
    tSequenceLengths(j)=static_cast<int>(wordIds[j].size());
  }
//   std::cout << sequenceLengthsT.DebugString() << "\n";

  auto tWordIds=wordIdsT.tensor<int,2>();
  for(auto i=0;i<batchSize;++i)
  {
    //Perform padding on the given data
    wordIds[i].resize(maxSizeSentence,0);
    for(auto j=0;j<maxSizeSentence;++j)
    {
      tWordIds(i,j)=wordIds[i][j];
    }
  }
//   std::cout << wordIdsT.DebugString() << "\n";

  auto tWordLengths=wordLengthsT.tensor<int,2>();
  auto tCharIds=charIdsT.tensor<int,3>();

  //Perform padding on the given data
  for(auto i=0;i<batchSize;++i)
  {
    charIds[i].resize(maxSizeSentence);
    for(auto j=0;j<maxSizeSentence;++j)
    {
      tWordLengths(i,j)=static_cast<int>(charIds[i][j].size());
      charIds[i][j].resize(maxSizeWord,0);
      for(auto k=0;k<maxSizeWord;++k)
      {
        tCharIds(i,j,k)=charIds[i][j][k];
      }
    }
  }
//   std::cout << wordLengthsT.DebugString() << "\n";
//   std::cout << charIdsT.DebugString() << "\n";

  dropoutT.scalar<float>()() =1.0;
//   std::cout << dropoutT.DebugString() << "\n";

  inputs =
  {
    {"word_ids"  , wordIdsT } ,
    {"sequence_lengths"  , sequenceLengthsT },
    {"char_ids"  , charIdsT },
    {"word_lengths"  , wordLengthsT },
    {"dropout",dropoutT},
  };

  return NERStatusCode::SUCCESS;
}

LIMA_TENSORFLOWSPECIFICENTITIES_EXPORT Eigen::MatrixXi viterbiDecode(
  const Eigen::MatrixXf& score,
  const Eigen::MatrixXf& transitionParams)
{
  if(score.size()==0)
  {
    std::cerr<<"The output is empty. Check the inputs.";
    return Eigen::MatrixXi();
  }
  if(transitionParams.size()==0)
  {
    std::cerr<<"The transition matrix is empty. Check it.";
    return Eigen::MatrixXi();
  }
  //1. Initialization of matrices
  Eigen::MatrixXf trellis=  Eigen::MatrixXf::Zero(score.rows(),score.cols());
  Eigen::MatrixXi backpointers= Eigen::MatrixXi::Zero(score.rows(),score.cols());
  trellis.row(0)=score.row(0);
  Eigen::MatrixXf v(transitionParams.rows(),transitionParams.cols());

  //2.Viterbi algorithm
  for(auto k=1;k<score.rows();++k)
  {
    for(auto i=0;i<transitionParams.rows();++i)
    {
      for(auto j=0;j<transitionParams.cols();++j)
      {
        v(i,j)=trellis(k-1,i)+transitionParams(i,j);
      }
    }
    trellis.row(k)=score.row(k)+v.colwise().maxCoeff();//equivalent to np.max() function
    for(auto i=0;i<backpointers.cols();++i)
    {
      v.col(i).maxCoeff(&backpointers(k,i));//equivalent to np.argmax() function
    }
  }
  //In Eigen, vector is just a particular matrix with one row or one column
  Eigen::VectorXi viterbi(score.rows());
  trellis.row(trellis.rows()-1).maxCoeff(&viterbi(0));
  Eigen::MatrixXi bp = backpointers.colwise().reverse();
  for(auto i=0;i<backpointers.rows()-1;++i)
  {
    viterbi(i+1)=bp(i,viterbi(i));
  }

  float viterbi_score=trellis.row(trellis.rows()-1).maxCoeff();
  LIMA_UNUSED(viterbi_score);
  //std::cout<<"Viterbi score of the sentence: "<<viterbi_score<<std::endl;

  return viterbi.colwise().reverse();
}
