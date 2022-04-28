# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that metabench_add_chart can be used with arguments
# like TITLE, SUBTITLE, XLABEL and YLABEL to customize the look of the chart.
# The test does not actually check anything; this must be done by manual
# inspection.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

metabench_add_dataset(dummy "dummy.cpp.erb" "[1, 2, 3]")
metabench_add_chart(check
    TITLE "my fancy title"
    SUBTITLE "my fancy subtitle"
    XLABEL "my fancy X label"
    YLABEL "my fancy Y label"
    DATASETS dummy
)
