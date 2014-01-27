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
