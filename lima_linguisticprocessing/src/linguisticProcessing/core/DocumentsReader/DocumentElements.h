/************************************************************************
 * @file   DocumentElements.h
 * @author Besancon Romaric
 * @date   Wed Oct 29 13:22:02 2003
 * @author Gael de Chalendar
 * @date   Wed Apr 08 2015
 ***********************************************************************/

#include "documentsreader_export.h"
#include "linguisticProcessing/LinguisticProcessingCommon.h"
#include "common/Data/LimaString.h"
#include "common/Data/genericDocumentProperties.h"
#include "documentProperty.h"
#include "linguisticProcessing/core/StructuredXmlDocumentHandler.h"

#include <deque>
#include <string>

#ifndef DOCUMENTELEMENTS_H
#define DOCUMENTELEMENTS_H

namespace Lima {
namespace DocumentsReader {

/** a type containing the type of an element */
typedef enum
{
    NODE_NOTYPE,       /** no type: field is ignored */
    NODE_ROOT,         /** root of XML file */
    NODE_DISCARDABLE,  /** node inside an indexing node whose content (including its children) is replaced by spaces*/
    NODE_INDEXING,     /** node whose content is analyzed and indexed */
    NODE_HIERARCHY,    /** node that can contain indexing nodes in its hierarchy. It is indexed in the structure index */
    NODE_PRESENTATION, /** node inside an indexing node whose content which is replaced by spaces but its content is kept */
    NODE_IGNORED,      /** nodes which are not children of hierarchy nodes are ignored */
    NODE_PROPERTY,     /** a property node */
} FieldType;


class DOCUMENTSREADER_EXPORT AbstractDocumentElement {
public:
  virtual ~AbstractDocumentElement() {}

  virtual bool isDiscarding() {return false;}
  virtual bool isPresentation() {return false;}

  virtual FieldType nodeType() {return NODE_NOTYPE;}
};

/**
 * @brief Partie de texte (contenu dans un element) partageant le meme offset caractere.
 * L'offset est defini par rapport au premier element englobant de type indexing
 * Recupere plusieurs chaines issues d'appels au callback character() si elles sont contigues.
 * The first part of the pair is the text and the second one is the offset.
 */
class DOCUMENTSREADER_EXPORT ContentElementPart : public AbstractDocumentElement, public std::pair<Lima::LimaString,unsigned int> {
public:
  ContentElementPart():std::pair<Lima::LimaString,unsigned int>(Lima::LimaString(),0) {}
  ContentElementPart(const Lima::LimaString& text, const unsigned int offset)
   : std::pair<Lima::LimaString,unsigned int>(text,offset) {}
  ~ContentElementPart() {}
  void addText(const Lima::LimaString& text) { first.append(text); }
  const Lima::LimaString& getText(void) const { return first; }
  unsigned int getOffset(void) const { return second; }
  Lima::LimaString& getText(void) { return first; }
};

/**
 * @brief Element avec contenu texte.
 * Le texte peut etre forme de plusieurs chaines avec des offset decales
 * ces chaines seront analysees separement
*/
class DOCUMENTSREADER_EXPORT AbstractStructuredDocumentElement : public AbstractDocumentElement, public std::deque<ContentElementPart*> {
public:
  virtual ~AbstractStructuredDocumentElement();

  // Ajoute une partie de contenu non contigue
  void addPart(const Lima::LimaString& text, const unsigned int offset);

  virtual bool hasPropType( ) { return false; }
  virtual const DocumentPropertyType& getPropType() { return m_propType; }
  virtual const Lima::LimaString& getPropertyValue() const { return m_propertyValue; }

  virtual const QString& getElementName() const {return m_elementName; };

  // Called when some character content is added
  virtual void addToCurrentOffset(const Lima::LimaString& value);

  // Accessor
  unsigned long int getOffset();

  // Accessor
  void setOffset( unsigned long int offset);

  void addSpaces(int nbSpaces);

protected:
  AbstractStructuredDocumentElement( const QString& elementName, unsigned int firstBytePos  );

  QString m_elementName; //!< nom du tag de l'element

  unsigned long int m_offset; //!< current position during parsing

  DocumentPropertyType m_propType;
  Lima::LimaString m_propertyValue;

};


/**
 * @brief Element d'un document Xml discardable
 * Exemple <USELESS>private comment</USELESS>
 *     => "                                  "
*/
class DOCUMENTSREADER_EXPORT DiscardableDocumentElement : public AbstractStructuredDocumentElement
{
public:
  DiscardableDocumentElement( const QString& elementName, unsigned int firstBytePos  );
  ~DiscardableDocumentElement() {}
  bool isDiscarding() override {return true;}
  virtual FieldType nodeType() override {return NODE_DISCARDABLE;}
};

class DOCUMENTSREADER_EXPORT AbstractStructuredDocumentElementWithProperties :
    public AbstractStructuredDocumentElement,
    public Lima::Common::Misc::GenericDocumentProperties
{
public:
  AbstractStructuredDocumentElementWithProperties( const QString& elementName, unsigned int firstBytePos ,
   const std::map<DocumentPropertyType, std::string>& toBePropagated );
  virtual ~AbstractStructuredDocumentElementWithProperties();

  void addProperty( const DocumentPropertyType& propType, const std::string& value );

  const std::map<DocumentPropertyType, std::string>& getPropertyList() const;

  void setPropagatedValue(
    const std::map<DocumentPropertyType, std::string>& toBePropagated );

protected:
  std::map<DocumentPropertyType, std::string> m_toBePropagated;

};

/**
 * @brief Element d'un document XML dont content est indexee
 * Exemple <section lang="fre">yeah</section>
 * Il peut avoir des fils
 * On peut lui affecter des proprietes: addProperty()
 * Il peut etre un element d'indexation: isIndexing()
 * Il a une liste de proprietes à propager
 * Lui ou un parent est une unite d'indexation : isLocated()
 * A la limite, il peut etre une propriete, mais c'est un cas exceptionel
 * de (con)fusion entre contenu, propriete et element indexe
 * exemple <doc><title>titre</title><content>bla bla</content><doc>
 * le doc n'est pas de type indexing mais le titre l'est et est une propriete de doc
 * auquel cas, il n'y a pas de proprietes a propager ??
 * Il herite de hasContent()
*/
class DOCUMENTSREADER_EXPORT IndexingDocumentElement :
    public AbstractStructuredDocumentElementWithProperties
{
public:
  IndexingDocumentElement(  const QString& elementName, unsigned int firstBytePos , const DocumentPropertyType& propType, const std::map< DocumentPropertyType, std::string >& toBePropagated );
  ~IndexingDocumentElement();

  bool hasPropType( )  override { return( m_propType.getValueCardinality() != CARDINALITY_NONE ); }
  virtual FieldType nodeType() override {return NODE_INDEXING;}

};

/**
 * @brief A hierarchy node.
 * Its children are ignored unless hierarchy or indexing
*/
class DOCUMENTSREADER_EXPORT HierarchyDocumentElement :
    public AbstractStructuredDocumentElementWithProperties
{
public:
  HierarchyDocumentElement( const QString& elementName, unsigned int firstBytePos ,
   const std::map<DocumentPropertyType, std::string>& toBePropagated  );
  ~HierarchyDocumentElement() {}
  virtual FieldType nodeType() override {return NODE_HIERARCHY;}
};

/**
 * @brief Element d'un document XML de presentation
 * Exemple <BOLD>important</BOLD> => "      important       "
 * ou bien <BOLD>C'est <UNDERLINED>tres<UNDERLINED>important</BOLD>
 *     => "      C'est             tres<           important       "
*/
class PresentationDocumentElement : public AbstractStructuredDocumentElementWithProperties
{
public:
  PresentationDocumentElement( const QString& elementName, unsigned int firstBytePos ,
   const std::map<DocumentPropertyType, std::string>& toBePropagated );
  ~PresentationDocumentElement() {}
  bool isPresentation() override {return true;}
  virtual FieldType nodeType() override {return NODE_PRESENTATION;}
};

/**
 * @brief An ignored node.
 * All its children are ignored
*/
class DOCUMENTSREADER_EXPORT IgnoredDocumentElement : public AbstractStructuredDocumentElementWithProperties
{
public:
  IgnoredDocumentElement( const QString& elementName, unsigned int firstBytePos, const DocumentPropertyType& propType, const std::map<DocumentPropertyType, std::string >& toBePropagated  );
  ~IgnoredDocumentElement() {}
  virtual FieldType nodeType() override {return NODE_IGNORED;}

  bool hasPropType( ) override { return( m_propType.getValueCardinality() != CARDINALITY_NONE ); }
};

/**
 * @brief Element d'un document XML definissant une propriete du noeud pere
 * Exemple <filename>d1.xml</filename>
 * la valeur sera convertie en valeur de propriete et affectee a l'element pere selon m_propType
*/
class DOCUMENTSREADER_EXPORT DocumentPropertyElement : public AbstractStructuredDocumentElement
{
public:
  DocumentPropertyElement(const QString& elementName, unsigned int firstBytePos , const DocumentPropertyType& propType );
  bool hasPropType( ) override { return true; }
  ~DocumentPropertyElement() {}
  // Called when some character content is added
  void addToCurrentOffset(const Lima::LimaString& value) override;
  virtual FieldType nodeType() override {return NODE_PROPERTY;}

};


} // namespace DocumentsReader
} // namespace Lima

#endif
