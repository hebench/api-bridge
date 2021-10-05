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

target_link_libraries(${PROJECT_NAME} PUBLIC "-Wl,--whole-archive" hebench_cpp "-Wl,--no-whole-archive")
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

install(TARGETS ${PROJECT_NAME} DESTINATION lib)
