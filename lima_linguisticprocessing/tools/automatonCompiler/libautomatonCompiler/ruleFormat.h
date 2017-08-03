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
/************************************************************************
*
* File        : ruleFormat.h
* Project     : DETECT - NamedEntities
* Author      : Besancon Romaric (besanconr@zoe.cea.fr)
* Created on  : Wed Nov 27 2002
* Copyright   : (c) 2002 by CEA
*
* Description : definition of the macros representing significant characters
*               and strings in the definition of the rules
*
*************************************************************************/

#ifndef RULEFORMAT_H
#define RULEFORMAT_H

#include "common/Data/LimaString.h"
#include "common/Data/strwstrtools.h"

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//***********************************************************************
// general formatting of the file
//***********************************************************************
const LimaChar CHAR_COMMENT('#');
const LimaString STRING_DEFINE_ENCODING=Common::Misc::utf8stdstring2limastring("set encoding=");
const LimaString STRING_DEFINE_DEFAULTACTION=Common::Misc::utf8stdstring2limastring("set defaultAction=");
const LimaString STRING_UNSET_DEFAULTACTION=Common::Misc::utf8stdstring2limastring("unset defaultAction=");
const LimaString STRING_DEFINE_ENTITYTYPES=Common::Misc::utf8stdstring2limastring("set entityTypes=");
const LimaString STRING_USING_MODEX=Common::Misc::utf8stdstring2limastring("using modex ");
const LimaString STRING_USING_ENTITYGROUPS=Common::Misc::utf8stdstring2limastring("using groups ");
const LimaString STRING_USING_LIBS=Common::Misc::utf8stdstring2limastring("using libs ");
const LimaChar CHAR_SEP_LIST(',');

//***********************************************************************
// syntax for the type definition
//***********************************************************************
// possible types are defined in the rules file by lines like
// define type=LOCATION;ENAMEX TYPE="LOCATION";/ENAMEX;
// define type=PERSON;ENAMEX TYPE="PERSON";/ENAMEX;prenom,nom

const LimaString STRING_DEFINE_TYPE=Common::Misc::utf8stdstring2limastring("define type=");
const LimaChar CHAR_SEP_TYPE(';');
const LimaChar CHAR_SEP_TYPE_ATTRIBUTES(',');

//***********************************************************************
// syntax for subautomaton definition
//***********************************************************************

const LimaString STRING_DEFINE_SUBAUTOMATON=Common::Misc::utf8stdstring2limastring("define subautomaton ");
const LimaChar CHAR_SUBAUTOMATON_BEGIN('{');
const LimaChar CHAR_SUBAUTOMATON_END('}');
const LimaString STRING_SUBAUTOMATON_PATTERN=Common::Misc::utf8stdstring2limastring("pattern");
const LimaString STRING_SUBAUTOMATON_INDEX=Common::Misc::utf8stdstring2limastring("index ");
const LimaChar CHAR_SUBAUTOMATON_NAMEVALUESEP('=');
// name of subautomaton in the rule
const LimaChar CHAR_BEGIN_NAMESUB('%');

//***********************************************************************
// syntax for the gazeteers definition
//***********************************************************************
const LimaChar CHAR_BEGIN_NAMEGAZ('@');
const LimaChar CHAR_EQUAL_GAZ('=');
const LimaChar CHAR_OPEN_GAZ('(');
const LimaChar CHAR_CLOSE_GAZ(')');
const LimaChar CHAR_WORDSEP_GAZ(',');

const LimaString STRING_USE_GAZ=Common::Misc::utf8stdstring2limastring("use ");
const LimaString STRING_INCLUDE_GAZ=Common::Misc::utf8stdstring2limastring("include ");
const LimaChar CHAR_SEP_LISTFILENAME(',');
// length of previous string (to avoid computing it each time)
const uint64_t LENGTH_USE_GAZ=STRING_USE_GAZ.size();
const uint64_t LENGTH_INCLUDE_GAZ=STRING_INCLUDE_GAZ.size();

//***********************************************************************
// general format of the rule
//***********************************************************************
const LimaChar CHAR_SEP_RULE(':');

const LimaString STRING_NEGATIVE_TYPE_RULE=Common::Misc::utf8stdstring2limastring("NOT_");
const uint64_t LENGTH_NEGATIVE_TYPE_RULE=STRING_NEGATIVE_TYPE_RULE.size();
const LimaString STRING_ABSOLUTE_TYPE_RULE=Common::Misc::utf8stdstring2limastring("ABS_");
const uint64_t LENGTH_ABSOLUTE_TYPE_RULE=STRING_ABSOLUTE_TYPE_RULE.size();

//**********************************************************************
// significant characters and strings in the description of automata
//**********************************************************************
const LimaChar CHAR_OPTIONAL_RE('?');
const LimaChar CHAR_GROUP_OPEN_RE('(');
const LimaChar CHAR_GROUP_CLOSE_RE(')');
const LimaChar CHAR_CHOICESEP_RE('|');
const LimaChar CHAR_CARDINALITY_OPEN_RE('{');
const LimaChar CHAR_CARDINALITY_CLOSE_RE('}');
const LimaChar CHAR_CARDINALITY_UNTIL_RE('-');
const LimaChar CHAR_NOKEEP_CLOSE_RE(']');
const LimaChar CHAR_NOKEEP_OPEN_RE('[');
const LimaChar CHAR_NEGATIVE_RE('^');
const LimaChar CHAR_SEP_RE(' ');
const LimaChar CHAR_HEAD_RE('&');

// representing infinite loop on same state (Kleene star)
// by a special max value in optionality {0-n} or {0-N}
const LimaString STRING_INFINITY_LC=Common::Misc::utf8stdstring2limastring("n");
const LimaString STRING_INFINITY_UC=Common::Misc::utf8stdstring2limastring("N");
const LimaChar CHAR_INFINITY_LC('n');
const LimaChar CHAR_INFINITY_UC('N');

//**********************************************************************
//format of the string defining a TStatus
//**********************************************************************
#define LENGTH_TSTATUS_ELT 1 // all significant substrings are of length 1
// status         = [ANUPWS]
// status alpha   = A[cs1am][con]
// status numeric = N[icdfo]
const LimaString TSTATUS_ALPHA=Common::Misc::utf8stdstring2limastring("A");
const LimaString TSTATUS_NUMERIC=Common::Misc::utf8stdstring2limastring("N");
const LimaString TSTATUS_ALPHANUM=Common::Misc::utf8stdstring2limastring("U");
const LimaString TSTATUS_PATTERN=Common::Misc::utf8stdstring2limastring("P");
const LimaString TSTATUS_WRD_BRK=Common::Misc::utf8stdstring2limastring("W");
const LimaString TSTATUS_SENTENCE_BRK=Common::Misc::utf8stdstring2limastring("S");
const LimaString TSTATUS_ALPHACAP_CAPITAL=Common::Misc::utf8stdstring2limastring("c");
const LimaString TSTATUS_ALPHACAP_SMALL=Common::Misc::utf8stdstring2limastring("s");
const LimaString TSTATUS_ALPHACAP_CAPITAL_1ST=Common::Misc::utf8stdstring2limastring("1");
const LimaString TSTATUS_ALPHACAP_ACRONYM=Common::Misc::utf8stdstring2limastring("a");
const LimaString TSTATUS_ALPHACAP_CAPITAL_SMALL=Common::Misc::utf8stdstring2limastring("m");
const LimaString TSTATUS_ALPHAROMAN_CARDINAL=Common::Misc::utf8stdstring2limastring("c");
const LimaString TSTATUS_ALPHAROMAN_ORDINAL=Common::Misc::utf8stdstring2limastring("o");
const LimaString TSTATUS_ALPHAROMAN_NOT=Common::Misc::utf8stdstring2limastring("n");
const LimaString TSTATUS_NUMERIC_INTEGER=Common::Misc::utf8stdstring2limastring("i");
const LimaString TSTATUS_NUMERIC_COMMA_NUMBER=Common::Misc::utf8stdstring2limastring("c");
const LimaString TSTATUS_NUMERIC_DOT_NUMBER=Common::Misc::utf8stdstring2limastring("d");
const LimaString TSTATUS_NUMERIC_FRACTION=Common::Misc::utf8stdstring2limastring("f");
const LimaString TSTATUS_NUMERIC_ORDINAL_INTEGER=Common::Misc::utf8stdstring2limastring("o");
const LimaString TSTATUS_ISHYPHEN=Common::Misc::utf8stdstring2limastring("h");
const LimaString TSTATUS_ISPOSSESSIVE=Common::Misc::utf8stdstring2limastring("p");

//**********************************************************************
// format of the transitions 
//**********************************************************************
const LimaChar CHAR_NOKEEP_OPEN_TR('[');
const LimaChar CHAR_NOKEEP_CLOSE_TR(']');
//#define STRING_NUM_NAME_TR Common::Misc::utf8stdstring2limastring("$NUM")
const LimaString STRING_NUM_NAME_TR=Common::Misc::utf8stdstring2limastring("T_Ni"); // for compatibility with TSTATUS notation
const LimaString STRING_NUM_NAME_TR2=Common::Misc::utf8stdstring2limastring("t_integer"); // for compatibility with TSTATUS notation
// const uint64_t LENGTH_NUM_NAME_TR=STRING_NUM_NAME_TR.size(); //to avoid re-computing it
const LimaChar CHAR_NUM_EQUAL_TR('=');
const LimaChar CHAR_NUM_GE_TR('>');
const LimaChar CHAR_NUM_LE_TR('<');
const LimaChar CHAR_POS_TR('$');
const LimaChar CHAR_SEP_MACROMICRO('-');
const LimaChar CHAR_SEP_MACROMICRO_STRING('_');
const LimaString STRING_ANY_TR=Common::Misc::utf8stdstring2limastring("*");
const LimaChar CHAR_NEGATIVE_TR('^');
const LimaChar CHAR_HEAD_TR('&');
const LimaString STRING_TSTATUS_TR=Common::Misc::utf8stdstring2limastring("T_");
const LimaString STRING_TSTATUS_TR_small=Common::Misc::utf8stdstring2limastring("t_");
const uint64_t LENGTH_TSTATUS_TR=STRING_TSTATUS_TR.size();

const LimaChar CHAR_BEGIN_ENTITY('<');
const LimaChar CHAR_END_ENTITY('>');

//***********************************************************************
// format of the agreement constraints
//***********************************************************************
const LimaChar CHAR_BEGIN_CONSTRAINT('+');
const LimaChar CHAR_BEGIN_ACTION('=');
const LimaChar CHAR_BEGIN_ACTION_IF_SUCCESS('>');
const LimaChar CHAR_BEGIN_ACTION_IF_FAILURE('<');
const LimaChar CHAR_NEGATIVE_CONSTRAINT('!');
const LimaChar CHAR_CONSTRAINT_BEGIN_ARG('(');
const LimaChar CHAR_CONSTRAINT_END_ARG(')');
const LimaChar CHAR_CONSTRAINT_SEP_ARG(',');
const LimaChar CHAR_CONSTRAINT_COMPLEMENT_ARG('"');
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_LEFT, (Common::Misc::utf8stdstring2limastring("left")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_RIGHT, (Common::Misc::utf8stdstring2limastring("right")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_TRIGGER, (Common::Misc::utf8stdstring2limastring("trigger")));

const LimaChar CHAR_CONSTRAINT_INDEX('.');
// to define sub indices in groups
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_FIRST, (Common::Misc::utf8stdstring2limastring("first")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_CURRENT, (Common::Misc::utf8stdstring2limastring("current")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_NEXT, (Common::Misc::utf8stdstring2limastring("next")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_LAST, (Common::Misc::utf8stdstring2limastring("last")));

// some lengths (to avoid recomputing them)
const int LENGTH_CONSTRAINT_LEFT=STRING_CONSTRAINT_LEFT->size();
const int LENGTH_CONSTRAINT_RIGHT=STRING_CONSTRAINT_RIGHT->size();
const int LENGTH_CONSTRAINT_TRIGGER=STRING_CONSTRAINT_TRIGGER->size();

} // end namespace
} // end namespace
} // end namespace

#endif
