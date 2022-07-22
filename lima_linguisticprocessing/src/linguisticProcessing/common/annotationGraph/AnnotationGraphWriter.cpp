// Copyright 2002-2013 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#include "AnnotationGraphWriter.h"

#include <fstream>
#include "common/misc/graphviznowarn.hpp"

namespace Lima
{
namespace Common
{
namespace AnnotationGraphs
{

std::string transcodeToXmlEntities(const std::string& str)
{
  std::string xml;
  std::string::const_iterator it, it_end;
  it = str.begin(); it_end = str.end();
  for (; it != it_end; it++)
  {
    switch (*it)
    {
      case '<' : /*xml += "\<";*/ break;
      case '&' : /*xml += "\&";*/ break;
      case '>' : /*xml += "\>";*/ break;
      case '"' : /*xml += "\\"";*/ break;
      case '\'' : /*xml += "\'";*/ break;
      case '\n' : xml += "\n"; break;
      default:
      {
        xml += *it;
      }
    }
  }
  return xml;
}

class AnnotationGraphVertexWriter
{
public:
  AnnotationGraphVertexWriter(MediaId language,
                              const AnnotationData& data);
  AnnotationGraphVertexWriter(const AnnotationGraphVertexWriter& a);
                              
  void operator()(std::ostream& out, const AnnotationGraphVertex& v) const;

private:
  AnnotationGraphVertexWriter& operator=(const AnnotationGraphVertexWriter&) {return *this;}

  const AnnotationData& m_data;
  MediaId m_language;
  const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;
};
  
class AnnotationGraphEdgeWriter
{
public:
  AnnotationGraphEdgeWriter(MediaId language, const AnnotationData& data);
  AnnotationGraphEdgeWriter(const AnnotationGraphEdgeWriter& a);
    
  void operator()(std::ostream& out, const AnnotationGraphEdge& e) const;

private:
  AnnotationGraphEdgeWriter& operator=(const AnnotationGraphEdgeWriter&) {return *this;}

  const AnnotationData& m_data;
  const Common::PropertyCode::PropertyCodeManager* m_propertyCodeManager;
};

AnnotationGraphVertexWriter::AnnotationGraphVertexWriter(
    MediaId language,
    const AnnotationData& data):
m_data(data),
m_language(language)
{
  m_propertyCodeManager=&( static_cast<const Common::MediaticData::LanguageData&>( Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager());
}
                            
AnnotationGraphVertexWriter::AnnotationGraphVertexWriter(const AnnotationGraphVertexWriter& a) :
  m_data(a.m_data),
  m_language(a.m_language),
  m_propertyCodeManager(a.m_propertyCodeManager)
{
}

void AnnotationGraphVertexWriter::operator()(std::ostream& out, const AnnotationGraphVertex& v) const
{
  //     AGLOGINIT;
  //     const Common::PropertyCode::PropertyManager& numberManager = m_propertyCodeManager->getPropertyManager("NUMBER");
  //     const Common::PropertyCode::PropertyManager& genderManager = m_propertyCodeManager->getPropertyManager("GENDER");
  //     const Common::PropertyCode::PropertyManager& syntaxManager = m_propertyCodeManager->getPropertyManager("SYNTAX");
  //     const Common::PropertyCode::PropertyManager& macroManager = m_propertyCodeManager->getPropertyManager("MACRO");
  //     const Common::PropertyCode::PropertyManager& microManager = m_propertyCodeManager->getPropertyManager("MICRO");
  //     const FsaStringsPool& stringspool= Common::MediaticData::MediaticData::single().stringsPool(m_language);
  
  out << "[label=\"" << v << ": ";
  CVertexAGIannotPropertyMap vimap = boost::get(vertex_iannot, m_data.getGraph());
  AGIannotProp::const_iterator iit, iit_end;
  iit = vimap[v].begin(); iit_end = vimap[v].end();
  if (iit != iit_end)
  {
    out << Misc::limastring2utf8stdstring(m_data.annotationName((*iit).first)) << "="<<(*iit).second;
    iit++;
  }
  for (; iit != iit_end; iit++)
  {
    out << "," << Misc::limastring2utf8stdstring(m_data.annotationName((*iit).first)) << "="
    << (*iit).second;
  }
  out << ";";
  CVertexAGSannotPropertyMap vsmap = boost::get(vertex_sannot, m_data.getGraph());
  AGSannotProp::const_iterator sit, sit_end;
  sit = vsmap[v].begin(); sit_end = vsmap[v].end();
  if (sit != sit_end)
  {
    out << Misc::limastring2utf8stdstring(m_data.annotationName((*sit).first))
    << "=" << Misc::limastring2utf8stdstring((*sit).second);
    sit++;
  }
  for (; sit != sit_end; sit++)
  {
    out << "," << Misc::limastring2utf8stdstring(m_data.annotationName((*sit).first)) << "="
    << Misc::limastring2utf8stdstring((*sit).second);
    
  }
  out << ";";
  CVertexAGGannotPropertyMap vgmap = boost::get(vertex_gannot, m_data.getGraph());
  AGGannotProp::const_iterator git, git_end;
  git = vgmap[v].begin(); git_end = vgmap[v].end();
  if (git != git_end)
  {
    std::string annot = Misc::limastring2utf8stdstring(m_data.annotationName((*git).first));
    out << annot << "=";
    std::ostringstream oss;
    const_cast<AnnotationData*>(&m_data)->dumpFunction((*git).first)->dump(
      oss,
      const_cast<GenericAnnotation&>(((*git).second))
    );
    out<< transcodeToXmlEntities(oss.str());
    git++;
  }
  for (; git != git_end; git++)
  {
    std::string annot = Misc::limastring2utf8stdstring(m_data.annotationName((*git).first));
    out << "," << annot << "=";
    std::ostringstream oss;
    const_cast<AnnotationData*>(&m_data)->dumpFunction((*git).first)->dump(
      oss,
      const_cast<GenericAnnotation&>(((*git).second))
    );
    out<< transcodeToXmlEntities(oss.str());
  }
  
  out << "\"]";
}

AnnotationGraphEdgeWriter::AnnotationGraphEdgeWriter(
  MediaId language,
  const AnnotationData& data) :
    m_data(data)
{
  m_propertyCodeManager=&( static_cast<const Common::MediaticData::LanguageData&>(
    Common::MediaticData::MediaticData::single().mediaData(language)).getPropertyCodeManager());
}
  
AnnotationGraphEdgeWriter::AnnotationGraphEdgeWriter(const AnnotationGraphEdgeWriter& a) : 
    m_data(a.m_data),
    m_propertyCodeManager(a.m_propertyCodeManager)
{
}

void AnnotationGraphEdgeWriter::operator()(std::ostream& out, const AnnotationGraphEdge& e) const
{
  
  //     const Common::PropertyCode::PropertyManager& microManager = m_propertyCodeManager->getPropertyManager("MICRO");
  
  out << "[label=\"";
  CEdgeAGIannotPropertyMap eimap = boost::get(edge_iannot, m_data.getGraph());
  AGIannotProp::const_iterator iit, iit_end;
  iit = eimap[e].begin(); iit_end = eimap[e].end();
  if (iit != iit_end)
  {
    out << Misc::limastring2utf8stdstring(m_data.annotationName((*iit).first)) << "="<<(*iit).second;
    iit++;
  }
  for (; iit != iit_end; iit++)
  {
    out << "," << Misc::limastring2utf8stdstring(m_data.annotationName((*iit).first)) << "="<<(*iit).second;
  }
  out << ";";
  CEdgeAGSannotPropertyMap esmap = boost::get(edge_sannot, m_data.getGraph());
  AGSannotProp::const_iterator sit, sit_end;
  sit = esmap[e].begin(); sit_end = esmap[e].end();
  if (sit != sit_end)
  {
    out << Misc::limastring2utf8stdstring(m_data.annotationName((*sit).first)) << "="
    << Misc::limastring2utf8stdstring((*sit).second);
    sit++;
  }
  for (; sit != sit_end; sit++)
  {
    out << "," << Misc::limastring2utf8stdstring(m_data.annotationName((*sit).first)) << "="
    << Misc::limastring2utf8stdstring((*sit).second);
    
  }
  out << ";";
  CEdgeAGGannotPropertyMap egmap = boost::get(edge_gannot, m_data.getGraph());
  AGGannotProp::const_iterator git, git_end;
  git = egmap[e].begin(); git_end = egmap[e].end();
  if (git != git_end)
  {
    std::string annot = Misc::limastring2utf8stdstring(m_data.annotationName((*git).first));
    out << annot << "=";
    std::ostringstream oss;
    const_cast<AnnotationData*>(&m_data)->dumpFunction((*git).first)->dump(
      oss,
      const_cast<GenericAnnotation&>(((*git).second))
    );
    out<< transcodeToXmlEntities(oss.str());
    git++;
  }
  for (; git != git_end; git++)
  {
    std::string annot = Misc::limastring2utf8stdstring(m_data.annotationName((*git).first));
    out << "," << annot << "=";
    std::ostringstream oss;
    const_cast<AnnotationData*>(&m_data)->dumpFunction((*git).first)->dump(
      oss,
      const_cast<GenericAnnotation&>(((*git).second))
    );
    out<< transcodeToXmlEntities(oss.str());
  }
  
  out << "\"]";
}


void AnnotationGraphWriter::writeToDotFile(const std::string& dotFileName)
{
  if (dotFileName != "")
  {
    std::ofstream ofs(dotFileName.c_str(), std::ofstream::binary);
    boost::write_graphviz(
      ofs,
      m_data.getGraph(),
      AnnotationGraphVertexWriter(m_language,m_data),
      AnnotationGraphEdgeWriter(m_language,m_data));
  }

}

} // closing namespace AnnotationGraphs
} // closing namespace Common
} // closing namespace Lima
