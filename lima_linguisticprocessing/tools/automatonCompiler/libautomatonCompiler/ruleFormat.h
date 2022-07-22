// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

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
const LimaChar CHAR_SEP_LIST(',');

//***********************************************************************
// syntax for the type definition
//***********************************************************************
// possible types are defined in the rules file by lines like
// define type=LOCATION;ENAMEX TYPE="LOCATION";/ENAMEX;
// define type=PERSON;ENAMEX TYPE="PERSON";/ENAMEX;prenom,nom

const LimaChar CHAR_SEP_TYPE(';');
const LimaChar CHAR_SEP_TYPE_ATTRIBUTES(',');

//***********************************************************************
// syntax for subautomaton definition
//***********************************************************************

const LimaChar CHAR_SUBAUTOMATON_BEGIN('{');
const LimaChar CHAR_SUBAUTOMATON_END('}');
const LimaChar CHAR_SUBAUTOMATON_NAMEVALUESEP('=');
// name of subautomaton in the rule
const LimaChar CHAR_BEGIN_NAMESUB('%');

//***********************************************************************
// syntax for the gazeteers definition
//***********************************************************************
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_USE_GAZ, (QLatin1String("use ")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_INCLUDE_GAZ, (QLatin1String("include ")));
const LimaChar CHAR_BEGIN_NAMEGAZ('@');
const LimaChar CHAR_EQUAL_GAZ('=');
const LimaChar CHAR_OPEN_GAZ('(');
const LimaChar CHAR_CLOSE_GAZ(')');
const LimaChar CHAR_WORDSEP_GAZ(',');

const LimaChar CHAR_SEP_LISTFILENAME(',');
// length of previous string (to avoid computing it each time)
const uint64_t LENGTH_USE_GAZ=STRING_USE_GAZ->size();
const uint64_t LENGTH_INCLUDE_GAZ=STRING_INCLUDE_GAZ->size();

//***********************************************************************
// general format of the rule
//***********************************************************************
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_NEGATIVE_TYPE_RULE, (QLatin1String("NOT_")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_ABSOLUTE_TYPE_RULE, (QLatin1String("ABS_")));

const LimaChar CHAR_SEP_RULE(':');

const uint64_t LENGTH_NEGATIVE_TYPE_RULE=STRING_NEGATIVE_TYPE_RULE->size();
const uint64_t LENGTH_ABSOLUTE_TYPE_RULE=STRING_ABSOLUTE_TYPE_RULE->size();

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
const LimaChar CHAR_INFINITY_LC('n');
const LimaChar CHAR_INFINITY_UC('N');

//**********************************************************************
//format of the string defining a TStatus
//**********************************************************************
#define LENGTH_TSTATUS_ELT 1 // all significant substrings are of length 1
// status         = [ANUPWS]
// status alpha   = A[cs1am][con]
// status numeric = N[icdfo]

//**********************************************************************
// format of the transitions 
//**********************************************************************
const LimaChar CHAR_NOKEEP_OPEN_TR('[');
const LimaChar CHAR_NOKEEP_CLOSE_TR(']');
//#define STRING_NUM_NAME_TR QLatin1String("$NUM") notation
// const uint64_t LENGTH_NUM_NAME_TR=STRING_NUM_NAME_TR.size(); //to avoid re-computing it
const LimaChar CHAR_NUM_EQUAL_TR('=');
const LimaChar CHAR_NUM_GE_TR('>');
const LimaChar CHAR_NUM_LE_TR('<');
const LimaChar CHAR_POS_TR('$');
const LimaChar CHAR_SEP_MACROMICRO('-');
const LimaChar CHAR_SEP_MACROMICRO_STRING('_');
const LimaChar CHAR_NEGATIVE_TR('^');
const LimaChar CHAR_HEAD_TR('&');
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_TSTATUS_TR, (QLatin1String("T_")));
const uint64_t LENGTH_TSTATUS_TR=STRING_TSTATUS_TR->size();

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

const LimaChar CHAR_CONSTRAINT_INDEX('.');

Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_LEFT, (QLatin1String("left")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_RIGHT, (QLatin1String("right")));
Q_GLOBAL_STATIC_WITH_ARGS(LimaString, STRING_CONSTRAINT_TRIGGER, (QLatin1String("trigger")));

// some lengths (to avoid recomputing them)
const int LENGTH_CONSTRAINT_LEFT=STRING_CONSTRAINT_LEFT->size();
const int LENGTH_CONSTRAINT_RIGHT=STRING_CONSTRAINT_RIGHT->size();
const int LENGTH_CONSTRAINT_TRIGGER=STRING_CONSTRAINT_TRIGGER->size();


} // end namespace
} // end namespace
} // end namespace

#endif
