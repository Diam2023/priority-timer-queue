cmake_minimum_required(VERSION 3.16.3)

project(libmono_test VERSION 2.1)

add_executable(${PROJECT_NAME} ./test_for_c_main.c)

target_link_libraries(${PROJECT_NAME} PRIVATE libmono pthread)

target_include_directories(${PROJECT_NAME} PRIVATE ./inc)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    MONO_USE_FULL_PTQ_MEMBER
    MONO_USE_FULL_PTN_MEMBER
    MONO_PTQ_DEBUG
)

# Validate that libmono code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()
