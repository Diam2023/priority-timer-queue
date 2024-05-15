cmake_minimum_required(VERSION 3.22)

project(libmono VERSION 2.1)
add_library(${PROJECT_NAME} INTERFACE)

target_compile_definitions(${PROJECT_NAME} INTERFACE
    # MONO_USE_FULL_PTQ_MEMBER
    # MONO_USE_FULL_PTN_MEMBER
    # MONO_PTQ_DEBUG
)

target_include_directories(${PROJECT_NAME} INTERFACE
    ./inc
)

target_sources(${PROJECT_NAME} INTERFACE
	./src/priority_timer_queue.c
)

# add_executable(PriorityTimerQueue ./test_for_c_main.c ${SRCS})

target_link_directories(${PROJECT_NAME} INTERFACE
)

target_link_libraries(${PROJECT_NAME} INTERFACE
)

add_compile_definitions (
)

# Validate that libapp code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()

# set(CPACK_PROJECT_NAME ${PROJECT_NAME})
# set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
# include(CPack)

