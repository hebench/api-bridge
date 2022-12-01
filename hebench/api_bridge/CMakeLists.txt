project(hebench_cpp)

# versioning
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h.in1")
configure_file("${CMAKE_CURRENT_BINARY_DIR}/version.h.in1" "${CMAKE_CURRENT_SOURCE_DIR}/version.h")

# project files
set(${PROJECT_NAME}_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/benchmark.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/engine.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/error_handling.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/utilities.cpp"
    )
set(${PROJECT_NAME}_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/benchmark.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/engine.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/engine_object.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/error_handling.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/hebench.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/utilities.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/cpp/workload_params.hpp"
    )

# target
add_library(${PROJECT_NAME} STATIC ${${PROJECT_NAME}_SOURCES} ${${PROJECT_NAME}_HEADERS})

target_link_libraries(${PROJECT_NAME} PUBLIC api_bridge)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

target_compile_options(${PROJECT_NAME} PRIVATE -fPIC -Wall -Wextra)

# installation steps
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
install(FILES ${${PROJECT_NAME}_HEADERS} DESTINATION include/hebench/${CMAKE_PROJECT_NAME}/cpp)
