cmake_minimum_required(VERSION 3.22)

# SUPPORTS_OS_Linux
# SUPPORTS_OS_Darwin
# SUPPORTS_OS_Windows

set(PROJECT_DESCRIPTION "OpenCV reference app")
set(PROJECT_VERSION 0.0.1.0)
set(PROJECT_COMPANY_NAME "MSSM")
set(PROJECT_COMPANY_NAMESPACE "edu.mssm")

set(LIBRARIES mssm opencv opencv_bridge)

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/ProjectInit.cmake)

add_executable(${PROJECT_NAME}
  main.cpp
)

if(DEFINED PROJECT_OUTPUT_NAME AND NOT PROJECT_OUTPUT_NAME STREQUAL "")
  set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_OUTPUT_NAME}")
endif()

include(${CMAKE_SOURCE_DIR}/../libraries/cmake/ProjectFinalize.cmake)
