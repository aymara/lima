// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/************************************************************************
 *
 * @file     Event.h
 * @author   Faiza GARA
 * @date     April 2008
 * copyright Copyright (C) 2007 by CEA LIST
 * version   $Id:
 *
 * @brief
 *
 *
 ***********************************************************************/

#ifndef EVENT_H
#define EVENT_H

#include "EventAnalysisExport.h"
#include "EventParagraph.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

#include <deque>
#include <queue>
#include <iostream>

namespace Lima {
namespace LinguisticProcessing
{

namespace EventAnalysis
{

/**
 * This class represents a list of elements, that are pointers on
 * polymmorphic annotations that can be datatype property annotations
 * or object property annotations
 *
 **/
class LIMA_EVENTANALISYS_EXPORT Event : public std::vector< EventParagraph* >
{
public:
    Event();
    virtual ~Event();

    bool hasFragment() const;

    void addParagraph(Paragraph *,bool,bool,Common::AnnotationGraphs::AnnotationData*, std::string graphId,LinguisticGraph* graph);
    //@{ binary input/output
    virtual void read(std::istream& file);
    virtual void write(std::ostream& file) const;
    //@}

    std::string toString(std::string parentURI,uint64_t index) const;

    void compute_entities_weight(std::map<Common::MediaticData::EntityType,unsigned short>,Common::AnnotationGraphs::AnnotationData*, std::string graphId);
    friend LIMA_EVENTANALISYS_EXPORT std::ostream& operator << (std::ostream&, const Event&);

    void compute_main_entities();

    Common::MediaticData::EntityType getEntityType(LinguisticGraphVertex,Common::AnnotationGraphs::AnnotationData*, std::string graphId) const;

    void setDate(std::pair<Common::MediaticData::EntityType,std::pair<std::string,LinguisticGraphVertex> >);

    uint64_t  get_weight() const;
    void setWeight(uint64_t w) { m_entities_weight=w; }

    void setMain();
    bool getMain() const;

    bool has_entity(Common::MediaticData::EntityType) const;
private:
  std::pair <Common::MediaticData::EntityType,std::pair<std::string,LinguisticGraphVertex> > m_date;
  uint64_t m_entities_weight;
  bool m_main;

};
//**********************************************************************
// inline functions
// **********************************************************************

inline void  Event::setMain()
{
  m_main=true;
}

inline bool Event::getMain() const
{
  return m_main;
}
inline uint64_t  Event::get_weight() const
{
  return m_entities_weight;
}

inline  void Event::setDate(std::pair<Common::MediaticData::EntityType,std::pair<std::string,LinguisticGraphVertex> > date){m_date=date;}

inline bool Event::hasFragment() const { return (!empty()); }


}
}
}// namespace Lima
#endif
