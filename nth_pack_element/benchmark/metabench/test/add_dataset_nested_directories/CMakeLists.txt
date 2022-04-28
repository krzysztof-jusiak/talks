# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that we can add a dataset in a nested directory,
# i.e. that the call to `metabench_add_dataset` does not need to be in
# the same directory or a direct parent of the ERB file we're adding.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

metabench_add_dataset(dummy path/to/benchmark/dummy.cpp.erb "[1, 2, 3]")
metabench_add_chart(check DATASETS dummy)
