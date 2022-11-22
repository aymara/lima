//
// Created by tderouet on 29/09/22.
//

#ifndef LIMA_RNNDEPENDENCYPARSER_H
#define LIMA_RNNDEPENDENCYPARSER_H
#include "RnnDependencyParserExport.h"
#include "common/MediaProcessors/MediaProcessUnit.h"

namespace Lima::LinguisticProcessing::DeepLimaUnits::RnnDependencyParser
{

    class CharChart;

    class RnnDependencyParserPrivate;
#define RNNDEPENDENCYPARSER_CLASSID "RnnDependencyParser"

/** @brief This is a @ref MediaProcessUnit that is usually the first element of the pipeline. It cuts the input text into tokens
*
* As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
*
* IOPES:
* - Output:          an AnalysisContent
* - Preconditions:   the AnalysisContent must contain an AnalysisData of type LimaStringText named "Text"
* - Effects:         the AnalysisContent will contain an AnalysisData of type AnalysisGraph
*                    named "AnalysisGraph" which is a linear graph (a string) containing one vertex
*                    for each detected token.
*/
    class LIMA_RNNDEPENDENCYPARSER_EXPORT RnnDependencyParser : public MediaProcessUnit
{
    public:
    RnnDependencyParser();
    ~RnnDependencyParser() override;

    void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
              Manager* manager) override;

    LimaStatusCode process(AnalysisContent& analysis) const override;

    private:
    RnnDependencyParserPrivate* m_d;
};

} // namespace Lima
#endif //LIMA_RNNDEPENDENCYPARSER_H
