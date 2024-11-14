cmake_minimum_required(VERSION 3.5)
project(null0)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
# set(FETCHCONTENT_QUIET OFF)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
  set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

if (NOT EMSCRIPTEN)
  add_subdirectory(cart)
endif()

add_subdirectory(host)
