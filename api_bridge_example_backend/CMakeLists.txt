project(hebench_example_backend)

# versioning
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/include/ex_version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/ex_version.h.in1")
configure_file("${CMAKE_CURRENT_BINARY_DIR}/ex_version.h.in1" "${CMAKE_CURRENT_SOURCE_DIR}/include/ex_version.h")

set(be_example_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ex_engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/ex_benchmark.cpp"
    )
set(be_example_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ex_engine.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ex_benchmark.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/ex_version.h"
    )

add_library(${PROJECT_NAME} SHARED ${be_example_SOURCES} ${be_example_HEADERS})

MACRO (TARGET_LINK_LIBRARIES_WHOLE_ARCHIVE target vis)
  IF (WIN32)
    FOREACH (arg IN LISTS ARGN)
      SET_TARGET_PROPERTIES(
        ${target} PROPERTIES LINK_FLAGS "/WHOLEARCHIVE:${lib}"
      )
    ENDFOREACH ()
  ELSE ()
    IF (APPLE)
      SET(LINK_FLAGS "-Wl,-all_load")
      SET(UNDO_FLAGS "-Wl,-noall_load")
    ELSE ()
      SET(LINK_FLAGS "-Wl,--whole-archive")
      SET(UNDO_FLAGS "-Wl,--no-whole-archive")
    ENDIF ()
    TARGET_LINK_LIBRARIES(${target} ${vis} ${LINK_FLAGS} ${ARGN} ${UNDO_FLAGS})
  ENDIF ()
ENDMACRO ()

TARGET_LINK_LIBRARIES_WHOLE_ARCHIVE(${PROJECT_NAME} PUBLIC hebench_cpp)

target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
