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
  * @file               textFormater.cpp
  * @date               Created on Tue Dec, 17 2002
  * @author             Gael de Chalendar <Gael.de-Chalendar@cea.fr> 

  *                     Copyright (C) 2002 by CEA
  * @version            $Id$
  */

#include "common/LimaCommon.h"
#include "common/Data/strwstrtools.h"

#include "textFormater.h"

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <limits.h>
#include <stdlib.h>

#define LILOGINIT LOGINIT("MediaIdent");

namespace Lima {
namespace Common {
namespace Misc {

TextFormater::TextFormater()
{
}

TextFormater::~TextFormater()
{
}

/** This method replaces all the tags in the UTF-8 encoded input stream by spaces or underscores in the output stream.
Underscores are written when the tag is adjacent to a word (directly or by another) tag. Spaces are written otherwise.
If an unclosed tag finishes the input stream, the result is written but an error message is written in <I>status</I> and a non zero-value is returned.
If an opening '<' is found inside a tag, it is ignored (replaced) but an error message is written in <I>status</I> and a non zero-value will be returned.
If a closing '>' is found outside a tag, it is ignored (replaced) but an error message is written in <I>status</I> and a non zero-value will be returned.
This method does not modify its receiver and thus is constant.
Throws a runtime_error in case of an unknown internal state
It does not record the removed tags. Should be done in another method;

@param std::ostream& status the stream where errors and warning will be written
@param std::istream& input the input stream. Should contain a valid SGML file
@param std::ostream& output the output stream. The result stream where un-tagged results are written
@return int 0 if there is no error nor warning ; non-zero otherwise.
 */
int TextFormater::untaggingWithSpaces(std::ostream& status, std::istream& input,
        std::ostream& output, bool wide, 
        const std::string& endSentenceTag) const
{
  LIMA_UNUSED(wide);
    LILOGINIT;
    setlocale(LC_CTYPE,"fr_FR.UTF-8");
    
    size_t nb = 0; // the number of chars in the current tag when we cannot know
                            // if it is bond to the following text (tag after a white space)
    size_t nbNewLines = 0; // the number of newlines in the current tag when we cannot know
                            // if it is bond to the following text (tag after a white space)
    size_t position=0; // position in the input stream;
    enum RetVal {SUCCESS, INVALID_OPENING_TAG_CHAR, INVALID_CLOSING_TAG_CHAR,
                    DUPLICATED_OPENING_TAG_CHAR, DUPLICATED_CLOSING_TAG_CHAR,
                    UNCLOSED_OPENING_TAG_CHAR};
    RetVal retVal = SUCCESS;
    
    enum Etat {TEXT, DEBCOL, FINCOL, BLANC, DEBBLANC, FINBLANC, BEGENTITY, ENTITY};
    Etat etat = BLANC;

    char carLu;
    char carLu2; 
    std::string s;
    
    std::ostringstream txt;
    std::ostringstream tag;
    std::ostringstream tagValue;
    std::ostringstream entity;
    
    while (input.good())
    {
        input.get(carLu);
        LDEBUG << carLu;
        if (input.eof()) continue;
        switch (etat)
        {
            case TEXT:
                switch (carLu)
                {
                    case '<':
                        output << txt.str();
                        txt.str("");
                        tag << '_';
                        tag << ' ';
                        tagValue << carLu;
                        LDEBUG << "TEXT-> DEBCOL" << LENDL;
                        etat = DEBCOL;
                    break;
                    case '>':
                        txt << carLu;
                        status << "Invalid '>' character at " << position << std::endl;
                        retVal = INVALID_CLOSING_TAG_CHAR;
                    break;
                    case '&':
                        output << txt.str();
                        txt.str("");
                        entity << carLu;
                        LDEBUG << "TEXT -> BEGENTITY;" << LENDL;
                        etat = BEGENTITY;
                    break;
                    case ' ':case '\t':case '\n':
                        output << txt.str();;
                        output << carLu;
                        txt.str("");
                        LDEBUG << "TEXT-> BLANC" << LENDL;
                        etat = BLANC;
                    break;
                    default:
                        txt << carLu;
                    break;
                }
            break;
            case BEGENTITY:
                switch (carLu)
                {
                    case 'A':;case 'B':;case 'C':;case 'D':;case 'E':;case 'F':;case 'G':;
                    case 'H':;case 'I':;case 'J':;case 'K':;case 'L':;case 'M':;case 'N':;
                    case 'O':;case 'P':;case 'Q':;case 'R':;case 'S':;case 'T':;case 'U':;
                    case 'V':;case 'W':;case 'X':;case 'Y':;case 'Z':;
                    case 'a':;case 'b':;case 'c':;case 'd':;case 'e':;case 'f':;case 'g':;
                    case 'h':;case 'i':;case 'j':;case 'k':;case 'l':;case 'm':;case 'n':;
                    case 'o':;case 'p':;case 'q':;case 'r':;case 's':;case 't':;case 'u':;
                    case 'v':;case 'w':;case 'x':;case 'y':;case 'z':
                        entity << carLu;
                        LDEBUG << "BEGENTITY-> ENTITY" << LENDL;
                        etat = ENTITY;
                    break;
                    case '<':
                        output.put('_');
                        output.put(' ');
                        entity.str("");
                        tag << '_';
                        tag << ' ';
                        LDEBUG << "BEGENTITY-> DEBCOL" << LENDL;
                        etat = DEBCOL;
                    break;
                    case ' ':case '\t':case '\n':
//                        output.put('_');
                        output.put(' ');
                        output << carLu;
                        entity.str("");
                        LDEBUG << "BEGENTITY-> BLANC" << LENDL;
                        etat = BLANC;
                    break;
                    default:
//                        output.put('_');
                        output.put(' ');
                        output << carLu;
                        entity.str("");
                        LDEBUG << "BEGENTITY-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                }
            break;
            case ENTITY:
                switch (carLu)
                {
                    case 'A':;case 'B':;case 'C':;case 'D':;case 'E':;case 'F':;case 'G':;
                    case 'H':;case 'I':;case 'J':;case 'K':;case 'L':;case 'M':;case 'N':;
                    case 'O':;case 'P':;case 'Q':;case 'R':;case 'S':;case 'T':;case 'U':;
                    case 'V':;case 'W':;case 'X':;case 'Y':;case 'Z':;
                    case 'a':;case 'b':;case 'c':;case 'd':;case 'e':;case 'f':;case 'g':;
                    case 'h':;case 'i':;case 'j':;case 'k':;case 'l':;case 'm':;case 'n':;
                    case 'o':;case 'p':;case 'q':;case 'r':;case 's':;case 't':;case 'u':;
                    case 'v':;case 'w':;case 'x':;case 'y':;case 'z':
                        entity << carLu;
                    break;
                    case '<':
                        for (uint64_t i = 0; i < entity.str().size(); i++)
                        {
//                            output << '_';
                            output << ' ';
                        }
                        entity.str("");
//                        tag << '_';
                        tag << ' ';
                        LDEBUG << "ENTITY-> DEBCOL" << LENDL;
                        etat = DEBCOL;
                    break;
                    case ' ':case '\t':case '\n':
                        for (uint64_t i = 0; i < entity.str().size(); i++)
                        {
//                            output << '_'; 
                            output << ' '; 
                        }
                        output << carLu;
                        entity.str("");
                        LDEBUG << "ENTITY-> BLANC" << LENDL;
                        etat = BLANC;
                    break;
                    case ';':
                        for (uint64_t i = 0; i < entity.str().size()+1; i++)
                        {
//                            output << '_'; 
                            output << ' '; 
                        }
                        entity.str("");
                        LDEBUG << "ENTITY-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                    default:
                        for (uint64_t i = 0; i < entity.str().size(); i++)
                        {
//                            output << '_'; 
                            output << ' '; 
                        }
                        output << carLu;
                        entity.str("");
                        LDEBUG << "ENTITY-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                }
            break;
            case DEBCOL:
                tagValue << carLu;
                switch (carLu)
                {
                    case '<':
//                        tag << '_';
                        tag << ' ';
                        status << "Invalid '<' character at " << position << std::endl;
                        retVal = DUPLICATED_OPENING_TAG_CHAR;
                    break;
                    case '>':
//                        tag << '_';
                        tag << ' ';
                        LDEBUG << "DEBCOL-> FINCOL" << LENDL;
                        etat = FINCOL;
                    break;
                    case ' ':case '\t':case '\n':
                        tag << carLu;
                    break;
                    default:
                        LDEBUG << "Looking at " << carLu << LENDL;
                        char buf[MB_LEN_MAX];
                        buf[0] = carLu;
                        input.rdbuf()-> sgetn(buf+1, 9);
                        wchar_t mbc;
                        int transRes = mbtowc(&mbc, buf, MB_LEN_MAX);
                        LDEBUG << "transres value is " << transRes << LENDL;
                        if (transRes > 1)
                        {
                            LDEBUG << "Got a multibyte char inside tag: " << mbc << LENDL;
                            for (int i = 1; i < transRes; i++)
                            {
                                input.get(carLu);
                            }
                        }
//                        tag << '_';
                        tag << ' ';
                    break;
                }
            break;
            case FINCOL:
                switch (carLu)
                {
                    case '<':
//                        tag << '_';
                        tag << ' ';
                        tagValue << carLu;
                        LDEBUG << "FINCOL-> DEBCOL" << LENDL;
                        etat = DEBCOL;
                    break;
                    case '>':
//                        tag << '_';
                        tag << ' ';
                        tagValue << carLu;
                        status << "Invalid '>' character at " << position << std::endl;
                        retVal = DUPLICATED_CLOSING_TAG_CHAR;
                    break;
                    case ' ':case '\t':case '\n':
                        putTag(status, output, wide, endSentenceTag, tag, tagValue);
                        output << carLu;
                        LDEBUG << "FINCOL-> BLANC" << LENDL;
                        etat = BLANC;
                    break;
                    case '&':
                        putTag(status, output, wide, endSentenceTag, tag, tagValue);
                        entity << carLu;
                        LDEBUG << "FINCOL -> BEGENTITY;" << LENDL;
                        etat = BEGENTITY;
                    break;
                    default:
                        putTag(status, output, wide, endSentenceTag, tag, tagValue);
                        txt << carLu;
                        tagValue << carLu;
                        LDEBUG << "FINCOL-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                }
            break;
            case BLANC:
                switch (carLu)
                {
                    case '&':
                        entity << carLu;
                        LDEBUG << "BLANC -> BEGENTITY;" << LENDL;
                        etat = BEGENTITY;
                    break;
                    case '<':
                        nb = 1;
                        nbNewLines = 0;
                        tagValue << carLu;
                        LDEBUG << "BLANC-> DEBBLANC" << LENDL;
                        etat = DEBBLANC;
                    break;
                    case '>':
                        output << '>';
                        status << "Invalid '>' character at " << position << std::endl;
                        retVal = INVALID_CLOSING_TAG_CHAR;
                        LDEBUG << "BLANC-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                    case ' ':case '\t':case '\n':
                        output << carLu;
                    break;
                    default:
                        txt << carLu;
                        LDEBUG << "BLANC-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                }
            break;
            case DEBBLANC:
                tagValue << carLu;
                switch (carLu)
                {
                    case '<':
                        nb++;
                        status << "Duplicated '<' character at " << position << std::endl;
                        retVal = DUPLICATED_OPENING_TAG_CHAR;
                    break;
                    case '>':
                        nb++;
                        LDEBUG << "DEBBLANC-> FINBLANC" << LENDL;
                        etat = FINBLANC;
                    break;
                    case '\n':
                        nbNewLines++;
                    break;
                    default:
                        LDEBUG << "Looking at " << carLu << LENDL;
                        char buf[MB_LEN_MAX];
                        buf[0] = carLu;
                        std::streamsize got = input.rdbuf()-> sgetn(buf+1, 9);
                        for (std::streamsize i = 0; i < got ; i++)
                            input.rdbuf()-> sungetc();
                        for (uint64_t i = 0; i<MB_LEN_MAX; i++)
                            LDEBUG << buf[i];
                        LDEBUG << LENDL;
                        wchar_t mbc;
                        int transRes = mbtowc(&mbc, buf, MB_LEN_MAX);
                        LDEBUG << "transres  is " << transRes << LENDL;
                        if (transRes > 1)
                        {
                            LDEBUG << "Got a multibyte char inside tag: " << mbc << LENDL;
                            for (int i = 1; i < transRes; i++)
                            {
                                input.get(carLu);
                            }
                        }
                        nb++;
                    break;
                }
            break;
            case FINBLANC:
                switch (carLu)
                {
                    case '<':
                        nb++;
                        tagValue << carLu;
                        LDEBUG << "FINBLANC-> DEBBLANC" << LENDL;
                        etat = DEBBLANC;
                    break;
                    case '>':
                        status << "Duplicated '>' character at " << position << std::endl;
                        retVal = DUPLICATED_CLOSING_TAG_CHAR;
                        putWhites(status, output, wide,endSentenceTag, tagValue,
                                output, carLu, ' ', nb, nbNewLines);
                        LDEBUG << "FINBLANC-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                    case ' ':case '\t':case '\n':
                        putWhites(status, output, wide,endSentenceTag, tagValue,
                                output, carLu, ' ', nb, nbNewLines);
                        LDEBUG << "FINBLANC-> BLANC" << LENDL;
                        etat = BLANC;
                    break;
                    case '&':
//                        putWhites(status, output, wide,endSentenceTag, tagValue,
//                                entity, carLu, '_', nb, nbNewLines);
                        putWhites(status, output, wide,endSentenceTag, tagValue,
                                entity, carLu, ' ', nb, nbNewLines);
                        LDEBUG << "FINBLANC -> BEGENTITY;" << LENDL;
                        etat = BEGENTITY;
                    break;
                    default:
//                        putWhites(status, output, wide,endSentenceTag, tagValue,
//                                txt, carLu, '_', nb, nbNewLines);
                        putWhites(status, output, wide,endSentenceTag, tagValue,
                                txt, carLu, ' ', nb, nbNewLines);
                        LDEBUG << "FINBLANC-> TEXT" << LENDL;
                        etat = TEXT;
                    break;
                }
            break;
            default:
                throw std::runtime_error((std::string("unknown state %d.\n", int(etat))).c_str());
        }
        ++position;
    }
    if ( (etat == DEBCOL) || (etat == DEBBLANC) )
    {
        status << "Unclosed tag at EOF (" << position << ")" << std::endl;
        retVal = UNCLOSED_OPENING_TAG_CHAR;
        if (etat == DEBCOL) 
        {
          output << tag.str();
        }
        else
        {
//                s = std::string(nb/2, '_');
            s = std::string(nb/2, ' ');
            s.append(nbNewLines, '\n');
//                if (nb%2 == 0) s.append(nb/2, '_');
            if (nb%2 == 0) s.append(nb/2, ' ');
//                else s.append(nb/2 + 1, '_');
            else s.append(nb/2 + 1, ' ');
            output << s;
            nb = 0;
            nbNewLines = 0;
        }
    }

    else if ((etat == FINCOL) || (etat == FINBLANC))
    {
      output << tag.str();
    }

    else if (etat == TEXT)
    {
      output << txt.str();
    }
    else {} // BLANC nothing to do

    return int(retVal);
}

void TextFormater::putTag(std::ostream& status, std::ostream& output, bool wide,
    const std::string& endSentenceTag, std::ostringstream& tag, std::ostringstream& tagValue) const
{
  LIMA_UNUSED(status);
  LIMA_UNUSED(wide);
    LILOGINIT;    
    std::string stag = tag.str();
    if ( (endSentenceTag != "")
            && (tagValue.str().find(endSentenceTag,0) != std::string::npos)
            && (stag.length() > 0) )
        stag[0] = '.';
    output << stag;
    tag.str("");
    tagValue.str("");
    
}

void TextFormater::putWhites(std::ostream& status, std::ostream& output, bool wide,
    const std::string& endSentenceTag, std::ostringstream& tagValue, 
    std::ostream& carLuTarget, char carLu, char fillChar,
    uint64_t nb, uint64_t nbNewLines) const
{
  LIMA_UNUSED(status);
  LIMA_UNUSED(wide);
    std::string s;
    
    s = std::string(nb/2, fillChar);
    s.append(nbNewLines, '\n');
    if (nb%2 == 0) s.append(nb/2, fillChar);
    else s.append(nb/2 + 1, fillChar);
    if ( (endSentenceTag != "")
            && (tagValue.str().find(endSentenceTag,0) != std::string::npos)
            && (s.length() > 0) )
        s[0] = '.';
    output << s;
    nb = 0;
    nbNewLines = 0;
    carLuTarget << carLu;
}

} // closing namespace Misc
} // closing Common
} // closing namespace Lima
