enable_testing()

message(STATUS "Running tasks/bmstu_unordered_map/CMakeLists.txt")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
get_filename_component(NAME_EXECUTABLE ${CMAKE_CURRENT_SOURCE_DIR} NAME)

file(GLOB_RECURSE SOURCES 
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp
)

message(STATUS "SOURCES: ${SOURCES}")

add_executable(${NAME_EXECUTABLE} ${SOURCES})

target_include_directories(${NAME_EXECUTABLE} PUBLIC 
    ${PROJECT_SOURCE_DIR}/tasks/bmstu_abstract_iterator/task_abstract_iterator
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_include_directories(${NAME_EXECUTABLE} PRIVATE 
    ${CMAKE_SOURCE_DIR}/fast-streebog/fast-streebog-lib/include
)

# Линкуем с Google Test
target_link_libraries(
        ${NAME_EXECUTABLE}
        GTest::gtest_main
        fast-streebog-lib
)
include(GoogleTest)
gtest_discover_tests(${NAME_EXECUTABLE})