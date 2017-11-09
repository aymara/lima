/*
    Copyright 2002-2017 CEA LIST

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

#include "ApproxStringMatcher.h"

#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/MediaticData/mediaticData.h"
#include "common/time/timeUtilsController.h"
#include "common/XMLConfigurationFiles/xmlConfigurationFileExceptions.h"
#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/Token.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/TStatus.h"
#include "linguisticProcessing/core/LinguisticAnalysisStructure/AnalysisGraph.h"
#include "linguisticProcessing/core/LinguisticResources/LinguisticResources.h"
#include "linguisticProcessing/core/AnalysisDict/AbstractDictionaryEntry.h"
#include "common/AbstractFactoryPattern/SimpleFactory.h"
#include "common/misc/fsaStringsPool.h"
#include "common/misc/AbstractAccessIterators.h"
#include "common/Data/strwstrtools.h"
#include <iostream>
#include <tre/regex.h>
#include <boost/regex.hpp>
//#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"

using namespace std;
using namespace Lima::Common::AnnotationGraphs;
using namespace Lima::Common::MediaticData;
using namespace Lima::Common::XMLConfigurationFiles;
using namespace Lima::LinguisticProcessing::LinguisticAnalysisStructure;
using namespace Lima::LinguisticProcessing::AnalysisDict;

namespace Lima
{
namespace LinguisticProcessing
{
namespace MorphologicAnalysis
{
typedef boost::basic_regex<wchar_t> wide_regex;

SimpleFactory<MediaProcessUnit,ApproxStringMatcher> ApproxStringMatcherFactory(APPROX_STRING_MATCHER_CLASSID);


std::ostream& operator<<(ostream& os, const Suggestion& suggestion)
{
  os << "Suggestion pos=(" << suggestion.startPosition << ","<< suggestion.endPosition<< ")"
     << " nbErr=" << suggestion.nb_error << std::endl;
  return os;
}

QDebug& operator<<(QDebug& os, const Suggestion& suggestion)
{
  os << "Suggestion pos=(" << suggestion.startPosition << ","<< suggestion.endPosition<< ")"
     << " nbErr=" << suggestion.nb_error;
  return os;
}

std::ostream& operator<<(ostream& os, const Solution& solution)
{
  os << "Solution: " << solution.suggestion
     << " vertices=";
  std::deque<LinguisticGraphVertex>::const_iterator it = solution.vertices.begin();
  if( it != solution.vertices.end() ) {
       os << *(it++);
  }
  for( ; it != solution.vertices.end() ; it++ ) {
       os << "," << *it;
  }
  os << Lima::Common::Misc::limastring2utf8stdstring(solution.form)
     << "(" << Lima::Common::Misc::limastring2utf8stdstring(solution.normalizedForm) << ")"
     << "start=" << solution.startPos << "length=" << solution.length
     << std::endl;
  return os;
}

QDebug& operator<<(QDebug& os, const Solution& solution)
{
  os << "Solution: " << solution.suggestion
     << " vertices=";
  std::deque<LinguisticGraphVertex>::const_iterator it = solution.vertices.begin();
  if( it != solution.vertices.end() ) {
       os << *it;
  }
  for( ; it != solution.vertices.end() ; it++ ) {
       os << "," << *it;
  }
  os << Lima::Common::Misc::limastring2utf8stdstring(solution.form)
     << "(" << Lima::Common::Misc::limastring2utf8stdstring(solution.normalizedForm) << ")"
     << "start=" << solution.startPos << "length=" << solution.length;
  return os;
}


ApproxStringMatcher::ApproxStringMatcher() :
    m_lexicon(0),
    m_sp(0),
    m_nbMaxNumError(0),
    m_nbMaxDenError(1),
    m_entityType(),
    m_entityGroupId()
{}

ApproxStringMatcher::~ApproxStringMatcher()
{
  // delete m_reader;
}

void ApproxStringMatcher::init(
  Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
  Manager* manager)
{
  MORPHOLOGINIT;
  
  MediaId language = manager->getInitializationParameters().media;
  m_sp=&Common::MediaticData::MediaticData::changeable().stringsPool(language);

  // Get groupId and Entity type in group
  Common::MediaticData::EntityGroupId foundGroup;
  try
  {
    std::string entityGroupName=unitConfiguration.getParamsValueAtKey("entityGoup");
    LimaString lsEntityGroupName = Lima::Common::Misc::utf8stdstring2limastring(entityGroupName);
    m_entityGroupId = Common::MediaticData::MediaticData::single().getEntityGroupId(lsEntityGroupName);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'entityGoup' in ApproxStringMatcher group for language " << (int) language;
    throw InvalidConfiguration();
  }
  try
  {
    std::string entityName=unitConfiguration.getParamsValueAtKey("entityName");
    LimaString lsentityName = Lima::Common::Misc::utf8stdstring2limastring(entityName);
    m_entityType = Common::MediaticData::MediaticData::single().getEntityType(lsentityName);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'entityGoup' in ApproxStringMatcher group for language " << (int) language;
    throw InvalidConfiguration();
  }
  
  // get dictionary of normalized forms
  string dico;
  try
  {
    dico=unitConfiguration.getParamsValueAtKey("dictionary");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in ApproxStringMatcher group for language " << (int) language;
    throw InvalidConfiguration();
  }

  // get max edit distance 
  try
  {
    std::string nbMaxErrorStr=unitConfiguration.getParamsValueAtKey("nbMaxNumError");
    std::istringstream iss(nbMaxErrorStr);
    iss >> m_nbMaxNumError;
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'nbMaxNumError' in ApproxStringMatcher group for language " << (int) language;
    throw InvalidConfiguration();
  }
  try
  {
    std::string nbMaxErrorStr=unitConfiguration.getParamsValueAtKey("nbMaxDenError");
    std::istringstream iss(nbMaxErrorStr);
    iss >> m_nbMaxDenError;
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'nbMaxDenError' in ApproxStringMatcher group for language " << (int) language;
    throw InvalidConfiguration();
  }

  AbstractResource* res=LinguisticResources::single().getResource(language,dico);
  AbstractAccessResource* lexicon = lexicon=static_cast<AbstractAccessResource*>(res);
  m_lexicon = lexicon->getAccessByString();

  // get generalization pattern 
  try
  {
    std::map <std::string, std::string >& regexes = unitConfiguration.getMapAtKey("generalization");
    for (std::map <std::string, std::string >::const_iterator it = regexes.begin(); it != regexes.end(); it++)
    {
      QString patternQ = QString::fromUtf8 ((*it).first.c_str());
      std::basic_string<wchar_t> patternWS = LimaStr2wcharStr(patternQ);
      QString substitutionQ = QString::fromUtf8 ((*it).second.c_str());
      std::basic_string<wchar_t> substitutionWS = LimaStr2wcharStr(substitutionQ);
      m_regexes.insert( std::pair<std::basic_string<wchar_t>,std::basic_string<wchar_t> >(patternWS,
                                    substitutionWS) );
    }
  }
  catch (NoSuchParam& )
  {
    LERROR << "no map 'generalization' in RegexReplacer group configuration fot language "
    << (int) language;
    throw InvalidConfiguration();
  }
}


LimaStatusCode ApproxStringMatcher::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("ApproxStringMatcher");
  MORPHOLOGINIT;
  LINFO << "starting process ApproxStringMatcher";

  AnalysisGraph* anagraph=static_cast<AnalysisGraph*>(analysis.getData("AnalysisGraph"));
  // initialize annotation data
  AnnotationData* annotationData = static_cast< AnnotationData* >(analysis.getData("AnnotationData"));
  if (annotationData==0)
  {
    LINFO << "ApproxStringMatcher::process no annotation data, creating and populating it";
    annotationData=new AnnotationData();
    analysis.setData("AnnotationData",annotationData);
  }
  anagraph->populateAnnotationGraph(annotationData, "AnalysisGraph");

  // initialize annotation data for SpecificEntity???
  if (annotationData==0)
  {
    LERROR << "ApproxStringMatcher::process: no AnnotationData, cannot store result";
    return INVALID_CONFIGURATION;
  }
  // see annotationData...
  if (annotationData->dumpFunction("SpecificEntity") == 0)
  {
    annotationData->dumpFunction("SpecificEntity", new Lima::LinguisticProcessing::SpecificEntities::DumpSpecificEntityAnnotation());
  }

  // Initalize list of suggestions, ordered by number of errors
  std::multimap<int,Solution> result;
  
  LinguisticGraphVertex currentVertex;
  currentVertex = anagraph->firstVertex();
  LinguisticGraph* g=anagraph->getGraph();
  for( ; currentVertex != anagraph->lastVertex() ; ) {
    Solution solution;
    matchApproxTokenAndFollowers(*g, currentVertex, anagraph->lastVertex(), solution);
    int len = solution.normalizedForm.length();
    if( solution.suggestion.nb_error <= (len*m_nbMaxNumError)/m_nbMaxDenError ) {
      createVertex(*g, anagraph->firstVertex(), anagraph->lastVertex(), solution, annotationData );

      result.insert(std::pair<int,Solution>(solution.suggestion.nb_error,solution));
      // skip vertices part of recognized pattern
      currentVertex =solution.vertices.back();
    }
    // following nodes
    LinguisticGraphOutEdgeIt outEdge,outEdge_end;
    boost::tie (outEdge,outEdge_end)=out_edges(currentVertex,*g);
    currentVertex =target(*outEdge,*g);
  }
  
  // remove double ?
  for (std::multimap<int, Solution>::iterator itr = result.begin(); itr != result.end(); ) {
    /* ... process *itr ... */

    /* Now, go skip to the first entry with a new key. */
    multimap<int, Solution>::iterator curr = itr;
    while (itr != result.end() && itr->first == curr->first) {
      Solution& solution = (*itr).second;
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::process() suggestion= " << solution;
#endif
       ++itr;
    }
  }  

#ifdef DEBUG_LP
  LDEBUG << "ending process ApproxStringMatcher";
#endif
  return SUCCESS_ID;
}

void ApproxStringMatcher::createVertex(
    LinguisticGraph& g, 
    LinguisticGraphVertex vStart,
    LinguisticGraphVertex vEnd,
    Solution& solution,
    AnnotationData* annotationData 
) const 
{
  MORPHOLOGINIT;
#ifdef DEBUG_LP
  LDEBUG << "ApproxStringMatcher::createVertex( solution=" << solution << ")";
#endif
  VertexTokenPropertyMap tokenMap=get(vertex_token,g);
  VertexDataPropertyMap dataMap = get(vertex_data, g);
  
  // create new vertex in analysis graph
  LinguisticGraphVertex newVertex = add_vertex(g);

  LinguisticGraphOutEdgeIt outEdge,outEdge_end;
  // Find previous vertex
  LinguisticGraphVertex previousVertex = vStart;
  for( ; previousVertex != vEnd ; ) {
    boost::tie (outEdge,outEdge_end)=out_edges(previousVertex,g);
    LinguisticGraphVertex nextVertex = target(*outEdge,g);
    if( nextVertex == solution.vertices.front() ) {
      break;
    }
    else {
      previousVertex = nextVertex;
    }
  }
  // Find next vertex
  boost::tie (outEdge,outEdge_end)=out_edges(solution.vertices.back(),g);
  LinguisticGraphVertex nextVertex = target(*outEdge,g);
  
  // remove edges 
  boost::remove_edge(previousVertex,solution.vertices.front(), g);
  boost::remove_edge(solution.vertices.back(),nextVertex, g);

  // replace edges
  bool success;
  LinguisticGraphEdge e;
  boost::tie(e, success) = add_edge(previousVertex, newVertex, g);
  boost::tie(e, success) = add_edge(newVertex, nextVertex, g);

 
  // Create specific entity annotation
  Lima::LinguisticProcessing::SpecificEntities::SpecificEntityAnnotation annot(solution.vertices, m_entityType,
                                 solution.form, solution.normalizedForm, solution.startPos, solution.length, *m_sp);
  // std::ostringstream oss;
  // ??? annot.dump(oss);

  // Create token for this vertex
  StringsPoolIndex form = annot.getString();
  StringsPoolIndex lemma = annot.getNormalizedString();
  StringsPoolIndex normalisation = annot.getNormalizedForm();

  Token* newToken = new Token(
      form,
      (*m_sp)[form],
      solution.startPos,
      solution.length);
  
  // specify status for token
  // TODO: duplicate status of head in fre and tail in eng?
  TStatus tStatus;
  tStatus.reset();
  tStatus.setStatus(T_ALPHA);
  tStatus.setAlphaCapital(T_CAPITAL);

  newToken->setStatus(tStatus);
  
  put(vertex_token,g,newVertex,newToken);
  put(vertex_data,g,newVertex,new MorphoSyntacticData());

  /*
  // TODO: create MorphoSyntacticData ?
  // all linguisticElements of this morphosyntacticData share common SE information
  LinguisticElement elem;
  elem.inflectedForm = newToken->form();            // (StringsPoolIndex)
  elem.lemma = newToken->form();                    // (StringsPoolIndex)
  elem.normalizedForm = annot.getNormalizedForm();  // StringsPoolIndex
  elem.type = SPECIFIC_ENTITY; // MorphoSyntacticType
  */
  
  AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
  //"AnalysisGraph"??? (replace graphId...)
  //annotationData->addMatching(graphId, newVertex, "annot", agv);
  //annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring(graphId), newVertex);
  annotationData->addMatching("AnalysisGraph", newVertex, "annot", agv);
  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("AnalysisGraph"), newVertex);
  tokenMap[newVertex] = newToken;
  // dataMap[newVertex] = newMorphData;
  GenericAnnotation ga(annot);
  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("SpecificEntity"), ga);
}

LimaStatusCode ApproxStringMatcher::matchExactTokenAndFollowers(
    LinguisticGraph& g, 
    LinguisticGraphVertex vStart,
    LinguisticGraphVertex vEnd,
    std::multimap<int,Suggestion>& result) const
{
  MORPHOLOGINIT;
  typedef Lima::Common::AccessSuperWordIterator WIt;
  VertexTokenPropertyMap tokenMap=get(vertex_token,g);

  // VertexDataPropertyMap dataMap=get(vertex_data,g);
    Token* currentToken=tokenMap[vStart];
    LimaString form;
    int max_length_element_in_lexicon = 49;
    // TODO: check if it is the case of start or end node in graph...
    if (currentToken==0)
      return SUCCESS_ID;
    int position = currentToken->position();
#ifdef DEBUG_LP
    LDEBUG << "ApproxStringMatcher::matchExactTokenAndFollowers() from token" << Lima::Common::Misc::limastring2utf8stdstring(currentToken->stringForm());
#endif
    // TODO: vérifier que vEndIt est le noeud 1 (sans token)
    LinguisticGraphVertex currentVertex = vStart;
    for( ; currentVertex != vEnd ; ) {
    //for( ; vStartIt != vEndIt ; vStartIt++ ) {
      currentToken=tokenMap[vStart];
      if (currentToken!=0)
      {
        // currentToken->status()?
#ifdef DEBUG_LP
        LDEBUG << "ApproxStringMatcher::matchExactTokenAndFollowers() append token" << Lima::Common::Misc::limastring2utf8stdstring(currentToken->stringForm());
#endif
        form.append(currentToken->stringForm());
#ifdef DEBUG_LP
        LDEBUG << "ApproxStringMatcher::matchExactTokenAndFollowers() form= "
               << Lima::Common::Misc::limastring2utf8stdstring(form);
#endif
      // get words in Lexicon with form as prefix
        std::pair<WIt,WIt>  wordsIt = m_lexicon->getSuperWords(form);
        for( ; wordsIt.first != wordsIt.second ; (wordsIt.first)++ ) 
        {
          Lima::LimaString word = *(wordsIt.first);
#ifdef DEBUG_LP
          LDEBUG << "ApproxStringMatcher::matchExactTokenAndFollowers() first superWords = "
                 << Lima::Common::Misc::limastring2utf8stdstring(word);
          LDEBUG << "ApproxStringMatcher::matchExactTokenAndFollowers(): compare to "
                 << Lima::Common::Misc::limastring2utf8stdstring(form) << "=" << word.compare(form);
#endif
          if( word.compare(form) == 0 ) {
            Suggestion suggestion={
              position,                     // startPsition
              position+form.length(),       // endPosition
              0                            // nb_error
              // std::deque<LinguisticGraphVertex>(1,currentVertex),                            // nb_error
              //m_lexicon->getIndex(form)
            };   // id of term in Lexicon
#ifdef DEBUG_LP
            LDEBUG << "ApproxStringMatcher::matchExactTokenAndFollowers() success: " << suggestion;
                  // << ", match_id=" << suggestion.match_id
//                  << ", vertices= (" << suggestion.vertices.front() << "..." << suggestion.vertices.front() << ") }";
#endif
            result.insert(std::pair<int,Suggestion>(suggestion.nb_error,suggestion));
            break;
          }
        }
        if( form.length() > max_length_element_in_lexicon + (max_length_element_in_lexicon*m_nbMaxNumError)/m_nbMaxDenError ) {
          break;
        }
      }
      // following nodes
      LinguisticGraphOutEdgeIt outEdge,outEdge_end;
      boost::tie (outEdge,outEdge_end)=out_edges(currentVertex,g);
      currentVertex =target(*outEdge,g);
    }
  return SUCCESS_ID;
}

QString ApproxStringMatcher::wcharStr2LimaStr(const std::basic_string<wchar_t>& wstring) const {
  // convert std::basic_string<wchar_t> to QString
      return QString::fromWCharArray(wstring.c_str(),wstring.length());
}

std::basic_string<wchar_t> ApproxStringMatcher::LimaStr2wcharStr( const QString& limastr ) const {
  // convert QString to std::basic_string<wchar_t>
      wchar_t warray[limastr.length()+1];
      int warray_len = limastr.toWCharArray(warray);
      warray[warray_len]=0;
      return std::basic_string<wchar_t>(warray, warray_len);
}

std::basic_string<wchar_t> ApproxStringMatcher::buildPattern(const QString& normalizedForm) const {
  MORPHOLOGINIT;
    // convert normalizedForm into std::basic_string<wchar_t>
    std::basic_string<wchar_t> wpattern = LimaStr2wcharStr(normalizedForm);
    for(RegexMap::const_iterator regexIt = m_regexes.begin() ;
        regexIt != m_regexes.end() ; regexIt++ )
    {
      // get regex as wstring
      std::pair< std::basic_string<wchar_t>, std::basic_string<wchar_t> > a_regex = *regexIt;
      wide_regex matching_rule(a_regex.first);
      std::basic_string<wchar_t> substitution = a_regex.second;
      wpattern = boost::regex_replace(wpattern, matching_rule,
                                      substitution, boost::match_default | boost::format_sed);
#ifdef DEBUG_LP
      QString pattern = wcharStr2LimaStr(wpattern);
      LDEBUG << "ApproxStringMatcher::buildPattern: pattern changed to  "
             << Lima::Common::Misc::limastring2utf8stdstring(pattern);
#endif
      break;
    }
    return wpattern;
}

void ApproxStringMatcher::matchApproxTokenAndFollowers(
    LinguisticGraph& g, 
    LinguisticGraphVertex vStart,
    LinguisticGraphVertex vEnd,
    Solution& result) const
{
  typedef Lima::Common::AccessSuperWordIterator WIt;
  MORPHOLOGINIT;
  VertexTokenPropertyMap tokenMap=get(vertex_token,g);
  result.suggestion.nb_error = 1000;
  // VertexDataPropertyMap dataMap=get(vertex_data,g);

  // Build string where search will be done
  // Build also a map of position of token in this string
  // Because the string formed is neither identical to the original text
  // nor identical to concatenation of tokens
  LimaString form;
  // Position of tokens in form
  std::deque<int> tokenStartPos;
  std::deque<int> tokenEndPos;
  int max_length_element_in_lexicon = 49;
  Token* currentToken=tokenMap[vStart];
  // position of form in original text
  uint64_t textStart, textEnd;
  // TODO: vérifier que vEndIt est le noeud 1 (sans token)
  LinguisticGraphVertex currentVertex = vStart;
  for( ; currentVertex != vEnd ; ) {
    currentToken=tokenMap[currentVertex];
#ifdef DEBUG_LP
    LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() from " << currentVertex;
#endif
    if (currentToken!=0)
    {
      if( tokenStartPos.empty() ) {
        tokenStartPos.push_back(0);
        tokenEndPos.push_back(currentToken->length());
        textStart = currentToken->position();
        // textEnd = textStart+currentToken->stringForm().length(); ???
        textEnd = currentToken->position() + currentToken->length();
      }
      else {
        int currentStartPos = tokenEndPos.back();
        // TODO: add space when previous and current tokens are not contiguous
        if( currentToken->position() > textEnd) {
          currentStartPos++;
          form.append(" ");
        }
        tokenStartPos.push_back(currentStartPos);
        tokenEndPos.push_back(currentStartPos+currentToken->length());
        textEnd = currentToken->position() + currentToken->length();
      }
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() posInform= "
            << tokenStartPos.back() << "," << tokenEndPos.back();
#endif
      assert( currentToken->length() == (uint64_t)(currentToken->stringForm().length()));
      form.append(currentToken->stringForm());
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() form= "
            << Lima::Common::Misc::limastring2utf8stdstring(form);
#endif
    }
    if( form.length() > max_length_element_in_lexicon + (max_length_element_in_lexicon*m_nbMaxNumError)/m_nbMaxDenError  ) {
      break;
    }
    // following nodes
    LinguisticGraphOutEdgeIt outEdge,outEdge_end;
    boost::tie (outEdge,outEdge_end)=out_edges(currentVertex,g);
    currentVertex =target(*outEdge,g);
  }
  
  // build pattern from word in lexicon
  std::pair<WIt,WIt>  wordsIt = m_lexicon->getSuperWords("");
  for( ; wordsIt.first != wordsIt.second ; (wordsIt.first)++ ) {
    // get normalized form from lexicon
    LimaString normalizedForm = *(wordsIt.first);
    std::basic_string<wchar_t> wpattern = buildPattern(normalizedForm);
    int nbMaxError = (normalizedForm.length()*m_nbMaxNumError)/m_nbMaxDenError;
    // Search for pattern in form
    Suggestion suggestion;
    int ret = findApproxPattern( wpattern, form, suggestion, nbMaxError);
    // keep suggestion if best
    if( (ret == 0) && (suggestion.nb_error < result.suggestion.nb_error) ) {
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers(): findApproxPattern()="
             << ret << "," << suggestion;
#endif
      Solution tempResult;

      tempResult.suggestion.nb_error = suggestion.nb_error;
      tempResult.vertices=std::deque<LinguisticGraphVertex>();
      tempResult.startPos = tokenStartPos.front();
      tempResult.normalizedForm = normalizedForm;
      // exact position in text
      tempResult.suggestion.startPosition = -1;
      tempResult.suggestion.endPosition = -1;
      int startInForm(0);
      int endInForm(0);
      std::deque<int>::const_iterator startPosIt= tokenStartPos.begin();
      std::deque<int>::const_iterator endPosIt= tokenEndPos.begin();
      for( currentVertex = vStart ; currentVertex != vEnd ; ) {
        currentToken=tokenMap[currentVertex];
        startInForm = *startPosIt;
        endInForm = *endPosIt;
        if (currentToken!=0)
        {
#ifdef DEBUG_LP
          LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() compare with (start,end)="
                 << "(" << startInForm
                 << "," << endInForm << "}";
#endif
          if( tempResult.suggestion.startPosition == -1 ) {
            // If begin of matched segment is bounded by current token
            if( ( suggestion.startPosition >= startInForm ) && ( suggestion.startPosition < endInForm ) ){
              tempResult.suggestion.startPosition = currentToken->position();
              tempResult.startPos = *startPosIt;
              LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() begin ="
                       << currentToken->position();
              if( suggestion.startPosition > startInForm ) {
                LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() correction error begin +"
                       << suggestion.startPosition - startInForm;
                tempResult.suggestion.startPosition += (suggestion.startPosition - startInForm);
                tempResult.suggestion.nb_error += (suggestion.startPosition - startInForm);
                tempResult.startPos += (suggestion.startPosition - startInForm);
              }
              tempResult.vertices.push_back(currentVertex);
            }
          }
          if( tempResult.suggestion.endPosition == -1 ) {
            // If end of matched segment is bounded by current token
            if( ( suggestion.endPosition >= startInForm ) && ( suggestion.endPosition <= endInForm ) ){
              tempResult.suggestion.endPosition = currentToken->position()+currentToken->length();
              LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() end ="
                       << currentToken->position()+currentToken->length();
              if( suggestion.endPosition < endInForm ) {
                LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() correction error end +"
                       << endInForm - suggestion.endPosition;
                tempResult.suggestion.endPosition -= (endInForm - suggestion.endPosition);
                tempResult.suggestion.nb_error += (endInForm - suggestion.endPosition);
              }
            }
          }
          if( tempResult.suggestion.startPosition != -1 ) {
            tempResult.vertices.push_back(currentVertex);
          }
          if( (tempResult.suggestion.startPosition != -1) && (tempResult.suggestion.endPosition != -1) ) {
            break;
          }
          startPosIt++;
          endPosIt++;
        }
        // following nodes
        LinguisticGraphOutEdgeIt outEdge,outEdge_end;
        boost::tie (outEdge,outEdge_end)=out_edges(currentVertex,g);
        currentVertex =target(*outEdge,g);
      }
      tempResult.form = form.mid(suggestion.startPosition,
                             suggestion.endPosition-suggestion.startPosition);
      tempResult.length = suggestion.endPosition-suggestion.startPosition;
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() " << tempResult;
#endif
      if( (tempResult.suggestion.nb_error < result.suggestion.nb_error) ) {
        result = tempResult;
      }
    }
  }
}

int ApproxStringMatcher::findApproxPattern(
    const std::basic_string<wchar_t>& pattern, LimaString text,
    Suggestion& suggestion, int nbMaxError) const {
  MORPHOLOGINIT;
#ifdef DEBUG_LP
      QString patternQ = wcharStr2LimaStr(pattern);
      LDEBUG << "ApproxStringMatcher::findApproxPattern("
             << Lima::Common::Misc::limastring2utf8stdstring(patternQ) << ","
             << Lima::Common::Misc::limastring2utf8stdstring(text) << ")";
      #endif

    // pattern buffer structure (result of compilation)
    regex_t preg;
    int cflags = REG_EXTENDED|REG_ICASE|REG_NEWLINE;
    // cflags |= REG_NOSUB;
    // cflags |= REG_LITERAL;
    // cflags |= REG_RIGHT_ASSOC;
    // cflags |= REG_UNGREEDY;
    
    // Compile pattern
    /*
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::findApproxPattern: compilation...";
#endif
      */
    int agrepStatus = regwncomp(&preg, pattern.c_str(), pattern.length(), cflags);
    // TODO: agrepStatus???
#ifdef DEBUG_LP
    LDEBUG << "ApproxStringMatcher::findApproxPattern: agrepStatus=" << agrepStatus;
#endif

    regaparams_t params = {
      1,    // int cost_ins;
      1,    // int cost_del;
      1,    // int cost_subst;
      nbMaxError,    // int max_cost;
      nbMaxError,    // int max_ins;
      nbMaxError,    // int max_del;
      nbMaxError,    // int max_subst;
      nbMaxError,    // int max_err;
    };
    int eflags = REG_NOTBOL;
    // eflags |= REGNOTEOL;
    // pmatch has more than 1 emplacement to be filled with match of subexpression (parenthese expr. in pattern)
    const size_t MAX_MATCH=10;
    regmatch_t pmatch[MAX_MATCH];
    regamatch_t amatch = {
      MAX_MATCH,    // size_t nmatch;
      pmatch,  // regmatch_t *pmatch
      0,    // int cost;
      0,    // int num_ins;
      0,    // int num_del;
      0,    //int num_subst;
    };
    wchar_t tarray[text.length()];
    int tlength = text.toWCharArray(tarray);
    /*
#ifdef DEBUG_LP
    LDEBUG << "ApproxStringMatcher::findApproxPattern: execution...";
#endif
    */
    int execStatus = regawnexec(&preg, tarray, tlength,
                &amatch, params, eflags);
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::findApproxPattern: execStatus=" << execStatus;
#endif
    if( execStatus == 0 ) {
      regmatch_t* current_match=&(pmatch[0]);
      suggestion.startPosition = current_match->rm_so;
      suggestion.endPosition = current_match->rm_eo;
      suggestion.nb_error = amatch.num_del+amatch.num_ins;
      // suggestion.???
    }
    return execStatus;
}


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
