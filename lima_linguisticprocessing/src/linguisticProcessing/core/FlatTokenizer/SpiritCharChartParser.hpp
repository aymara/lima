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
#ifndef SPIRITCHARCHARTPARSER_HPP
#define SPIRITCHARCHARTPARSER_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/program_options.hpp>

#include <boost/shared_ptr.hpp>

#include <fstream>

namespace po = boost::program_options;


#include <iostream>
#include <string>

#define skipper qi::space | '#' >> *(ascii::char_ - qi::eol) >> qi::eol

enum ModifierTypes {
  MIN,
  MAJ,
  UNMARK
};

struct charchart_class
{
  std::string name;
  boost::optional<std::string> parent;
  std::string comment;
};

struct modifierdef
{
  ModifierTypes first;
  int second;
};
struct charchart_char
{
  int code;
  std::string name;
  std::string charclass;
  std::vector<modifierdef> modifiers;
};

struct charchart
{
  std::vector<charchart_class> classes;
  std::vector<charchart_char> chars;
};

BOOST_FUSION_ADAPT_STRUCT(
                          modifierdef,
                          (ModifierTypes, first)
                          (int, second)
                          )
                          
BOOST_FUSION_ADAPT_STRUCT(
                          charchart_class,
                          (std::string, name)
                          (boost::optional<std::string>, parent)
                          (std::string, comment)
                          )
                          
BOOST_FUSION_ADAPT_STRUCT(
                          charchart_char,
                          (int, code)
                          (std::string, name)
                          (std::string, charclass)
                          (std::vector<modifierdef>, modifiers)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          charchart,
                          (std::vector<charchart_class>, classes)
                          (std::vector<charchart_char>, chars)
                          )
                          
namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

struct modifiersymbol_ : qi::symbols<char, unsigned>
{
  // > is +1 store
  // / is +1 flush (forget the current token)
  // = is +1 token
  // ^ is exit
  // & is token (without +1)
  modifiersymbol_()
  {
    add
    ("m", MIN)
    ("M", MAJ)
    ("u", UNMARK)
    ;
  }

};
boost::shared_ptr<modifiersymbol_> modifiersymbol(new modifiersymbol_);

typedef BOOST_TYPEOF(skipper) skipper_type_;


template <typename Iterator>
struct charchart_parser : qi::grammar<Iterator, charchart(), skipper_type_>
{

  charchart_parser() : charchart_parser::base_type(start, "start")
  {
    using qi::eps;
    using qi::lit;
    using qi::_val;
    using qi::_1;
    using ascii::char_;
    using ascii::space_type;
    using qi::lexeme;
    using qi::alnum;
    using qi::hex;
    using qi::eol;

    start %= classes >> chars ;
    classes %= lit("classes") >> '{' >> -(classdef % ';') >> '}';
    chars %= lit("chars") >> '{' >> -(chardef % ';') >> '}';
    identifier %= lexeme[+(alnum | char_('_'))];
    // c_M < C_Mm : Upper case letter
    classdef %= identifier >> -('<' >> identifier) >> ':' >> lexeme[+(char_ - (';'|eol))];
    // 0000, NULL, m_parag
    chardef %= hex >> ',' >> lexeme[+(char_ - ',')] >> ',' >> identifier >> -(',' >> modifier % ',');
    modifier %= *modifiersymbol >> hex;
  }

  qi::rule<Iterator, charchart(), skipper_type_> start;
  qi::rule<Iterator, std::vector<charchart_class>(), skipper_type_> classes;
  qi::rule<Iterator, std::vector<charchart_char>(), skipper_type_> chars;
  qi::rule<Iterator, charchart_class(), skipper_type_> classdef;
  qi::rule<Iterator, charchart_char(), skipper_type_> chardef;
  qi::rule<Iterator, std::string(), skipper_type_> identifier;
  qi::rule<Iterator, modifierdef(), skipper_type_> modifier;
};
//]

#endif // SPIRITCHARCHARTPARSER_HPP
