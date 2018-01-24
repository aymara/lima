/************************************************************************
 *
 * @file       limaSimpleClient.h
 * @author     Romaric Besancon (romaric.besancon@cea.fr)
 * @date       Fri Dec 15 2017
 * copyright   Copyright (C) 2017 by CEA - LIST
 * Project     LIMA
 * 
 * @brief      simple wrapper of LIMA analyzer with a limited interface, using default configuration
 * 
 * 
 ***********************************************************************/

#ifndef LIMASIMPLECLIENT_H
#define LIMASIMPLECLIENT_H

#include "common/LimaCommon.h"

#ifdef WIN32

#ifdef LIMA_LP_EXPORTING
#define LIMA_LP_EXPORT    __declspec(dllexport)
#else
#define LIMA_LP_EXPORT    __declspec(dllimport)
#endif


#else // Not WIN32

#define LIMA_LP_EXPORT

#endif

#include <iostream>

namespace Lima {

// internal delegate class to implement the API
class LimaSimpleClientDelegate;

class LIMA_LP_EXPORT LimaSimpleClient
{
public:
  LimaSimpleClient(); 
  ~LimaSimpleClient();
  
  /** 
   * initialize the LIMA analyzer with one language and one pipeline
   * 
   * @param language the language of the documents (3-letters ISO code)
   * @param pipeline the name of the pipeline to initialize
   */
  void initialize(const std::string& language, const std::string& pipeline);
    
  /** 
   * analyze a text, encoded in UTF-8. The text is supposed to be in the language with which LIMA
   * has been initialized. The analysis will be performed using the pipeline given at the
   * initialization.
   * 
   * @param text the text to analyze, in UTF-8.
   * 
   * @return a text string containing the result of the analysis (the format of the result depends
   * on the pipeline)
   */
  std::string analyze(const std::string& text);
  
private:
  LimaSimpleClientDelegate* m_delegate;
};

} // end namespace

#endif
