//  Copyright (c) 2020 Thomas Heller
//  Copyright (c) 2020 Hartmut Kaiser
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <pika/config.hpp>
#if defined(PIKA_HAVE_P2300_REFERENCE_IMPLEMENTATION)
#include <pika/execution_base/p2300_forward.hpp>

namespace pika::execution::experimental {
    template <typename Receiver>
    inline constexpr bool is_receiver_v = receiver<Receiver>;

    template <typename Receiver>
    struct is_receiver
    {
        static constexpr bool value = is_receiver_v<Receiver>;
    };

    template <typename Receiver, typename Completions>
    inline constexpr bool is_receiver_of_v = receiver_of<Receiver, Completions>;

    template <typename Receiver, typename Completions>
    struct is_receiver_of
    {
        static constexpr bool value = is_receiver_of_v<Receiver, Completions>;
    };

    namespace detail {
        struct empty_env
        {
        };
    }    // namespace detail
}    // namespace pika::execution::experimental
#else
#include <pika/config/constexpr.hpp>
#include <pika/functional/tag_invoke.hpp>

#include <exception>
#include <type_traits>
#include <utility>

namespace pika::execution::experimental {

#if defined(DOXYGEN)
    /// set_value is a customization point object. The expression
    /// `pika::execution::set_value(r, as...)` is equivalent to:
    ///     * `r.set_value(as...)`, if that expression is valid. If the function selected
    ///       does not send the value(s) `as...` to the Receiver `r`'s value channel,
    ///       the program is ill-formed (no diagnostic required).
    ///     * Otherwise, `set_value(r, as...), if that expression is valid, with
    ///       overload resolution performed in a context that include the declaration
    ///       `void set_value();`
    ///     * Otherwise, the expression is ill-formed.
    ///
    /// The customization is implemented in terms of `pika::functional::tag_invoke`.
    template <typename R, typename... As>
    void set_value(R&& r, As&&... as);

    /// set_stopped is a customization point object. The expression
    /// `pika::execution::set_stopped(r)` is equivalent to:
    ///     * `r.set_stopped()`, if that expression is valid. If the function selected
    ///       does not signal the Receiver `r`'s done channel,
    ///       the program is ill-formed (no diagnostic required).
    ///     * Otherwise, `set_stopped(r), if that expression is valid, with
    ///       overload resolution performed in a context that include the declaration
    ///       `void set_stopped();`
    ///     * Otherwise, the expression is ill-formed.
    ///
    /// The customization is implemented in terms of `pika::functional::tag_invoke`.
    template <typename R>
    void set_stopped(R&& r);

    /// set_error is a customization point object. The expression
    /// `pika::execution::set_error(r, e)` is equivalent to:
    ///     * `r.set_stopped(e)`, if that expression is valid. If the function selected
    ///       does not send the error `e` the Receiver `r`'s error channel,
    ///       the program is ill-formed (no diagnostic required).
    ///     * Otherwise, `set_error(r, e), if that expression is valid, with
    ///       overload resolution performed in a context that include the declaration
    ///       `void set_error();`
    ///     * Otherwise, the expression is ill-formed.
    ///
    /// The customization is implemented in terms of `pika::functional::tag_invoke`.
    template <typename R, typename E>
    void set_error(R&& r, E&& e);
#endif

    /// Receiving values from asynchronous computations is handled by the `Receiver`
    /// concept. A `Receiver` needs to be able to receive an error or be marked as
    /// being canceled. As such, the Receiver concept is defined by having the
    /// following two customization points defined, which form the completion-signal
    /// operations:
    ///     * `pika::execution::experimental::set_stopped`
    ///     * `pika::execution::experimental::set_error`
    ///
    /// Those two functions denote the completion-signal operations. The Receiver
    /// contract is as follows:
    ///     * None of a Receiver's completion-signal operation shall be invoked
    ///       before `pika::execution::experimental::start` has been called on the operation
    ///       state object that was returned by connecting a Receiver to a sender
    ///       `pika::execution::experimental::connect`.
    ///     * Once `pika::execution::start` has been called on the operation
    ///       state object, exactly one of the Receiver's completion-signal operation
    ///       shall complete without an exception before the Receiver is destroyed
    ///
    /// Once one of the Receiver's completion-signal operation has been completed
    /// without throwing an exception, the Receiver contract has been satisfied.
    /// In other words: The asynchronous operation has been completed.
    ///
    /// \see pika::execution::experimental::is_receiver_of
    template <typename T, typename E = std::exception_ptr>
    struct is_receiver;

    /// The `receiver_of` concept is a refinement of the `Receiver` concept by
    /// requiring one additional completion-signal operation:
    ///     * `pika::execution::set_value`
    ///
    /// This completion-signal operation adds the following to the Receiver's
    /// contract:
    ///     * If `pika::execution::set_value` exits with an exception, it
    ///       is still valid to call `pika::execution::set_error` or
    ///       `pika::execution::set_stopped`
    ///
    /// \see pika::execution::traits::is_receiver
    template <typename T, typename... As>
    struct is_receiver_of;

    PIKA_HOST_DEVICE_INLINE_CONSTEXPR_VARIABLE
    struct set_value_t : pika::functional::tag<set_value_t>
    {
    } set_value{};

    PIKA_HOST_DEVICE_INLINE_CONSTEXPR_VARIABLE
    struct set_error_t : pika::functional::tag_noexcept<set_error_t>
    {
    } set_error{};

    PIKA_HOST_DEVICE_INLINE_CONSTEXPR_VARIABLE
    struct set_stopped_t : pika::functional::tag_noexcept<set_stopped_t>
    {
    } set_stopped{};

    ///////////////////////////////////////////////////////////////////////
    namespace detail {
        template <bool ConstructionRequirements, typename T, typename E>
        struct is_receiver_impl;

        template <typename T, typename E>
        struct is_receiver_impl<false, T, E> : std::false_type
        {
        };

        template <typename T, typename E>
        struct is_receiver_impl<true, T, E>
          : std::integral_constant<bool,
                std::is_invocable_v<set_stopped_t, std::decay_t<T>&&> &&
                    std::is_invocable_v<set_error_t, std::decay_t<T>&&, E>>
        {
        };
    }    // namespace detail

    template <typename T, typename E>
    struct is_receiver
      : detail::is_receiver_impl<
            std::is_move_constructible<std::decay_t<T>>::value &&
                std::is_constructible<std::decay_t<T>, T>::value,
            T, E>
    {
    };

    template <typename T, typename E = std::exception_ptr>
    inline constexpr bool is_receiver_v = is_receiver<T, E>::value;

    ///////////////////////////////////////////////////////////////////////
    namespace detail {
        template <bool IsReceiverOf, typename T, typename... As>
        struct is_receiver_of_impl;

        template <typename T, typename... As>
        struct is_receiver_of_impl<false, T, As...> : std::false_type
        {
        };

        template <typename T, typename... As>
        struct is_receiver_of_impl<true, T, As...>
          : std::integral_constant<bool,
                std::is_invocable_v<set_value_t, std::decay_t<T>&&, As...>>
        {
        };
    }    // namespace detail

    template <typename T, typename... As>
    struct is_receiver_of
      : detail::is_receiver_of_impl<is_receiver_v<T>, T, As...>
    {
    };

    template <typename T, typename... As>
    inline constexpr bool is_receiver_of_v = is_receiver_of<T, As...>::value;

    namespace detail {
        struct empty_env
        {
        };
    }    // namespace detail

    inline constexpr struct get_env_t
    {
    } get_env{};
}    // namespace pika::execution::experimental
#endif

namespace pika::execution::experimental::detail {
    template <typename CPO>
    struct is_receiver_cpo : std::false_type
    {
    };

    template <>
    struct is_receiver_cpo<set_value_t> : std::true_type
    {
    };

    template <>
    struct is_receiver_cpo<set_error_t> : std::true_type
    {
    };

    template <>
    struct is_receiver_cpo<set_stopped_t> : std::true_type
    {
    };

    template <typename CPO>
    inline constexpr bool is_receiver_cpo_v = is_receiver_cpo<CPO>::value;
}    // namespace pika::execution::experimental::detail
