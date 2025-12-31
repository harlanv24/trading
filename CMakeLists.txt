cmake_minimum_required(VERSION 3.20)
project(polymarket_simulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(PMS_BUILD_TESTS "Build tests" ON)

add_library(polymarket_core
  src/core_placeholder.cpp
)

target_include_directories(polymarket_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/rapidjson/include
)
target_compile_features(polymarket_core PUBLIC cxx_std_20)

add_executable(pm_cli
  src/cli/main.cpp
)

target_link_libraries(pm_cli
  PRIVATE polymarket_core
)

if(PMS_BUILD_TESTS)
  enable_testing()
  add_executable(pm_tests
    tests/test_placeholder.cpp
  )
  target_link_libraries(pm_tests PRIVATE polymarket_core)
  add_test(NAME pm_tests COMMAND pm_tests)
endif()
