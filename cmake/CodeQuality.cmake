  find_program(CLANG_FORMAT_EXE NAMES clang-format)
  find_program(CLANG_TIDY_EXE NAMES clang-tidy)

  file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/*.cpp ${CMAKE_SOURCE_DIR}/src/*.hpp)

  if(CLANG_FORMAT_EXE)
      add_custom_target(format
              COMMAND ${CLANG_FORMAT_EXE} -i ${ALL_SOURCE_FILES}
              COMMENT "Formatting sources")
      add_custom_target(format-check
              COMMAND ${CLANG_FORMAT_EXE} --dry-run --Werror ${ALL_SOURCE_FILES}
              COMMENT "Checking formatting")
  endif()

  if(CLANG_TIDY_EXE)
      add_custom_target(lint
              COMMAND ${CLANG_TIDY_EXE}  --config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -p ${CMAKE_BINARY_DIR} ${ALL_SOURCE_FILES}
              COMMENT "Running clang-tidy")
  endif()