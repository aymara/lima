// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/**
 * @file       xmlConfigurationFileExceptions.h
 * @brief      launch exception related to the configuration file parsing
 * @date       begin Mon Oct, 13 2003 (lun oct 21 2002)
 * @author     Gael de Chalendar <Gael.de-Chalendar@cea.fr>

 *             copyright (C) 2002-2003 by CEA

 */

#ifndef XMLCONFIGURATIONFILEEXCEPTIONS_H
#define XMLCONFIGURATIONFILEEXCEPTIONS_H

#include "common/LimaCommon.h"
#include <string>
#include <iostream>

namespace Lima {
  namespace Common {
    namespace XMLConfigurationFiles
    {

      //! @brief exception related to the configuration file parsing
      class LIMA_XMLCONFIGURATIONFILES_EXPORT XMLConfigurationFileException : public LimaException
      {
      public:
        //! @brief defaut constructor

        //! @brief constructor with the error message
        explicit XMLConfigurationFileException ( const std::string &mess = "") : LimaException ( mess ) {}

        XMLConfigurationFileException& operator=(const XMLConfigurationFileException&) = delete;
        XMLConfigurationFileException(const XMLConfigurationFileException&) = default;
      };

      //! @brief return a message when a 'module' is not found
      class LIMA_XMLCONFIGURATIONFILES_EXPORT NoSuchModule : public XMLConfigurationFileException
      {
      public:
        //! @brief constructor (send a message was default)
        //! @param name the <i>module</i> that was not found
        explicit NoSuchModule ( const std::string &name ) : XMLConfigurationFileException ( "No such module " + name ), moduleName ( name ) {}
        //! @brief destructor (throw the exception)
        NoSuchModule& operator=(const NoSuchModule&) = delete;
        NoSuchModule(const NoSuchModule&) = default;
        virtual ~NoSuchModule() = default;
      private:
        //! @brief the name of the <i>module</i> that was not found
        const std::string moduleName;
      };

      //! @brief return a message when a 'group' was not found
      class LIMA_XMLCONFIGURATIONFILES_EXPORT NoSuchGroup : public XMLConfigurationFileException
      {
      public:
        //! @brief constructor (send a message was default)
        //! @param name the <i>group</i> that was not found
        NoSuchGroup ( const std::string &name ) : XMLConfigurationFileException ( "No such group " + name ),groupName ( name ) {/*std::cout << "No such group " << name << std::endl;*/}
        virtual ~NoSuchGroup() throw() = default;
        NoSuchGroup(const NoSuchGroup&) = default;
        NoSuchGroup& operator=(const NoSuchGroup&) = delete;
      private:
        //! @brief the name of the <i>group</i> that was not found
        const std::string groupName;
      };

      //! @brief return a message when an 'attribute' was not found
      class LIMA_XMLCONFIGURATIONFILES_EXPORT NoSuchAttribute : public XMLConfigurationFileException
      {
      public:
        //! @brief constructor (send a message was default)
        //! @param name the <i>attribute</i> that was not found
        NoSuchAttribute ( const std::string &name ) : XMLConfigurationFileException ( "No such attribute " + name ),attName ( name ) {/*std::cout << "No such attribute " << name << std::endl;*/}
        NoSuchAttribute(const NoSuchAttribute&) = default;
        NoSuchAttribute& operator=(const NoSuchAttribute&) = delete;
        virtual ~NoSuchAttribute() throw() {}
      private:
        //! @brief the name of the <i>attribute</i> that was not found
        const std::string attName;
      };

      //! @brief return a message when a 'param' was not found
      class LIMA_XMLCONFIGURATIONFILES_EXPORT NoSuchParam : public XMLConfigurationFileException
      {
      public:
        //! @brief constructor (send a message was default)
        //! @param name the <i>param</i> that was not found
        NoSuchParam ( const std::string &name ) : XMLConfigurationFileException ( "No such param '" + name + "'" ),paramName ( name ) {/*std::cout << "No such param " << name << std::endl;*/}
        NoSuchParam(const NoSuchParam&) = default;
        NoSuchParam& operator=(const NoSuchParam&) = delete;
        virtual ~NoSuchParam() throw() {}
      private:
        //! @brief the name of the <i>param</i> that was not found
        const std::string paramName;
      };

      //! @brief return a message when a 'list' was not found
      class LIMA_XMLCONFIGURATIONFILES_EXPORT NoSuchList : public XMLConfigurationFileException
      {
      public:
        //! @brief constructor (send a message was default)
        //! @param name the <i>list</i> that was not found
        NoSuchList ( const std::string &name ) : XMLConfigurationFileException ( "No such list " + name ),listName ( name ) {/*std::cout << "No such list " << name << std::endl;*/}
        NoSuchList(const NoSuchList&) = default;
        NoSuchList& operator=(const NoSuchList&) = delete;
        virtual ~NoSuchList() throw() {}
      private:
        //! @brief the name of the <i>list</i> that was not found
        const std::string listName;
      };

      //! @brief return a message when a 'map' was not found
      class LIMA_XMLCONFIGURATIONFILES_EXPORT NoSuchMap : public XMLConfigurationFileException
      {
      public:
        //! @brief constructor (send a message was default)
        //! @param name the <i>map</i> that was not found
        NoSuchMap ( const std::string &name ) : XMLConfigurationFileException ( "No such map " + name ),mapName ( name ) {/*std::cout << "No such map " << name << std::endl;*/}
        NoSuchMap(const NoSuchMap&) = default;
        NoSuchMap& operator=(const NoSuchMap&) = delete;
        virtual ~NoSuchMap() throw() {}
      private:
        //! @brief the name of the <i>map</i> that was not found
        const std::string mapName;
      };


    } // closing namespace Lima
  } // closing namespace Common
} // closing namespace XMLConfigurationFiles

#endif
