#remove_json_comments: customization.json
#cpp -xc++ $< | sed -e '/^#/d' > customization_no_comments.json
#python3 -m json.tool customization_no_comments.json > /dev/null
#
#process_grades.out: \
#${RAINBOW_GRADES_DIRECTORY}/main.cpp \
#${RAINBOW_GRADES_DIRECTORY}/output.cpp \
#${RAINBOW_GRADES_DIRECTORY}/table.cpp \
#${RAINBOW_GRADES_DIRECTORY}/student.cpp \
#${RAINBOW_GRADES_DIRECTORY}/zone.cpp \
#${RAINBOW_GRADES_DIRECTORY}/benchmark.cpp \
#${RAINBOW_GRADES_DIRECTORY}/submini_polls.cpp
#clang++ -Wall -Wextra ${flags} ${memory_flags} ${json_include} $^ -g -o $@
#
cmake_minimum_required(VERSION 3.16)
project(process_grades LANGUAGES CXX)

CMAKE_POLICY(SET CMP0135 NEW)
include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz)
FetchContent_MakeAvailable(json)

add_library(rainbow_grades
            output.cpp
            table.cpp
            student.cpp
            zone.cpp
            benchmark.cpp
            submini_polls.cpp
            grade.cpp
            gradeable.cpp
            section.cpp)
target_link_libraries(rainbow_grades PRIVATE nlohmann_json::nlohmann_json)
target_include_directories(rainbow_grades PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_features(rainbow_grades PUBLIC cxx_std_17)

add_executable(process_grades.out main.cpp )
target_link_libraries(process_grades.out PRIVATE nlohmann_json::nlohmann_json rainbow_grades)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()
