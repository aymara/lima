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

/** @brief This is a @ref MediaProcessUnit that executes a RNN-based dependency parser model and fills LIMA data
 * structures with its results.
 *
 * As a ProcessUnit, it has an init and a process function. See @ref ProcessUnit for details.
 *
 * IOPES:
 * - Output:
 * - Preconditions:
 * - Effects:
 *
 */
class LIMA_RNNDEPENDENCYPARSER_EXPORT RnnDependencyParser : public MediaProcessUnit
{
    public:
    RnnDependencyParser();
    virtual ~RnnDependencyParser() override;

    void init(Lima::Common::XMLConfigurationFiles::GroupConfigurationStructure& unitConfiguration,
              Manager* manager) override;

    LimaStatusCode process(AnalysisContent& analysis) const override;

    private:
    RnnDependencyParserPrivate* m_d;
};

} // namespace Lima
#endif //LIMA_RNNDEPENDENCYPARSER_H
