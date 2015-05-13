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
/**
 *   Copyright (C) 2007-2012 by CEA LIST                               
 *                                             
 * @file        preprocessCorpus.cpp
 * @author      Claire Mouton (Claire.Mouton@cea.fr) 

 *              Copyright (c) 2007 by CEA
 * @date        Created on May, 31 2007
 */


#include "common/time/traceUtils.h"

#include "common/LimaCommon.h"

#include <iostream>
#include <fstream>
#include <deque>
#ifndef WIN32
#include <cstdint> //uint32_t
#endif
#include <boost/regex.hpp>

#include <QtCore/QCoreApplication>

using namespace std;
//using namespace boost;
using namespace Lima;

typedef map<string,string> CorefsMap;
//help mode & usage
static const string USAGE("usage: preprocessCorpus --xml=corpusFormat (wh|cl) inputFile outputFile\n"
"Supported formats are:\n"
"wh: LIMA coref logger format\n"
"cl: 'Clouzot' format (from reference data by Catherine Clouzot from her DEA (Cf. doc_clouzot.rtf for more details))\n"
); 

static const boost::regex eWhStart("<TEXT>");
static const boost::regex eWhBeginTag("<COREFID=\"([[:alnum:]]+)\"([^>]*REF=\"([[:alnum:]]+)\")?([^>]*CATEG=\"([[:alnum:]]+)\")?>");
static const boost::regex eWhEndTag("</COREF>");
/*
static const string eBeginString("[.[.<COREF ID=\".]([[:alnum:]]+)[.\"([^>]*REF=\".]([[:alnum:]]+)[.\")?>.].]");
static const string eEndString("[.</COREF>.]");
static const regex eTag("["+eBeginString+"[^"+eEndString+"]*]*"+ eBeginString + "[^"+eEndString+"]*"+eEndString+eEndString+"*");*/

static const boost::regex eClStart("<PID=\"[[:digit:]]+\">");
static const boost::regex eClStop("</P>");
static const boost::regex eClBeginTag("<RSID=\"([[:digit:]]+[.][[:digit:]]+)\"[^>]*>");
// <LINKARGS='"1.1""1.2"'TYPE="COREF"TYPANA="AG"/>
static const boost::regex eClEndTag("</RS>");
static const boost::regex eClLinkBeginTag("<LINKARGS=\'(\"[?]*[[:digit:]]+[.][[:digit:]]+\")+\'TYPE=\"([[:alnum:]]+)\"TYPANA=\"([[:alnum:]]+)\"/>");
static const boost::regex eClLinkEndTag("</LINK>");
static const boost::regex
eClLinkExtract("[?]*([[:digit:]]+[.][[:digit:]]+)");

int cpt = 0;

int dowork(int argc,char* argv[]);
void whProcess(ifstream& fin, ofstream& fout);
void clProcess(ifstream& fin, ofstream& fout/*, ofstream& ftmp*/);
string preprocessLine(string s);
void doWhActionOnOpeningString(
    boost::match_results<std::string::const_iterator>& what,
    std::string::const_iterator start,
    std::string::const_iterator end,
    boost::match_flag_type& flags,
    int& offset,
    ofstream& fout,
    string& np);
void doClActionOnOpeningString(/*
    match_results<std::string::const_iterator>& what,*/
    std::string::const_iterator& start,
    std::string::const_iterator& end,
    boost::match_flag_type& flags,
    int& offset,
    ofstream& fout,
    std::string& np);
void doRsTag(
    std::string::const_iterator& start,
    std::string::const_iterator& end,
    boost::match_results<std::string::const_iterator> what1,
    boost::match_flag_type& flags,
    int& offset,
    deque<std::string>* ids,
    deque<std::string>* nps,
    deque<uint64_t>* npBegins,
    deque<uint64_t>* npEnds,
    std::string& np);

int main(int argc,char* argv[])
{
  QCoreApplication a(argc, argv);
  QsLogging::initQsLog();
  bool docatch = false;
  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      std::string arg(argv[i]);
//      int pos = -1;
      if (arg== "--catch")
          docatch = true;
    }
  }
  if (docatch)
  {
    try 
    { 
      std::cerr << "Doing work in try block." << std::endl;
      return dowork(argc, argv); 
    }
    catch (const std::exception& e) 
    {
      std::cerr << "Catched an exception: " << e.what() << std::endl;
    }
    catch (...) 
    {
      std::cerr << "Catched an unknown exception " << std::endl;
    }
  }
  else 
    return dowork(argc,argv);
}
// ofstream ftmp;
int dowork(int argc,char* argv[])
{
  uint64_t beginTime=TimeUtils::getCurrentTime();
  string inputFile("");
  string outputFile("");
  string corpusFormat(""); 
  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      std::string arg(argv[i]);
      int pos = -1;
      if ( arg[0] == '-' )
      {
        if (arg == "--help" || arg == "-h")
          cout << USAGE << endl;
//         else if ( (pos = arg.find("--inputT=")) != -1 )
//           input_t = arg.substr(pos+9);
        else if ( (pos = arg.find("--xml=")) != -1 )
          corpusFormat = arg.substr(pos+6);
        else cout << USAGE << endl;
      }
      else
      {
        if (inputFile=="")
          inputFile=arg;
        else
          outputFile=arg;
      }
    }
  }

  
  if (corpusFormat.size()<1)
  {
    corpusFormat="wh";
    std::cerr << "warning: no corpus format defined. Default corpus format is \"wh\"." << std::endl;   
  }

  if (outputFile == "")
    outputFile = inputFile + ".csv";

  // loading of the input file
  TimeUtils::updateCurrentTime();
  std::ifstream fin(inputFile.c_str(), std::ifstream::binary);
  ofstream fout(outputFile.c_str(), std::ofstream::binary);
//    ofstream ftmp((inputFile +".txt").c_str(), std::ofstream::binary);
  if (! fout.good()) {
    cerr << "cannot open output file [" << outputFile << "]\n" << endl;
    exit(1);
  }
  
  if (corpusFormat=="wh")
    whProcess(fin, fout);
  else if (corpusFormat=="cl")
    clProcess(fin, fout/*, ftmp*/);
  else
  {
    cerr << "format " << corpusFormat << " not recognized... used wh|cl\n" << endl;
    exit(1);
  }
  //close stream
  fout.close();
  
  TIMELOGINIT;
  LINFO << "Total: " << TimeUtils::diffTime(beginTime,TimeUtils::getCurrentTime()) << " ms";
//   
  return SUCCESS_ID;

}


string preprocessLine (string s) 
{
  const string format_string1(" $1");
  const string format_string2("$1 $2");
  const string format_string3("$1 $2 $3");
  boost::regex e;
  
  e = boost::regex("[[:space:]]");
  s = boost::regex_replace(s, e, "");

  e = boost::regex("[-_]");
  s = boost::regex_replace(s, e, "");

  e = boost::regex("</?SEG>");
  s = boost::regex_replace(s, e, "");


//produce .txt for analyzeText from Clouzot.xml
//   e = boost::regex("<LIST>[^<]*</LIST>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<ITEM>[^<]*</ITEM>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<HEAD>[^<]*</HEAD>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<BIBL>[^<]*</BIBL>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<TITLE>[^<]*</TITLE>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<AUTHOR>[^<]*</AUTHOR>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<FRONT>[^<]*</FRONT>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<DOCAUTHOR>[^<]*</DOCAUTHOR>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<DOCDATE>[^<]*</DOCDATE>");
//   s = boost::regex_replace(s, e, "");
//   e = boost::regex("<[^>]*>");
//   s = boost::regex_replace(s, e, "");


  return s;
}


void whProcess(
  ifstream& fin,
  ofstream& fout)
{
  string s;
  int offset;
  boost::regex expression(eWhStart);

  while( getline(fin,s) ) 
  {
    if (expression==eWhBeginTag)
    {
      s = preprocessLine(s);
    }

    std::string::const_iterator start, end; 
    start = s.begin(); 
    end = s.end(); 
    boost::match_results<std::string::const_iterator> what; 
    boost::match_flag_type flags = boost::match_default; 
    while (regex_search(start, end, what, expression, flags))   
    {
      if (expression == eWhStart)
      {
        offset = start - what[0].second ;
        expression = eWhBeginTag;
      }
      else if (expression == eWhBeginTag)
      {
        string np("");
        doWhActionOnOpeningString(what, start, end, flags, offset,  fout, np);
        // update search position: 
        start = what[0].second; 
        // update flags: 
        flags |= boost::match_prev_avail; 
      }
    }
    if (s.size()>1)
      offset += end-start;
  }
}


void doWhActionOnOpeningString(
    boost::match_results<std::string::const_iterator>& what,
    std::string::const_iterator start,
    std::string::const_iterator end,
    boost::match_flag_type& flags,
    int& offset,
    ofstream& fout,
    string& np) 
{
  uint64_t npBegin;
  uint64_t npEnd;
  boost::match_results<std::string::const_iterator> what1;
  boost::match_results<std::string::const_iterator> what2;

  // update data to be written
  // npBegin = offset + (what[0].second-start) - (what[0].second - what[0].first); 
  npBegin = offset + what[0].first - start; 
  offset += what[0].first - start; 
  string id = string(what[1].first,what[1].second);
  string ref= string(what[3].first,what[3].second);
  string categ = string(what[5].first,what[5].second);
  string::const_iterator begin = what[0].second;
  start = what[0].second;
  // if next is opening tag
  while (regex_search(start, end, what1, eWhBeginTag, flags)&&
      regex_search(start, end, what2, eWhEndTag, flags) &&
      what1[0].first<what2[0].first)
  {
    np += string(start,what1[0].first);
    string internNp("");
    doWhActionOnOpeningString(what1, start, end, flags, offset, fout, internNp);
    start = what1[0].second;
    np += internNp;
  }

  if (!regex_search(start, end, what, eWhEndTag, flags))
  {
    cerr << "error: miss a closing tag </COREF>" << endl;
    return;
  }
  npEnd = offset + (what[0].first - start);
  if (np =="")
    np = string(begin, what[0].first); 
  else
    np += string(start, what[0].first); 
  offset += (what[0].first - start); 
  // specific processing of "demanda-t-il" "elle-même" etc....
  if (np=="til"||np=="tils"||np=="telle"||np=="telles"||np=="ton")
    npBegin++;
//   if (int pos = np.find("même") != -1)
//     npEnd==pos;
  // write on ofstream
  fout << id << ";" << ref << ";" << npBegin << ";" << npEnd << ";" << np << ";" << categ <<";" << endl;  
  fout.flush();
}

void clProcess(ifstream& fin, ofstream& fout/*,ofstream& ftmp*/)
{
  string s;
  int offset=0;
  while( getline(fin,s) ) 
  {
    s = preprocessLine(s);
// ftmp << s;

    std::string::const_iterator start(s.begin());
    std::string::const_iterator end(s.end()); 
    boost::match_results<std::string::const_iterator> what; 
    boost::match_flag_type flags = boost::match_default; 
    while (regex_search(start, end, what, eClStart, flags))   
    {
      cpt++;
      string np("");
      start = what[0].second; 
      doClActionOnOpeningString(start, end, flags, offset, fout, np); 
      // update flags: 
      flags |= boost::match_prev_avail; 
    }
  }
}

void doClActionOnOpeningString(
    std::string::const_iterator& start,
    std::string::const_iterator& end,
    boost::match_flag_type& flags,
    int& offset,
    ofstream& fout,
    std::string& np    )
{
  LIMA_UNUSED(np);
  CorefsMap CMap;
  deque<std::string>* ids = new deque<std::string>();
  deque<std::string>* nps = new deque<std::string>();
  deque<uint64_t>* npBegins = new deque<uint64_t>();
  deque<uint64_t>* npEnds = new deque<uint64_t>();

  while(true)
  {
    boost::match_results<std::string::const_iterator> what;
    boost::match_results<std::string::const_iterator> what1;
    boost::match_results<std::string::const_iterator> what2;
    boost::match_results<std::string::const_iterator> what3;

    regex_search(start, end, what1, eClBeginTag, flags);
    regex_search(start, end, what2, eClLinkBeginTag, flags);
    regex_search(start, end, what3, eClStop, flags);

    // if next is <RS ...>
    if (what1[0]!="" && what3[0] != "" && what1[0].first<what3[0].first)
    {   
      string internNp("");
      doRsTag(start, end, what1, flags, offset, ids, nps, npBegins, npEnds, internNp);
    }
    // if next is <LINK ...>
    else if (what2[0]!="" && what3[0] != "" && what2[0].first<what3[0].first)
    {
      if (string(what2[2].first,what2[2].second)=="COREF"||string(what2[2].first,what2[2].second)=="IDSEM") 
      {
        // extraction de la reference la plus à gauche
        regex_search(what2[0].first,what2[0].second,what,eClLinkExtract, flags);
        stringstream sstream;
        std::string ref = string(what[1].first,what[1].second);
        boost::match_results<std::string::const_iterator> whatCompAnte;
        boost::match_results<std::string::const_iterator> whatCompAna;
        regex_search(what[1].first,what[1].second, whatCompAnte, boost::regex("([[:digit:]]+)\\.[[:digit:]]+"));
        // extraction de la référence la plus à droite
        // et inscription dans la map
//         cerr<<string(what[0].second, what2[0].second)<<endl;;
        sstream << cpt;
        
        while(regex_search(what[0].second, what2[0].second,what, eClLinkExtract, flags))        
        {
          what1 = what;
        }
        regex_search(what1[1].first,what1[1].second,whatCompAna,boost::regex("([[:digit:]]+)\\.[[:digit:]]+"));
        // Renumérotation. Exemples :
        // si id = 12.4 et ref = 12.3
        // alors => id = 118.12.4 et ref = 118.12.3
        // si id = 12.4 et ref = 10.3
        // alors => id = 118.12.4 et ref = 116.10.3
        stringstream sstream2;
        sstream2 << cpt-
          (atoi(const_cast<char*>(string(whatCompAna[1].first,whatCompAna[1].second).c_str()))
          -atoi(const_cast<char*>(string(whatCompAnte[1].first,whatCompAnte[1].second).c_str())));
          CMap.insert(make_pair(sstream.str() + "." + string(what1[1].first,what1[1].second),sstream2.str() + "." +ref));

//         // extraction des autres références (de gauche à droite)
//         // et inscription dans la map 
// n'est pas valide !! quand 3 membres dans balise link il ne s'agit pas d une chaine de coreferences mais de l'entité la plus à droite qui réfèrent aux deux autres à la fois.
// //         cerr<<string(what[0].second, what2[0].second)<<endl;;
//         sstream << cpt;
//         while(regex_search(what[0].second, what2[0].second,what, eClLinkExtract, flags))        
//         {
//           regex_search(what[1].first,what[1].second,whatCompAna,regex("([[:digit:]]+)\\.[[:digit:]]+"));
//           // Renumérotation. Exemples :
//           // si id = 12.4 et ref = 12.3
//           // alors => id = 118.12.4 et ref = 118.12.3
//           // si id = 12.4 et ref = 10.3
//           // alors => id = 118.12.4 et ref = 116.10.3
//           stringstream sstream2;
//           sstream2 << cpt-
//             (atoi(const_cast<char*>(string(whatCompAna[1].first,whatCompAna[1].second).c_str()))
//             -atoi(const_cast<char*>(string(whatCompAnte[1].first,whatCompAnte[1].second).c_str())));
//             CMap.insert(make_pair(sstream.str() + "." + string(what[1].first,what[1].second),sstream2.str() + "." +ref));
// //             cerr << sstream.str() + "." + string(what[1].first,what[1].second) << " - " << sstream2.str() + "." +ref << endl;
//           
// //  cerr<<string(what[0].second, what2[0].second)<<endl;;
//         }
      }
        offset+= what2[0].first-start;
        start = what2[0].second;
    }
    // if next is </P>
    else
    {
      while (!ids->empty())
      {
        std::string ref;
        if (CMap.find(*ids->begin())!=CMap.end())
          ref = (*CMap.find(*ids->begin())).second;
        else ref = "";
        // write on ofstream
        fout << *ids->begin() << ";";
        fout << ref << ";" ;
        fout << *npBegins->begin() << ";";
        fout << *npEnds->begin() << ";";
        fout << *nps->begin() << ";" << endl;
        ids->pop_front();
        npBegins->pop_front();
        npEnds->pop_front();
        nps->pop_front();
      }
      offset += what3[0].first-start;
      start = what3[0].second;
      fout.flush();
      return;
    }
  }
}

void doRsTag(
    std::string::const_iterator& start,
    std::string::const_iterator& end,
    boost::match_results<std::string::const_iterator> what,
    boost::match_flag_type& flags,
    int& offset,
    deque<std::string>* ids,
    deque<std::string>* nps,
    deque<uint64_t>* npBegins,
    deque<uint64_t>* npEnds,
    std::string& np) 
{
  boost::match_results<std::string::const_iterator> what2;
  stringstream sstream;
  sstream << cpt;
  string id =  sstream.str() + "." + string(what[1].first,what[1].second);
  uint64_t npBegin = offset + what[0].first - start;
  offset += what[0].first -start;
//   string::const_iterator begin = what[0].second;
  start = what[0].second;
  // if next is opening tag
  while (regex_search(start, end, what, eClBeginTag, flags)&&
         regex_search(start, end, what2, eClEndTag, flags) &&
         what[0].first<what2[0].first)
  {
    string internNp("");
    np += string(start,what[0].first);
    doRsTag(start, end, what, flags, offset, ids, nps, npBegins, npEnds, internNp);
    np += internNp;

  }
  if (!regex_search(start, end, what, eClEndTag, flags))
  {
    cerr << "error: miss a closing tag </COREF>" << endl;
    return;
  }
  uint64_t npEnd = offset + (what[0].first - start);
//   if (np =="")
//     np += string(start, what[0].first); 
//   else
    np += string(start, what[0].first); 
  ids->push_back(id);
  npBegins->push_back(npBegin); 
  nps->push_back(np);
  npEnds->push_back(npEnd);
  offset+= what[0].first - start;
  start = what[0].second;
}
