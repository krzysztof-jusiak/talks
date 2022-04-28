# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that we can use an absolute path to specify the ERB
# template in `metabench_add_dataset`.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

metabench_add_dataset(dummy "${CMAKE_CURRENT_SOURCE_DIR}/dummy.cpp.erb" "[1, 2, 3]")
metabench_add_chart(check DATASETS dummy)
