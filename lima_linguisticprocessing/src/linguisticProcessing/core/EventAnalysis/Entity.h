// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 * @file     Entity.h
 * @author   Faiza GARA
 * @date     October 2007
 * @version  
 * copyright Copyright (C) 2007 by CEA LIST
 *
 * @brief
 *
 ***********************************************************************/

#ifndef ENTITY_H
#define ENTITY_H

#include "EventAnalysisExport.h"
#include "linguisticProcessing/core/Automaton/EntityFeatures.h"


namespace Lima {
namespace LinguisticProcessing {
namespace EventAnalysis {


class LIMA_EVENTANALISYS_EXPORT Entity 
{
public:

  Entity(const uint64_t position,
         const uint64_t length,
         Lima::LinguisticProcessing::Automaton::EntityFeatures features);
  Entity();


  uint64_t getPosition(void) const {return m_position;};
  uint64_t getLength(void) const {return m_length;};
  uint64_t getStart(){return m_position;}
  uint64_t getEnd(){return (m_position+m_length-1);}
  bool getMain(void) const {return m_main;};
  Lima::LinguisticProcessing::Automaton::EntityFeatures getFeatures(void) const {return m_features;};
  
  std::map<std::string,std::string> getMapFeatures(void) const;

  void setPosition(uint64_t position){m_position=position;}
  void setLength(uint64_t length){m_length=length;}
  void setFeatures(Lima::LinguisticProcessing::Automaton::EntityFeatures features){m_features=features;}
  void setMain(bool main) {m_main=main;}
  
  
  virtual ~Entity();

  virtual void clear() {};

  virtual Entity* clone() const;


  /** get a string of the OntoAnnotation for output function */
  virtual std::string getOutputUTF8String(void) const;

  /** function used to identify an annotation in a text (used in Dumper) */
  virtual std::string getIdUTF8String() const;

  /** function used to print the annotation in a string*/
  virtual std::string toString(std::string parentURI) const;

  //@{ binary input/output
  virtual void read(std::istream& file);
  virtual void write(std::ostream& file) const;


  LIMA_EVENTANALISYS_EXPORT friend std::ostream& operator << (std::ostream&, const Entity&);



  /**
    * @brief redefine equality
    */
  bool operator==(const Entity&) const;

  /**
    * @brief redefine inequality as negation of equality.
    * @author Benoit Mathieu
    */
  bool operator!=(const Entity&) const;



 protected:

  uint64_t m_position;
  uint64_t m_length;
  bool m_main;
  Lima::LinguisticProcessing::Automaton::EntityFeatures  m_features;


};


inline Entity* Entity::clone() const
{
  return new Entity(*this);
}



} // namespace EventAnalysis
} // namespace LinguisticProcessing
} // namespace Lima

#endif
