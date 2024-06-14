cmake_minimum_required(VERSION 3.16.3)

project(libmono VERSION 2.1)
add_library(${PROJECT_NAME} INTERFACE)

add_subdirectory(test)

target_compile_definitions(${PROJECT_NAME} INTERFACE
)

target_include_directories(${PROJECT_NAME} INTERFACE
    ./inc
)

target_sources(${PROJECT_NAME} INTERFACE
	./src/priority_timer.c
)

target_link_directories(${PROJECT_NAME} INTERFACE
)

target_link_libraries(${PROJECT_NAME} INTERFACE
)

add_compile_definitions (
)

if(DEFINED LIB_MONO_TEST)
    message(INFO "Build Test")
    add_subdirectory(test)
endif()

# Validate that libmono code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

