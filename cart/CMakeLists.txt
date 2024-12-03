# build for carts

cmake_minimum_required (VERSION 3.18)

if(NOT WASI_SDK_PREFIX)
    set(WASI_SDK_PREFIX /opt/wasi-sdk)
endif()

set(CMAKE_SYSTEM_NAME WASI)
set(CMAKE_SYSTEM_VERSION 1)
set(CMAKE_SYSTEM_PROCESSOR wasm32)
set(triple wasm32-wasi)
set(CMAKE_C_COMPILER ${WASI_SDK_PREFIX}/bin/clang${WASI_HOST_EXE_SUFFIX})
set(CMAKE_CXX_COMPILER ${WASI_SDK_PREFIX}/bin/clang++${WASI_HOST_EXE_SUFFIX})
set(CMAKE_ASM_COMPILER ${WASI_SDK_PREFIX}/bin/clang${WASI_HOST_EXE_SUFFIX})
set(CMAKE_AR ${WASI_SDK_PREFIX}/bin/llvm-ar${WASI_HOST_EXE_SUFFIX})
set(CMAKE_RANLIB ${WASI_SDK_PREFIX}/bin/llvm-ranlib${WASI_HOST_EXE_SUFFIX})
set(CMAKE_C_COMPILER_TARGET ${triple})
set(CMAKE_CXX_COMPILER_TARGET ${triple})
set(CMAKE_ASM_COMPILER_TARGET ${triple})
set(HAVE_FLAG_SEARCH_PATHS_FIRST 0)
set(CMAKE_C_LINK_FLAGS "")
set(CMAKE_CXX_LINK_FLAGS "")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
set(CMAKE_EXECUTABLE_SUFFIX ".wasm")

function(BUILD_C_CART name)
  add_executable(${name} c/${name}/main.c)
  target_include_directories(${name} PUBLIC c)
  add_custom_command(
    TARGET ${name}
    POST_BUILD
    COMMAND "${PROJECT_SOURCE_DIR}/tools/buildcart.sh"
    ARGS
      "${PROJECT_SOURCE_DIR}/docs/carts/${name}.null0"
      "${CMAKE_CURRENT_BINARY_DIR}/${name}.wasm"
      "${CMAKE_CURRENT_SOURCE_DIR}/c/${name}"
  )
endfunction()

function(BUILD_AS_CART name)
  add_custom_target(
    as_${name}
    DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assemblyscript/${name}/main.ts" "${CMAKE_CURRENT_SOURCE_DIR}/assemblyscript/cart.ts"
    COMMAND npx asc "${CMAKE_CURRENT_SOURCE_DIR}/assemblyscript/${name}/main.ts" -o "${CMAKE_CURRENT_BINARY_DIR}/as_${name}.wasm" --exportStart --lib "${CMAKE_CURRENT_SOURCE_DIR}/assemblyscript/cart.ts" --use abort=_cart_abort --use trace=_cart_trace --memoryBase 0 --initialMemory 1 --maximumMemory 32 --runtime minimal
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/as_${name}.wasm"
  )
  add_custom_command(
    TARGET as_${name}
    POST_BUILD
    COMMAND "${PROJECT_SOURCE_DIR}/tools/buildcart.sh"
    ARGS
      "${PROJECT_SOURCE_DIR}/docs/carts/as_${name}.null0"
      "${CMAKE_CURRENT_BINARY_DIR}/as_${name}.wasm"
      "${CMAKE_CURRENT_SOURCE_DIR}/assemblyscript/${name}"
  )
endfunction()


BUILD_C_CART(runtests)
BUILD_AS_CART(runtests)
