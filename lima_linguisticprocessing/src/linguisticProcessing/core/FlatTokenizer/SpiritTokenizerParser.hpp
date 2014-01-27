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
#ifndef SPIRITTOKENIZERPARSER_HPP
#define SPIRITTOKENIZERPARSER_HPP

#include <boost/config/warning_disable.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/io.hpp>

#include <string>

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#define skipper qi::space | ascii::char_('#') >> *(ascii::char_ - qi::eol) >> qi::eol
typedef BOOST_TYPEOF(skipper) skipper_type_;

enum transitions {
  STORE,
  FLUSH,
  TOKEN,
  EXIT,
  LTOKEN
};

typedef std::vector<std::string> tokenizer_precondition;
typedef std::vector<std::string> tokenizer_postcondition;
struct tokenizer_transition
{
  std::vector<tokenizer_precondition> preconditions;
  std::vector<std::string> event;
  std::vector<tokenizer_postcondition> postconditions;
  transitions transition;
  std::string target;
  std::vector<std::string> statuses;
};

struct tokenizer_state
{
  std::string id;
  std::vector<tokenizer_transition> transitions;
};

typedef std::vector<tokenizer_state> tokenizer_automaton;

BOOST_FUSION_ADAPT_STRUCT(
                          tokenizer_transition,
                          (std::vector<tokenizer_precondition>, preconditions)
                          (std::vector<std::string>, event)
                          (std::vector<tokenizer_postcondition>, postconditions)
                          (transitions, transition)
                          (std::string, target)
                          (std::vector<std::string>, statuses)
                          )
                          
BOOST_FUSION_ADAPT_STRUCT(
                          tokenizer_state,
                          (std::string, id)
                          (std::vector<tokenizer_transition>, transitions)
                          )

using ascii::space_type;

template <typename Iterator>
struct tokenizer : qi::grammar<Iterator, tokenizer_automaton(), skipper_type_>
{
  struct transitionsymbol_ : qi::symbols<char, unsigned>
  {
    // > is +1 store
    // / is +1 flush (forget the current token)
    // = is +1 token
    // ^ is exit

    transitionsymbol_()
    {
      add
      (">", STORE)
      ("/", FLUSH)
      ("=", TOKEN)
      ("^", EXIT)
      ;
    }

  } transitionsymbol;

  tokenizer() : tokenizer::base_type(start)
  {
    using qi::alnum;
    using qi::lexeme;
    using ascii::char_;

    start %= *state ;
    state %= '(' >> identifier >> ')' >> '{' >> *transition >> '}';
    transition %= '-' >> *precondition >> event >> *postcondition >> transitionsymbol >> identifier >> -('(' >> identifier % ',' >> ')');
    identifier %= lexeme[+(alnum | char_('_'))];
    precondition %= '[' >> (identifier % '|') >> ']';
    event %= identifier % '|';
    postcondition %= identifier % '|';
  }

  qi::rule<Iterator, tokenizer_automaton(), skipper_type_> start;
  qi::rule<Iterator, tokenizer_state(), skipper_type_> state;
  qi::rule<Iterator, tokenizer_transition(), skipper_type_> transition;
  qi::rule<Iterator, std::string(), skipper_type_> identifier;
  qi::rule<Iterator, std::vector<std::string>(), skipper_type_> precondition;
  qi::rule<Iterator, std::vector<std::string>(), skipper_type_> event;
  qi::rule<Iterator, std::vector<std::string>(), skipper_type_> postcondition;
};

//]

#endif // SPIRITTOKENIZERPARSER_HPP
