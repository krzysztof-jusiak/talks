# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that we generate a HTML file containing the chart in
# the current CMake binary directory, and that the name of the file is the
# target passed to `metabench_add_chart`.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

metabench_add_dataset(dummy dummy.cpp.erb "[1, 2, 3]")
metabench_add_chart(check DATASETS dummy)

enable_testing()

add_test(NAME generate_chart
    COMMAND ${CMAKE_COMMAND} --build ${PROJECT_BINARY_DIR} --target check)

add_test(NAME check_generated_chart
    COMMAND ${RUBY_EXECUTABLE}
        -e "html = '${CMAKE_CURRENT_BINARY_DIR}/check.html'"
        -e "raise \"file #{html} is missing\" if !File.exist?(html)"
)

set_tests_properties(check_generated_chart PROPERTIES DEPENDS generate_chart)
