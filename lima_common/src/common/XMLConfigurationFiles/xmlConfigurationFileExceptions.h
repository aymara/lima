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

        //! should not be used since it does not return the error message
        XMLConfigurationFileException() : LimaException() {}
        
        //! @brief constructor with the error message
        XMLConfigurationFileException ( const std::string &mess ) : LimaException ( mess ) {}
        
      private:
        XMLConfigurationFileException& operator=(const XMLConfigurationFileException&) {return *this;}
      };
      
      //! @brief return a message when a 'module' is not found
      class NoSuchModule : public XMLConfigurationFileException
      {
      public:
        //! @brief constructor (send a message was default)
        //! @param name the <i>module</i> that was not found
        NoSuchModule ( const std::string &name ) : XMLConfigurationFileException ( "No such module " + name ),moduleName ( name ) {/*std::cout << "No such module " << name << std::endl;*/}
        //! @brief destructor (throw the exception)
        virtual ~NoSuchModule() throw() {}
    private:
      NoSuchModule& operator=(const NoSuchModule&) {return *this;}
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
      virtual ~NoSuchGroup() throw() {}
  private:
    NoSuchGroup& operator=(const NoSuchGroup&) {return *this;}
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
    virtual ~NoSuchAttribute() throw() {}
private:
  NoSuchAttribute& operator=(const NoSuchAttribute&) {return *this;}
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
  virtual ~NoSuchParam() throw() {}
private:
  NoSuchParam& operator=(const NoSuchParam&) {return *this;}
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
  virtual ~NoSuchList() throw() {}
private:
  NoSuchList& operator=(const NoSuchList&) {return *this;}
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
  virtual ~NoSuchMap() throw() {}
private:
  NoSuchMap& operator=(const NoSuchMap&) {return *this;}
  //! @brief the name of the <i>map</i> that was not found
  const std::string mapName;
};


} // closing namespace Lima
} // closing namespace Common
} // closing namespace XMLConfigurationFiles

#endif
