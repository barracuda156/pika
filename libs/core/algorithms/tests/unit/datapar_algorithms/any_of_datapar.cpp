//  Copyright (c) 2021 Srinivas Yadav
//  Copyright (c) 2014-2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <hpx/include/datapar.hpp>
#include <hpx/local/init.hpp>

#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

#include "../algorithms/any_of_tests.hpp"

////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_any_of()
{
    struct proj
    {
        //This projection should cause tests to fail if it is not applied
        //because it causes predicate to evaluate the opposite
        constexpr std::size_t operator()(std::size_t x) const
        {
            return !static_cast<bool>(x);
        }
    };
    using namespace hpx::execution;

    test_any_of(simd, IteratorTag());
    test_any_of(par_simd, IteratorTag());

    test_any_of_async(simd(task), IteratorTag());
    test_any_of_async(par_simd(task), IteratorTag());
}

////////////////////////////////////////////////////////////////////////////
void any_of_test()
{
    test_any_of<std::random_access_iterator_tag>();
    test_any_of<std::forward_iterator_tag>();
}

template <typename IteratorTag>
void test_any_of_exception()
{
    using namespace hpx::execution;

    test_any_of_exception(IteratorTag());

    // If the execution policy object is of type vector_execution_policy,
    // std::terminate shall be called. therefore we do not test exceptions
    // with a vector execution policy
    test_any_of_exception(simd, IteratorTag());
    test_any_of_exception(par_simd, IteratorTag());

    test_any_of_exception_async(simd(task), IteratorTag());
    test_any_of_exception_async(par_simd(task), IteratorTag());
}

////////////////////////////////////////////////////////////////////////////
void any_of_exception_test()
{
    test_any_of_exception<std::random_access_iterator_tag>();
    test_any_of_exception<std::forward_iterator_tag>();
}

template <typename IteratorTag>
void test_any_of_bad_alloc()
{
    using namespace hpx::execution;

    // If the execution policy object is of type vector_execution_policy,
    // std::terminate shall be called. therefore we do not test exceptions
    // with a vector execution policy
    test_any_of_bad_alloc(simd, IteratorTag());
    test_any_of_bad_alloc(par_simd, IteratorTag());

    test_any_of_bad_alloc_async(simd(task), IteratorTag());
    test_any_of_bad_alloc_async(par_simd(task), IteratorTag());
}

void any_of_bad_alloc_test()
{
    test_any_of_bad_alloc<std::random_access_iterator_tag>();
    test_any_of_bad_alloc<std::forward_iterator_tag>();
}

///////////////////////////////////////////////////////////////////////////////
int hpx_main()
{
    any_of_test();
    any_of_exception_test();
    any_of_bad_alloc_test();
    return hpx::local::finalize();
}

int main(int argc, char* argv[])
{
    // add command line option which controls the random number generator seed
    using namespace hpx::program_options;
    options_description desc_commandline(
        "Usage: " HPX_APPLICATION_STRING " [options]");

    // By default this test should run on all available cores
    std::vector<std::string> const cfg = {"hpx.os_threads=all"};

    // Initialize and run HPX
    hpx::local::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    HPX_TEST_EQ_MSG(hpx::local::init(hpx_main, argc, argv, init_args), 0,
        "HPX main exited with non-zero status");

    return hpx::util::report_errors();
}
