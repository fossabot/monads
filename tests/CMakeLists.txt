cmake_minimum_required( VERSION 3.14...3.17 FATAL_ERROR )

CPMAddPackage(
    NAME doctest
    VERSION 2.4.0
    GITHUB_REPOSITORY onqtam/doctest
    GIT_TAG 2.4.0
)

add_executable(monads_test)

set_target_properties(monads_test PROPERTIES CXX_EXTENSIONS OFF)

target_compile_features(monads_test PRIVATE cxx_std_20)

target_compile_options(monads_test
    PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:DEBUG>>:--coverage -o0 -g -Wall -Wextra -Werror -fsanitize=address>
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:RELEASE>>:-o3>

        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:--coverage -o0 -g -Wall -Wextra -Werror -fsanitize=address -fsanitize=undefined>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:RELEASE>>:-o3>)

target_link_libraries(monads_test
    PUBLIC
        monads::monads 
        doctest::doctest
        --coverage
    PRIVATE
        $<$<AND:$<CXX_COMPILER_ID:Clang>,$<CONFIG:DEBUG>>:-lasan>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-lasan>
        $<$<AND:$<CXX_COMPILER_ID:GNU>,$<CONFIG:DEBUG>>:-lubsan>)

target_sources(monads_test
    PRIVATE
        monads/main.cpp
)

add_test( NAME monads_test COMMAND monads_test )

