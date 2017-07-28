#include "NamedEntitiesParser.h"
#include "ConllParser.h"

namespace Lima {
namespace Gui {

std::string markupa(const std::string& content, const std::string& markup, const std::string& style, const std::string& metadata) {
  return "<" + markup  + (style.length() ? " style=\"" + style + "\"" : "") + " " + metadata + ">" + content + "</" + markup + ">";
}

void replace_all(std::string& text, const std::string& occurence, const std::string& sub) {
  std::size_t it = -1;
  while (it = text.find(occurence, it + 1), it != std::string::npos) {

    text.replace(it, occurence.length(), sub);
    it += sub.length();
  }
}


std::string highlightNamedEntities(
    const std::string& raw,
    const std::vector<EntityItem>& entities)
{
  std::string text = raw;
//  for (auto& type : types) {
//    for (auto& entity : type.second) {
//      boost::replace_all(text, entity, markup(entity,"strong","color:"+colors[type.first]));
//    }
//  }

  for (auto& entity : entities) {
    for (auto& o : entity.occurences) {
      replace_all(text, o, markupa(o, "mark", "border-radius:10; background-color:"+entity.color + "; border: 1px solid #aaeeee", "name=\"" + entity.name + "\""));
    }
  }

  return text;
}

int randint(int a, int b) {
  return rand()%(b-a)+a;
}

std::vector<std::string> generateDistinctColors(int quantity) {

  srand(time(NULL));

  std::vector<std::string> colors;

  for (int i=0; i<quantity; i++) {

//    do {

//    } while (std::find(colors.begin(), colors.end(), color) != colors.end());

      std::string str = "#";

      for (int i=0;i<6;i++) {
        int x = randint(8,15);
        str += (x > 9 ? 'A' - 9:'0') + x;
      }

      colors.push_back(str);
  }

  return colors;
}

///////////////////////////////////////////////////////////////

NamedEntitiesParser::NamedEntitiesParser(QObject* p) : QObject(p)
{

}

void NamedEntitiesParser::parse(const QString& rawtext, const QString& conllText) {

  entities.clear();

  this->rawText = rawtext;
  this->conllText = conllText;

  std::map<std::string, std::vector<std::string> > data = getNamedEntitiesFromConll(conllText.toStdString());
  std::vector<std::string> colors = generateDistinctColors(data.size());
  int i = 0;
  for (auto& pair : data) {
    entities.push_back(EntityItem(pair.first, colors[i++], pair.second));
  }
}

QStringList NamedEntitiesParser::getEntityTypes() {
  QStringList qsl;
  for (auto& item : entities) {
    std::string str = item.name + ":" + item.color;
    qsl << QString(str.c_str());
  }
  return qsl;
}

QString NamedEntitiesParser::getHighlightedText() {

//  CONLL_List conllList = conllRawToLines(conllText);
//  std::string rawtext = "";

//  for (auto& line : conllList) {
//    rawtext
//  }

  /// this only highlights all substrs that are occurences.
  /// ^ Like this, you need to use the conll data to know what to highlight

  return QString(highlightNamedEntities(rawText.toStdString(),entities).c_str());
}


} // Gui
} // Lima
