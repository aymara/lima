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
/***************************************************************************
 *   Copyright (C) 2004-2012 by CEA LIST                               *
 *                                                                         *
 ***************************************************************************/
#ifndef LIMA_COMMON_MEDIAPROCESSORSMEDIAPROCESSORS_H
#define LIMA_COMMON_MEDIAPROCESSORSMEDIAPROCESSORS_H

#include "MediaProcessUnitPipeline.h"

#include "common/AbstractFactoryPattern/Singleton.h"
#include "common/LimaCommon.h"

#include <deque>
#include <string>

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

private:

  MediaProcessors();
  MediaProcessors(const MediaProcessors& mp);
  MediaProcessors& operator=(const MediaProcessors& mp);

  MediaProcessorsPrivate* m_d;
};


} // Lima

#endif
