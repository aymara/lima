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

namespace Lima {
namespace LinguisticProcessing {
namespace Automaton {

//***********************************************************************
// general formatting of the file
//***********************************************************************
const LimaChar CHAR_COMMENT('#');
const LimaString STRING_DEFINE_ENCODING=QLatin1String("set encoding=");
const LimaString STRING_DEFINE_DEFAULTACTION=QLatin1String("set defaultAction=");
const LimaString STRING_UNSET_DEFAULTACTION=QLatin1String("unset defaultAction=");
const LimaString STRING_DEFINE_ENTITYTYPES=QLatin1String("set entityTypes=");
const LimaString STRING_USING_MODEX=QLatin1String("using modex ");
const LimaString STRING_USING_ENTITYGROUPS=QLatin1String("using groups ");
const LimaString STRING_USING_LIBS=QLatin1String("using libs ");
const LimaChar CHAR_SEP_LIST(',');

//***********************************************************************
// syntax for the type definition
//***********************************************************************
// possible types are defined in the rules file by lines like
// define type=LOCATION;ENAMEX TYPE="LOCATION";/ENAMEX;
// define type=PERSON;ENAMEX TYPE="PERSON";/ENAMEX;prenom,nom

const LimaString STRING_DEFINE_TYPE=QLatin1String("define type=");
const LimaChar CHAR_SEP_TYPE(';');
const LimaChar CHAR_SEP_TYPE_ATTRIBUTES(',');

//***********************************************************************
// syntax for subautomaton definition
//***********************************************************************

const LimaString STRING_DEFINE_SUBAUTOMATON=QLatin1String("define subautomaton ");
const LimaChar CHAR_SUBAUTOMATON_BEGIN('{');
const LimaChar CHAR_SUBAUTOMATON_END('}');
const LimaString STRING_SUBAUTOMATON_PATTERN=QLatin1String("pattern");
const LimaString STRING_SUBAUTOMATON_INDEX=QLatin1String("index ");
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

const LimaString STRING_USE_GAZ=QLatin1String("use ");
const LimaString STRING_INCLUDE_GAZ=QLatin1String("include ");
const LimaChar CHAR_SEP_LISTFILENAME(',');
// length of previous string (to avoid computing it each time)
const uint64_t LENGTH_USE_GAZ=STRING_USE_GAZ.size();
const uint64_t LENGTH_INCLUDE_GAZ=STRING_INCLUDE_GAZ.size();

//***********************************************************************
// general format of the rule
//***********************************************************************
const LimaChar CHAR_SEP_RULE(':');

const LimaString STRING_NEGATIVE_TYPE_RULE=QLatin1String("NOT_");
const uint64_t LENGTH_NEGATIVE_TYPE_RULE=STRING_NEGATIVE_TYPE_RULE.size();
const LimaString STRING_ABSOLUTE_TYPE_RULE=QLatin1String("ABS_");
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
const LimaString STRING_INFINITY_LC=QLatin1String("n");
const LimaString STRING_INFINITY_UC=QLatin1String("N");
const LimaChar CHAR_INFINITY_LC('n');
const LimaChar CHAR_INFINITY_UC('N');

//**********************************************************************
//format of the string defining a TStatus
//**********************************************************************
#define LENGTH_TSTATUS_ELT 1 // all significant substrings are of length 1
// status         = [ANUPWS]
// status alpha   = A[cs1am][con]
// status numeric = N[icdfo]
const LimaString TSTATUS_ALPHA=QLatin1String("A");
const LimaString TSTATUS_NUMERIC=QLatin1String("N");
const LimaString TSTATUS_ALPHANUM=QLatin1String("U");
const LimaString TSTATUS_PATTERN=QLatin1String("P");
const LimaString TSTATUS_WRD_BRK=QLatin1String("W");
const LimaString TSTATUS_SENTENCE_BRK=QLatin1String("S");
const LimaString TSTATUS_ALPHACAP_CAPITAL=QLatin1String("c");
const LimaString TSTATUS_ALPHACAP_SMALL=QLatin1String("s");
const LimaString TSTATUS_ALPHACAP_CAPITAL_1ST=QLatin1String("1");
const LimaString TSTATUS_ALPHACAP_ACRONYM=QLatin1String("a");
const LimaString TSTATUS_ALPHACAP_CAPITAL_SMALL=QLatin1String("m");
const LimaString TSTATUS_ALPHAROMAN_CARDINAL=QLatin1String("c");
const LimaString TSTATUS_ALPHAROMAN_ORDINAL=QLatin1String("o");
const LimaString TSTATUS_ALPHAROMAN_NOT=QLatin1String("n");
const LimaString TSTATUS_NUMERIC_INTEGER=QLatin1String("i");
const LimaString TSTATUS_NUMERIC_COMMA_NUMBER=QLatin1String("c");
const LimaString TSTATUS_NUMERIC_DOT_NUMBER=QLatin1String("d");
const LimaString TSTATUS_NUMERIC_FRACTION=QLatin1String("f");
const LimaString TSTATUS_NUMERIC_ORDINAL_INTEGER=QLatin1String("o");
const LimaString TSTATUS_ISHYPHEN=QLatin1String("h");
const LimaString TSTATUS_ISPOSSESSIVE=QLatin1String("p");

//**********************************************************************
// format of the transitions 
//**********************************************************************
const LimaChar CHAR_NOKEEP_OPEN_TR('[');
const LimaChar CHAR_NOKEEP_CLOSE_TR(']');
//#define STRING_NUM_NAME_TR QLatin1String("$NUM")
const LimaString STRING_NUM_NAME_TR=QLatin1String("T_Ni"); // for compatibility with TSTATUS notation
const LimaString STRING_NUM_NAME_TR2=QLatin1String("t_integer"); // for compatibility with TSTATUS notation
// const uint64_t LENGTH_NUM_NAME_TR=STRING_NUM_NAME_TR.size(); //to avoid re-computing it
const LimaChar CHAR_NUM_EQUAL_TR('=');
const LimaChar CHAR_NUM_GE_TR('>');
const LimaChar CHAR_NUM_LE_TR('<');
const LimaChar CHAR_POS_TR('$');
const LimaChar CHAR_SEP_MACROMICRO('-');
const LimaChar CHAR_SEP_MACROMICRO_STRING('_');
const LimaString STRING_ANY_TR=QLatin1String("*");
const LimaChar CHAR_NEGATIVE_TR('^');
const LimaChar CHAR_HEAD_TR('&');
const LimaString STRING_TSTATUS_TR=QLatin1String("T_");
const LimaString STRING_TSTATUS_TR_small=QLatin1String("t_");
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
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_LEFT, (QLatin1String("left")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_RIGHT, (QLatin1String("right")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_TRIGGER, (QLatin1String("trigger")));

const LimaChar CHAR_CONSTRAINT_INDEX('.');
// to define sub indices in groups
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_FIRST, (QLatin1String("first")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_CURRENT, (QLatin1String("current")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_NEXT, (QLatin1String("next")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_GROUP_LAST, (QLatin1String("last")));

// some lengths (to avoid recomputing them)
const int LENGTH_CONSTRAINT_LEFT=STRING_CONSTRAINT_LEFT->size();
const int LENGTH_CONSTRAINT_RIGHT=STRING_CONSTRAINT_RIGHT->size();
const int LENGTH_CONSTRAINT_TRIGGER=STRING_CONSTRAINT_TRIGGER->size();

} // end namespace
} // end namespace
} // end namespace

#endif
