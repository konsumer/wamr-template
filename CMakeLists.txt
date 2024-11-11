cmake_minimum_required(VERSION 3.5)
project(null0)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
# set(FETCHCONTENT_QUIET OFF)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

add_subdirectory(cart)
add_subdirectory(host)
