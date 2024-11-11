find_package(wamr REQUIRED)
add_library(vmlib ${WAMR_RUNTIME_LIB_SOURCE})

add_executable(${PROJECT_NAME} src/main.c)
target_link_libraries(${PROJECT_NAME} vmlib)
