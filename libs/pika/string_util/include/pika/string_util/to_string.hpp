//  Copyright (c) 2019 Agustin Berge
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <pika/config.hpp>
#include <pika/string_util/bad_lexical_cast.hpp>

#include <fmt/format.h>

#include <string>
#include <type_traits>

namespace pika::util {

    namespace detail {
        template <typename T, typename Enable = void>
        struct to_string
        {
            static std::string call(T const& value)
            {
                return fmt::format("{}", value);
            }
        };

        template <typename T>
        struct to_string<T,
            typename std::enable_if<std::is_integral<T>::value ||
                std::is_floating_point<T>::value>::type>
        {
            static std::string call(T const& value)
            {
                return std::to_string(value);
            }
        };
    }    // namespace detail

    template <typename T>
    std::string to_string(T const& v)
    {
        try
        {
            return detail::to_string<T>::call(v);
        }
        catch (...)
        {
            return detail::throw_bad_lexical_cast<T, std::string>();
        }
    }

}    // namespace pika::util
