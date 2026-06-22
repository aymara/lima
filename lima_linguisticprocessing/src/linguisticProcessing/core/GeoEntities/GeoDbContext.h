// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 * Copyright (C) 2003 by CEA-LIST
 * author Olivier Mesnard         olivier.mesnard@localhost
 *        Hervé Le Borgne pour l'interfacage SOCI
 *                                                                 *
 * database of documents.
 * Documents are indexed in inverted files (terms, frequency and pos)
 * and in a relational database (metadata: name, date, source...)
 ***************************************************************************/
#ifndef GEOENTITIES_DB_CONTEXT_H
#define GEOENTITIES_DB_CONTEXT_H

#include "soci/soci.h"


namespace Lima
{
namespace LinguisticProcessing
{
namespace GeoEntities
{

//! rief This class manages the connection to the RDBMS database 

//! Exemple of RDBMS database is PostGreSQL.
class GeoDbContext
{
public:
    //! rief Constructor of the class = connection to the database 

    //! @param backend type of connection (e.g postgresql)
    //! @param connectString a simple string containing the following elements, separated with spaces:

    //!   - the database name (e.g "dbname=myTestDb") 

    //!   - the user login (e.g "user=postgres") 

    //!   - the user password (e.g "password=sec08RET") 

    GeoDbContext ( std::string const &backend, std::string const &connectString );

    //! rief Destructor 

    //! implicit deconnection
    ~GeoDbContext();

    //! Returns a "database connection session" object
    soci::session & getDb2() {
        return  (*m_db2);
    };

    //! Check whether a  "database connection session" exists.
    bool isConnected() const {
        return m_connected;
    }

private:
    //! The "database connection session" object
    soci::session * m_db2;

    //! connected sucessfully with database
    bool m_connected;
};
} // namespace LinguisticProcessing
} // namespace GeoEntities
} // namespace Lima

#endif // GEOENTITIES_DB_CONTEXT_H
