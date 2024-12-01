add_executable(${PROJECT_NAME} src/main.c)

if (EMSCRIPTEN)
  target_link_options(${PROJECT_NAME} PUBLIC -Oz -sSINGLE_FILE -sUSE_WEBGL2=1 -sNO_FILESYSTEM=1)
  set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/docs/"
    SUFFIX ".mjs"
  )
else()
  find_package(wamr REQUIRED)
  target_link_libraries(${PROJECT_NAME} wamr)
endif()
