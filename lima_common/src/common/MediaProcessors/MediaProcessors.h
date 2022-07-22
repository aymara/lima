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
        const MediaProcessUnitPipeline* getPipelineForId ( MediaId med,const std::string& id ) const;

        /**
        * @brief get the dumper for the given id and media
        * @param med media requested
        * @param type dumper id
        * @return ObjetAnalysisDumper*
        * @retval 0 if can't get the corresponding dumper
        */
        const MediaAnalysisDumper* getAnalysisDumperForId ( MediaId med,const std::string& type ) const;

        void initMedia (
            MediaId,
            Common::XMLConfigurationFiles::ModuleConfigurationStructure& procConf );

        void initPipelines (
            Common::XMLConfigurationFiles::GroupConfigurationStructure& confModule,
            const std::deque<std::string>& pipelines );

        void getAvailableMedia ( std::set<MediaId>& ids ) const;
        void getAvailablePipelinesForMedia ( MediaId id, std::set<std::string>& pipelines ) const;

private:

  MediaProcessors();
  MediaProcessors(const MediaProcessors& mp);
  MediaProcessors& operator=(const MediaProcessors& mp);

  MediaProcessorsPrivate* m_d;
};


} // Lima

#endif
