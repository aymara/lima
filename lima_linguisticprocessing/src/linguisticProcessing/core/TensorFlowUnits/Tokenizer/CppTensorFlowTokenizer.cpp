// Copyright 2002-2019 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "CppTensorFlowTokenizer.h"
#include "tokUtils.h"

#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/TextSegmentation/SegmentationData.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include <QtCore/QTemporaryFile>
#include <QtCore/QRegularExpression>

#include <string>

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;
using namespace tensorflow;

#define EIGEN_DONT_VECTORIZE

namespace Lima
{
namespace LinguisticProcessing
{
namespace TensorFlowUnits
{
namespace Tokenizer
{

static SimpleFactory<MediaProcessUnit,CppTensorFlowTokenizer> cpptokenizerFactory(CPPTENSORFLOWTOKENIZER_CLASSID); // clazy:exclude=non-pod-global-static

class CppTokenizerPrivate
{
public:
  CppTokenizerPrivate();
  virtual ~CppTokenizerPrivate();

  void computeDefaultStatus(LinguisticAnalysisStructure::TStatus& curSettings);

  void init(const QString& corpus,
            const QString& model_path,
            int window_size,
            int batch_size);

  TokStatusCode getFeedDict(
    std::vector< std::pair< std::string, tensorflow::Tensor > >& inputs,
    std::vector<std::vector< int >>& data,
    tensorflow::Tensor& batchT,
    tensorflow::Tensor& dropoutT,
    int batchSize,
    int windowSize);

  /**
   * We load the protobuf file from the disk and parse it to retrieve the
   * unserialized graph_def
   * Code from
   * https://blog.metaflow.fr/tensorflow-how-to-freeze-a-model-and-serve-it-with-a-python-api-d4f3596b3adc
   */
  void load_graph(const QString& frozen_graph_filename);

  std::pair< std::map<QString,int>, std::map<int,QString> > load_embeddings_dictionary();

  /**
   * Replace sentence characters by their codes in the dictionary. return the
   * concatenation of all resulting coded sentences
   */
  std::vector<int> encode_sentences(const QString& sentences,
                                    const std::map<QString, int>& dictionary);

  /**
   * Replace sentence characters by their codes in the dictionary. return the
   * concatenation of all resulting coded sentences
   */
  QString decode_sentences(const std::vector<int>& sentences,
                           const std::map<int, QString>& dictionary);

  /** Generate a batch of maximum size batch_size
   *
   * The only element is an array of size window with characters from data where
   * the character to classify is the first of the array, at position offset
   * and the last one at position offset+window. So, the batch is:
   * [
   *   [data[offset]...data[offset+window]]
   *   [data[offset+1]...data[offset+1+window]]
   *   ...
   *   [data[offset+batch_size]...data[offset+batch_size+window]]
   * ]
   * When we are near data end, pad last element of batch with zeros. Do not
   * add batch elements with zeros only
   *
   * Fill a python array with the data and then use it as a buffer to fill the
   * final numpy darray
   */
  std::vector< std::vector< int > > generate_batch(const std::vector<int>& data,
                                                   int offset);

  std::vector< std::vector< std::pair<QString, int> > > tokenize(const QString& text);

  void computeDefaultStatus(Token& token);


  MediaId m_language;
  FsaStringsPool* m_stringsPool;
  LinguisticGraphVertex m_currentVx;

  QString m_model_path;
  long long m_window_size;
  long long m_max_batch_size;

  std::map<QString,int> m_vocabWords;
  std::map<QChar,int> m_vocabChars;
  std::map<unsigned int,QString> m_vocabTags;
  Session* m_session;
  GraphDef m_graph;
  QString m_data;
  std::map<QString,int> m_dictionary;
  std::map<int,QString> m_reverse_dictionary;


};

CppTokenizerPrivate::CppTokenizerPrivate() :
    m_stringsPool(nullptr),
    m_currentVx(0)
{
}

CppTokenizerPrivate::~CppTokenizerPrivate()
{
}

CppTensorFlowTokenizer::CppTensorFlowTokenizer() : m_d(new CppTokenizerPrivate())
{}

CppTensorFlowTokenizer::~CppTensorFlowTokenizer()
{
  delete m_d;
}

void CppTensorFlowTokenizer::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppTensorFlowTokenizer::init";
// #endif
  m_d->m_language = manager->getInitializationParameters().media;
  m_d->m_stringsPool = &Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language);

  try
  {
    m_d->m_data = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("data").c_str());
  }
  catch (NoSuchParam& )
  {
    m_d->m_data = QString::fromUtf8("SentenceBoundaries");
  }

  QString udCorpus; // The name/id of the Universal Dependencies corpus used for training
  try
  {
    udCorpus = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("corpus").c_str());
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'corpus' in CppTensorFlowTokenizer configuration";
    throw InvalidConfiguration();
  }

  QString modelPath; // The path to the LIMA python tensorflow-based tokenizer
  try
  {
    modelPath = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("model_path").c_str());
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'model_path' in PythonTensorFlowTokenizer group configuration";
    throw InvalidConfiguration();
  }

  int windowSize = -1; // The window size used with the tensorflow-based tokenizer
  try
  {
    bool success;
    windowSize = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("window_size").c_str()).toInt(&success);
    if (!success)
    {
      TOKENIZERLOGINIT;
      LERROR << "Param 'window_size' in PythonTensorFlowTokenizer group configuration is not an integer";
      throw InvalidConfiguration();
    }
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'window_size' in PythonTensorFlowTokenizer group configuration";
    throw InvalidConfiguration();
  }

  int batchSize = -1; // The batch size used with the tensorflow-based tokenizer
  try
  {
    bool success;
    batchSize = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("batch_size").c_str()).toInt(&success);
    if (!success)
    {
      TOKENIZERLOGINIT;
      LERROR << "Param 'batch_size' in PythonTensorFlowTokenizer group configuration is not an integer";
      throw InvalidConfiguration();
    }
  }
  catch (NoSuchParam& )
  {
    TOKENIZERLOGINIT;
    LERROR << "no param 'batch_size' in PythonTensorFlowTokenizer group configuration";
    throw InvalidConfiguration();
  }

  m_d->init(udCorpus, modelPath, windowSize, batchSize);
}


LimaStatusCode CppTensorFlowTokenizer::process(AnalysisContent& analysis) const
{
  TimeUtilsController TensorFlowTokenizerProcessTime("TensorFlowTokenizer");
  TOKENIZERLOGINIT;
  LINFO << "start tokenizer process";
  auto anagraph = new AnalysisGraph("AnalysisGraph",m_d->m_language,true,true);
  analysis.setData("AnalysisGraph",anagraph);
  LinguisticGraph* graph=anagraph->getGraph();
  m_d->m_currentVx = anagraph->firstVertex();
  // Get text from analysis
  auto originalText = std::dynamic_pointer_cast<LimaStringText>(analysis.getData("Text"));

  // Evaluate TensorFlow model on the text
  auto sentencesTokens = m_d->tokenize(*originalText);

  // Insert the tokens in the graph and create sentence limits
  SegmentationData* sb = new SegmentationData("AnalysisGraph");
  analysis.setData(m_d->m_data.toUtf8().constData(), sb);

  remove_edge(anagraph->firstVertex(),
              anagraph->lastVertex(),
              *graph);
  LinguisticGraphVertex beginSentence = 0;
  // Insert the tokens in the graph and create sentence limits
  for (const auto& sentence: sentencesTokens)
  {
    LinguisticGraphVertex endSentence = std::numeric_limits< LinguisticGraphVertex >::max();
    for (const auto& token: sentence)
    {
      const auto& str = token.first;
#ifdef DEBUG_LP
      LDEBUG << "      Adding token '" << str << "'";
#endif
      StringsPoolIndex form=(*m_d->m_stringsPool)[str];
      Token *tToken = new Token(form,str, token.second, token.first.size());
      if (tToken == 0) throw MemoryErrorException();

      m_d->computeDefaultStatus(*tToken);
#ifdef DEBUG_LP
      //   LDEBUG << "      curSettings is " << curSettings.toString();
      LDEBUG << "      status is " << tToken->status().toString();
#endif

      // Adds on the path
      LinguisticGraphVertex newVx = add_vertex(*graph);
//       if (beginSentence == std::numeric_limits< LinguisticGraphVertex >::max())
//         beginSentence = newVx;
      endSentence = newVx;
      put(vertex_token, *graph, newVx, tToken);
      put(vertex_data, *graph, newVx, new MorphoSyntacticData());
      add_edge(m_d->m_currentVx, newVx, *graph);
      m_d->m_currentVx = newVx;
    }
#ifdef DEBUG_LP
    LDEBUG << "adding sentence" << beginSentence << endSentence;
#endif
    sb->add(Segment("sentence", beginSentence, endSentence, anagraph));
    beginSentence = endSentence;
  }

  add_edge(m_d->m_currentVx,anagraph->lastVertex(),*graph);
  return SUCCESS_ID;
}

// set default key in status according to other elements in status
void CppTokenizerPrivate::computeDefaultStatus(LinguisticAnalysisStructure::TStatus& curSettings)
{
  std::string defaultKey;
  switch (curSettings.getStatus()) {
    case T_ALPHA : {
      switch (curSettings.getAlphaCapital()) {
        case T_CAPITAL       : defaultKey = "t_capital"      ; break;
        case T_SMALL         : defaultKey = "t_small"        ; break;
        case T_CAPITAL_1ST   : defaultKey = "t_capital_1st"  ; break;
        case T_ACRONYM       : defaultKey = "t_acronym"      ; break;
        case T_CAPITAL_SMALL : defaultKey = "t_capital_small"; break;
        case T_ABBREV       : defaultKey = "t_abbrev"      ; break;
        default : break;
      }
      switch (curSettings.getAlphaRoman()) { // Roman supersedes Cardinal
        case T_CARDINAL_ROMAN : defaultKey = "t_cardinal_roman"; break;
        case T_ORDINAL_ROMAN  : defaultKey = "t_ordinal_roman" ; break;
        case T_NOT_ROMAN      : defaultKey = "t_not_roman"     ; break;
        default : break;
      }
      if (curSettings.isAlphaHyphen()) {
        //no change
        //defaultKey = "t_alpha_hyphen";
      }
      if (curSettings.isAlphaPossessive()) {
        defaultKey = "t_alpha_possessive";
      }
      break;
    } // end T_ALPHA
    case T_NUMERIC : {
      switch (curSettings.getNumeric()) {
        case T_INTEGER         : defaultKey = "t_integer"       ; break;
        case T_COMMA_NUMBER    : defaultKey = "t_comma_number"  ; break;
        case T_DOT_NUMBER      : defaultKey = "t_dot_number"    ; break;
        case T_FRACTION        : defaultKey = "t_fraction"      ; break;
        case T_ORDINAL_INTEGER : defaultKey = "t_ordinal_integer"; break;
        default: break;
      }
      break;
    }
    case T_ALPHANUMERIC : defaultKey = "t_alphanumeric" ; break;
    case T_PATTERN      : defaultKey = "t_pattern"      ; break;
    case T_WORD_BRK     : defaultKey = "t_word_brk"     ; break;
    case T_SENTENCE_BRK : defaultKey = "t_sentence_brk" ; break;
    case T_PARAGRAPH_BRK: defaultKey = "t_paragraph_brk" ; break;
    default: defaultKey = "t_fallback";
  }
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppTokenizerPrivate::computeDefaultStatus" << defaultKey;
// #endif
  curSettings.setDefaultKey(Common::Misc::utf8stdstring2limastring(defaultKey));
}


void CppTokenizerPrivate::init(const QString& corpus,
                               const QString& model_path,
                               int window_size,
                               int batch_size)
{
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppTokenizerPrivate::init" << corpus
//           << model_path << window_size << batch_size;
// #endif
  m_model_path = QString("%1/%2").arg(model_path).arg(corpus);
  m_window_size = window_size;
  m_max_batch_size = batch_size;
  // load char embeddings
  std::tie(m_dictionary, m_reverse_dictionary) = load_embeddings_dictionary();

  // Initialize a tensorflow session
  tensorflow::SessionOptions options;
  tensorflow::ConfigProto & config = options.config;
  config.set_inter_op_parallelism_threads(8);
  config.set_intra_op_parallelism_threads(8);
  config.set_use_per_session_threads(false);
  auto status = NewSession(options, &m_session);
  if (!status.ok()) {
    LIMA_EXCEPTION_LOGINIT(TOKENIZERLOGINIT, status.ToString() );
  }

//   QString frozenFile = Common::Misc::findFileInPaths(
//     Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
//     QString("%1/frozen_model.pb").arg(m_model_path));
  QString frozenFile = QString("%1/frozen_model.pb").arg(m_model_path);

  // load frozen graph
  load_graph(frozenFile);

  // Add the graph to the session
  status = m_session->Create(m_graph);
  if (!status.ok())
  {
    LIMA_EXCEPTION_LOGINIT(
      TOKENIZERLOGINIT,
      "CppTokenizerPrivate::init error creating graph:" << status.ToString() );
  }
}

void CppTokenizerPrivate::load_graph(const QString& frozen_graph_filename)
{
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppTokenizerPrivate::load_graph" << frozen_graph_filename;
// #endif
  // Read in the protobuf graph we have exported
  auto status = ReadBinaryProto(Env::Default(),
                              frozen_graph_filename.toUtf8().constData(),
                              &m_graph);
  if (!status.ok())
  {
    LIMA_EXCEPTION_LOGINIT(
      TOKENIZERLOGINIT,
      "CppTokenizerPrivate::load_graph error reading binary proto:"
      << status.ToString() );
  }
}

std::pair< std::map<QString,int>, std::map<int,QString> >
CppTokenizerPrivate::load_embeddings_dictionary()
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "CppTokenizerPrivate::load_embeddings_dictionary";
#endif
  std::map<QString,int> dictionary;
  std::map<int,QString> reverse_dictionary;
//   QString metadataFile = Common::Misc::findFileInPaths(
//     Common::MediaticData::MediaticData::single().getResourcesPath().c_str(),
//     QString("%1/metadata.tsv").arg(m_model_path));
  QString metadataFile = QString("%1/metadata.tsv").arg(m_model_path);

  QFile metadata(metadataFile);
  if (!metadata.open(QFile::ReadOnly))
  {
    LIMA_EXCEPTION_LOGINIT(
      TOKENIZERLOGINIT,
      "CppTokenizerPrivate::load_embeddings_dictionary unable to open metadata file"
      << metadata.fileName() );
  }
  // dictionary - map of chars(strings) to their codes(integers)
  // reverse_dictionary - maps codes(integers) to chars(strings)
#ifdef DEBUG_LP
  LDEBUG << "CppTokenizerPrivate::load_embeddings_dictionary" << metadata.fileName();
#endif

  // skip first line (UNK - code 0)
  if (!metadata.atEnd())
  {
    metadata.readLine();
  }
  dictionary["UNK"] = 0;
  reverse_dictionary[0] = "UNK";
  int count = 1;
  while (!metadata.atEnd())
  {
    auto line = QString::fromUtf8(metadata.readLine());
    line.chop(1);
    if (!line.isEmpty())
    {
      dictionary[line] = count;
      reverse_dictionary[count] = line;
    }
    ++count;
  }
#ifdef DEBUG_LP
  LDEBUG << "Text::load_embeddings_dictionary"
          << dictionary << reverse_dictionary;
#endif

  return std::make_pair(dictionary, reverse_dictionary);
}


std::vector<int> CppTokenizerPrivate::encode_sentences(const QString& sentences,
                                                       const std::map<QString,int>& dictionary)
{
  std::vector<int> data;
  for (const auto& character: sentences)
  {
    data.push_back(dictionary.find(character) != dictionary.end()
      ? (*dictionary.find(character)).second
      : 0);
  }
  return data;
}

QString CppTokenizerPrivate::decode_sentences(const std::vector<int>& sentences,
                                              const std::map<int, QString>& dictionary)
{
  QString data;
  for (const auto& character: sentences)
  {
    data.push_back(dictionary.find(character) != dictionary.end()
      ? (*dictionary.find(character)).second
      : "UNK");
  }
  return data;
}


std::vector<std::vector<int> > CppTokenizerPrivate::generate_batch(
    const std::vector<int>& data, int offset)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "CppTokenizerPrivate::generate_batch" << offset;
#endif
  std::vector<int> batch_data;
  for (auto i = 0; i < m_max_batch_size; ++i)
  {
    if (offset+i >= data.size())
    {
      break;
    }
    else if (offset+i+m_window_size >= data.size())
    {
      batch_data.insert(std::end(batch_data),
                        std::begin(data)+(offset+i),
                        std::end(data));
      std::vector<int> complement(m_window_size-(data.size()-(offset+i)));
      batch_data.insert(std::end(batch_data),
                        std::begin(complement),
                        std::end(complement));
    }
    else
    {
      batch_data.insert(std::end(batch_data),
                        std::begin(data)+(offset+i),
                        std::begin(data)+(offset+i+m_window_size));
    }
  }
  if (batch_data.size() % m_window_size != 0)
  {
    std::vector<int> complement(m_window_size - (batch_data.size() % m_window_size));
    batch_data.insert(std::end(batch_data),
                      std::begin(complement),
                      std::end(complement));
  }
#ifdef DEBUG_LP
  LDEBUG << "CppTokenizerPrivate::generate_batch batch_data:" << batch_data;
#endif

#ifdef DEBUG_LP
  LDEBUG << "CppTokenizerPrivate::generate_batch batch_data.size()/m_window_size:"
          << batch_data.size() << m_window_size;
#endif
  std::vector< std::vector<int> > batch;

  for (auto i = 0; i < batch_data.size()/m_window_size; ++i)
  {
    std::vector<int> window;
    window.insert(std::end(window),
                  batch_data.begin()+i*m_window_size,
                  batch_data.begin()+(i+1)*m_window_size);
    batch.push_back(window);
  }
#ifdef DEBUG_LP
  LDEBUG << "CppTokenizerPrivate::generate_batch batch:" << batch;
#endif

  return batch;
}

std::vector< std::vector< std::pair<QString, int> > > CppTokenizerPrivate::tokenize(
    const QString& text)
{
#ifdef DEBUG_LP
  TOKENIZERLOGINIT;
  LDEBUG << "CppTokenizerPrivate::tokenize" << text.left(100);
#endif
  std::vector< std::vector< std::pair<QString, int> > > sentences;
  std::vector< std::pair<QString, int> > current_sentence;
  QString current_token;
  int current_token_offset = 0;

  // Encode text
  auto encoded = encode_sentences(text, m_dictionary);

  int offset = 0;
  while (offset < encoded.size())
  {
#ifdef DEBUG_LP
    LDEBUG << "CppTokenizerPrivate::tokenize offset: " << offset
            << "; length(encoded): " << encoded.size()
            << "; current_token:" << current_token << current_token_offset;
#endif
    auto batch = generate_batch(encoded, offset);

    std::vector<std::pair<std::string, Tensor>> inputs(3);

    //shape = (batch.size(), m_window_size)
    const long long batch_size = batch.size();
    Tensor batchT(DT_INT32, TensorShape({batch_size, m_window_size}));
    //shape = (batch size)
    Tensor dropoutT(DT_FLOAT,TensorShape());


    if (getFeedDict(inputs, batch, batchT, dropoutT, batch.size(), m_window_size)
          != TokStatusCode::SUCCESS)
    {
      TOKENIZERLOGINIT;
      LERROR << "CppTensorFlowTokenizer::tokenize error when building feed dict";
      throw LimaException("CppTensorFlowTokenizer::tokenize error when building feed dict");
    }
#ifdef DEBUG_LP
    LDEBUG << "CppTokenizerPrivate::tokenize batch: " << inputs[0].second.DebugString();
#endif


    std::vector<Tensor> outputs(2);

    auto status = m_session->Run(inputs,
                                 {"prediction/Softmax"},
                                 {},
                                 &outputs);
    if (!status.ok())
    {
      TOKENIZERLOGINIT;
      LERROR << "CppTensorFlowTokenizer::tokenize error in session run:"
            << status.ToString();
      throw LimaException("CppTensorFlowTokenizer::tokenize error in session run");
    }

    // Print the results
    //"tensorflow/core/framework/tensor_shape.h"
#ifdef DEBUG_LP
    LDEBUG << "The results :" << QTENDL
              << "  -logits            : "<<outputs[0].DebugString() << QTENDL
              << "  -transition matrix : "<<outputs[1].DebugString();
#endif
    //   Grab all the outputs and convert the nodes to a matrix representation (Eigen::TensorMap<Eigen::Tensor>)
    auto oPrediction = outputs[0].tensor<float, 2>(); //resulting a matrix (2D batch_size, nb classes)

    std::vector<int> prediction(batch.size());
    for(auto k=0;k<batch.size();++k)
    {
#ifdef DEBUG_LP
      LDEBUG << "CppTensorFlowTokenizer::tokenize k :" << k;
#endif
      int maxId = 0;
      double max = 0;
      for(auto l=0;l<3;++l)
      {
#ifdef DEBUG_LP
        LDEBUG << "CppTensorFlowTokenizer::tokenize l :" << l
                << "; prediction("<<k<<","<<l<<"):" << oPrediction(k,l);
#endif
        if (oPrediction(k,l) > max)
        {
          max = oPrediction(k,l);
          prediction[k] = l;
        }
      }
#ifdef DEBUG_LP
      if ((k+offset) < text.size())
      {
        LDEBUG << "CppTensorFlowTokenizer::tokenize prediction[" << k
                << "] =" << text[k+offset] << prediction[k];
      }
#endif
    }

    //     (Different methods for vectors and matrices here:
    //     https://github.com/tensorflow/tensorflow/blob/master/tensorflow/core/public/tensor.h)

    // print("batch: {} ; pred: {}".format(m_decode_sentences(batch,
    // m_reverse_dictionary), pred))
    for (auto i = 0; i < prediction.size(); ++i)
    {
      if (offset+i >= text.size())
        break;
      if (prediction[i] == 0)
      {
        if (!text[offset+i].isSpace())
        {
          if (current_token.isEmpty())
            current_token_offset = offset+i;
          current_token += text[offset+i];
#ifdef DEBUG_LP
          LDEBUG << "CppTensorFlowTokenizer::tokenize current token is now:"
                  << current_token << current_token_offset;
#endif
        }
        else if (!current_token.isEmpty())
        {
#ifdef DEBUG_LP
          LDEBUG << "CppTensorFlowTokenizer::tokenize 1:"
                  << current_token << current_token_offset;
#endif
          current_sentence.push_back(std::make_pair(current_token.trimmed(),
                                                    current_token_offset));
          current_token.clear();
          current_token_offset += 1;
        }
        else
        {
          current_token_offset += 1;
#ifdef DEBUG_LP
          LDEBUG << "CppTensorFlowTokenizer::tokenize 2:"
                  << current_token << current_token_offset;
#endif
        }
      }
      else if (prediction[i] == 1)
      {
        if (!text[offset+i].isSpace())
        {
          current_token += text[offset+i];
#ifdef DEBUG_LP
        LDEBUG << "CppTensorFlowTokenizer::tokenize 3:"
                << current_token << current_token_offset;
#endif
        }
        if (!current_token.isEmpty())
        {
            current_sentence.push_back(std::make_pair(current_token.trimmed(),
                                                      current_token_offset));
        }
        current_token = "";
        current_token_offset = offset + i + 1;
      }
      else if (prediction[i] == 2)
      {
        current_token += text[offset+i];
#ifdef DEBUG_LP
        LDEBUG << "CppTensorFlowTokenizer::tokenize 4:"
                << current_token << current_token_offset;
#endif
        current_sentence.push_back(std::make_pair(current_token.trimmed(),
                                                  current_token_offset));
        sentences.push_back(current_sentence);
        current_token.clear();
        current_token_offset = offset + i + 1;
        current_sentence.clear();
      }
    }
    offset += prediction.size();

    if (offset >= text.size())
    {
      if (!current_sentence.empty())
      {
        sentences.push_back(current_sentence);
        current_sentence.clear();
        current_token.clear();
      }
      break;
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "CppTensorFlowTokenizer::tokenize final sentences:" << sentences;
#endif
  return sentences;
}

// set default key in status according to other elements in status
void CppTokenizerPrivate::computeDefaultStatus(Token& token)
{
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppTokenizerPrivate::computeDefaultStatus"
//           << token.stringForm();
// #endif
  static QRegularExpression reCapital("^[[:upper:]]+$");
  static QRegularExpression reSmall("^[[:lower:]]+$");
  static QRegularExpression reCapital1st("^[[:upper:]]\\w+$");
  static QRegularExpression reAcronym("^([[:upper:]]\\.)+$");
  static QRegularExpression reCapitalSmall("^([[:upper:][:lower:]])+$");
  static QRegularExpression reAbbrev("^\\w+\\.$");
  static QRegularExpression reTwitter("^[@#]\\w+$");

//       t_cardinal_roman
  static QRegularExpression reCardinalRoman("^(?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})$");
//       t_ordinal_roman
  static QRegularExpression reOrdinalRoman("^(?=[MDCLXVI])M*(C[MD]|D?C{0,3})(X[CL]|L?X{0,3})(I[XV]|V?I{0,3})(st|nd|d|th|er|ème)$");
//       t_integer
  static QRegularExpression reInteger("^\\d+$");
//       t_comma_number
  static QRegularExpression reCommaNumber("^\\d+,\\d$");
//       t_dot_number
  static QRegularExpression reDotNumber("^\\d\\.\\d$");
//       t_fraction
  static QRegularExpression reFraction("^\\d([.,]\\d+)?/\\d([.,]\\d+)?$");
//       t_ordinal_integer
  static QRegularExpression reOrdinalInteger("^\\d+(st|nd|d|th|er|ème)$");
//       t_alphanumeric
  static QRegularExpression reAlphanumeric("^[\\d[:lower:][:upper:]]+$");
  static QRegularExpression reSentenceBreak("^[;.!?]$");

  LinguisticAnalysisStructure::TStatus curSettings;
  if (reCapital.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_capital";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital"));
  }
  else if (reSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_small";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_small"));
  }
  else if (reCapital1st.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_capital_1st";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital_1st"));
  }
  else if (reAcronym.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_acronym";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_acronym"));
  }
  else if (reCapitalSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_capital_small";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_capital_small"));
  }
  else if (reAbbrev.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_abbrev";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_abbrev"));
  }
  else if (reTwitter.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_twitter";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_twitter"));
  }
  else if (reCardinalRoman.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_cardinal_roman";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_cardinal_roman"));
  }
  else if (reOrdinalRoman.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_ordinal_roman";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_ordinal_roman"));
  }
  else if (reInteger.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_integer";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_integer"));
  }
  else if (reCommaNumber.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_comma_number";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_comma_number"));
  }
  else if (reDotNumber.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_dot_number";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_dot_number"));
  }
  else if (reFraction.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_fraction";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_fraction"));
  }
  else if (reOrdinalInteger.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_ordinal_integer";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_ordinal_integer"));
  }
  else if (reAlphanumeric.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_alphanumeric";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_alphanumeric"));
  }
  else if (reSentenceBreak.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_sentence_brk";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_sentence_brk"));
  }
  else // if (reSmall.match(token.stringForm()).hasMatch())
  {
// #ifdef DEBUG_LP
//     LDEBUG << "CppTokenizerPrivate::computeDefaultStatus t_word_brk (default)";
// #endif
    curSettings.setDefaultKey(QString::fromUtf8("t_word_brk"));
  }
  // //       t_not_roman
//   static QRegularExpression reNotRoman("^$");
// //     t_alpha_concat_abbrev
//   static QRegularExpression reAlphConcatAbbrev("^$");
// //       t_pattern
//   static QRegularExpression rePattern("^$");
// //       t_word_brk
//   static QRegularExpression reWordBreak("^$");
// //       t_sentence_brk
//   static QRegularExpression reSentenceBreak("^$");
// //       t_paragraph_brk
//   static QRegularExpression reParagraphBreak("^$");

  token.setStatus(curSettings);
}

TokStatusCode CppTokenizerPrivate::getFeedDict(
  std::vector< std::pair< std::string, tensorflow::Tensor > >& inputs,
  std::vector< std::vector< int > >& batch,
  tensorflow::Tensor& batchT,
  tensorflow::Tensor& dropoutT,
  int batchSize,
  int windowSize)
{
// #ifdef DEBUG_LP
//   TOKENIZERLOGINIT;
//   LDEBUG << "CppTokenizerPrivate::getFeedDict" << batch;
// #endif
  if (batch.empty())
  {
    std::cerr<<"Check that the data have been loaded.\n";
    return TokStatusCode::MISSING_DATA;
  }

  if (batchT.NumElements()==0
    || dropoutT.NumElements()==0)
  {
    std::cerr<<"Tensors have not been initialized.\n";
    return TokStatusCode::MISSING_DATA;
  }

  if (windowSize==0 || batchSize==0)
  {
    std::cerr<<"Sizes have not been initialized.\n";
    return TokStatusCode::MISSING_DATA;
  }

  //Transfer data to each input tensor

  auto tBatch=batchT.tensor<int,2>();
  for(auto i=0;i<batchSize;++i)
  {
    //Perform padding on the given data
    batch[i].resize(m_window_size,0);
    for(auto j=0;j<m_window_size;++j)
    {
      tBatch(i,j)=batch[i][j];
    }
  }

// #ifdef DEBUG_LP
//   LDEBUG << "CppTokenizerPrivate::getFeedDict batchT: " << batchT.DebugString();
// #endif

  dropoutT.scalar<float>()() = 1.0;
// #ifdef DEBUG_LP
//   LDEBUG << "CppTokenizerPrivate::getFeedDict dropoutT: " << dropoutT.DebugString();
// #endif

  inputs = {
    {"data"  , batchT } ,
    {"dropout",dropoutT},
  };

  return TokStatusCode::SUCCESS;
}

} // namespace Tokenizer
} // namespace TensorFlowUnits
} // namespace LinguisticProcessing
} // namespace Lima
