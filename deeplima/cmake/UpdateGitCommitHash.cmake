# Copyright 2021 CEA LIST
#
# This file is part of LIMA.
#
# LIMA is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# LIMA is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with LIMA.  If not, see <https://www.gnu.org/licenses/>

set(CURRENT_LIST_DIR ${CMAKE_CURRENT_LIST_DIR})

if (NOT DEEPLIMA_STANDALONE)
    set(src_prefix "deeplima")
endif()

if (NOT DEFINED src_dir)
    set(src_dir ${CMAKE_SOURCE_DIR}/${src_prefix}/libs/version)
endif ()

if (NOT DEFINED bin_dir)
    set(bin_dir ${CMAKE_BINARY_DIR}/libs/version/generated)
endif ()

set(SRC_VERSION_CPP "${src_dir}/version.cpp.in")
set(DST_VERSION_CPP "${bin_dir}/version.cpp")

SET(SRC_VERSION_H "${src_dir}/version.h")
SET(DST_VERSION_H "${bin_dir}/version.h")
 
function(SaveToCache git_hash git_branch)
    file(WRITE ${CMAKE_BINARY_DIR}/git-hash.txt ${git_hash})
    file(WRITE ${CMAKE_BINARY_DIR}/git-branch.txt ${git_branch})
endfunction()

function(LoadFromCache git_hash git_branch)
    if (EXISTS ${CMAKE_BINARY_DIR}/git-hash.txt)
        file(STRINGS ${CMAKE_BINARY_DIR}/git-hash.txt CONTENT)
        LIST(GET CONTENT 0 var)

        set(${git_hash} ${var} PARENT_SCOPE)
    endif ()

    if (EXISTS ${CMAKE_BINARY_DIR}/git-branch.txt)
        file(STRINGS ${CMAKE_BINARY_DIR}/git-branch.txt CONTENT)
        LIST(GET CONTENT 0 var)

        set(${git_branch} ${var} PARENT_SCOPE)
    endif ()
endfunction()

function(UpdateGitData)
    # Get commit hash
    execute_process(
        COMMAND git describe --dirty --tags
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    # Get current branch
    execute_process(
        COMMAND git branch --show-current
        WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    LoadFromCache(GIT_HASH_CACHE, GIT_BRANCH_CACHE)
    if (NOT EXISTS ${bin_dir})
        file(MAKE_DIRECTORY ${bin_dir})
    endif ()

    if (NOT EXISTS ${DST_VERSION_H})
        MESSAGE("Creating ${DST_VERSION_H} ...")
        file(COPY ${SRC_VERSION_H} DESTINATION ${bin_dir})
    else()
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E compare_files ${SRC_VERSION_H} ${DST_VERSION_H}
            RESULT_VARIABLE VERSION_H_COMPARE_RESULT)
        if (${VERSION_H_COMPARE_RESULT})
            MESSAGE("Updating ${DST_VERSION_H} ...")
            file(COPY ${SRC_VERSION_H} DESTINATION ${bin_dir})
        endif()
    endif()

    if (NOT DEFINED GIT_HASH_CACHE)
        set(GIT_HASH_CACHE "INVALID")
    endif ()

    # Only update the version.cpp if the hash or branch has changed.
    if (NOT "${GIT_HASH}" STREQUAL "${GIT_HASH_CACHE}"
        OR NOT "${GIT_BRANCH}" STREQUAL "${GIT_BRANCH_CACHE}"
	OR NOT EXISTS "${DST_VERSION_CPP}")
        # Save current values of GIT_HASH/BRANCH_CACHE until the next build
	SaveToCache(${GIT_HASH}, ${GIT_BRANCH})

	configure_file(${SRC_VERSION_CPP} ${DST_VERSION_CPP} @ONLY)
    endif()

endfunction()

function(AddUpdateGitCommitHashTarget)

    add_custom_target(update_git_commit_hash COMMAND ${CMAKE_COMMAND}
        -DRUN_UPDATE_GIT_DATA=1
        -Dsrc_dir=${src_dir}
        -Dbin_dir=${bin_dir}
        -DGIT_HASH_CACHE=${GIT_HASH_CACHE}
        -DGIT_BRANCH_CACHE=${GIT_BRANCH_CACHE}
	-P ${CURRENT_LIST_DIR}/UpdateGitCommitHash.cmake
        BYPRODUCTS ${post_configure_file}
        )

    add_library(version ${CMAKE_BINARY_DIR}/libs/version/generated/version.cpp)
    target_include_directories(version PUBLIC ${CMAKE_BINARY_DIR}/libs/version/generated)
    add_dependencies(version update_git_commit_hash)

    UpdateGitData()
endfunction()

# If called from AddUpdateGitCommitHashTarget / add_custom_target / update_git_commit_hash
if (RUN_UPDATE_GIT_DATA)
    UpdateGitData()
endif()

