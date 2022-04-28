# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that providing the ALL keyword to `metabench_add_chart`
# causes the benchmark target to be added to the default target.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")

add_custom_target(check) # dummy 'check' target run by tools/run_tests.sh

include(metabench)
metabench_add_dataset(dummy a_template_file_with_a_very_unique_name.cpp.erb "[1, 2, 3]")
metabench_add_chart(my_benchmark ALL DATASETS dummy)

enable_testing()

# If the benchmark is generated when we build the default target, the name of
# the ERB template we're benchmarking should appear somewhere in the output.
add_test(NAME ensure_ALL_contains_the_benchmark
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_CURRENT_BINARY_DIR}
)
set_tests_properties(ensure_ALL_contains_the_benchmark
    PROPERTIES PASS_REGULAR_EXPRESSION "a_template_file_with_a_very_unique_name"
)
