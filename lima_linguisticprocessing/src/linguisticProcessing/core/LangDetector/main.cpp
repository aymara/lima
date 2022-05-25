#include "LangDetectorCore.h"
#include <QtXml/QXmlSimpleReader>
#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include "common/tools/LimaMainTaskRunner.h"
#include "common/LimaCommon.h"
#include "common/tools/FileUtils.h"

int run(int aargc,char** aargv);

using namespace Lima::LinguisticProcessing::LDetector;
using namespace Lima::Common::Misc;

int main(int argc, char *argv[]) {
    QCoreApplication a(argc, argv);

    // Task parented to the application so that it
    // will be deleted by the application.
    Lima::LimaMainTaskRunner *task = new Lima::LimaMainTaskRunner(argc, argv, run, &a);

    // This will cause the application to exit when
    // the task signals finished.
    QObject::connect(task, &Lima::LimaMainTaskRunner::finished, &a, &QCoreApplication::exit);

    // This will run the task from the application event loop.
    QTimer::singleShot(0, task, SLOT(run()));

    return QCoreApplication::exec();
}
int run(int , char** ){
    auto resourcesDirs = buildResourcesDirectoriesList(QStringList({"lima"}),
                                                       QStringList());
    QString resourcesPath = resourcesDirs.join(LIMA_PATH_SEPARATOR);
    QCommandLineParser parser;
    parser.setApplicationDescription("LIMA file language detector.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("files", QCoreApplication::translate("main", "File(s) to analyze."), "[file...]");
    parser.process(QCoreApplication::arguments());
    QStringList files = parser.positionalArguments();
    if(files.isEmpty()){
        parser.showHelp();
    }
    auto ld = std::make_unique<LangDetectorCore>();
    ld->loadModel(resourcesPath.toStdString() + std::string("/LinguisticProcessings/LangDetector/lid.176.ftz"));
    for (auto & it : files) {
        const QString &fileName = it;
        QFile file(fileName);
        if (!file.open(QIODevice::ReadOnly)) {
            std::cerr << "Error opening " << it.toUtf8().constData() << std::endl;
            return 1;
        }
        QByteArray text = file.readAll();
        std::string label = ld->detectLang(text.toStdString());
        std::cout << "The detected language for file : " << fileName.toStdString() << " is: " << LangDetectorCore::labelToPrintable(label) << "\n";
    }
    return 0;
}