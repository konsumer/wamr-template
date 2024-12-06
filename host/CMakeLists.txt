find_package(PhysFS REQUIRED)

add_executable(${PROJECT_NAME})

if (EMSCRIPTEN)
  target_sources(${PROJECT_NAME} PRIVATE src/main.c src/host_emscripten.c)
  target_link_libraries(${PROJECT_NAME} physfs-static)
  target_link_options(${PROJECT_NAME} PUBLIC -Oz -sSINGLE_FILE -sUSE_WEBGL2=1 -sFORCE_FILESYSTEM -sEXTRA_EXPORTED_RUNTIME_METHODS=FS -sASYNCIFY)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs/"
    RUNTIME_OUTPUT_NAME "host"
    SUFFIX ".mjs"
  )
else()
  target_sources(${PROJECT_NAME} PRIVATE src/main.c src/host_wamr.c)
  find_package(wamr REQUIRED)
  target_link_libraries(${PROJECT_NAME} wamr physfs-static m)
endif()
