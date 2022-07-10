// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 * Copyright (C) 2003 by CEA-LIST
 * author Olivier Mesnard         olivier.mesnard@localhost
 *        Hervé Le Borgne pour l'interfaçage SOCI
 *
 * database of documents.
 * Documents are indexed in inverted files (terms, frequency and pos)
 * and in a relational database (metadata: name, date, source...)
 ***************************************************************************/
#include "GeoDbContext.h"
#include <iostream>
#include <string>

#ifdef WITHMYSQL
#include "soci/soci-mysql.h"
#include "mysql/mysql.h"
#endif

#ifdef WITHSQLITE
#include "soci/soci-sqlite3.h"
#endif

#ifdef WITHPOSTGRESQL
#include "soci/soci-postgresql.h"
#endif

using namespace soci;
using namespace std;

namespace Lima{
namespace LinguisticProcessing{
namespace GeoEntities{
  

//------------------------------------------
// Constructeur:
//    - Connection à la base de donnée
//    - Initialisation
//------------------------------------------
GeoDbContext::GeoDbContext ( string const &backend, string const &connectString )
        
{
    // Ouverture de la session et connection à l'initialisation de la classe
    // l'objet "session" est m_db2

    m_db2=NULL;
    m_connected = false;
    if (backend.find("mysql")==0)
    {
      #ifdef WITHMYSQL
        std::cout << "Connexion avec mysql" << std::endl;
        m_db2=new session(mysql,connectString);
        statement stmt(*m_db2);
        stmt.prepare("SET NAMES utf8");
        stmt.execute();
      #else
        m_connected = false;
      #endif
    }
    if (backend.find("sqlite3")==0 || backend.find("sqlite")==0 || backend.find("SQLite3")==0)
    {
      #ifdef WITHSQLITE
        std::cout << "Connexion avec sqlite3" << std::endl;
        m_db2=new session(sqlite3,connectString);   
        statement stmt(*m_db2);
        stmt.prepare("PRAGMA synchronous=OFF");
        stmt.execute();    
      #else
        m_connected = false;
      #endif
    }
    if (backend.find("PostgreSQL")==0 || backend.find("postgresql")==0)
    {
      #ifdef WITHPOSTGRESQL
       m_db2=new session(postgresql,connectString);
    #else
      m_connected = false;
    #endif
    }
    
    if (m_db2 !=NULL) m_connected = true;
}

//------------------------------------------
// Destructeur:
//------------------------------------------
GeoDbContext::~GeoDbContext()
{
 if( m_connected)
 {
   m_connected=false;
   if (m_db2!=NULL) delete(m_db2);
 }
}
} // namespace GeoEntities
} // namespace LinguisticProcessing
} // namespace Lima
