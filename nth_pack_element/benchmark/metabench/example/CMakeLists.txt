# Copyright Louis Dionne 2017
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE.md or copy at http://boost.org/LICENSE_1_0.txt)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

metabench_add_dataset(nth_pack_element.recursive_1 nth_pack_element/recursive.1.cpp.erb "(0...201).step(10)" NAME recursive_1_nth_pack_element IAN_OF 3)
metabench_add_dataset(nth_pack_element.recursive_2 nth_pack_element/recursive.2.cpp.erb "(0...201).step(10)" NAME recursive_2_nth_pack_element IAN_OF 3)
metabench_add_dataset(nth_pack_element.arg_expansion nth_pack_element/arg_expansion.cpp.erb "(0...201).step(10)" NAME arg_expansion IAN_OF 3)
metabench_add_dataset(nth_pack_element.concept_expansion nth_pack_element/concept_expansion.cpp.erb "(0...201).step(10)" NAME concept_expansion IAN_OF 3)
metabench_add_dataset(nth_pack_element.mp11 nth_pack_element/mp11.cpp.erb "(0...201).step(10)" NAME mp11 IAN_OF 3)
metabench_add_dataset(nth_pack_element.preprocessor nth_pack_element/preprocessor.cpp.erb "(0...201).step(10)" NAME preprocessor IAN_OF 3)
metabench_add_dataset(nth_pack_element.__type_pack_element nth_pack_element/__type_pack_element.cpp.erb "(0...201).step(10)" NAME __type_pack_element IAN_OF 3)
metabench_add_dataset(nth_pack_element.circle nth_pack_element/circle.cpp.erb "(0...201).step(10)" NAME circle IAN_OF 3)
metabench_add_dataset(nth_pack_element.std_tuple nth_pack_element/tuple.cpp.erb "(0...201).step(10)" NAME std::tuple IAN_OF 3)

metabench_add_dataset(tuple.recursive_1 tuple/recursive.1.cpp.erb "(0...201).step(10)" NAME recursive_1_nth_pack_element IAN_OF 3)
metabench_add_dataset(tuple.recursive_2 tuple/recursive.2.cpp.erb "(0...201).step(10)" NAME recursive_2_nth_pack_element IAN_OF 3)
metabench_add_dataset(tuple.arg_expansion tuple/arg_expansion.cpp.erb "(0...201).step(10)" NAME arg_expansion IAN_OF 3)
metabench_add_dataset(tuple.concept_expansion tuple/concept_expansion.cpp.erb "(0...201).step(10)" NAME concept_expansion IAN_OF 3)
metabench_add_dataset(tuple.mp11 tuple/mp11.cpp.erb "(0...201).step(10)" NAME mp11 IAN_OF 3)
metabench_add_dataset(tuple.preprocessor tuple/preprocessor.cpp.erb "(0...201).step(10)" NAME preprocessor IAN_OF 3)
metabench_add_dataset(tuple.__type_pack_element tuple/__type_pack_element.cpp.erb "(0...201).step(10)" NAME __type_pack_element IAN_OF 3)
metabench_add_dataset(tuple.circle tuple/circle.cpp.erb "(0...201).step(10)" NAME circle IAN_OF 3)
metabench_add_dataset(tuple.std_tuple tuple/tuple.cpp.erb "(0...201).step(10)" NAME std::tuple IAN_OF 3)

# circle
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
      nth_pack_element.circle)

metabench_add_chart(tuple
    TITLE "tuple.get"
    SUBTITLE "(smaller is better)"
    XLABEL "Number of elements"
    YLABEL "Time"
    DATASETS
      tuple.recursive_1
      tuple.recursive_2
      tuple.arg_expansion
      tuple.concept_expansion
      tuple.mp11
      tuple.preprocessor
      tuple.circle
      tuple.std_tuple)

# gcc
#metabench_add_chart(nth_pack_element
    #TITLE "nth_pack_element"
    #SUBTITLE "(smaller is better)"
    #XLABEL "Number of elements"
    #YLABEL "Time"
    #DATASETS
      #nth_pack_element.recursive_1
      #nth_pack_element.recursive_2
      #nth_pack_element.arg_expansion
      #nth_pack_element.mp11
      #nth_pack_element.preprocessor)

#metabench_add_chart(tuple
    #TITLE "tuple.get"
    #SUBTITLE "(smaller is better)"
    #XLABEL "Number of elements"
    #YLABEL "Time"
    #DATASETS
      #tuple.recursive_1
      #tuple.recursive_2
      #tuple.arg_expansion
      #tuple.mp11
      #tuple.preprocessor
      #tuple.std_tuple)

# clang
#metabench_add_chart(nth_pack_element
    #TITLE "nth_pack_element"
    #SUBTITLE "(smaller is better)"
    #XLABEL "Number of elements"
    #YLABEL "Time"
    #DATASETS
      #nth_pack_element.recursive_1
      #nth_pack_element.recursive_2
      #nth_pack_element.arg_expansion
      #nth_pack_element.concept_expansion
      #nth_pack_element.mp11
      #nth_pack_element.preprocessor
      #nth_pack_element.__type_pack_element
      #)

#metabench_add_chart(tuple
    #TITLE "tuple.get"
    #SUBTITLE "(smaller is better)"
    #XLABEL "Number of elements"
    #YLABEL "Time"
    #DATASETS
      #tuple.recursive_1
      #tuple.recursive_2
      #tuple.arg_expansion
      #tuple.concept_expansion
      #tuple.mp11
      #tuple.preprocessor
      #tuple.__type_pack_element
      #tuple.std_tuple)

add_custom_target(example ALL DEPENDS nth_pack_element tuple)
