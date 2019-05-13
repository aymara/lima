/***************************************************************************
                       tokensstream.h  -  description
                             -------------------
    begin                : lun mar 10 2003
    last revised         : mar 10, 2003
    author               : Gael de Chalendar <de-chalendarg@zoe.cea.fr>
    copyright            : Copyright (C) 2003 by CEA LIST (LVIC)
 ***************************************************************************/

#ifndef LINGUISTICPROCESSINGSTOKENSSTREAM_H
#define LINGUISTICPROCESSINGSTOKENSSTREAM_H

#include "documentsreader_export.h"
#include "indextoken.h"

#include <queue>
#include <iostream>

namespace Lima {
namespace DocumentsReader {
/**
 * This is a stream of tokens. It is fed up by the linguistic analysis and emptied when reading a document during indexing
 * Gael de Chalendar
 **/
class DOCUMENTSREADER_EXPORT TokensStream : public std::deque< IndexToken* >
{
public:
    TokensStream();
    TokensStream(const TokensStream&);
    TokensStream& operator = (const TokensStream&);
    virtual ~TokensStream();

    // helper functions for constructors, destructor and copy assignment
    void copy(const TokensStream&);
    void clear();


    void push(const IndexToken&);
    
    /**
     * Tests if a token is available
     */
    bool hasToken() const;
    
    /**
     * Returns the next token. It will be no more available
     */
    IndexToken* nextToken();

  //    friend std::wostream& operator << (std::wostream&, TokensStream&);
  //    friend std::wistream& operator >> (std::wistream&, TokensStream&);

}
;

} // namespace DocumentsReader
} // namespace Lima

#endif // LINGUISTICPROCESSINGSTOKENSSTREAM_H
