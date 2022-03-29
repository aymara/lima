/*
    Copyright 2017 CEA LIST

    This file is part of LIMA.

    LIMA is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    LIMA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with LIMA.  If not, see <http://www.gnu.org/licenses/>
*/
/**
 * \file    LimaGuiApplication.h
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 *
 */

#ifndef LIMA_GUI_APPLICATION_H
#define LIMA_GUI_APPLICATION_H

#include "LimaGuiExport.h"

#include "linguisticProcessing/client/LinguisticProcessingClientFactory.h"
#include "common/Handler/AbstractAnalysisHandler.h"

#include <QObject>
#include <QString>

class QCommandLineParser;

namespace Lima {
namespace Gui {

typedef std::shared_ptr<
                        LinguisticProcessing::AbstractLinguisticProcessingClient
                        > LimaClientSharedPtr;

/// \class LimaGuiFile
/// \brief A simple structure to hold information relative to files
///
struct LimaGuiFile
{
  std::string name; ///< name of the file. (<name>.<ext>, without the full path)
  std::string url; ///< path of the file (relative or absolute)
  std::string text; ///< text content of the file (after extraction)
  bool modified = false; ///< This indicates whether the file has been modified inside the applicaiton. Obsolete.
};

class LimaGuiThread;
namespace Config
{
  class LimaConfiguration;
}

typedef std::shared_ptr<Config::LimaConfiguration> LimaConfigurationSharedPtr;

///
/// \class LimaGuiApplication
/// \brief Main class of Lima Gui.
///
class LimaGuiApplication : public QObject
{
  Q_OBJECT

  /// BUFFER PROPERTIES EXPOSED TO QML

  Q_PROPERTY(QString fileContent MEMBER m_fileContent READ fileContent WRITE setFileContent)
  Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged READ text WRITE setText)
  Q_PROPERTY(QString fileName MEMBER m_fileName READ fileName WRITE setFileName)
  Q_PROPERTY(QString fileUrl MEMBER m_fileUrl READ fileUrl WRITE setFileUrl)
  Q_PROPERTY(bool ready MEMBER m_analyzerAvailable READ available WRITE setAnalyzerState NOTIFY readyChanged)
  Q_PROPERTY(QString console MEMBER m_consoleOutput NOTIFY consoleChanged READ consoleOutput WRITE setConsoleOuput)
  Q_PROPERTY(QStringList languages MEMBER m_languages NOTIFY languagesChanged READ languages WRITE setLanguages)
  Q_PROPERTY(QString language MEMBER m_language NOTIFY languageChanged READ language WRITE setLanguage)
  Q_PROPERTY(QStringList configs MEMBER m_configs NOTIFY configsChanged)
  Q_PROPERTY(QStringList pipelines MEMBER m_pipelines NOTIFY pipelinesChanged READ pipelines)
  Q_PROPERTY(QString pipeline MEMBER m_pipeline NOTIFY pipelineChanged READ pipeline WRITE setPipeline)

public:
  ///
  /// \brief LimaGuiApplication
  /// \param parent
  ///
  LimaGuiApplication(const QCommandLineParser& options, QObject* parent = 0);

  /// \brief open file in application and add a new entry to open_files
  /// \param filepath is the path of the file
  Q_INVOKABLE bool openFile(const QString& filepath);

  ///
  /// \brief used when multiple files are selected in the file dialog
  Q_INVOKABLE bool openMultipleFiles(const QStringList& urls);

  ///
  /// \brief close file registered in open files, save if modified and requested
  Q_INVOKABLE void closeFile(const QString& filename, bool save = false);

  /// \brief search amongst open files for the requested file
  /// \param name : the name of the file
  /// \return a reference on the file if it is open, otherwise nullptr
  LimaGuiFile* getFile(const std::string& name);

  /// \brief if the file is open, set buffers file content, name and url to selected file's
  /// \brief opening a file sets is as selected
  Q_INVOKABLE bool selectFile(const QString& filename);

  /// ANALYZER METHODS

  ///
  /// \brief This is the main analysis method, ideally called in a thread
  void analyze(const QString&);

  ///
  /// \brief This creates the thread for the previous function
  void beginNewAnalysis(const QString&, QObject* target = 0);

  /// Those are all gui functions for ::analyze()

  /// analyze raw text
  /// \brief An open file <that was edited but not saved> (it may not even be the case, all open files may be as well treated as text) will be passed to this function instead of analyzeFileFromUrl (then analyzeFile may as well call analyzeText)
  Q_INVOKABLE void analyzeText(const QString& content, QObject* target = 0);

  ///
  /// \brief Will analyze a file from its name, assuming it's open
  Q_INVOKABLE void analyzeFile(const QString& filename, QObject* target = 0);

  ///
  /// \brief Analyze file directly from an url, without opening the file in the text editor ; (saved file content)
  Q_INVOKABLE void analyzeFileFromUrl(const QString& url, QObject* target = 0);

  Q_INVOKABLE void test();

  void configure();

  Q_INVOKABLE QString highlightNamedEntities(const QString& text);

  Q_INVOKABLE QStringList getNamedEntitiesList(const QString& text);

  /// INITIALIZING

  ///
  /// \brief initialize Lima::m_analyzer
  bool initializeLimaAnalyzer();

  ///
  /// \brief for the moment, to reset Lima configuration, we can only reinstantiate m_analyzer
  bool resetLimaAnalyzer();

  /// THREADS MANAGEMENT

  ///
  /// \brief Sets the output stream for the next analysis. Usually, it will be an AnalysisThread ostream
  void setOut(std::ostream* o) { out = o; }

  friend class InitializeThread;

  void setTextBuffer(const std::string& str);

  /// actual qml console.
  /// we'll need to bind the qapplication with this class
  /// and then maybe we'll be able to access the related qml object from the root object
  /// https://stackoverflow.com/questions/9062189/how-to-modify-a-qml-text-from-c
  /// or
  /// https://stackoverflow.com/questions/35204281/use-signals-or-q-property-to-update-qml-objects
  void writeInConsole(const std::string& str);

  /// GETTERS
  ///
  LimaConfigurationSharedPtr configuration() const;
  QString fileContent() const;
  QString fileName() const;
  QString fileUrl() const;
  QString text() const;
  QString consoleOutput() const;
  QStringList languages() const;
  QString language() const;
  QStringList pipelines() const;
  QString pipeline() const;

  /// SETTERS
  ///
  void setFileContent(const QString& s);
  void setFileName(const QString& s);
  void setFileUrl(const QString& s);
  void setText(const QString& s);
  void setConsoleOuput(const QString& s);
  void setLanguages(const QStringList& l);
  void setLanguage(const QString& s);
  void setPipeline(const QString& s);

  /// ANALYZER STATE : to avoid simultaneous analysis

  /// is an analysis already underway ? if so, analyzer is unavailable (state = false)

  /// Those functions were meant to disable simultaneous analysis with the same analyser object
  /// Now that the analyzer is thread-safe, monitoring analyzer state is no longer necessary

  void toggleAnalyzerState();
  void setAnalyzerState(bool);
  bool available();

  /// QML OBJECTS REFERENCES

  /// This description should be in thread.h
  ///
  /// \brief The model is the analyzer, the view is the QML elements, and the Controller is this very class.
  /// Hence, the controller needs to be able to notify the view when the results are ready.
  /// This way the aim of these functions, but now analysis threads handles their own views.
  /// If need be, this will be able to reference a QML Object inside the C++ application.
  Q_INVOKABLE void registerQmlObject(QString, QObject*);
  Q_INVOKABLE QObject* getQmlObject(const QString&);

  void selectLimaConfiguration(const QString& name);

Q_SIGNALS:
  void textChanged();
  void consoleChanged();
  void readyChanged();

  void languagesChanged();
  void languageChanged();

  void pipelinesChanged();
  void pipelineChanged();

  void configsChanged();

  void error(const QString& error, const QString& details);

private:
  void loadLimaConfigurations();

  void addLimaConfiguration(LimaConfigurationSharedPtr newconfig);

  void createLimaConfiguration(const Config::LimaConfiguration& newconfig);

  void selectLimaConfiguration(int it);

  void setLimaConfiguration(const Config::LimaConfiguration& config);

  void updateAvailablePipelines();

  /// BUFFERS
  /// All those members are exposed to QML, as in they can be accessed from there.
  /// Qt requires a special syntax for this (Q_PROPERTY macros), thus we can either
  /// create members for this, like we have done here, or only rely on READ and WRITE
  /// functions like setters and getters. Depends on the behavior of the property.

  QString m_fileContent;
  QString m_fileName;
  QString m_fileUrl;
  QString m_text;
  QString m_consoleOutput;
  QStringList m_languages;
  QString m_language;
  QStringList m_pipelines;
  QString m_pipeline;
  QStringList m_formats;
  QStringList m_configs;
  bool m_analyzerAvailable = false;

  /// MEMBERS

  ///< buffers to access QML objects
  std::map<QString, QObject*> m_qmlObjects;

  ///< list of open files;
  std::vector<LimaGuiFile> m_openFiles;

  ///< LIMA analyzer
  LimaClientSharedPtr m_analyzer;
  LimaConfigurationSharedPtr m_configuration;

  std::map<QString, LimaClientSharedPtr> m_clients;

  ///< application analysis output stream
  std::ostream* out = &std::cout;

  /// GUI APPLICATION CONFIGURATION
  ///
  /// global, results, conll table, text, ... :
  /// - font
  /// - font size
  ///
  /// style, color themes ?
  ///
  /// workspace directory (where all files are created when running analysis)
  ///
  /// we could have a temp directory that would be wiped out when not needed
  ///
  std::map<QString, LimaConfigurationSharedPtr> m_configurations;

  const QCommandLineParser& m_options;
};

} // END namespace Gui
} // END namespace Lima

#endif // LIMA_GUI_APPLICATION_H
