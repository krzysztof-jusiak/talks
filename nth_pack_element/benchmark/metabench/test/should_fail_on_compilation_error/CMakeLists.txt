# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that the benchmark fails when a compilation error
# happens.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

add_custom_target(check) # dummy 'check' target run by tools/run_tests.sh

enable_testing()
metabench_add_dataset(dummy dummy.cpp.erb "[1, 2, 3]")
metabench_add_chart(my_benchmark DATASETS dummy)
set_tests_properties(dummy PROPERTIES WILL_FAIL TRUE)
