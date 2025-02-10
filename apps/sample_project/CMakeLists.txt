cmake_minimum_required(VERSION 3.22)

# SUPPORTS_OS_Linux
# SUPPORTS_OS_Darwin
# SUPPORTS_OS_Windows

#set(PROJECT_ID "hello") # <<<<= override default project name if desired
set(PROJECT_DESCRIPTION "Hello World Project")
set(PROJECT_VERSION 0.0.1.0) 
set(PROJECT_COMPANY_NAME "My Company")
set(PROJECT_COMPANY_NAMESPACE "com.mycompany")  # Reverse domain name notation

set(LIBRARIES ) # <<<<= list of libraries to link against. This should be a list of 
# folder names from the libraries folder two levels above this file
#  for example: set(LIBRARIES eigen linmath)


# -----------  call ProjectInit.cmake before project()  ----------------
include(${CMAKE_SOURCE_DIR}/../libraries/cmake/ProjectInit.cmake)

add_executable(${PROJECT_NAME}
  main.cpp
)

add_subdirectory(packaging)

# -----------  call ProjectFinalize.cmake last thing ----------------
include(${CMAKE_SOURCE_DIR}/../libraries/cmake/ProjectFinalize.cmake)

