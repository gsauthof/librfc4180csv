cmake_minimum_required(VERSION 2.8.6)
project(rfc4180csv CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake/ragel")
find_package(RAGEL 6.6 REQUIRED)

RAGEL_TARGET(rfc4180csv csv/rfc4180csv.rl ${CMAKE_CURRENT_BINARY_DIR}/rfc4180csv.cc)

# only for the unittests, the lexer doesn't depend on it
find_package(Boost 1.53
  COMPONENTS
    system
    filesystem
    unit_test_framework
  REQUIRED
)

# guard from super-projects, i.e. when it is added as subdirectory
if(${CMAKE_PROJECT_NAME} STREQUAL "rfc4180csv")

  add_subdirectory(libbuffer)

  add_executable(ut
    unittest/main.cc
    unittest/rfc4180csv.cc
    ${RAGEL_rfc4180csv_OUTPUTS})
  target_link_libraries(ut ${Boost_LIBRARIES} bufferlite)
  set_property(TARGET ut PROPERTY INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/libbuffer
  )

  add_executable(test test.cc ${RAGEL_rfc4180csv_OUTPUTS})
  target_link_libraries(test ${Boost_SYSTEM_LIBRARY} ${Boost_FILESYSTEM_LIBRARY} bufferlite)
  set_property(TARGET test PROPERTY INCLUDE_DIRECTORIES
    ${Boost_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/libbuffer
    ${CMAKE_CURRENT_SOURCE_DIR}/csv
  )

endif()

add_library(rfc4180csv SHARED 
  ${RAGEL_rfc4180csv_OUTPUTS}
)
set_property(TARGET rfc4180csv PROPERTY INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/libbuffer
)

add_library(rfc4180csv_static STATIC
  ${RAGEL_rfc4180csv_OUTPUTS}
)
# under windows shared/static libraries have the same extension ...
if(${UNIX})
  set_target_properties(rfc4180csv_static PROPERTIES OUTPUT_NAME rfc4180csv)
endif()
set_property(TARGET rfc4180csv_static PROPERTY INCLUDE_DIRECTORIES
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/libbuffer
)
