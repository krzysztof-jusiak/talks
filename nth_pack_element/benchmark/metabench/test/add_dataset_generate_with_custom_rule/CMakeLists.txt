# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that we can trigger the generation of a dataset from a
# custom target by making the custom target dependent on the JSON file of the
# dataset.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)


metabench_add_dataset(dummy dummy.cpp.erb "[1, 2, 3]")
add_custom_target(check
    COMMAND ${RUBY_EXECUTABLE}
        -e "json = '${CMAKE_CURRENT_BINARY_DIR}/dummy.json'"
        -e "raise \"file #{json} is missing\" if !File.exist?(json)"
    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/dummy.json
    VERBATIM
)
