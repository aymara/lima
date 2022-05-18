// Copyright 2021 CEA LIST
// SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
//
// SPDX-License-Identifier: MIT

#ifndef DEEPLIMA_VERSION_H
#define DEEPLIMA_VERSION_H

#include <string>

namespace deeplima
{
namespace version
{

const std::string& get_git_commit_hash();
const std::string& get_git_branch();

} // version
} // deeplima

#endif // DEEPLIMA_VERSION_H

