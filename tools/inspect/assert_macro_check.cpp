//  assert_macro_check implementation  ------------------------------------------------//

//  Copyright Eric Niebler 2010.
//  Based on the assert_macro_check checker by Marshall Clow
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0.
//  (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <pika/config.hpp>
#include <pika/string_util/to_string.hpp>

#include "assert_macro_check.hpp"
#include "boost/regex.hpp"
#include "function_hyper.hpp"

#include <filesystem>
#include <functional>

namespace {
    boost::regex assert_macro_regex(
        "("
        "^\\s*#\\s*undef\\s*"    // # undef
        "\\b(assert)\\b"         // followed by assert macro, whole word
        ")"
        "|"    // or (ignored)
        "("
        "//[^\\n]*"    // single line comments (//)
        "|"
        "/\\*.*?\\*/"    // multi line comments (/**/)
        "|"
        "\"(?:\\\\\\\\|\\\\\"|[^\"])*\""    // string literals
        ")"
        "|"    // or
        "("
        "\\b(assert)\\b"    // assert macro, whole word
        "\\s*\\("           // followed by 0 or more spaces and an opening paren
        ")",
        boost::regex::normal);

}    // unnamed namespace

namespace boost { namespace inspect {
    assert_macro_check::assert_macro_check()
      : m_files_with_errors(0)
    {
        register_signature(".c");
        register_signature(".cpp");
        register_signature(".cu");
        register_signature(".cxx");
        register_signature(".h");
        register_signature(".hpp");
        register_signature(".hxx");
        register_signature(".ipp");
    }

    void assert_macro_check::inspect(const string& library_name,
        const std::filesystem::path&
            full_path,             // example: c:/foo/boost/filesystem/path.hpp
        const string& contents)    // contents of file to be inspected
    {
        if (contents.find("pikainspect:"
                          "noassert_macro") != string::npos)
            return;

        long errors = 0;
        boost::sregex_iterator cur(
            contents.begin(), contents.end(), assert_macro_regex),
            end;
        for (; cur != end; ++cur)
        {
            if (!(*cur)[3].matched)
            {
                string::const_iterator it = contents.begin();
                string::const_iterator match_it = (*cur)[0].first;

                string::const_iterator line_start = it;

                string::size_type line_number = 1;
                for (; it != match_it; ++it)
                {
                    if (string::traits_type::eq(*it, '\n'))
                    {
                        ++line_number;
                        line_start = it + 1;    // could be end()
                    }
                }
                std::string lineloc =
                    linelink(full_path, pika::util::to_string(line_number));
                ++errors;
                error(library_name, full_path,
                    "C-style assert macro on line " + lineloc);
            }
        }
        if (errors > 0)
            ++m_files_with_errors;
    }
}}    // namespace boost::inspect
