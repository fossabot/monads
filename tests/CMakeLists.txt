cmake_minimum_required( VERSION 3.14...3.17 FATAL_ERROR )

CPMAddPackage(
    NAME gtest
    VERSION 1.10.0
    GITHUB_REPOSITORY google/googletest
    GIT_TAG release-1.10.0)

add_executable(monads_test)

set_target_properties(monads_test PROPERTIES CXX_EXTENSIONS OFF)

target_compile_features(monads_test PRIVATE cxx_std_20)

target_compile_options(monads_test
    PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:DEBUG>>:-o0 -g>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RELEASE>>:-o3>

        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-o0 -g -fsanitize=address -fsanitize=undefined>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-o3>)

target_link_libraries(monads_test
    PUBLIC
        monads::monads 
        gtest
    PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-lasan>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-lubsan>)

target_sources(monads_test
    PRIVATE
        monads/main.cpp
)

add_test( NAME monads_test COMMAND monads_test )
