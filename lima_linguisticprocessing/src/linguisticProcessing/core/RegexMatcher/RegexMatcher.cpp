/*
    Copyright 2002-2013 CEA LIST

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
/** @brief      A process unit able to match regex against analysed text
 * 
 * @file        RegexMatcher.cpp
 * @author      Gael de Chalendar (Gael.de-Chalendar@cea.fr) 

 *              Copyright (c) 2011 by CEA LIST
 * @date        Created on Nov, 1 2011
 * @version     $Id:  $
 *
 */

#include "RegexMatcher.h"

#include "common/time/timeUtilsController.h"
#include "common/misc/Exceptions.h"
#include "common/Data/strwstrtools.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "common/MediaticData/mediaticData.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/LinguisticProcessors/LimaStringText.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"

#include <QtCore/QRegExp>

#include <vector>

#include <string>
#include <queue>
#include <set>

// To use tuples
#include <boost/tuple/tuple.hpp>
//Comparison operators can be included with:
#include <boost/tuple/tuple_comparison.hpp>
// To use tuple input and output operators,
#include <boost/tuple/tuple_io.hpp>

typedef boost::tuple< size_t, size_t, QString, QString > RegexMatch;

using namespace std;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::Common::Misc;
using namespace Lima::Common::AnnotationGraphs;

#define REGEXREPLACERLOGINIT LOGINIT("LP::RegexReplacer")


namespace Lima
{
namespace LinguisticProcessing
{
namespace RegexMatching
{

class RegexMatcherPrivate
{
public:
  RegexMatcherPrivate() {}
  virtual ~RegexMatcherPrivate() {}

  bool checkGraphIsString(const LinguisticAnalysisStructure::AnalysisGraph* anagraph) const;
  
  MediaId m_language;
  // map between a regex and the label it marks
  QMap< QString, QString > m_regexes;
};

SimpleFactory<MediaProcessUnit,RegexMatcher> tokenizerFactory(REGEXREPLACER_CLASSID);

RegexMatcher::RegexMatcher() :
  m_d(new RegexMatcherPrivate())
{}

RegexMatcher::~RegexMatcher()
{
  delete m_d;
}

void RegexMatcher::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)

{
  TOKENIZERLOGINIT;
  m_d->m_language=manager->getInitializationParameters().media;

  try
  {
    std::map <std::string, std::string >& regexes = unitConfiguration.getMapAtKey("regexes");
    for (std::map <std::string, std::string >::const_iterator it = regexes.begin(); it != regexes.end(); it++)
    {
      m_d->m_regexes.insert( QString::fromUtf8 ((*it).first.c_str()), QString::fromUtf8((*it).second.c_str()) );
    }
  }
  catch (NoSuchMap& )
  {
    LERROR << "no map 'regexes' in RegexReplacer group configuration (language="
    << (int) m_d->m_language << ")";
    throw InvalidConfiguration();
  }
  // when input XML file is syntactically wrong
  catch (XmlSyntaxException exc)
  {
    std::ostringstream mess;
    mess << "XmlSyntaxException at line "<<exc._lineNumber<<" cause: ";
    switch (exc._why)
    {
        case XmlSyntaxException::SYNTAX_EXC : mess << "SYNTAX_EXC"; break;
        case XmlSyntaxException::NO_DATA_EXC : mess << "NO_DATA_EXC"; break;
        case XmlSyntaxException::DOUBLE_EXC : mess << "DOUBLE_EXC"; break;
        case XmlSyntaxException::FWD_CLASS_EXC : mess << "FWD_CLASS_EXC"; break;
        case XmlSyntaxException::MULT_CLASS_EXC : mess << "MULT_CLASS_EXC"; break;
        case XmlSyntaxException::EOF_EXC : mess << "EOF_EXC"; break;
        case XmlSyntaxException::NO_CODE_EXC : mess << "NO_CODE_EXC"; break;
        case XmlSyntaxException::BAD_CODE_EXC : mess << "BAD_CODE_EXC"; break;
        case XmlSyntaxException::NO_CLASS_EXC : mess << "NO_CLASS_EXC"; break;
        case XmlSyntaxException::UNK_CLASS_EXC : mess << "UNK_CLASS_EXC"; break;
        case XmlSyntaxException::INT_ERROR_EXC : mess << "INT_ERROR_EXC"; break;
        case XmlSyntaxException::INV_CLASS_EXC : mess << "INV_CLASS_EXC"; break;
        default: mess << "??";
    }
    LERROR << mess.str();
    throw InvalidConfiguration();
  }
  catch (std::exception &exc)
  {
    // @todo remove all causes of InfiniteLoopException
    LERROR << exc.what();
    throw InvalidConfiguration();
  }

}

LimaStatusCode RegexMatcher::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("RegexReplacer");
  REGEXREPLACERLOGINIT;
  LINFO << "start RegexMatcher process";
  AnalysisGraph* anagraph = static_cast< AnalysisGraph* >(analysis.getData("AnalysisGraph"));
  if (anagraph == 0) {
    LERROR << "no AnalysisGraph available for RegexMatcher ! abort";
    return MISSING_DATA;
  }
  if (!m_d->checkGraphIsString(anagraph)) {
    LERROR << "AnalysisGraph is not a string ! abort";
    return UNKNOWN_ERROR;
  }
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    annotationData=new AnnotationData();
    anagraph->populateAnnotationGraph(annotationData, "AnalysisGraph");
    if (static_cast<AnalysisGraph*>(analysis.getData("PosGraph")) != 0)
    {
      static_cast<AnalysisGraph*>(analysis.getData("PosGraph"))->populateAnnotationGraph(annotationData, "PosGraph");
    }
    analysis.setData("AnnotationData",annotationData);
  }
  
  std::map< size_t, RegexMatch > matches;
  LinguisticGraph* graph = anagraph->getGraph();
  
  VertexTokenPropertyMap tokenMap = get( vertex_token, *graph );
  VertexDataPropertyMap dataMap = get( vertex_data, *graph );

  LimaStringText* originalText=static_cast<LimaStringText*>(analysis.getData("Text"));
  std::string text = Common::Misc::limastring2utf8stdstring(*originalText);
  QMap< QString, QString >::const_iterator reit, reit_end;
  reit = m_d->m_regexes.begin(); reit_end = m_d->m_regexes.end();
  std::string::const_iterator start = text.begin();
  for (auto reit = m_d->m_regexes.begin(); reit != m_d->m_regexes.end(); reit++)
  {
    QRegExp re(reit.key());
    QString& tstatus = reit.value();
    
    int position = 0;
    while ((position = re.indexIn(*originalText, position)) != -1)  
    {
      QString matchedString = originalText->mid(position, re.matchedLength());
#ifdef DEBUG_LP
      LDEBUG << "Matched '" << matchedString << "' at " << position << " as " << tstatus;
#endif
      RegexMatch match = boost::make_tuple(position+1, matchedString.size(), matchedString, tstatus);
      matches.insert(std::make_pair(position+1, match));

      position += re.matchedLength();
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "Matching finished. Updating graph";
#endif
  // must ensure that we handle only one regex on a given token

  if (!matches.empty())
  {
    std::map< size_t, RegexMatch >::const_iterator matchesIt = matches.begin();
    size_t currentMatchPosition = (*matchesIt).second.get<0>();
    size_t currentMatchLength = (*matchesIt).second.get<1>();
    QString currentMatchString = (*matchesIt).second.get<2>();
    QString currentMatchStatus = (*matchesIt).second.get<3>();

    LinguisticGraphVertex v = anagraph->firstVertex();
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    for (boost::tie(outItr,outItrEnd)=boost::out_edges(v,*graph); outItr!=outItrEnd; outItr++)
    {
      v=target(*outItr,*graph);
      break;
    }

    // hypothesis : the graph is a string of tokens, there is only one path
    while (matchesIt != matches.end() && v != anagraph->lastVertex())
    {
      Token* token=get(vertex_token,*graph,v);
      uint64_t tokenPosition = token->position();
#ifdef DEBUG_LP
      LDEBUG << "Working on current match '" << currentMatchString << "' (" << currentMatchPosition << ", " << currentMatchLength << ") and token" << v << "at position" << tokenPosition;
#endif
      while (v != anagraph->lastVertex() && tokenPosition < currentMatchPosition)
      {
        for (boost::tie(outItr,outItrEnd)=boost::out_edges(v,*graph); outItr!=outItrEnd; outItr++)
        {
          v=target(*outItr,*graph);
          break;
        }
        if (v!= anagraph->lastVertex())
        {
          token=get(vertex_token,*graph,v);
          tokenPosition = token->position();
        }
      }
        
#ifdef DEBUG_LP
        LDEBUG << "current token ("<<v<<", "<<token->stringForm()<<") position is: " << tokenPosition << " ; current match position is: " << currentMatchPosition;
#endif
        // current token is at current match position
      if (tokenPosition == currentMatchPosition)
      {
#ifdef DEBUG_LP
        LDEBUG << "    current match found on token on token" << v << "at position" << tokenPosition << ". Now filling mach vertices" ;
#endif
        std::vector< LinguisticGraphVertex > matchVertices;
        matchVertices.push_back(v);
        // get all tokens inside current match
        LinguisticGraphVertex next=v;
        Token* nextToken = 0;
        uint64_t nextTokenPosition = std::numeric_limits<uint64_t>::max();
        for (boost::tie(outItr,outItrEnd)=boost::out_edges(next,*graph); outItr!=outItrEnd; outItr++)
        {
          next=target(*outItr,*graph);
          nextToken=get(vertex_token,*graph,next);
          nextTokenPosition = nextToken!=0?nextToken->position():0;
          break;
        }
        
        while (next != anagraph->lastVertex() && nextTokenPosition < (currentMatchPosition + currentMatchLength))
        {
#ifdef DEBUG_LP
          LDEBUG << "        next token ("<<next<<") position is: " << nextTokenPosition;
#endif
          matchVertices.push_back(next);
          for (boost::tie(outItr,outItrEnd)=boost::out_edges(next,*graph); outItr!=outItrEnd; outItr++)
          {
            next=target(*outItr,*graph);
            nextToken=get(vertex_token,*graph,next);
            nextTokenPosition = nextToken!=0?nextToken->position():0;
            break;
          }
        }
        std::vector< LinguisticGraphVertex >::const_iterator matchVerticesIt, matchVerticesIt_end;
        matchVerticesIt = matchVertices.begin(); matchVerticesIt_end = matchVertices.end();

#ifdef DEBUG_LP
        LDEBUG << "Match Found. Vertices are: ";
        for (; matchVerticesIt != matchVerticesIt_end; matchVerticesIt++)
        {
          LDEBUG << (*matchVerticesIt) << ", ";
        }
        LDEBUG;
#endif
        if (matchVertices.empty()) continue;

        // before  = get vertex before first match vertex
        LinguisticGraphInEdgeIt inIt, inIt_end;
        boost::tie (inIt, inIt_end) = boost::in_edges(*matchVertices.begin(), *graph);
        LinguisticGraphVertex before = boost::source(*inIt, *graph);
        // remove all before out_edges
        for (; inIt != inIt_end; inIt++)
        {
          boost::remove_edge(*inIt, *graph);
        }
        // after = get vertex after last match vertex
        LinguisticGraphOutEdgeIt outIt, outIt_end;
        boost::tie (outIt, outIt_end) = boost::out_edges(*matchVertices.rbegin(), *graph);
        LinguisticGraphVertex after = boost::target(*outIt, *graph);
        if (outIt != outIt_end) after = boost::target(*outIt, *graph);
        // remove all after in_edges
        for (; outIt != outIt_end; outIt++)
        {
          boost::remove_edge(*outIt, *graph);
        }
        // create newvertex, with new fulltoken
        StringsPoolIndex form=(Common::MediaticData::MediaticData::changeable().stringsPool(m_d->m_language))[currentMatchString];
        Token* newToken=new Token( form, currentMatchString, currentMatchPosition, currentMatchLength );
        newToken->setStatus(TStatus());
        newToken->status().setDefaultKey(currentMatchStatus);
        MorphoSyntacticData* newData=new MorphoSyntacticData();
        // add newvertex
        LinguisticGraphVertex newVertex = boost::add_vertex(*graph);
        tokenMap[newVertex]=newToken;
        dataMap[newVertex]=newData;
        newToken-> setPosition(currentMatchPosition);
        // create edge before -> newvertex
        boost::add_edge(before, newVertex, *graph);
        // create edge newvertex -> before
        boost::add_edge(newVertex, after, *graph);
        AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
        annotationData->addMatching("AnalysisGraph", newVertex, "annot", agv);
        annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("AnalysisGraph"), static_cast< uint64_t >(newVertex));

        GenericAnnotation ga(matchVertices);
        annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("regexmatches"), ga);
        v = after;
        matchesIt++;
        if (matchesIt != matches.end())
        {
          currentMatchPosition = (*matchesIt).second.get<0>();
          currentMatchLength = (*matchesIt).second.get<1>();
          currentMatchString = (*matchesIt).second.get<2>();
          currentMatchStatus = (*matchesIt).second.get<3>();
#ifdef DEBUG_LP
          LDEBUG << "    current match is now: " << currentMatchPosition << currentMatchLength << currentMatchString << currentMatchStatus;
#endif
        }
        else
        {
#ifdef DEBUG_LP
          LDEBUG << "    no more match";
#endif
          break;
        }
      }
      else // tokenPosition > currentMatchPosition 
      {
#ifdef DEBUG_LP
        LDEBUG << "Skiping matches up to after next token position: " << tokenPosition << currentMatchPosition << currentMatchLength;
#endif
        // advances current match until we find one which starts after the end of the last token which is in the current match
        while ((matchesIt != matches.end()) && ((currentMatchPosition ) <= tokenPosition))
        {
          matchesIt++;
          if (matchesIt == matches.end()) break;
          currentMatchPosition = (*matchesIt).second.get<0>();
          currentMatchLength = (*matchesIt).second.get<1>();
          currentMatchString = (*matchesIt).second.get<2>();
          currentMatchStatus = (*matchesIt).second.get<3>();
        }
#ifdef DEBUG_LP
        if (matchesIt != matches.end())
          LDEBUG << "    current match is now: " << currentMatchPosition << currentMatchLength << currentMatchString << currentMatchStatus;
        else
          LDEBUG << "    no more match";
#endif
      }
    }
  }
#ifdef DEBUG_LP
  LDEBUG << "DONE";
#endif
  return SUCCESS_ID;
}

bool RegexMatcherPrivate::checkGraphIsString(const LinguisticAnalysisStructure::AnalysisGraph* anagraph) const
{
  // there is a first vertex, a last vertex and at least one token vertex
  if (boost::num_vertices(*anagraph->getGraph()) <= 2) return false;
  
  LinguisticGraphVertex next = anagraph->firstVertex();
  LinguisticGraphVertex last = anagraph->lastVertex();
  while (next != last)
  {
    // each vertex must have exactly one next vertex
    if (boost::out_degree(next, *anagraph->getGraph()) != 1) return false;
    LinguisticGraphOutEdgeIt outItr,outItrEnd;
    boost::tie(outItr,outItrEnd)=boost::out_edges(next,*anagraph->getGraph()); 
    next=target(*outItr,*anagraph->getGraph());
  }
  return true;
}

} //namespace RegexReplace
} // namespace LinguisticProcessing
} // namespace Lima
