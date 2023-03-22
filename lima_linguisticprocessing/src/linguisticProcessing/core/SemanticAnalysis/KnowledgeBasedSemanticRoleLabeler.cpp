// Copyright 2016 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "KnowledgeBasedSemanticRoleLabeler.h"

#include "common/Data/LimaString.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "linguisticProcessing/core/SemanticAnalysis/ConllDumper.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/common/linguisticData/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/tools/FileUtils.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"

#include <string>
#ifdef _DEBUG
  #undef _DEBUG
  #include <Python.h>
  #define _DEBUG
#else
  #include <Python.h>
#endif
#include <QRegularExpression>
#include <QtCore/QTemporaryFile>

using namespace std;
using namespace Lima::LinguisticProcessing::AnalysisDumpers;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;

namespace Lima
{
namespace LinguisticProcessing
{
namespace SemanticAnalysis
{

static SimpleFactory<MediaProcessUnit,KnowledgeBasedSemanticRoleLabeler> knowledgeBasedSemanticRoleLabelerFactory(KNOWLEDGEBASEDSEMANTICROLELABELER_CLASSID); // clazy:exclude=non-pod-global-static


class KnowledgeBasedSemanticRoleLabelerPrivate
{
public:
  KnowledgeBasedSemanticRoleLabelerPrivate();
  virtual ~KnowledgeBasedSemanticRoleLabelerPrivate();

  PyObject* m_instance;
  std::shared_ptr<MediaProcessUnit> m_dumper;
  std::shared_ptr<MediaProcessUnit> m_loader;
  QString m_inputSuffix;
  QString m_outputSuffix;
  QString m_temporaryFileMetadata;
};

KnowledgeBasedSemanticRoleLabelerPrivate::KnowledgeBasedSemanticRoleLabelerPrivate() :
  m_instance(0),
  m_dumper(new ConllDumper())
{}

KnowledgeBasedSemanticRoleLabelerPrivate::~KnowledgeBasedSemanticRoleLabelerPrivate()
{
}

KnowledgeBasedSemanticRoleLabeler::KnowledgeBasedSemanticRoleLabeler() : m_d(new KnowledgeBasedSemanticRoleLabelerPrivate())
{}


KnowledgeBasedSemanticRoleLabeler::~KnowledgeBasedSemanticRoleLabeler()
{
  delete m_d;
}

auto failed_to_import_the_sys_module = []()
{
  SEMANTICANALYSISLOGINIT;
  LERROR << "Failed to import the sys module";
  PyErr_Print();
};

auto cannot_instantiate_the_semanticrolelabeler_python_class = []()
{
  SEMANTICANALYSISLOGINIT;
  LERROR << "Cannot instantiate the SemanticRoleLabeler python class";
  PyErr_Print();
  Py_Exit(1);
};

void KnowledgeBasedSemanticRoleLabeler::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
#ifdef DEBUG_LP
  SEMANTICANALYSISLOGINIT;
  LDEBUG << "KnowledgeBasedSemanticRoleLabeler::init";
#endif

  MediaId language=manager->getInitializationParameters().media;
  try {
    string dumperName=unitConfiguration.getParamsValueAtKey("dumper");
    // create the dumper
    m_d->m_dumper = manager->getObject(dumperName);
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    SEMANTICANALYSISLOGINIT;
    LERROR << "Missing 'dumper' parameter in KnowledgeBasedSemanticRoleLabeler group for language "
           << (int)language << " !";
    throw InvalidConfiguration();
  }

  try {
    string loaderName=unitConfiguration.getParamsValueAtKey("loader");
    // create the loader
    m_d->m_loader=manager->getObject(loaderName);
  }
  catch (InvalidConfiguration& ) {
    m_d->m_loader = 0;
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    SEMANTICANALYSISLOGINIT;
    LERROR << "Missing 'loader' parameter in KnowledgeBasedSemanticRoleLabeler group for language "
           << (int)language << " !";
    throw InvalidConfiguration();
  }

  try {
    m_d->m_temporaryFileMetadata = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("temporaryFileMetadata").c_str());
  }
  catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
    // optional parameter: keep default value (empty)
  }

  if (m_d->m_temporaryFileMetadata.isEmpty())
  {
    try {
      m_d->m_inputSuffix=QString::fromUtf8(unitConfiguration.getParamsValueAtKey("inputSuffix").c_str());
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
      SEMANTICANALYSISLOGINIT;
      LERROR << "Missing 'inputSuffix' parameter in KnowledgeBasedSemanticRoleLabeler group for language "
            << (int)language << " !";
      throw InvalidConfiguration();
    }

    try {
      m_d->m_outputSuffix=QString::fromUtf8(unitConfiguration.getParamsValueAtKey("outputSuffix").c_str());
    }
    catch (Common::XMLConfigurationFiles::NoSuchParam& ) {
      SEMANTICANALYSISLOGINIT;
      LERROR << "Missing 'outputSuffix' parameter in KnowledgeBasedSemanticRoleLabeler group for language "
            << (int)language << " !";
      throw InvalidConfiguration();
    }
  }
  QString kbsrlLogLevel = "error";
  try
  {
    kbsrlLogLevel = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("loglevel").c_str());
  }
  catch (NoSuchParam& )
  {
    // keep default
  }

  QString path;
  try
  {
    path = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("path").c_str());
  }
  catch (NoSuchParam& )
  {
    SEMANTICANALYSISLOGINIT;
    LERROR << "no param 'path' in KnowledgeBasedSemanticRoleLabeler group configuration";
    throw InvalidConfiguration();
  }

  QString mode = "VerbNet";
  try
  {
    mode = QString::fromUtf8(unitConfiguration.getParamsValueAtKey("mode").c_str());
    if (mode != "VerbNet" && mode != "FrameNet")
    {
      SEMANTICANALYSISLOGINIT;
      LERROR << "Unknown semantic annotation mode" << mode;
      throw InvalidConfiguration();
    }
  }
  catch (NoSuchParam& )
  {
    // keep default
  }

  // Initialize the python SRL system
  /*
   * Find the first python executable in the path and use it as the program name.
   *
   * This allows to find the modules set up in an activated virtualenv
   */
  QString str_program_name;
  QString pathEnv = QString::fromUtf8(qgetenv("PATH").constData());
  QStringList paths = pathEnv.split(QRegularExpression("[;:]"));
  for (auto it = paths.begin(); it != paths.end(); ++it)
  {
    const auto & path = *it;
    if (QFile::exists(path + "/python" ))
    {
      str_program_name = path + "/python";
      break;
    }
  }
#ifndef WIN32
  Py_SetProgramName(const_cast<wchar_t*>( str_program_name.toStdWString().c_str()));
#else
  Py_SetProgramName( (wchar_t*)str_program_name.unicode() );
#endif


  Py_Initialize();

  PyObject* main_module = PyImport_ImportModule("__main__");
  PyObject* main_dict = PyModule_GetDict(main_module);
  PyObject* sys_module = PyImport_ImportModule("sys");
  if (sys_module == nullptr)
  {
    failed_to_import_the_sys_module();
    Py_Exit(1);
  }

  PyDict_SetItemString(main_dict, "sys", sys_module);

  // Add the path to the knowledgesrl pachkage to putho path
  PyObject* pythonpath = PySys_GetObject("path");
  if (PyList_Append(pythonpath, PyUnicode_DecodeFSDefault(path.toUtf8().constData())) ==  -1)
  {
    SEMANTICANALYSISLOGINIT;
    LERROR << "Failed to append to python path";
    PyErr_Print();
    Py_Exit(1);
  }

  // Import the semanticrolelabeler module
  PyObject* semanticrolelabeler_module = PyImport_ImportModule("semanticrolelabeler");
  if (semanticrolelabeler_module == NULL)
  {
    SEMANTICANALYSISLOGINIT;
    LERROR << "KnowledgeBasedSemanticRoleLabeler::init"<< __FILE__ << __LINE__ << ": Failed to import srl semanticrolelabeler module";
    PyErr_Print();
    Py_Exit(1);
  }

  // Create the semantic role labeller instance
  m_d->m_instance = PyObject_CallMethod(
    semanticrolelabeler_module, "SemanticRoleLabeler", "[sss]",
    QString(QLatin1String("--loglevel=%1")).arg(kbsrlLogLevel).toUtf8().constData(),
    QString(QLatin1String("--frame-lexicon=%1")).arg(mode).toUtf8().constData(),
    QString(QLatin1String("--language=%1")).arg(Lima::Common::MediaticData::MediaticData::single().getMediaId(language).c_str()).toUtf8().constData());
  if (m_d->m_instance == nullptr)
  {
    cannot_instantiate_the_semanticrolelabeler_python_class();
  }
}

auto metadata_equal_zero = []()
{
  SEMANTICANALYSISLOGINIT;
  LERROR << "no LinguisticMetaData ! abort";
};

auto temporary_file_not_open = []()
{
  SEMANTICANALYSISLOGINIT;
  LERROR << "KnowledgeBasedSemanticRoleLabeler: unable to create temporary file";
};

auto temporary_file_srl_not_open = [](QScopedPointer<QTemporaryFile>& temporaryFile)
{
  SEMANTICANALYSISLOGINIT;
  LERROR << "KnowledgeBasedSemanticRoleLabeler: unable to open temporary file for dumping SRL CoNLL data to it"<< temporaryFile->fileName();
  LERROR << "KnowledgeBasedSemanticRoleLabeler: keep (do not auto remove) it for debug purpose." ;
  temporaryFile->setAutoRemove(false);
};

auto failed_to_load_data_from_temporary_file = [](QScopedPointer<QTemporaryFile>& temporaryFile)
{
  SEMANTICANALYSISLOGINIT;
  LERROR << "KnowledgeBasedSemanticRoleLabeler: failed to load data from temporary file" << temporaryFile->fileName();
  LERROR << "KnowledgeBasedSemanticRoleLabeler: keep (do not auto remove) it for debug purpose." << temporaryFile->fileName();
  temporaryFile->setAutoRemove(false);
};

auto failure_during_call_of_the_annotate_method_on = [](QString& conllInput)
{
  SEMANTICANALYSISLOGINIT;
  LERROR << "Failure during call of the annotate method on" << conllInput;
  PyErr_Print();
  Py_Exit(1);
};

LimaStatusCode KnowledgeBasedSemanticRoleLabeler::process(
  AnalysisContent& analysis) const
{
  TimeUtilsController knowledgeBasedSemanticRoleLabelerProcessTime("KnowledgeBasedSemanticRoleLabeler");
#ifdef DEBUG_LP
  SEMANTICANALYSISLOGINIT;
  LINFO << "start SRL process";
#endif

  auto metadata = std::dynamic_pointer_cast<LinguisticMetaData>(analysis.getData("LinguisticMetaData"));
  if (metadata == nullptr)
  {
    metadata_equal_zero();
    return MISSING_DATA;
  }

  QScopedPointer<QTemporaryFile> temporaryFile;
  if (!m_d->m_temporaryFileMetadata.isEmpty())
  {
    QScopedPointer<QTemporaryFile> otherTemp(new QTemporaryFile());
    temporaryFile.swap(otherTemp);
    if (!temporaryFile->open())
    {
      temporary_file_not_open();
      return CANNOT_OPEN_FILE_ERROR;
    }
    metadata->setMetaData(m_d->m_temporaryFileMetadata.toUtf8().constData(),
                          temporaryFile->fileName().toUtf8().constData());
  }

  // Use CoNLL duper to produce the input to the SRL
  LimaStatusCode returnCode(SUCCESS_ID);
  returnCode=m_d->m_dumper->process(analysis);
  if (returnCode!=SUCCESS_ID) {
    SEMANTICANALYSISLOGINIT;
    LERROR << "KnowledgeBasedSemanticRoleLabeler: failed to dump data to temporary file";
    return returnCode;
  }

  QString conllInput;

  if (m_d->m_temporaryFileMetadata.isEmpty())
  {
    QString fileName = QString::fromUtf8(metadata->getMetaData("FileName").c_str());
    QString inputFilename;
    if (!m_d->m_inputSuffix.isEmpty())
    {
      inputFilename = fileName+ m_d->m_inputSuffix;
    }
    QFile inputFile(inputFilename);
    inputFile.open(QIODevice::ReadOnly);
    conllInput = QString::fromUtf8(inputFile.readAll().constData());
    inputFile.close();
  }
  else
  {
    if (!temporaryFile->open())
    {
      SEMANTICANALYSISLOGINIT;
      LERROR << "KnowledgeBasedSemanticRoleLabeler: unable to open temporary file after dumping CoNLL data to it"<< temporaryFile->fileName();
      LERROR << "KnowledgeBasedSemanticRoleLabeler: keep (do not auto remove) it for debug purpose." ;
      temporaryFile->setAutoRemove(false);
      return CANNOT_OPEN_FILE_ERROR;
    }
    conllInput = QString::fromUtf8(temporaryFile->readAll().constData());
#ifdef DEBUG_LP
    temporaryFile->setAutoRemove(false);
    SEMANTICANALYSISLOGINIT;
    LDEBUG << "KnowledgeBasedSemanticRoleLabeler: keeping temporary file after dumping CoNLL data to it for debugging"<< temporaryFile->fileName();
#endif
    temporaryFile->close();
  }

  // Run the semantic role labeller
  PyObject* callResult = PyObject_CallMethod(m_d->m_instance, "annotate", "ss",
                                             conllInput.toUtf8().constData(),
                                             metadata->getMetaData("Lang").c_str()
                                            );
  if (callResult == nullptr)
  {
    failure_during_call_of_the_annotate_method_on(conllInput);
  }

  // Display the SRL result
  const char* result = PyUnicode_AsUTF8(callResult);
  if (result == NULL)
  {
    SEMANTICANALYSISLOGINIT;
    LERROR << "Cannot convert result item to string";
    PyErr_Print();
    Py_Exit(1);
  }
#ifdef DEBUG_LP
  LDEBUG << "Python result is:" << result;
#endif
  if (m_d->m_temporaryFileMetadata.isEmpty())
  {
    QString outputFilename;
    if (!m_d->m_outputSuffix.isEmpty())
    {
      QString fileName = QString::fromUtf8(metadata->getMetaData("FileName").c_str());
      outputFilename = fileName + m_d->m_outputSuffix;
    }
    QFile outputFile(outputFilename);
    outputFile.open(QIODevice::WriteOnly);
    outputFile.write(result);
    outputFile.close();
  }
  else
  {
    if ( !temporaryFile->open() )
    {
      temporary_file_srl_not_open(temporaryFile);
      return CANNOT_OPEN_FILE_ERROR;
    }
    if (!temporaryFile->seek(0))
    {
      SEMANTICANALYSISLOGINIT;
      LERROR << "KnowledgeBasedSemanticRoleLabeler: unable to seek to the beginning of temporary file"<< temporaryFile->fileName();
      LERROR << "KnowledgeBasedSemanticRoleLabeler: keep (do not auto remove) it for debug purpose." ;
      temporaryFile->setAutoRemove(false);
      return UNKNOWN_ERROR;
    }
    if (temporaryFile->write(result) == -1)
    {
      SEMANTICANALYSISLOGINIT;
      LERROR << "KnowledgeBasedSemanticRoleLabeler: unable to write SRL result to temporary file"<< temporaryFile->fileName();
      LERROR << "KnowledgeBasedSemanticRoleLabeler: keep (do not auto remove) it for debug purpose." ;
      temporaryFile->setAutoRemove(false);
      return UNKNOWN_ERROR;
    }
    temporaryFile->close();
  }
  Py_DECREF(callResult);
  // Import the CoNLL result
  returnCode=m_d->m_loader->process(analysis);
  if (returnCode != SUCCESS_ID)
  {
    failed_to_load_data_from_temporary_file(temporaryFile);
    return returnCode;
  }

  return returnCode;
}

} //namespace SemanticAnalysis
} // namespace LinguisticProcessing
} // namespace Lima
