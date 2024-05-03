cmake_minimum_required(VERSION 3.22)
project(libmono VERSION 1.3)

target_compile_definitions(libmono INTERFACE
    # MONO_USE_FULL_PTQ_MEMBER
    # MONO_USE_FULL_PTN_MEMBER
    # MONO_PTQ_DEBUG
)

target_include_directories(libmono INTERFACE
    ./inc
)

target_sources(libmono INTERFACE
	./src/priority_timer_queue.c
)

# add_executable(PriorityTimerQueue ./test_for_c_main.c ${SRCS})

target_link_directories(libmono INTERFACE
)

target_link_libraries(libmono INTERFACE
)

add_compile_definitions (
)

# Validate that libapp code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

