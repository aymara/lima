// Copyright 2002-2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

/***************************************************************************
 *   Copyright (C) 2004-2021 by CEA LIST                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_MEDIAPROCESSORSMEDIAPROCESSORS_H
#define LIMA_COMMON_MEDIAPROCESSORSMEDIAPROCESSORS_H

#include "MediaProcessUnitPipeline.h"

#include "common/AbstractFactoryPattern/Singleton.h"
#include "common/LimaCommon.h"

#include <deque>
#include <string>
#include <set>

namespace Lima{

  namespace Common {
    namespace XMLConfigurationFiles {
      class ModuleConfigurationStructure;
      class GroupConfigurationStructure;
    }
  }
class MediaAnalysisDumper;

class MediaProcessorsPrivate;
class LIMA_MEDIAPROCESSORS_EXPORT MediaProcessors : public Singleton<MediaProcessors>
{
        friend class Singleton<MediaProcessors>;

    public:

        virtual ~MediaProcessors();

        /**
        * @brief get the pipeline for the given id and media
        * @param med media requested
        * @param id pipeline id
        * @return ProcessUnitPipeline*
        * @retval 0 if can't get the corresponding pipeline
        */
        std::shared_ptr<MediaProcessUnitPipeline> getPipelineForId ( MediaId med,const std::string& id ) const;

        /**
        * @brief get the dumper for the given id and media
        * @param med media requested
        * @param type dumper id
        * @return ObjetAnalysisDumper*
        * @retval 0 if can't get the corresponding dumper
        */
        std::shared_ptr<MediaAnalysisDumper> getAnalysisDumperForId ( MediaId med,const std::string& type ) const;

        /**
        * @brief (re)initialize the process units manager of a media.
        *
        * A media already initialized has its process units (and the models
        * they loaded) released first, then rebuilt from @ref procConf: the
        * pipelines it had are instantiated again by the next call to
        * initPipelines, which must follow.
        */
        void initMedia (
            MediaId,
            Common::XMLConfigurationFiles::ModuleConfigurationStructure& procConf );

        void initPipelines (
            Common::XMLConfigurationFiles::GroupConfigurationStructure& confModule,
            const std::deque<std::string>& pipelines );

        /**
        * @brief release the pipelines and the process units (and thus the
        * models they loaded) of a media. Its pipelines are no longer available
        * until it is initialized again (initMedia then initPipelines).
        */
        void releaseMedia ( MediaId med );

        void getAvailableMedia ( std::set<MediaId>& ids ) const;
        void getAvailablePipelinesForMedia ( MediaId id, std::set<std::string>& pipelines ) const;

        const std::map<MediaId,MediaProcessUnit::Manager*>& managers() const;


private:

  MediaProcessors();
  MediaProcessors(const MediaProcessors& mp);
  MediaProcessors& operator=(const MediaProcessors& mp);

  MediaProcessorsPrivate* m_d;
};


} // Lima

#endif
