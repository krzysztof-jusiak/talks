# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

# This test makes sure that the MEDIAN_OF option of metabench_add_dataset can
# be used without all hell breaking loose. It would be difficult to test that
# we indeed do the correct thing, but here we at least test that using the
# option does not cause an error.

cmake_minimum_required(VERSION 3.1)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../..")
include(metabench)

# Test MEDIAN_OF with some values
metabench_add_dataset(dummy1 dummy.cpp.erb "[1, 2, 3, 4]" MEDIAN_OF 1)
metabench_add_dataset(dummy2 dummy.cpp.erb "[1, 2, 3, 4]" MEDIAN_OF 2)
metabench_add_dataset(dummy3 dummy.cpp.erb "[1, 2, 3, 4]" MEDIAN_OF 3)
metabench_add_dataset(dummy4 dummy.cpp.erb "[1, 2, 3, 4]" MEDIAN_OF 4)

# On Windows, Ninja used to error out for high values of MEDIAN_OF, so test this.
# See https://github.com/ldionne/metabench/issues/115 for details.
metabench_add_dataset(dummy5 dummy.cpp.erb "[1, 2, 3, 4]" MEDIAN_OF 15)

metabench_add_chart(check DATASETS dummy1 dummy2 dummy3 dummy4 dummy5)
