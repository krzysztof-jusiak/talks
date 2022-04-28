# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that we can set properties on individual dataset targets.
# The test also makes sure that the properties are not leaked across targets.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

metabench_add_dataset(dummy1 dummy1.cpp.erb "[1, 2, 3]")
metabench_add_dataset(dummy2 dummy2.cpp.erb "[1, 2, 3]")
metabench_add_chart(check DATASETS dummy1 dummy2)

target_compile_definitions(dummy1 PUBLIC -DCUSTOM_DEFINITION_DUMMY1=1234)
target_compile_definitions(dummy2 PUBLIC -DCUSTOM_DEFINITION_DUMMY2=5678)
