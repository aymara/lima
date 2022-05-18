# SPDX-FileCopyrightText: 2022 CEA LIST <gael.de-chalendar@cea.fr>
#
# SPDX-License-Identifier: MIT

 install(FILES
  Eigen_VERSION.cmake
  FindEigen.cmake
  FindProtobuf.cmake
  FindTensorFlow.cmake
  Protobuf_VERSION.cmake
  DESTINATION share/apps/lima/cmake/Modules
  COMPONENT devel)

