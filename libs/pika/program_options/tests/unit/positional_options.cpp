//  Copyright Vladimir Prus 2004.
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt
//  or copy at http://www.boost.org/LICENSE_1_0.txt)

#include <pika/detail/filesystem.hpp>
#include <pika/program_options/errors.hpp>
#include <pika/program_options/options_description.hpp>
#include <pika/program_options/parsers.hpp>
#include <pika/program_options/positional_options.hpp>
#include <pika/program_options/value_semantic.hpp>
#include <pika/testing.hpp>

#include <cstddef>
#include <limits>
#include <vector>

using namespace pika::program_options;
using namespace std;

void test_positional_options()
{
    positional_options_description p;
    p.add("first", 1);

    PIKA_TEST_EQ(p.max_total_count(), 1u);
    PIKA_TEST_EQ(p.name_for_position(0), "first");

    p.add("second", 2);

    PIKA_TEST_EQ(p.max_total_count(), 3u);
    PIKA_TEST_EQ(p.name_for_position(0), "first");
    PIKA_TEST_EQ(p.name_for_position(1), "second");
    PIKA_TEST_EQ(p.name_for_position(2), "second");

    p.add("third", -1);

    PIKA_TEST_EQ(p.max_total_count(), (std::numeric_limits<unsigned>::max)());
    PIKA_TEST_EQ(p.name_for_position(0), "first");
    PIKA_TEST_EQ(p.name_for_position(1), "second");
    PIKA_TEST_EQ(p.name_for_position(2), "second");
    PIKA_TEST_EQ(p.name_for_position(3), "third");
    PIKA_TEST_EQ(p.name_for_position(10000), "third");
}

void test_parsing()
{
    options_description desc;
    // clang-format off
    desc.add_options()
        ("first", value<int>())
        ("second", value<int>())
        ("input-file", value< vector<string> >())
        ("some-other", value<string>())
    ;
    // clang-format on

    positional_options_description p;
    p.add("input-file", 2).add("some-other", 1);

    vector<string> args;
    args.emplace_back("--first=10");
    args.emplace_back("file1");
    args.emplace_back("--second=10");
    args.emplace_back("file2");
    args.emplace_back("file3");

    // Check that positional options are handled.
    parsed_options parsed =
        command_line_parser(args).options(desc).positional(p).run();

    PIKA_TEST_EQ(parsed.options.size(), std::size_t(5));
    PIKA_TEST_EQ(parsed.options[1].string_key, "input-file");
    PIKA_TEST_EQ(parsed.options[1].value[0], "file1");
    PIKA_TEST_EQ(parsed.options[3].string_key, "input-file");
    PIKA_TEST_EQ(parsed.options[3].value[0], "file2");
    PIKA_TEST_EQ(parsed.options[4].value[0], "file3");

    args.emplace_back("file4");

    // Check that excessive number of positional options is detected.
    PIKA_TEST_THROW(command_line_parser(args).options(desc).positional(p).run(),
        too_many_positional_options_error);
}

int main(int, char*[])
{
    test_positional_options();
    test_parsing();

    return 0;
}
