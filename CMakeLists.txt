cmake_minimum_required(VERSION 3.5.0)
project(PASSWORD-MANAGER VERSION 0.1.0 LANGUAGES C CXX)

add_executable(PASSWORD-MANAGER main.cpp)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
