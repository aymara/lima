/************************************************************************
 *
 * @file       documentReader.h
 * @author     Besancon Romaric (besanconr@zoe.cea.fr)
 * @date       Mon Oct 27 2003
 * @version    $Id$
 * copyright   Copyright (C) 2003 by CEA LIST (LVIC)
 * Project     detectlibraries
 ***********************************************************************/

#ifndef DOCUMENTREADER_H
#define DOCUMENTREADER_H

#include "documentsreader_export.h"

#include <string>
#include <cstdint>

namespace Lima {
  namespace Common {
    namespace XMLConfigurationFiles {
      class ModuleConfigurationStructure;
    }
  }
}

namespace Lima {
  class StructuredXmlDocumentHandler;
namespace DocumentsReader {

class DocumentReaderPrivate;
//! @brief read a XML document
class DOCUMENTSREADER_EXPORT DocumentReader
{
    public:
        //DocumentReader(const std::string &configurationFileName);
        //DocumentReader(Common::XMLConfigurationFiles::XMLConfigurationFileParser&);
        /**
         * @brief 
         */
        DocumentReader ( Lima::Common::XMLConfigurationFiles::ModuleConfigurationStructure& conf );

        virtual ~DocumentReader();

        void setLinguisticXMLDocHandler ( StructuredXmlDocumentHandler* handler );

        /**
         * @brief initialize the reader with a file containing documents in XML format
         *
         * @param filename name of the file
         *
         * @return true if initalization succeeded
         */
        bool initFile ( const std::string& filename );

        bool initWithString ( const std::string& text );

        /**
         * @brief read the next element in the current file (initFile must have been
         * @brief called beforehand, to initialize current file)
         *
         * @return true if parsing was successful and false otherwise
         */
        bool readXMLDocument();

        /**
         * @brief Returns the reader's current character offset, starting with 0.
         */
        uint64_t getCurrentOffsetFromXmlReader();

    private:
      DocumentReader ( const DocumentReader& );
      DocumentReader& operator = ( const DocumentReader& );
      
      DocumentReaderPrivate* m_d;
};

} // end namespace
} // end namespace

#endif
