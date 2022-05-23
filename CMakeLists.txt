cmake_minimum_required(VERSION 3.5)
project(PriorityTimerQueue VERSION 1.1)

include_directories("./inc")
aux_source_directory("./src" SRCS)

add_executable(PriorityTimerQueue ./test_for_c_main.c ${SRCS})


add_compile_definitions (
    # MONO_USE_FULL_PTQ_MEMBER
    # MONO_USE_FULL_PTN_MEMBER
    # MONO_PTQ_DEBUG
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
