# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that properties set on a dataset target are
# also set for the corresponding CTest target created by Metabench.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

enable_testing()
metabench_add_dataset(dummy dummy.cpp.erb "[1, 2, 3]")
metabench_add_chart(check DATASETS dummy)

target_compile_definitions(dummy PUBLIC -DCUSTOM_DEFINITION)
