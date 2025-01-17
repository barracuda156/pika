//  Copyright (c) 2007-2015 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <pika/config.hpp>

namespace pika {
    struct no_mutex
    {
        void lock() {}

        bool try_lock()
        {
            return true;
        };

        void unlock() {}
    };
}    // namespace pika
