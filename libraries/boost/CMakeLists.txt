# libraries/boost/CMakeLists.txt
set(NAME "boost")

CPMAddPackage(
  NAME boost_source
  GITHUB_REPOSITORY boostorg/boost
  GIT_TAG boost-1.86.0
  GIT_SHALLOW TRUE
  EXCLUDE_FROM_ALL TRUE
  DOWNLOAD_ONLY TRUE
)

add_library(${NAME} INTERFACE)
add_library(Boost::multiprecision ALIAS ${NAME})
add_library(boost::multiprecision ALIAS ${NAME})

target_include_directories(${NAME} INTERFACE
  $<BUILD_INTERFACE:${boost_source_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${boost_source_SOURCE_DIR}>
)