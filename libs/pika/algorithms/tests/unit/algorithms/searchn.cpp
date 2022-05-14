//  Copyright (c) 2020 ETH Zurich
//  Copyright (c) 2014 Grant Mercer
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/init.hpp>
#include <pika/parallel/algorithms/search.hpp>
#include <pika/testing.hpp>

#include <cstddef>
#include <iostream>
#include <iterator>
#include <numeric>
#include <string>
#include <vector>

#include "test_utils.hpp"

////////////////////////////////////////////////////////////////////////////
template <typename IteratorTag>
void test_search_n1_without_expolicy(IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence in middle of vector
    c[c.size() / 2] = 1;
    c[c.size() / 2 + 1] = 2;

    std::size_t h[] = {1, 2};

    iterator index = pika::search_n(
        iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));

    base_iterator test_index = std::begin(c) + c.size() / 2;

    PIKA_TEST(index == iterator(test_index));
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n1(ExPolicy policy, IteratorTag)
{
    static_assert(pika::is_execution_policy<ExPolicy>::value,
        "pika::is_execution_policy<ExPolicy>::value");

    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence in middle of vector
    c[c.size() / 2] = 1;
    c[c.size() / 2 + 1] = 2;

    std::size_t h[] = {1, 2};

    iterator index = pika::search_n(
        policy, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));

    base_iterator test_index = std::begin(c) + c.size() / 2;

    PIKA_TEST(index == iterator(test_index));
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n1_async(ExPolicy p, IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence in middle of vector
    c[c.size() / 2] = 1;
    c[c.size() / 2 + 1] = 2;

    std::size_t h[] = {1, 2};

    pika::future<iterator> f = pika::search_n(
        p, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));
    f.wait();

    // create iterator at position of value to be found
    base_iterator test_index = std::begin(c) + c.size() / 2;

    PIKA_TEST(f.get() == iterator(test_index));
}

template <typename IteratorTag>
void test_search_n1()
{
    using namespace pika::execution;
    test_search_n1_without_expolicy(IteratorTag());

    test_search_n1(seq, IteratorTag());
    test_search_n1(par, IteratorTag());
    test_search_n1(par_unseq, IteratorTag());

    test_search_n1_async(seq(task), IteratorTag());
    test_search_n1_async(par(task), IteratorTag());
}

void search_n_test1()
{
    test_search_n1<std::random_access_iterator_tag>();
    test_search_n1<std::forward_iterator_tag>();
}

template <typename IteratorTag>
void test_search_n2_without_expolicy(IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values about 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence at start and end
    c[0] = 1;
    c[1] = 2;
    c[c.size() - 1] = 2;
    c[c.size() - 2] = 1;

    std::size_t h[] = {1, 2};

    iterator index = pika::search_n(
        iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));

    base_iterator test_index = std::begin(c);

    PIKA_TEST(index == iterator(test_index));
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n2(ExPolicy policy, IteratorTag)
{
    static_assert(pika::is_execution_policy<ExPolicy>::value,
        "pika::is_execution_policy<ExPolicy>::value");

    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values about 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence at start and end
    c[0] = 1;
    c[1] = 2;
    c[c.size() - 1] = 2;
    c[c.size() - 2] = 1;

    std::size_t h[] = {1, 2};

    iterator index = pika::search_n(
        policy, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));

    base_iterator test_index = std::begin(c);

    PIKA_TEST(index == iterator(test_index));
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n2_async(ExPolicy p, IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence at start and end
    c[0] = 1;
    c[1] = 2;
    c[c.size() - 1] = 2;
    c[c.size() - 2] = 1;

    std::size_t h[] = {1, 2};

    pika::future<iterator> f = pika::search_n(
        p, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));
    f.wait();

    // create iterator at position of value to be found
    base_iterator test_index = std::begin(c);

    PIKA_TEST(f.get() == iterator(test_index));
}

template <typename IteratorTag>
void test_search_n2()
{
    using namespace pika::execution;
    test_search_n2_without_expolicy(IteratorTag());

    test_search_n2(seq, IteratorTag());
    test_search_n2(par, IteratorTag());
    test_search_n2(par_unseq, IteratorTag());

    test_search_n2_async(seq(task), IteratorTag());
    test_search_n2_async(par(task), IteratorTag());
}

void search_n_test2()
{
    test_search_n2<std::random_access_iterator_tag>();
    test_search_n2<std::forward_iterator_tag>();
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n3(ExPolicy policy, IteratorTag)
{
    static_assert(pika::is_execution_policy<ExPolicy>::value,
        "pika::is_execution_policy<ExPolicy>::value");

    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence large enough to always be split into multiple partitions
    std::iota(std::begin(c), std::begin(c) + c.size() / 16 + 1, 1);
    std::size_t sub_size = c.size() / 16 + 1;
    std::vector<std::size_t> h(sub_size);
    std::iota(std::begin(h), std::end(h), 1);

    iterator index = pika::search_n(
        policy, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));

    base_iterator test_index = std::begin(c);

    PIKA_TEST(index == iterator(test_index));
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n3_async(ExPolicy p, IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 6
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 7);
    // create subsequence large enough to always be split into multiple partitions
    std::iota(std::begin(c), std::begin(c) + c.size() / 16 + 1, 1);
    std::size_t sub_size = c.size() / 16 + 1;
    std::vector<std::size_t> h(sub_size);
    std::iota(std::begin(h), std::end(h), 1);

    // create only two partitions, splitting the desired sub sequence into
    // separate partitions.
    pika::future<iterator> f = pika::search_n(
        p, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));
    f.wait();

    //create iterator at position of value to be found
    base_iterator test_index = std::begin(c);

    PIKA_TEST(f.get() == iterator(test_index));
}

template <typename IteratorTag>
void test_search_n3()
{
    using namespace pika::execution;
    test_search_n3(seq, IteratorTag());
    test_search_n3(par, IteratorTag());
    test_search_n3(par_unseq, IteratorTag());

    test_search_n3_async(seq(task), IteratorTag());
    test_search_n3_async(par(task), IteratorTag());
}

void search_n_test3()
{
    test_search_n3<std::random_access_iterator_tag>();
    test_search_n3<std::forward_iterator_tag>();
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n4(ExPolicy policy, IteratorTag)
{
    static_assert(pika::is_execution_policy<ExPolicy>::value,
        "pika::is_execution_policy<ExPolicy>::value");

    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence before the middle of the vector, and only run
    // search_n on half of C
    std::size_t dx = rand() % (c.size() / 2 - 2);
    c[dx] = 1;
    c[dx + 1] = 2;

    std::size_t h[] = {1, 2};

    iterator index = pika::search_n(policy, iterator(std::begin(c)),
        c.size() / 2, std::begin(h), std::end(h));

    base_iterator test_index = std::begin(c) + dx;

    PIKA_TEST(index == iterator(test_index));
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n4_async(ExPolicy p, IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence before the middle of the vector, and only run
    // search_n on half of C
    std::size_t dx = rand() % (c.size() / 2 - 2);
    c[dx] = 1;
    c[dx + 1] = 2;

    std::size_t h[] = {1, 2};

    // create only two partitions, splitting the desired sub sequence into
    // separate partitions.
    pika::future<iterator> f = pika::search_n(
        p, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h));
    f.wait();

    //create iterator at position of value to be found
    base_iterator test_index = std::begin(c) + dx;

    PIKA_TEST(f.get() == iterator(test_index));
}

template <typename IteratorTag>
void test_search_n4()
{
    using namespace pika::execution;
    test_search_n4(seq, IteratorTag());
    test_search_n4(par, IteratorTag());
    test_search_n4(par_unseq, IteratorTag());

    test_search_n4_async(seq(task), IteratorTag());
    test_search_n4_async(par(task), IteratorTag());
}

void search_n_test4()
{
    test_search_n4<std::random_access_iterator_tag>();
    test_search_n4<std::forward_iterator_tag>();
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n5(ExPolicy policy, IteratorTag)
{
    static_assert(pika::is_execution_policy<ExPolicy>::value,
        "pika::is_execution_policy<ExPolicy>::value");

    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence in middle of vector
    c[c.size() / 2] = 1;
    c[c.size() / 2 + 1] = 2;

    std::size_t h[] = {1, 2};

    auto op = [](std::size_t a, std::size_t b) { return !(a != b); };

    iterator index = pika::search_n(policy, iterator(std::begin(c)), c.size(),
        std::begin(h), std::end(h), op);

    base_iterator test_index = std::begin(c) + c.size() / 2;

    PIKA_TEST(index == iterator(test_index));
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n5_async(ExPolicy p, IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using iterator = test::test_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    // fill vector with random values above 2
    std::fill(std::begin(c), std::end(c), (std::rand() % 100) + 3);
    // create subsequence in middle of vector
    c[c.size() / 2] = 1;
    c[c.size() / 2 + 1] = 2;

    std::size_t h[] = {1, 2};

    auto op = [](std::size_t a, std::size_t b) { return !(a != b); };

    pika::future<iterator> f = pika::search_n(
        p, iterator(std::begin(c)), c.size(), std::begin(h), std::end(h), op);
    f.wait();

    // create iterator at position of value to be found
    base_iterator test_index = std::begin(c) + c.size() / 2;

    PIKA_TEST(f.get() == iterator(test_index));
}

template <typename IteratorTag>
void test_search_n5()
{
    using namespace pika::execution;
    test_search_n5(seq, IteratorTag());
    test_search_n5(par, IteratorTag());
    test_search_n5(par_unseq, IteratorTag());

    test_search_n5_async(seq(task), IteratorTag());
    test_search_n5_async(par(task), IteratorTag());
}

void search_n_test5()
{
    test_search_n5<std::random_access_iterator_tag>();
    test_search_n5<std::forward_iterator_tag>();
}

///////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_search_n_exception(ExPolicy policy, IteratorTag)
{
    static_assert(pika::is_execution_policy<ExPolicy>::value,
        "pika::is_execution_policy<ExPolicy>::value");

    using base_iterator = std::vector<std::size_t>::iterator;
    using decorated_iterator =
        test::decorated_iterator<base_iterator, IteratorTag>;
    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), std::rand() + 1);
    c[c.size() / 2] = 1;
    c[c.size() / 2 + 1] = 2;

    std::vector<std::size_t> h;
    h.push_back(1);
    h.push_back(2);

    bool caught_exception = false;
    try
    {
        pika::search_n(policy,
            decorated_iterator(
                std::begin(c), []() { throw std::runtime_error("test"); }),
            c.size(), std::begin(h), std::end(h));
        PIKA_TEST(false);
    }
    catch (pika::exception_list const& e)
    {
        caught_exception = true;
        test::test_num_exceptions<ExPolicy, IteratorTag>::call(policy, e);
    }
    catch (...)
    {
        PIKA_TEST(false);
    }

    PIKA_TEST(caught_exception);
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n_async_exception(ExPolicy p, IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using decorated_iterator =
        test::decorated_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), std::rand() + 1);
    c[c.size() / 2] = 1;
    c[c.size() / 2 + 1] = 2;

    std::size_t h[] = {1, 2};

    bool caught_exception = false;
    try
    {
        pika::future<decorated_iterator> f = pika::search_n(p,
            decorated_iterator(
                std::begin(c), []() { throw std::runtime_error("test"); }),
            c.size(), std::begin(h), std::end(h));
        f.get();

        PIKA_TEST(false);
    }
    catch (pika::exception_list const& e)
    {
        caught_exception = true;
        test::test_num_exceptions<ExPolicy, IteratorTag>::call(p, e);
    }
    catch (...)
    {
        PIKA_TEST(false);
    }

    PIKA_TEST(caught_exception);
}

template <typename IteratorTag>
void test_search_n_exception()
{
    using namespace pika::execution;
    //If the execution policy object is of type vector_execution_policy,
    //  std::terminate shall be called. therefore we do not test exceptions
    //  with a vector execution policy
    test_search_n_exception(seq, IteratorTag());
    test_search_n_exception(par, IteratorTag());

    test_search_n_async_exception(seq(task), IteratorTag());
    test_search_n_async_exception(par(task), IteratorTag());
}

void search_n_exception_test()
{
    test_search_n_exception<std::random_access_iterator_tag>();
    test_search_n_exception<std::forward_iterator_tag>();
}

//////////////////////////////////////////////////////////////////////////////
template <typename ExPolicy, typename IteratorTag>
void test_search_n_bad_alloc(ExPolicy policy, IteratorTag)
{
    static_assert(pika::is_execution_policy<ExPolicy>::value,
        "pika::is_execution_policy<ExPolicy>::value");

    using base_iterator = std::vector<std::size_t>::iterator;
    using decorated_iterator =
        test::decorated_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(100007);
    std::iota(std::begin(c), std::end(c), std::rand() + 1);
    c[c.size() / 2] = 0;

    std::size_t h[] = {1, 2};

    bool caught_bad_alloc = false;
    try
    {
        pika::search_n(policy,
            decorated_iterator(std::begin(c), []() { throw std::bad_alloc(); }),
            c.size(), std::begin(h), std::end(h));
        PIKA_TEST(false);
    }
    catch (std::bad_alloc const&)
    {
        caught_bad_alloc = true;
    }
    catch (...)
    {
        PIKA_TEST(false);
    }

    PIKA_TEST(caught_bad_alloc);
}

template <typename ExPolicy, typename IteratorTag>
void test_search_n_async_bad_alloc(ExPolicy p, IteratorTag)
{
    using base_iterator = std::vector<std::size_t>::iterator;
    using decorated_iterator =
        test::decorated_iterator<base_iterator, IteratorTag>;

    std::vector<std::size_t> c(10007);
    std::iota(std::begin(c), std::end(c), std::rand() + 1);
    c[c.size() / 2] = 0;

    std::size_t h[] = {1, 2};

    bool caught_bad_alloc = false;
    try
    {
        pika::future<decorated_iterator> f = pika::search_n(p,
            decorated_iterator(std::begin(c), []() { throw std::bad_alloc(); }),
            c.size(), std::begin(h), std::end(h));

        f.get();

        PIKA_TEST(false);
    }
    catch (std::bad_alloc const&)
    {
        caught_bad_alloc = true;
    }
    catch (...)
    {
        PIKA_TEST(false);
    }

    PIKA_TEST(caught_bad_alloc);
}

template <typename IteratorTag>
void test_search_n_bad_alloc()
{
    using namespace pika::execution;

    // If the execution policy object is of type vector_execution_policy,
    // std::terminate shall be called. therefore we do not test exceptions
    // with a vector execution policy
    test_search_n_bad_alloc(par, IteratorTag());
    test_search_n_bad_alloc(seq, IteratorTag());

    test_search_n_async_bad_alloc(seq(task), IteratorTag());
    test_search_n_async_bad_alloc(par(task), IteratorTag());
}

void search_n_bad_alloc_test()
{
    test_search_n_bad_alloc<std::random_access_iterator_tag>();
    test_search_n_bad_alloc<std::forward_iterator_tag>();
}

int pika_main(pika::program_options::variables_map& vm)
{
    unsigned int seed = (unsigned int) std::time(nullptr);
    if (vm.count("seed"))
        seed = vm["seed"].as<unsigned int>();

    std::cout << "using seed: " << seed << std::endl;
    std::srand(seed);

    search_n_test1();
    search_n_test2();
    search_n_test3();
    search_n_test4();
    search_n_test5();
    search_n_exception_test();
    search_n_bad_alloc_test();
    return pika::finalize();
}

int main(int argc, char* argv[])
{
    using namespace pika::program_options;
    options_description desc_commandline(
        "Usage: " PIKA_APPLICATION_STRING " [options]");

    desc_commandline.add_options()("seed,s", value<unsigned int>(),
        "the random number generator seed to use for this run");

    std::vector<std::string> const cfg = {"pika.os_threads=all"};

    pika::init_params init_args;
    init_args.desc_cmdline = desc_commandline;
    init_args.cfg = cfg;

    PIKA_TEST_EQ_MSG(pika::init(pika_main, argc, argv, init_args), 0,
        "pika main exited with non-zero status");

    return pika::util::report_errors();
}
