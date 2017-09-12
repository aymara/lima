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
 * @file        evalCoref.cpp
 * @author      Claire Mouton (Claire.Mouton@cea.fr) 

 *              Copyright (c) 2007 by CEA
 * @date        Created on Jun, 6 2007
 */

#include <string>
#include <set>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <boost/regex.hpp>
#include <math.h>

#include <QtGlobal>

using namespace std;
using namespace boost;
// using namespace Lima;


typedef pair < string, pair<float, float> > NamedBounds;
typedef map<string, string> CorefsMap;
typedef map<string, pair<float, float> > NamedBoundsMap;
typedef map<string,string> TextMap;
typedef map<string,bool> DefinitesMap;

//help mode & usage
Q_GLOBAL_STATIC_WITH_ARGS(std::string, USAGE, ( "usage: evalCoref [-p] [-r] [-d] [-v] limaCSV corpusCSV outputFile\n"
" -p precision\n"
" -r recall\n"
" -d processDefinites\n"
" -v verbose\n" ) ); 
double prec = 0;
double rec = 0;


int dowork(int argc,char* argv[]);


// Handle reference corpus
void init(string csvFile, vector<set<string> >* corefChains, NamedBoundsMap* BMap, TextMap* TMap, DefinitesMap* DMap);

// Handle LIMA results
void init(string csvFile, CorefsMap* CMap, NamedBoundsMap* BMap, TextMap* TMap, DefinitesMap* DMap);

// Compare data from reference and from LIMA
void process(vector<set<string> >* corefChains, CorefsMap* CMap, NamedBoundsMap* mainBMap, TextMap* mainTMap, NamedBoundsMap* refBMap, TextMap* refTMap, DefinitesMap* DMap, bool inclusion, bool processDefinites, string& errors, string& rate);

bool isIncluded(NamedBounds a, NamedBounds b);
// bool areInTheSameCorefChain(vector<set<string> >* corefChains, string id2, string ref2);

#ifdef WIN32
double round(double value) {
     return floor(value + 0.5);
}
#endif

int main(int argc,char* argv[])
{
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
      cerr << "Doing work in try block." << endl;
      return dowork(argc, argv); 
    }
    catch (const std::exception& e) 
    {
      cerr << "Catched an exception: " << e.what() << endl;
    }
    catch (...) 
    {
      cerr << "Catched an unknown exception " << endl;
    }
  }
  else 
    return dowork(argc,argv);
}

int dowork(int argc,char* argv[])
{
  bool precision = false;
  bool recall = false;
  bool verbose = false;
  bool processDefinites = false;
  string start("<?xml version=\"1.0\" encoding=\"utf-8\"?><BODY><a name=\"top\">");
  string precisionErrors("Precision Errors\n<br/>-------------\n<br/>\n<br/>");
  string precisionRate("Precision Rate\n<br/>-------------\n<br/>\n<br/>");
  string recallErrors("Recall Errors\n<br/>-------------\n<br/>\n<br/>");
  string recallRate("Recall Rate\n<br/>-------------\n<br/>\n<br/>");
  string errors("\n<br/>ERRORS\n<br/>-------------\n<br/>\n<br/>");
  string rates("\n<br/>RATES\n<br/>------------\n<br/>\n<br/>");
  char* fmeasure = new char();

  vector<set<string> >* corefChains = new vector<set<string> >();
  NamedBoundsMap* corpusBMap = new NamedBoundsMap();
  TextMap* corpusTMap = new TextMap();
  CorefsMap* CMap = new CorefsMap();
  NamedBoundsMap* limaBMap = new NamedBoundsMap();
  TextMap* limaTMap = new TextMap();
  DefinitesMap* limaDMap = new DefinitesMap();

  string configDir=string(getenv("LIMA_CONF"));
  string corpusCsv("");
  string limaCsv("");
  string outputFile(""); 
  if (argc>1)
  {
    for (int i = 1 ; i < argc; i++)
    {
      std::string arg(argv[i]);
      if ( arg[0] == '-' )
      {
        if (arg == "--help" || arg == "-h")
          cout << *USAGE << endl;
        else if (arg == "-p")
          precision=true;
        else if (arg == "-r")
          recall=true;
        else if (arg == "-d")
          processDefinites=true;
        else if (arg == "-v")
          verbose=true;
        else cout << *USAGE << endl;
      }
      else
      {
        if (limaCsv=="")
          limaCsv=arg;
        else 
        if (corpusCsv=="")
          corpusCsv=arg;
        else
          outputFile=arg;
      }
    }
  }

  if (outputFile == "" )
    outputFile = limaCsv.substr(0,limaCsv.size()-8) + ".eval.html";

  ofstream fout(outputFile.c_str(), std::ofstream::binary);
  if (! fout.good()) {
    cerr << "cannot open output file [" << outputFile << "]" << endl;
    exit(1);
  }

  if (precision)
  {
    std::cerr << "Compute precision" << std::endl;
    init(corpusCsv, corefChains, corpusBMap, corpusTMap, new DefinitesMap());
    init(limaCsv, CMap, limaBMap, limaTMap, limaDMap);
    process(corefChains, CMap, limaBMap, limaTMap, corpusBMap, corpusTMap, limaDMap, true, processDefinites,precisionErrors, precisionRate);
    errors += precisionErrors;
    rates += precisionRate;

    CMap->clear();
    corefChains->clear();
    corpusBMap->clear();
    corpusTMap->clear();
    limaBMap->clear();
    limaTMap->clear();
    limaDMap->clear();
  }
  if (recall)
  {
    std::cerr << "Compute recall" << std::endl;
    init(corpusCsv, CMap, corpusBMap, corpusTMap, new DefinitesMap());
    init(limaCsv, corefChains, limaBMap, limaTMap, limaDMap);
    process(corefChains, CMap, corpusBMap, corpusTMap, limaBMap, limaTMap, limaDMap, false, processDefinites, recallErrors, recallRate);
    errors += recallErrors;
    rates += recallRate;
  }
  std::cerr << "Outputs results" << std::endl;
  sprintf(fmeasure,"%.2f", 2*(prec*rec)/(prec+rec));
  string fmeas(fmeasure);
  fmeas=("\n<br/>F-Measure = " + fmeas + "\n<br/>\n<br/>");
  fout << start << errors << rates << fmeas << endl;
  string s;
  std::ifstream fcorp(corpusCsv.substr(0,corpusCsv.size()-4).c_str(), std::ifstream::binary);
  while( getline(fcorp,s) ) 
  {
    boost::regex e;  
    // Wolverhampton format
    e = regex("(<COREF ID=\")([[:alnum:]]+)(\"[^>]*REF=\")?([[:alnum:]]+)?(\"[^>]*>)([^<]*)(</COREF>)");
    s = regex_replace(s, e, "<a name=\"L$2\">$1$2$3$4$5<font color=red>$6</font><sub>$2</sub><sup>$4</sup>$7");

    // Catherine Clouzot format (Cf. doc_clouzot.rtf for details)
    e = regex("(<RS ID=\")([[:alnum:]]+[.][[:alnum:]]+)(\"[^>]*>)([^<]*)(</RS>)");
    s = regex_replace(s, e, "<a name=\"C$2\">$1$2$3$4$5");
    e = regex("(<RS ID=\")([[:alnum:]]+[.][[:alnum:]]+)(\"[^>]*>)([^<]*)(</RS>)");
    s = regex_replace(s, e, "$1$2$3<font color=red>$4</font><sub>$2</sub>$5");

    s += "<br><a href=\"#top\">top</a><br>";   
    fout << s;
  }
  std::ifstream flima(limaCsv.substr(0,limaCsv.size()-4).c_str(), std::ifstream::binary);
  while( getline(flima,s) ) 
  {
    boost::regex e;  

    e = regex("(<COREF ID=\")([[:alnum:]]+)(\"[^>]*REF=\")?([[:alnum:]]+)?(\"[^>]*>)([^<]*)(</COREF>)");
    s = regex_replace(s, e, "<a name=\"L$2\">$1$2$3$4$5<font color=red>$6</font><sub>$2</sub><sup>$4</sup>$7");
    e = regex("(<COREF[^>]*CATEG=\"other\"[^>]*><font color=)red(>[^<]*</font>)");
    s = regex_replace(s, e, "$1blue$2"); 
    e = regex("([>[:alnum:]] [.] )");
    s = regex_replace(s, e, "$1 <br/><a href=\"#top\">top</a><br/>");

    fout << s;
  }
  fout << "</BODY>" << endl;
  fout.flush();
  fout.close();
  if (verbose)
  {
    std::cerr << "Show F-measures:" << std::endl;
    cerr << rates << fmeas << endl;
  }

  delete corefChains;
  delete CMap;
  delete corpusBMap;
  delete corpusTMap;
  delete limaBMap;
  delete limaTMap;
  delete limaDMap;
  return 0;
}

// Handle LIMA results
void init(string csvFile, CorefsMap* CMap, NamedBoundsMap* BMap, TextMap* TMap, DefinitesMap* DMap)
{
  std::cerr << "Init LIMA results" << std::endl;
  std::ifstream fin(csvFile.c_str(), std::ifstream::binary);
  string s;
  regex expression("([^;]*);([^;]*);([^;]*);([^;]*);([^;]*);([^;]*);?$");
  while( getline(fin,s) ) 
  {
    match_results<std::string::const_iterator> what;
    string::const_iterator start = s.begin();
    string::const_iterator end = s.end();
    regex_search(start, end, what, expression);
    string id = string(what[1].first, what[1].second);
    string ref = string(what[2].first, what[2].second);
    string text = string(what[5].first, what[5].second);
    bool definites =  string(what[6].first, what[6].second)!="other"
               && string(what[6].first,what[6].second)!="reflPron";   
    float boundA = atoi(string(what[3].first, what[3].second).c_str());
    float boundB = atoi(string(what[4].first, what[4].second).c_str());
    BMap->insert(make_pair(id, make_pair(boundA, boundB)));
    TMap->insert(make_pair(id, text));
    DMap->insert(make_pair(id, definites));
    if (ref!="")
      CMap->insert(make_pair(id, ref));
  }

//   cerr << "map : "<< endl;
//   for (CorefsMap::iterator it = CMap->begin(); it!=CMap->end();it++)
//   { 
//     cerr << (*it).first << " - " << (*it).second << endl;
//   }    
}

// Handle reference corpus
void init(string csvFile, vector<set<string> >*corefChains, NamedBoundsMap* BMap, TextMap* TMap, DefinitesMap* DMap)
{
  std::cerr << "Init reference corpus" << std::endl;
  std::ifstream fin(csvFile.c_str(), std::ifstream::binary);
  string line;
  regex expression("([^;]*);([^;]*);([^;]*);([^;]*);([^;]*);([^;]*);?$");
  match_results<std::string::const_iterator> what; 
  while( getline(fin,line) )
  {
    string::const_iterator start = line.begin();
    string::const_iterator end = line.end();
    if (regex_search(start, end, what, expression))
    {
      string id = string(what[1].first, what[1].second);
      string ref = string(what[2].first, what[2].second);
      string text = string(what[5].first, what[5].second);
      bool definites = string(what[6].first, what[6].second)!="other"
            && string(what[6].first, what[6].second)!="reflPron";
      float boundA = atoi(string(what[3].first, what[3].second).c_str());
      float boundB = atoi(string(what[4].first, what[4].second).c_str());
      BMap->insert(make_pair(id, make_pair(boundA, boundB)));
      TMap->insert(make_pair(id, text));
      DMap->insert(make_pair(id, definites));

      if (ref != "")
      {
        bool inserted = false;
        for (vector<set<string> >::iterator it = corefChains->begin();  it!=corefChains->end();it++)
        {
          if ((*it).find(ref)!=(*it).end())
          {
            (*it).insert(id);
            inserted = true;
            break;
          }
        }
        if (!inserted)
        {
          for (vector<set<string> >::iterator it = corefChains->begin();  it!=corefChains->end();it++)
          {
            if ((*it).find(id)!=(*it).end())
            {
              (*it).insert(ref);
              inserted = true;
              break;
            }
          }
        }
        if (!inserted)
        {
          corefChains->push_back(set<string>());
          (*corefChains->rbegin()).insert(id);
          (*corefChains->rbegin()).insert(ref);
        }
      }
    }
    else
    {
      std::cerr << "Error matching reference file line '"<<line<<"'" << std::endl;
    }
  }
//   for (vector<set<string> >::iterator it1 = corefChains->begin(); it1!=corefChains->end();it1++)
//   { 
//     cerr << "chaine : "<< endl;
//     for (set<string>::iterator it2 = (*it1).begin(); it2!=(*it1).end(); it2++)
//       cerr << *it2 << endl;
//   }    
}

void process(vector<set<string> >* corefChains, CorefsMap* CMap, NamedBoundsMap* mainBMap, TextMap* mainTMap, NamedBoundsMap* refBMap, TextMap* refTMap, DefinitesMap* DMap, bool inclusion, bool processDefinites, string& errors, string& rate) 
{
  stringstream sstream;
  float cpt = 0;
  float cptTrue = 0;
  float cptStandardError = 0;
  float cptNotAnnotatedError = 0;
  float cptNotFoundError = 0;
  
  for (CorefsMap::iterator mainCIt = CMap->begin();
          mainCIt != CMap->end();
          mainCIt++)
  {
    bool pass=false;
    cpt++;
    string id1((*mainCIt).first);
    string ref1((*mainCIt).second);
    string id2("");
    string ref2("");
    set<string> chain;
    for (NamedBoundsMap::iterator refBIt = refBMap->begin();
          refBIt != refBMap->end() && id2=="";
          refBIt++)
    {
      // inclusion should be interval(COREF) € interval(COREF')\interval(COREF'' encapsulated)
      // done automatically from the structure of the csv file
      // hence the relevance of the for_loop condition id2 == "" 

      // precision case
      if (inclusion)
      {
        if ((!processDefinites) && (*DMap->find(id1)).second)
        {
          pass = true;
          id2 = " ";
          ref2 = " ";
          cpt--; 
          break;
        }
        // if id1 has an equivalent mark in 2
        if (id2=="" && isIncluded(*mainBMap->find(id1),*refBIt))
        {
          id2 = (*refBIt).first;
        }
        if (id2!="" && ref2=="")
        {
          for(vector<set<string> >::iterator itc = corefChains->begin(); itc != corefChains->end(); itc++)
          {
            if((*itc).find(id2)!=(*itc).end())
            {
              chain = (*itc);
            }
          }
          for (set<string>::iterator itc = chain.begin(); itc != chain.end(); itc++)
          {
            // if ref1 has an equivalent mark in 2
            if (mainBMap->find(ref1)!=mainBMap->end() && refBMap->find(*itc)!=refBMap->end() && isIncluded(*mainBMap->find(ref1),*refBMap->find(*itc)))
            {
              ref2 = *itc;
              cptTrue++;
              break;
            }
          }
        }
      }
      // recall case
      else if (!inclusion)
      {
        // if id1 has an equivalent mark in 2
        if (id2== "" && mainBMap->find(id1)!=mainBMap->end() && isIncluded(*refBIt,*mainBMap->find(id1)))
        {
          if (!processDefinites && (*DMap->find((*refBIt).first)).second)
          {
            pass = true;
            id2 = " ";
            cpt--;
            break;
          }
          id2 = (*refBIt).first;
        }
        if (id2!="" && ref2=="")
        {
          for(vector<set<string> >::iterator itc = corefChains->begin(); itc != corefChains->end(); itc++)
          {
            if((*itc).find(id2)!=(*itc).end())
            {
              chain = (*itc);
              break;
            }
          }
          for (set<string>::iterator itc = chain.begin(); itc != chain.end(); itc++)
          {
            // if ref1 has an equivalent mark in corefsChain 2
            if (mainBMap->find(ref1)!=mainBMap->end() && refBMap->find(*itc) != refBMap->end() && isIncluded(*refBMap->find(*itc),*mainBMap->find(ref1)))
            {
              ref2 = (*itc);
              cptTrue++;
              break;
            }
          }
        }
      }
    }
    if (inclusion)
    {
      // si les bornes de ref1 sont référencées par la sortie de Lima
      // et si ref2 appartient à une chaine de coreferences référencée
      // et si les bornes de ref2 ne sont pas référencées
      //    ou si bornes(ref1) n'est pas inclut dans bornes(ref2)
      if (mainBMap->find(ref1)!=mainBMap->end() &&  (!chain.empty()) && (refBMap->find(ref2)==refBMap->end() || !isIncluded(*mainBMap->find(ref1),*refBMap->find(ref2))))
      {
        ref2 = *chain.begin();
        if (ref2==id2)
        {
          set<string>::iterator itc = chain.begin();
          itc++;
          ref2 = *itc;
        }
        string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        string token_id2;
        if (refTMap->find(id2)!=refTMap->end())
          token_id2 = (*refTMap->find(id2)).second;
        else
          token_id2 = "?"; 
        string token_ref2;
        if (refTMap->find(ref2)!=refTMap->end())
          token_ref2 = (*refTMap->find(ref2)).second;
        else
          token_ref2 = "?";
        errors += "IdLima: <a href=\"#L"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefLima: "+ "<a href=\"#L"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") IdCorpus: "+ "<a href=\"#C"+ id2 + "\">" + id2 + "</a>" +" ("+token_id2+") RefCorpus: "+ "<a href=\"#C"+ ref2 + "\">" + ref2 + "</a>" +" ("+token_ref2+")\n<br/>\n<br/>";
        cptStandardError++;
        continue;
      }
      else if (processDefinites && ref2=="" && id2!="")
      {
        string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        string token_id2;
        if (refTMap->find(id2)!=refTMap->end())
          token_id2 = (*refTMap->find(id2)).second;
        else
          token_id2 = "?";
        errors += "IdLima: <a href=\"#L"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefLima: "+ "<a href=\"#L"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") IdCorpus: "+ "<a href=\"#C"+ id2 + "\">" + id2 + "</a>" +" ("+token_id2+") - No reference found in the corpus for this id -> not definite? pleonastic?\n<br/>\n<br/>";
        cptNotAnnotatedError++;
        continue;
      }
      else if (!processDefinites && !pass && ref2=="" && id2!="")
      { 
        string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        string token_id2;
        if (refTMap->find(id2)!=refTMap->end())
          token_id2 = (*refTMap->find(id2)).second;
        else
          token_id2 = "?";
        errors += "IdLima: <a href=\"#L"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefLima: "+ "<a href=\"#L"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") IdCorpus: "+ "<a href=\"#C"+ id2 + "\">" + id2 + "</a>" +" ("+token_id2+") - No reference found in the corpus for this id -> pleonastic?\n<br/>\n<br/>";
        cptNotAnnotatedError++;
        continue;
      }
      else if (id2=="")
      {
        string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        errors += "IdLima: <a href=\"#L"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefLima: "+ "<a href=\"#L"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") - Np ("+token_id1+") not refered as Np in the corpus\n<br/>\n<br/>";
        cptNotFoundError++;
        continue;
      }
      else if (ref2=="")
      { 
        cerr << "error: at this point ref 2 should be or resolved or already caught " << cpt<< endl;
        continue;
      }
    }
    else if (!inclusion)
    {
      if (mainBMap->find(ref1)!=mainBMap->end() &&  (!chain.empty()) && (refBMap->find(ref2)==refBMap->end() || !isIncluded(*refBMap->find(ref2),*mainBMap->find(ref1))))
      {
        ref2 = *chain.begin();
        string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        string token_id2;
        if (refTMap->find(id2)!=refTMap->end())
          token_id2 = (*refTMap->find(id2)).second;
        else
          token_id2 = "?"; 
        string token_ref2;
        if (refTMap->find(ref2)!=refTMap->end())
          token_ref2 = (*refTMap->find(ref2)).second;
        else
          token_ref2 = "?";
        errors += "IdCorpus: <a href=\"#C"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefCorpus: "+ "<a href=\"#C"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") IdLima: "+ "<a href=\"#L"+ id2 + "\">" + id2 + "</a>" +" ("+token_id2+") RefLima: "+ "<a href=\"#L"+ ref2 + "\">" + ref2 + "</a>" +"("+token_ref2+")\n<br/>\n<br/>";
        cptStandardError++;
        continue;
      }
      else if (id2=="")
      {
      string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        errors += "IdCorpus: <a href=\"#C"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefCorpus: "+ "<a href=\"#C"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") - Np ("+token_id1+") not referenced as Np in the lima output\n<br/>\n<br/>";
        cptNotFoundError++;
        continue;
      }
      else if (processDefinites && ref2=="")
      {
        string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        string token_id2;
        if (refTMap->find(id2)!=refTMap->end())
          token_id2 = (*refTMap->find(id2)).second;
        else
          token_id2 = "?";          
        errors += "IdCorpus: <a href=\"#C"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefCorpus: "+ "<a href=\"#C"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") IdLima: "+ "<a href=\"#L"+ id2 + "\">" + id2 + "</a>" +" ("+token_id2+") - No reference found in the lima output for this id -> definite ?\n<br/>\n<br/>";
        cptNotAnnotatedError++;
        continue;
      }
      else if (!processDefinites && !pass && ref2=="")
      {
        string token_id1;
        if (mainTMap->find(id1)!=mainTMap->end())
          token_id1 = (*mainTMap->find(id1)).second;
        else
          token_id1 = "?";
        string token_ref1;
        if (mainTMap->find(ref1)!=mainTMap->end())
          token_ref1 = (*mainTMap->find(ref1)).second;
        else
          token_ref1 = "?";
        string token_id2;
        if (refTMap->find(id2)!=refTMap->end())
          token_id2 = (*refTMap->find(id2)).second;
        else
          token_id2 = "?";          
        errors += "IdCorpus: <a href=\"#C"+ id1 + "\">" + id1 + "</a>" +" ("+token_id1+") RefCorpus: "+ "<a href=\"#C"+ ref1 + "\">" + ref1 + "</a>" +" ("+token_ref1+") IdLima: "+ "<a href=\"#L"+ id2 + "\">" + id2 + "</a>" +" ("+token_id2+") - No reference found in the lima output for this id...!\n<br/>\n<br/>";
        cptNotAnnotatedError++;
        continue;
      }
    }
  }
  if (cpt!=0)
  {
    if (inclusion)
      prec = 100.*cptTrue/cpt;
    else
      rec =  100.*cptTrue/cpt;
    sstream 
    << "-----------------------------------" 
    << "\n<br/> Positive: " << round(100.*cptTrue/cpt) << " % (" << cptTrue 
    << ")\n<br/>-----------------------------------"
    << "\n<br/> Standard errors: " << round(100.*cptStandardError/cpt) << " % (" << cptStandardError
    << ")\n<br/> Not coreferent errors: " << round(100.*cptNotAnnotatedError/cpt) << " % (" << cptNotAnnotatedError
    << ") \n<br/> NP not noticed errors: " << round(100.*cptNotFoundError/cpt) << " % (" << cptNotFoundError << ")\n<br/>-----------------------------------";
  }
  else
    rate += "no coref found...";
  rate += sstream.str() + "\n<br/>\n<br/>\n<br/>";
}


bool isIncluded (NamedBounds a, NamedBounds b)
{
  return (a.second.first >= b.second.first
           && a.second.second <= b.second.second);
}
