cmake_minimum_required(VERSION 3.22)

get_filename_component(MSSM_BUNDLE_NAME "${CMAKE_CURRENT_LIST_DIR}" NAME)
string(REPLACE " " "_" MSSM_BUNDLE_NAME "${MSSM_BUNDLE_NAME}")
project(${MSSM_BUNDLE_NAME} LANGUAGES CXX C)

set(MSSM_BUILD_TESTS ON CACHE BOOL "Build library unit tests" FORCE)

include("${CMAKE_CURRENT_SOURCE_DIR}/../libraries/cmake/MssmAggregateRoot.cmake")
