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
 * \file    Threads.cpp
 * \author  Jocelyn Vernay
 * \date    Wed, Sep 06 2017
 * 
 */

#include "Threads.h"
#include "LimaGuiCommon.h"
#include "LimaGuiApplication.h"
#include "ConllListModel.h"

#include "common/LimaCommon.h"


namespace Lima 
{
namespace Gui 
{

LimaGuiThread::LimaGuiThread(LimaGuiApplication* app) : QThread() 
{
  m_application = app;
  connect(this, SIGNAL(finished()), this, SLOT(deleteLater()));
}

///
///
///
///
///

AnalysisThread::AnalysisThread(LimaGuiApplication* app) : LimaGuiThread(app) 
{

}

AnalysisThread::AnalysisThread(LimaGuiApplication *app, const QString& s) : LimaGuiThread(app) 
{
  m_text = s;
}

void AnalysisThread::run() 
{
  if (m_application->available()) 
  {
    std::stringstream buffer;
//     std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());
    std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());

    m_application->setOut(&out);
    m_application->analyze(m_text);

    // reset app out
    m_application->setOut(&std::cout);
    m_application->setTextBuffer(buffer.str());
//     std::cout.rdbuf(old_cout);
    std::cerr.rdbuf(old_cerr);

    notifyView();
  }
  else 
  {
    ANALYSISTHREADLOGINIT;
    LINFO << ("Can't analyze : Analyzer is not available.");
  }
}

void AnalysisThread::notifyView()
{
  ANALYSISTHREADLOGINIT;
  LDEBUG << "AnalysisThread::notifyView";
  // create model from out.str()
  QObject* view;
  if (m_resultView)
  {
    view = m_resultView;
  }
  else 
  {
    view = m_application->getQmlObject("resultView"); // specific name
  }
  if (view) 
  {
//      related: https://stackoverflow.com/questions/27092756/call-qml-function-from-c-with-another-qml-object-as-parameter

    QString qstr(out.str().c_str());
    LDEBUG << "AnalysisThread::notifyView invoke displayResults with" 
            << m_text << "and" << qstr;
    QMetaObject::invokeMethod(view, 
                              "displayResults", 
                              Q_ARG(QVariant, QVariant::fromValue(m_text)), 
                              Q_ARG(QVariant, QVariant::fromValue(qstr)));
//    QMetaObject::invokeMethod(view, "tableUp", Q_ARG(QVariant, QVariant::fromValue(clmodel)));
  }
  else 
  {
    ANALYSISTHREADLOGINIT;
    LERROR << "Error : No result view specified.";
  }
}

void AnalysisThread::setText(const QString& s) 
{
  m_text = s;
}

void AnalysisThread::setName(const std::string& name) 
{
  m_name = name;
}

void AnalysisThread::setResultView(QObject* o) 
{
  m_resultView = o;
}

//////

InitializeThread::InitializeThread(LimaGuiApplication* app) : LimaGuiThread(app) 
{
}

void InitializeThread::run() 
{
  std::stringstream buffer;
  /// This won't work though
  // std::streambuf* old_cout = std::cout.rdbuf(buffer.rdbuf());
  // std::streambuf* old_cerr = std::cerr.rdbuf(buffer.rdbuf());
  
  //  for (int i = 0; i < 200000000; i++) {
  //    m_application->setTextBuffer("Hello patient n°" + std::to_string(i) + "!");
  //  }
  
  if (m_application->initializeLimaAnalyzer())
  {
    
    //  m_application->setTextBuffer(buffer.str());
    
    m_application->setAnalyzerState(true);
  }
  // restore previous streams
  // std::cout.rdbuf(old_cout);
  // std::cerr.rdbuf(old_cerr);
}

} // END namespace Gui
} // END namespace Lima
