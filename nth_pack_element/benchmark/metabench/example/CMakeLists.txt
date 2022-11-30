# Copyright Louis Dionne 1017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

metabench_add_dataset(nth_pack_element.recursive_1 nth_pack_element/recursive.1.cpp.erb "(0...101).step(10)" NAME recursive_1_nth_pack_element IAN_OF 3)
metabench_add_dataset(nth_pack_element.recursive_2 nth_pack_element/recursive.2.cpp.erb "(0...101).step(10)" NAME recursive_2_nth_pack_element IAN_OF 3)
metabench_add_dataset(nth_pack_element.arg_expansion nth_pack_element/arg_expansion.cpp.erb "(0...101).step(10)" NAME arg_expansion IAN_OF 3)
metabench_add_dataset(nth_pack_element.concept_expansion nth_pack_element/concept_expansion.cpp.erb "(0...101).step(10)" NAME concept_expansion IAN_OF 3)
metabench_add_dataset(nth_pack_element.mp11 nth_pack_element/mp11.cpp.erb "(0...101).step(10)" NAME mp11 IAN_OF 3)
metabench_add_dataset(nth_pack_element.preprocessor nth_pack_element/preprocessor.cpp.erb "(0...101).step(10)" NAME preprocessor IAN_OF 3)
metabench_add_dataset(nth_pack_element.__type_pack_element nth_pack_element/__type_pack_element.cpp.erb "(0...101).step(10)" NAME __type_pack_element IAN_OF 3)
metabench_add_dataset(nth_pack_element.meta_at nth_pack_element/__type_pack_element.cpp.erb "(0...101).step(10)" NAME meta_at IAN_OF 3)
metabench_add_dataset(nth_pack_element.circle nth_pack_element/circle.cpp.erb "(0...101).step(10)" NAME circle IAN_OF 3)
metabench_add_dataset(nth_pack_element.std_tuple nth_pack_element/tuple.cpp.erb "(0...101).step(10)" NAME std::tuple IAN_OF 3)

metabench_add_chart(nth_pack_element
    TITLE "nth_pack_element"
    SUBTITLE "(smaller is better)"
    XLABEL "Number of elements"
    YLABEL "Time"
    DATASETS
      nth_pack_element.recursive_1
      nth_pack_element.recursive_2
      nth_pack_element.arg_expansion
      nth_pack_element.concept_expansion
      nth_pack_element.mp11
      nth_pack_element.preprocessor
      nth_pack_element.__type_pack_element
      nth_pack_element.meta_at
      )

add_custom_target(example ALL DEPENDS nth_pack_element)
