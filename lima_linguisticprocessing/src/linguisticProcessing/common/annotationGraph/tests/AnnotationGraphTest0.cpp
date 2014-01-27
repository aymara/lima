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
