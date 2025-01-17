//  Copyright (c) 2021 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/modules/iterator_support.hpp>

#include <iterator>
#include <type_traits>

int main()
{
    // convert std tags to traversal tags
    static_assert(std::is_same<pika::traits::iterator_category_to_traversal_t<
                                   std::random_access_iterator_tag>,
                      pika::random_access_traversal_tag>::value,
        "std::random_access_iterator_tag == pika::random_access_traversal_tag");
    static_assert(std::is_same<pika::traits::iterator_category_to_traversal_t<
                                   std::bidirectional_iterator_tag>,
                      pika::bidirectional_traversal_tag>::value,
        "std::bidirectional_iterator_tag == pika::bidirectional_traversal_tag");
    static_assert(std::is_same<pika::traits::iterator_category_to_traversal_t<
                                   std::forward_iterator_tag>,
                      pika::forward_traversal_tag>::value,
        "std::forward_iterator_tag == pika::forward_traversal_tag");
    static_assert(std::is_same<pika::traits::iterator_category_to_traversal_t<
                                   std::output_iterator_tag>,
                      pika::incrementable_traversal_tag>::value,
        "std::output_iterator_tag == pika::incrementable_traversal_tag");
    static_assert(std::is_same<pika::traits::iterator_category_to_traversal_t<
                                   std::input_iterator_tag>,
                      pika::single_pass_traversal_tag>::value,
        "std::input_iterator_tag == pika::single_pass_traversal_tag");
}
