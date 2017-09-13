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
// clazy:skip


#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE AnnotationGraphTest
#include <boost/test/unit_test.hpp>

#include "linguisticProcessing/common/annotationGraph/AnnotationData.h"

using namespace Lima;
using namespace Lima::Common::AnnotationGraphs;

BOOST_AUTO_TEST_CASE( AnnotationGraphTest0_1 )
{
  AnnotationData data;
  AnnotationGraphVertex v = data.createAnnotationVertex();
  data.annotate(v, "test", 1);
  BOOST_REQUIRE( data.hasIntAnnotation(v, "test") );
}

BOOST_AUTO_TEST_CASE( AnnotationGraphTest0_2 )
{
  AnnotationData data;
  AnnotationGraphVertex v = data.createAnnotationVertex();
  data.annotate(v, "test", LimaString("value"));
  BOOST_REQUIRE( data.hasStringAnnotation(v, "test") );
}
