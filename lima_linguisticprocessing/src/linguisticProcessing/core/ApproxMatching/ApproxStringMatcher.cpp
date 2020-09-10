/*
    Copyright 2002-2019 CEA LIST

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

QChar BLANK_SEPARATOR= ' ';

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
     << "length=" << solution.length
     << std::endl;
  return os;
}

QDebug& operator<<(QDebug& os, const Solution& solution)
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
     << "length=" << solution.length;
  return os;
}

bool SolutionCompare::operator() (const Solution& s1, const Solution& s2) const {
  if( s1.suggestion.nb_error < s2.suggestion.nb_error ) {
    return true;
  }
  if( s1.suggestion.nb_error > s2.suggestion.nb_error ) {
    return false;
  }
  if( s1.suggestion.startPosition < s2.suggestion.startPosition ) {
    return true;
  }
  if( s1.suggestion.startPosition > s2.suggestion.startPosition ) {
    return false;
  }
  if( s1.length > s2.length) {
    return true;
  }
  if( s1.length < s2.length) {
    return false;
  }
  return true;
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
  std::string nameindexId;
  try
  {
    nameindexId =unitConfiguration.getParamsValueAtKey("nameindex");
  }
  catch (NoSuchParam& )
  {
    LERROR << "no param 'nameindex' in ApproxStringMatcher group for language " << (int) m_language;
    throw InvalidConfiguration();
  }
  const AbstractResource* res=LinguisticResources::single().getResource(m_language,nameindexId);
  m_nameIndex=static_cast<const NameIndexResource*>(res);

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
    std::map <std::string, std::string >& regexes = unitConfiguration.getMapAtKey("generalizationRules");
    std::deque <std::string >& regexesOrder = unitConfiguration.getListsValueAtKey("generalizationRulesOrder");
    for (std::deque <std::string >::const_iterator it = regexesOrder.begin(); it != regexesOrder.end(); it++)
    {
      const std::string& key = *it;
      QString patternQ = QString::fromUtf8 (key.c_str());
      std::basic_string<wchar_t> patternWS = NameIndexResource::LimaStr2wcharStr(patternQ);
      const std::string& value=regexes.at(key);
      QString substitutionQ = QString::fromUtf8 (value.c_str());
      std::basic_string<wchar_t> substitutionWS = NameIndexResource::LimaStr2wcharStr(substitutionQ);
      m_regexes.push_back( std::pair<std::basic_string<wchar_t>,std::basic_string<wchar_t> >(patternWS,
                                    substitutionWS) );
    }
  }
  catch (NoSuchParam& )
  {
    LERROR << "no map 'generalization' in RegexReplacer group configuration fot language "
    << (int) m_language;
    throw InvalidConfiguration();
  }
  catch (std::out_of_range& )
  {
    LERROR << "no value for some key in 'generalizationRules' in RegexReplacer group configuration fot language "
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
  LDEBUG << "ApproxStringMatcher::process: index from metadata= "
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
  if( (indexName.length() == 0) || !(m_nameIndex->withIndex()) ) {
    nameRange.first = m_nameIndex->begin();
    nameRange.second = m_nameIndex->end();
  }
  else {
    nameRange = m_nameIndex->equal_range(indexName);
    // nameRange.first = m_nameIndex->lower_bound(indexName);
    // nameRange.second = m_nameIndex->upper_bound(indexName);
  }
#ifdef DEBUG_LP
  const std::basic_string<wchar_t> firstNameW = (*(nameRange.first)).second;
  std::basic_string<wchar_t> lastNameW;
  for( NameIndex::const_iterator it= nameRange.first ; it != nameRange.second ; it++ )
    lastNameW = (*it).second;
  const QString firstNameQ =  wcharStr2LimaStr(firstNameW);
  const QString lastNameQ =  wcharStr2LimaStr(lastNameW);
  LDEBUG << "ApproxStringMatcher::process: nameRange= from " << firstNameQ << " to " << lastNameQ;
#endif
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
    const Solution solution= *sIt;
    bool outOfGraph=false;
#ifdef DEBUG_LP
    std::deque<LinguisticGraphVertex>::const_iterator vIt2 = solution.vertices.begin();
    if( vIt2 != solution.vertices.end() ) {
      LDEBUG << *(vIt2++);
    }

    for( ; vIt2 != solution.vertices.end() ; vIt2++ ) {
      LDEBUG << "," << *vIt2;
    }
#endif
    for( std::deque<LinguisticGraphVertex>::const_iterator vIt = solution.vertices.begin();
        vIt != solution.vertices.end() ; vIt++ )
    {
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::process: outOfGraph = " << outOfGraph << ", check vertex " << *vIt;
#endif
      // If one of vertices has no successor or no predecessor, it means is belongs no more to the graph
      LinguisticGraphOutEdgeIt outEdge,outEdge_end;
      boost::tie (outEdge,outEdge_end)=out_edges(*vIt,g);
      if( outEdge == outEdge_end ) {
        outOfGraph=true;
      }
      LinguisticGraphInEdgeIt inEdge,inEdge_end;
      boost::tie (inEdge,inEdge_end)=in_edges(*vIt,g);
      if( inEdge == inEdge_end ) {
        outOfGraph=true;
      }
      if( outOfGraph )
        break;
    }
    if( !outOfGraph ) {
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
  // Qu'est ce qu'on affecte comme chaîne et comme position à ce nouveau token ???
  Token* newToken = new Token(
      form,
      (*m_sp)[form],
      solution.suggestion.startPosition,
      solution.suggestion.endPosition - solution.suggestion.startPosition);
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
        solution.suggestion.startPosition, solution.length, *m_sp);
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
    // escape 'regex special character', to avoid to interpret a character in a name as regex special character:
    // '.', '^', '$', '|', '(', ')', '[', ']', '{', '}', '*', '+', '?', '\'
    wide_regex esc(L"[.^$|()\\[\\]{}*+?\\\\]");
    const std::basic_string<wchar_t> rep(L"\\\\&");
    wpattern = boost::regex_replace(wpattern, esc, rep, boost::match_default | boost::format_sed);
#ifdef DEBUG_LP
      QString pattern = wcharStr2LimaStr(wpattern);
      QString name = wcharStr2LimaStr(normalizedForm);
      LDEBUG << "ApproxStringMatcher::buildPattern: (escaping) name "
             << Lima::Common::Misc::limastring2utf8stdstring(name) << " changed to "
             << Lima::Common::Misc::limastring2utf8stdstring(pattern);
#endif
    // apply user supplied regex (generalization)
    for(RegexMap::const_iterator regexIt = m_regexes.begin() ;
        regexIt != m_regexes.end() ; regexIt++ )
    {
      // get regex as wstring
      Regex a_regex = *regexIt;
      wide_regex matching_rule(a_regex.first);
      std::basic_string<wchar_t> substitution = a_regex.second;
      wpattern = boost::regex_replace(wpattern, matching_rule,
                                      substitution, boost::match_default | boost::format_sed);
#ifdef DEBUG_LP
      QString pattern = wcharStr2LimaStr(wpattern);
      QString name = wcharStr2LimaStr(normalizedForm);
      LDEBUG << "ApproxStringMatcher::buildPattern: name "
             << Lima::Common::Misc::limastring2utf8stdstring(name) << " changed to "
             << Lima::Common::Misc::limastring2utf8stdstring(pattern);
#endif
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
  LimaString text;
  Token* currentToken=tokenMap[vStart];
  // TODO: vérifier que vEndIt est le noeud 1 (sans token)
  for( LinguisticGraphVertex currentVertex = vStart ; currentVertex != vEnd ; ) {
    currentToken=tokenMap[currentVertex];
#ifdef DEBUG_LP
    LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers() from " << currentVertex;
#endif
    if (currentToken!=0)
    {
      // Add enough space characters to adjust text to beginning of token
      if (currentToken->position() > text.length()) {
        for( int i = currentToken->position() - text.length() ; i > 0 ; i-- )
          text.append(BLANK_SEPARATOR);
      }
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
    std::vector<Suggestion> suggestions;
    int ret = findApproxPattern( wpattern, text, suggestions, nbMaxError);
    // keep all suggestions
    if(ret == 0) {
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers(): findApproxPattern()="
             << ret;
      for( std::vector<Suggestion>::const_iterator sIt = suggestions.begin() ; sIt != suggestions.end() ; sIt++ ) {
        LDEBUG << *sIt;
      }
#endif
      // compute which vertices contribute to the match and compute (position,length) in original text
      for( std::vector<Suggestion>::const_iterator sIt = suggestions.begin() ;
          sIt != suggestions.end() ; sIt++ ) {
        Solution tempResult;
        computeVertexMatches( g, vStart, vEnd, *sIt, tempResult);
        // TODO: à revoir, peut être qu'il faut recalculer la chaîne à partir du match au moment du calcul de suggestion.startPosition et suggestion.endPosition
        //tempResult.form = text.mid(sIt->startPosition, sIt->endPosition-sIt->startPosition);
        // tempResult.length = sIt->endPosition-sIt->startPosition;
        tempResult.length = tempResult.suggestion.endPosition-tempResult.suggestion.startPosition;
        tempResult.form = text.mid(tempResult.suggestion.startPosition,
                                   tempResult.length);
        if( (tempResult.suggestion.nb_error <= nbMaxError) ) {
          tempResult.normalizedForm = wcharStr2LimaStr(normalizedForm);
          result.insert(tempResult);
        }
#ifdef DEBUG_LP
        LDEBUG << "ApproxStringMatcher::matchApproxTokenAndFollowers: tempResult= " << tempResult;
#endif
      }
    }
  }
}


void ApproxStringMatcher::computeVertexMatches(
    const LinguisticGraph& g,
    const LinguisticGraphVertex vStart,
    const LinguisticGraphVertex vEnd,
    const Suggestion& suggestion, Solution& tempResult) const
{
  MORPHOLOGINIT;
  Token* currentToken=get(vertex_token,g,vStart);

      tempResult.suggestion.nb_error = suggestion.nb_error;
      tempResult.vertices=std::deque<LinguisticGraphVertex>();
      bool pushVertex=false;
      for( LinguisticGraphVertex currentVertex = vStart ; currentVertex != vEnd ; ) {
        currentToken=get(vertex_token,g,currentVertex);
        if (currentToken!=0)
        {
          int startTok = currentToken->position();
          int endTok = currentToken->position() + currentToken->length();
#ifdef DEBUG_LP
          LDEBUG << "ApproxStringMatcher::computeVertexMatches() compare with (start,end)="
                 << "(" << startTok
                 << "," << endTok << "}";
#endif
          // Search for token where matches begins
          if( tempResult.vertices.size() == 0 ) {
            if(  (suggestion.startPosition <= startTok )
              || ( (suggestion.startPosition >= startTok) && ( suggestion.startPosition < endTok) ) ) {
              pushVertex=true;
              if(suggestion.startPosition > startTok) {
                tempResult.suggestion.nb_error += (suggestion.startPosition - startTok);
#ifdef DEBUG_LP
                LDEBUG << "ApproxStringMatcher::computeVertexMatches: error +="
                       << suggestion.startPosition - startTok;
#endif
              }
            }
          }
          if( pushVertex ) {
#ifdef DEBUG_LP
            LDEBUG << "ApproxStringMatcher::computeVertexMatches: push "
                   << currentVertex;
#endif
            tempResult.vertices.push_back(currentVertex);
            if( ( suggestion.endPosition >= startTok ) && ( suggestion.endPosition <= endTok) ) {
              if(suggestion.endPosition < endTok) {
                tempResult.suggestion.nb_error += (endTok-suggestion.endPosition);
#ifdef DEBUG_LP
                LDEBUG << "ApproxStringMatcher::computeVertexMatches: error +="
                       << endTok-suggestion.endPosition;
#endif
              }
              break;
            }
          }
        }
        // following nodes
        LinguisticGraphOutEdgeIt outEdge,outEdge_end;
        boost::tie (outEdge,outEdge_end)=out_edges(currentVertex,g);
        currentVertex =target(*outEdge,g);
      }
      Token* firstToken=get(vertex_token,g,tempResult.vertices.front());
      tempResult.suggestion.startPosition = firstToken->position();
      Token* lastToken=get(vertex_token,g,tempResult.vertices.back());
      tempResult.suggestion.endPosition = lastToken->position()+lastToken->length();
}

int ApproxStringMatcher::findApproxPattern(
    const std::basic_string<wchar_t>& pattern, LimaString text,
    std::vector<Suggestion>& suggestions, int nbMaxError) const {
    int returnStatus=1;
    MORPHOLOGINIT;
      QString patternQ = wcharStr2LimaStr(pattern);
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::findApproxPattern("
             << Lima::Common::Misc::limastring2utf8stdstring(patternQ) << ","
             << Lima::Common::Misc::limastring2utf8stdstring(text) << "), nbErr=" << nbMaxError;
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
    if(agrepStatus!= 0) {
      QString patternQ = wcharStr2LimaStr(pattern);
      LWARN << "ApproxStringMatcher::findApproxPattern: error when compiling ="
            << Lima::Common::Misc::limastring2utf8stdstring(patternQ);
      return returnStatus;
    }

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
    int offset=0;
    int execStatus;
    do {
      execStatus = regawnexec(&preg, tarray+offset, tlength-offset, &amatch, params, eflags);
#ifdef DEBUG_LP
      LDEBUG << "ApproxStringMatcher::findApproxPattern: execStatus=" << execStatus;
#endif
      if( execStatus == 0 ) {
        returnStatus=0;
        regmatch_t* current_match=&(pmatch[0]);
        Suggestion suggestion;
        suggestion.startPosition = current_match->rm_so+offset;
        suggestion.endPosition = current_match->rm_eo+offset;
        suggestion.nb_error = amatch.num_del+amatch.num_ins+amatch.num_subst;
        suggestions.push_back(suggestion);
        offset += current_match->rm_eo;
      }
    } while ( (tlength-offset > 0) && (execStatus == 0) );
    regfree(&preg);
    return returnStatus;
}


} // MorphologicAnalysis
} // LinguisticProcessing
} // Lima
