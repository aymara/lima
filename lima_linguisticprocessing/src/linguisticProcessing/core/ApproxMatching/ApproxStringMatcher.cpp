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
#include <tre/regex.h>
#include <boost/regex.hpp>
//#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"
#include "linguisticProcessing/core/Automaton/SpecificEntityAnnotation.h"
#include "linguisticProcessing/core/LinguisticProcessors/LinguisticMetaData.h"

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

bool SolutionCompare::operator() (const Solution& s1, const Solution& s2) {
  if( s1.suggestion.nb_error < s2.suggestion.nb_error ) {
    return true;
  }
  if( s1.suggestion.nb_error > s2.suggestion.nb_error ) {
    return false;
  }
  if( s1.startPos < s2.startPos ) {
    return true;
  }
}
  
ApproxStringMatcher::ApproxStringMatcher() :
    m_language(0),
    m_neCode(0),
    m_neMicroCode(0),
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
  
  m_language = manager->getInitializationParameters().media;
  m_sp=&Common::MediaticData::MediaticData::changeable().stringsPool(m_language);

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
    LERROR << "no param 'entityGoup' in ApproxStringMatcher group for language " << (int) m_language;
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
    LERROR << "no param 'entityGoup' in ApproxStringMatcher group for language " << (int) m_language;
    throw InvalidConfiguration();
  }

  try
  {
    std::string np = unitConfiguration.getParamsValueAtKey("NPCategory");
    m_neCode=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MACRO").getPropertyValue(np);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'NPCategory' in ApproxStringMatcher group for language " << (int) m_language;
    throw InvalidConfiguration();
  }

  try
  {
    std::string microCategory=unitConfiguration.getParamsValueAtKey("NPMicroCategory");
    m_neMicroCode=static_cast<const Common::MediaticData::LanguageData&>(Common::MediaticData::MediaticData::single().mediaData(m_language)).getPropertyCodeManager().getPropertyManager("MICRO").getPropertyValue(microCategory);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'NPMicroCategory' in ApproxStringMatcher group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  
  // get index of names
  try
  {
    std::string nameindexId =unitConfiguration.getParamsValueAtKey("nameindex");
    const AbstractResource* res=LinguisticResources::single().getResource(m_language,nameindexId);
    m_nameIndex=static_cast<const NameIndexResource*>(res);
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'nameindex' in ApproxStringMatcher group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  
  /*
  // get dictionary of normalized forms
  string dico;
  try
  {
    dico=unitConfiguration.getParamsValueAtKey("dictionary");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'dictionary' in ApproxStringMatcher group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  AbstractResource* res=LinguisticResources::single().getResource(m_language,dico);
  AbstractAccessResource* lexicon = lexicon=static_cast<AbstractAccessResource*>(res);
  m_lexicon = lexicon->getAccessByString();
  */

  // get max edit distance 
  try
  {
    std::string nbMaxErrorStr=unitConfiguration.getParamsValueAtKey("nbMaxNumError");
    std::istringstream iss(nbMaxErrorStr);
    iss >> m_nbMaxNumError;
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'nbMaxNumError' in ApproxStringMatcher group for language " << (int) m_language;
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
    LERROR << "no param 'nbMaxDenError' in ApproxStringMatcher group for language " << (int) m_language;
    throw InvalidConfiguration();
  }

  // get generalization pattern 
  try
  {
    std::map <std::string, std::string >& regexes = unitConfiguration.getMapAtKey("generalization");
    for (std::map <std::string, std::string >::const_iterator it = regexes.begin(); it != regexes.end(); it++)
    {
      QString patternQ = QString::fromUtf8 ((*it).first.c_str());
      std::basic_string<wchar_t> patternWS = NameIndexResource::LimaStr2wcharStr(patternQ);
      QString substitutionQ = QString::fromUtf8 ((*it).second.c_str());
      std::basic_string<wchar_t> substitutionWS = NameIndexResource::LimaStr2wcharStr(substitutionQ);
      m_regexes.insert( std::pair<std::basic_string<wchar_t>,std::basic_string<wchar_t> >(patternWS,
                                    substitutionWS) );
    }
  }
  catch (NoSuchParam& )
  {
    LERROR << "no map 'generalization' in RegexReplacer group configuration fot language "
    << (int) m_language;
    throw InvalidConfiguration();
  }
}


LimaStatusCode ApproxStringMatcher::process(
  AnalysisContent& analysis) const
{
  Lima::TimeUtilsController timer("ApproxStringMatcher");
  MORPHOLOGINIT;
  LINFO << "starting process ApproxStringMatcher";

  LinguisticMetaData* metadata=static_cast<LinguisticMetaData*>(analysis.getData("LinguisticMetaData"));
  std::basic_string<wchar_t> indexName;
  try {
    if (metadata != 0) {
      std::string indexNametds = metadata->getMetaData("index");
      QString indexNamels = QString::fromUtf8 (indexNametds.c_str());
      indexName = NameIndexResource::LimaStr2wcharStr(indexNamels);
    }
  }
  catch (LinguisticProcessingException& ) {
    // do nothing: try full set of names
  }
#ifdef DEBUG_LP
  QString name = wcharStr2LimaStr(indexName);
  LDEBUG << "ApproxStringMatcher::buildPattern: process: country from metadata= "
         << Lima::Common::Misc::limastring2utf8stdstring(name);
#endif

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

  // Initalize list of suggestions, ordered by number of errors and position in text
  OrderedSolution solutions;
  // std::vector<Solution> result;
  
  // Initalize set of names to search for
  std::pair<NameIndex::const_iterator,NameIndex::const_iterator> nameRange;
  if( indexName.length() == 0 ) {
    nameRange.first = m_nameIndex->begin();
    nameRange.second = m_nameIndex->end();
  }
  else {
    nameRange = m_nameIndex->equal_range(indexName);
    // nameRange.first = m_nameIndex->lower_bound(indexName);
    // nameRange.second = m_nameIndex->upper_bound(indexName);
  }
  LinguisticGraph & g = *(anagraph->getGraph());
  matchApproxTokenAndFollowers(g, anagraph->firstVertex(), anagraph->lastVertex(), nameRange, solutions);
#ifdef DEBUG_LP
  LDEBUG << "ApproxStringMatcher::process: solution.size()=" << solutions.size();
#endif
  for( OrderedSolution::const_iterator sIt = solutions.begin() ; sIt != solutions.end() ; sIt++ ) {
    // check that vertices of solution are still in graph
#ifdef DEBUG_LP
    LDEBUG << "ApproxStringMatcher::process: check " << *sIt;
#endif
    const Solution& solution= *sIt;
    for( std::deque<LinguisticGraphVertex>::const_iterator vIt = solution.vertices.begin();
        vIt != solution.vertices.end() ; vIt++ )
    {
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::process: check vertex " << *vIt;
#endif
      LinguisticGraphOutEdgeIt outEdge,outEdge_end;
      boost::tie (outEdge,outEdge_end)=out_edges(*vIt,g);
      if( outEdge == outEdge_end )
          break;
      LinguisticGraphInEdgeIt inEdge,inEdge_end;
      boost::tie (inEdge,inEdge_end)=in_edges(*vIt,g);
      if( inEdge == inEdge_end )
          break;
      // TODO: check if( solution.suggestion.nb_error <= (len*m_nbMaxNumError)/m_nbMaxDenError ) ??
      createVertex(g, anagraph->firstVertex(), anagraph->lastVertex(), solution, annotationData );
      // result.push_back(solution);
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
    const Solution& solution,
    AnnotationData* annotationData 
) const 
{
  MORPHOLOGINIT;
#ifdef DEBUG_LP
  LDEBUG << "ApproxStringMatcher::createVertex( solution=" << solution << ")";
#endif
  // VertexTokenPropertyMap tokenMap=get(vertex_token,g);
  // VertexDataPropertyMap dataMap = get(vertex_data, g);
  
  // create new vertex in analysis graph
  LinguisticGraphVertex newVertex = add_vertex(g);
  // Find previous vertex
  LinguisticGraphOutEdgeIt outEdge,outEdge_end;
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
 
  // Create token for this vertex
  StringsPoolIndex form = (*m_sp)[solution.form];
  Token* newToken = new Token(
      form,
      (*m_sp)[form],
      solution.startPos,
      solution.length);
  // specify status for token
  TStatus tStatus;
  tStatus.reset();
  tStatus.setStatus(T_PATTERN);
  tStatus.setDefaultKey(Common::Misc::utf8stdstring2limastring("t_pattern"));
  newToken->setStatus(tStatus);
  // tokenMap[newVertex] = newToken;
  put(vertex_token,g,newVertex,newToken);

  // Create MorphoSyntacticData for this vertex
  MorphoSyntacticData* newMorphoSyntacticData = new MorphoSyntacticData ();
  LinguisticElement elem;
  elem.inflectedForm = newToken->form();            // (StringsPoolIndex)
  elem.lemma = newToken->form();                    // (StringsPoolIndex)
  elem.normalizedForm = (*m_sp)[solution.normalizedForm];
  elem.type = SPECIFIC_ENTITY; // MorphoSyntacticType
  elem.properties = m_neCode;      //   LinguisticCode
  elem.properties = m_neMicroCode;      //   LinguisticCode
  newMorphoSyntacticData->push_back(elem);
  // dataMap[newVertex] = newMorphoSyntacticData;
  put(vertex_data,g,newVertex,newMorphoSyntacticData);
  
  // Create vertex fot annotation graph
  AnnotationGraphVertex agv =  annotationData->createAnnotationVertex();
  // make access to this annotation vertex from newVertex
  annotationData->addMatching("AnalysisGraph", newVertex, "annot", agv);
  // make access back to newVertex from this annotation
  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("AnalysisGraph"),
                           newVertex);
  // Create annotation data of type 'SpecificEntity'
  Lima::LinguisticProcessing::SpecificEntities::SpecificEntityAnnotation spAnnot(
        solution.vertices,
        m_entityType,
        solution.form, solution.normalizedForm, solution.suggestion.nb_error,
        solution.startPos, solution.length, *m_sp);
  GenericAnnotation spGa(spAnnot);
  // make access to annotation data from annotation vertex
  annotationData->annotate(agv, Common::Misc::utf8stdstring2limastring("SpecificEntity"), spGa);
  // Créer une 'feature' 'value' sans information de localisation
  // pour qu'elle soit produite comme 'normalization' dans le logger
}

QString ApproxStringMatcher::wcharStr2LimaStr(const std::basic_string<wchar_t>& wstring) const {
  // convert std::basic_string<wchar_t> to QString
      return QString::fromWCharArray(wstring.c_str(),wstring.length());
}

std::basic_string<wchar_t> ApproxStringMatcher::buildPattern(const std::basic_string<wchar_t>& normalizedForm) const {
  MORPHOLOGINIT;
    // convert normalizedForm into std::basic_string<wchar_t>
    // std::basic_string<wchar_t> wpattern = LimaStr2wcharStr(normalizedForm);
    std::basic_string<wchar_t> wpattern = normalizedForm;
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
    std::pair<NameIndex::const_iterator,NameIndex::const_iterator> nameRange,
    OrderedSolution& result) const
{
  MORPHOLOGINIT;
  VertexTokenPropertyMap tokenMap=get(vertex_token,g);
  // VertexDataPropertyMap dataMap=get(vertex_data,g);

  // Build string (text) where search will be done
  // Build also a queue of positions of tokens in this string
  // Because the string formed is neither identical to the original text
  // nor identical to concatenation of tokens
  LimaString text;
  // Position of tokens in text
  std::deque<int> tokenStartPos;
  std::deque<int> tokenEndPos;
  Token* currentToken=tokenMap[vStart];
  // position of text in original text
  uint64_t textEnd;
  // TODO: vérifier que vEndIt est le noeud 1 (sans token)
  for( LinguisticGraphVertex currentVertex = vStart ; currentVertex != vEnd ; ) {
    currentToken=tokenMap[currentVertex];
#ifdef DEBUG_LP
    LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() from " << currentVertex;
#endif
    if (currentToken!=0)
    {
      if( tokenStartPos.empty() ) {
        tokenStartPos.push_back(0);
        tokenEndPos.push_back(currentToken->length());
        textEnd = currentToken->position() + currentToken->length();
      }
      else {
        int currentStartPos = tokenEndPos.back();
        // TODO: add space when previous and current tokens are not contiguous
        if( currentToken->position() > textEnd) {
          currentStartPos++;
          text.append(" ");
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
      text.append(currentToken->stringForm());
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() text= "
            << Lima::Common::Misc::limastring2utf8stdstring(text);
#endif
    }
    // following nodes
    LinguisticGraphOutEdgeIt outEdge,outEdge_end;
    boost::tie (outEdge,outEdge_end)=out_edges(currentVertex,g);
    currentVertex =target(*outEdge,g);
  }
  
  // search for names in text
  for( NameIndex::const_iterator wordIt = nameRange.first ; wordIt != nameRange.second ; wordIt++ ) {
  // get normalized form of name from lexicon
    std::basic_string<wchar_t> normalizedForm = (*wordIt).second;
    // build pattern from name
    std::basic_string<wchar_t> wpattern = buildPattern(normalizedForm);
    // compute nb max error for name
    int nbMaxError = (normalizedForm.length()*m_nbMaxNumError)/m_nbMaxDenError;
    // Search for pattern in text
    Suggestion suggestion;
    int ret = findApproxPattern( wpattern, text, suggestion, nbMaxError);
    // keep all suggestions
    if(ret == 0) {
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers(): findApproxPattern()="
             << ret << "," << suggestion;
#endif
      Solution tempResult;

      tempResult.suggestion.nb_error = suggestion.nb_error;
      tempResult.vertices=std::deque<LinguisticGraphVertex>();
      tempResult.startPos = tokenStartPos.front();
      tempResult.normalizedForm = wcharStr2LimaStr(normalizedForm);
      // exact position in text
      tempResult.suggestion.startPosition = -1;
      tempResult.suggestion.endPosition = -1;
      int startInForm(0);
      int endInForm(0);
      std::deque<int>::const_iterator startPosIt= tokenStartPos.begin();
      std::deque<int>::const_iterator endPosIt= tokenEndPos.begin();
      for( LinguisticGraphVertex currentVertex = vStart ; currentVertex != vEnd ; ) {
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
      tempResult.form = text.mid(suggestion.startPosition,
                             suggestion.endPosition-suggestion.startPosition);
      tempResult.length = suggestion.endPosition-suggestion.startPosition;
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers: tempResult= " << tempResult;
#endif
      if( (tempResult.suggestion.nb_error <= nbMaxError) ) {
        result.insert(tempResult);
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
