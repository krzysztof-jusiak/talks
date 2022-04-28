# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that we can use the same template to create different
# datasets. Thus, we can't use the name of the template to differentiate
# between datasets; we really must use the name of the dataset target.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

metabench_add_dataset(dummy1 dummy.cpp.erb "[0, 1, 2]")
target_compile_definitions(dummy1 PUBLIC -DDUMMY1)

metabench_add_dataset(dummy2 dummy.cpp.erb "[3, 4, 5]")
target_compile_definitions(dummy2 PUBLIC -DDUMMY2)

metabench_add_chart(check DATASETS dummy1 dummy2)
