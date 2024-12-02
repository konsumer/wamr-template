cmake_minimum_required(VERSION 3.5)
project(null0)
set(CMAKE_C_STANDARD 11)

include(FetchContent)
# set(FETCHCONTENT_QUIET OFF)

if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
  set(CMAKE_POLICY_DEFAULT_CMP0135 NEW)
endif()

if(POLICY CMP0077)
  cmake_policy(SET CMP0077 NEW)
  set(CMAKE_POLICY_DEFAULT_CMP0077 NEW)
endif()

# enable colors for ninja
set(CMAKE_COLOR_DIAGNOSTICS ON)

# set(CMAKE_BUILD_TYPE Debug)

# everyone wants to make an "uninstall" target
set_property(GLOBAL PROPERTY ALLOW_DUPLICATE_CUSTOM_TARGETS 1)

set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/tools/cmake")

add_subdirectory(cart)
add_subdirectory(host)
