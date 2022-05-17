//  Copyright (c) 2007-2021 Hartmut Kaiser
//  Copyright (c) 2021 Giannis Gonidelis
//
//  SPDX-License-Identifier: BSL-1.0
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/// \file parallel/algorithms/transform_xxx.hpp

#pragma once

#include <pika/config.hpp>
#include <pika/assert.hpp>
#include <pika/async_mpi/mpi_future.hpp>
#include <pika/concepts/concepts.hpp>
#include <pika/datastructures/tuple.hpp>
#include <pika/datastructures/variant.hpp>
#include <pika/execution/algorithms/detail/partial_algorithm.hpp>
#include <pika/execution_base/receiver.hpp>
#include <pika/execution_base/sender.hpp>
#include <pika/functional/detail/tag_fallback_invoke.hpp>
#include <pika/functional/invoke.hpp>
#include <pika/functional/invoke_fused.hpp>
#include <pika/functional/traits/is_invocable.hpp>
#include <pika/mpi_base/mpi.hpp>

#include <exception>
#include <type_traits>
#include <utility>

namespace pika { namespace mpi { namespace experimental {
    namespace detail {

        template <typename Receiver, typename... Ts>
        void set_value_request_callback_helper(
            int mpi_status, Receiver&& receiver, Ts&&... ts)
        {
            static_assert(sizeof...(Ts) <= 1, "Expecting at most one value");
            if (mpi_status == MPI_SUCCESS)
            {
                pika::execution::experimental::set_value(
                    PIKA_FORWARD(Receiver, receiver), PIKA_FORWARD(Ts, ts)...);
            }
            else
            {
                pika::execution::experimental::set_error(
                    PIKA_FORWARD(Receiver, receiver),
                    std::make_exception_ptr(mpi_exception(mpi_status)));
            }
        }

        template <typename OperationState>
        void set_value_request_callback_void(
            MPI_Request request, OperationState& op_state)
        {
            detail::add_request_callback(
                [&op_state](int status) mutable {
                    op_state.ts = {};
                    set_value_request_callback_helper(
                        status, PIKA_MOVE(op_state.receiver));
                },
                request);
        }

        template <typename Result, typename OperationState>
        void set_value_request_callback_non_void(
            MPI_Request request, OperationState& op_state)
        {
            detail::add_request_callback(
                [&op_state](int status) mutable {
                    op_state.ts = {};
                    PIKA_ASSERT(
                        std::holds_alternative<Result>(op_state.result));
                    set_value_request_callback_helper(status,
                        PIKA_MOVE(op_state.receiver),
                        PIKA_MOVE(pika::get<Result>(op_state.result)));
                },
                request);
        }

        template <typename F, typename... Ts>
        inline constexpr bool is_mpi_request_invocable_v =
            pika::is_invocable_v<F,
                std::add_lvalue_reference_t<std::decay_t<Ts>>..., MPI_Request*>;

        template <typename F, typename... Ts>
        using mpi_request_invoke_result_t =
            std::decay_t<pika::util::invoke_result_t<F,
                std::add_lvalue_reference_t<std::decay_t<Ts>>...,
                MPI_Request*>>;

        template <typename Sender, typename F>
        struct transform_mpi_sender
        {
            std::decay_t<Sender> sender;
            std::decay_t<F> f;

            template <typename Tuple>
            struct invoke_result_helper;

            template <template <typename...> class Tuple, typename... Ts>
            struct invoke_result_helper<Tuple<Ts...>>
            {
                static_assert(is_mpi_request_invocable_v<F, Ts...>,
                    "F not invocable with the value_types specified.");
                using result_type = mpi_request_invoke_result_t<F, Ts...>;
                using type =
                    std::conditional_t<std::is_void<result_type>::value,
                        Tuple<>, Tuple<result_type>>;
            };

            template <template <typename...> class Tuple,
                template <typename...> class Variant>
            using value_types =
                pika::util::detail::unique_t<pika::util::detail::transform_t<
                    typename pika::execution::experimental::sender_traits<
                        Sender>::template value_types<Tuple, Variant>,
                    invoke_result_helper>>;

            template <template <typename...> class Variant>
            using error_types =
                pika::util::detail::unique_t<pika::util::detail::prepend_t<
                    typename pika::execution::experimental::sender_traits<
                        Sender>::template error_types<Variant>,
                    std::exception_ptr>>;

            static constexpr bool sends_done = false;

            template <typename Receiver>
            struct operation_state
            {
                std::decay_t<Receiver> receiver;
                std::decay_t<F> f;

                struct transform_mpi_receiver
                {
                    operation_state& op_state;

                    template <typename Error>
                    friend constexpr void tag_invoke(
                        pika::execution::experimental::set_error_t,
                        transform_mpi_receiver&& r, Error&& error) noexcept
                    {
                        pika::execution::experimental::set_error(
                            PIKA_MOVE(r.op_state.receiver),
                            PIKA_FORWARD(Error, error));
                    }

                    friend constexpr void tag_invoke(
                        pika::execution::experimental::set_stopped_t,
                        transform_mpi_receiver&& r) noexcept
                    {
                        pika::execution::experimental::set_stopped(
                            PIKA_MOVE(r.op_state.receiver));
                    };

                    template <typename... Ts,
                        typename = std::enable_if_t<
                            is_mpi_request_invocable_v<F, Ts...>>>
                    friend constexpr void tag_invoke(
                        pika::execution::experimental::set_value_t,
                        transform_mpi_receiver&& r, Ts&&... ts) noexcept
                    {
                        using ts_element_type =
                            pika::tuple<std::decay_t<Ts>...>;
                        r.op_state.ts.template emplace<ts_element_type>(
                            PIKA_FORWARD(Ts, ts)...);
                        auto& t = pika::get<ts_element_type>(r.op_state.ts);

                        pika::detail::try_catch_exception_ptr(
                            [&]() {
                                MPI_Request request;

                                using invoke_result_type =
                                    mpi_request_invoke_result_t<F, Ts...>;

                                if constexpr (std::is_void_v<
                                                  invoke_result_type>)
                                {
                                    pika::util::invoke_fused(
                                        [&](auto&... ts) mutable {
                                            PIKA_INVOKE(PIKA_MOVE(r.op_state.f),
                                                ts..., &request);
                                            // When the return type is void,
                                            // there is no value to forward to
                                            // the receiver
                                            set_value_request_callback_void(
                                                request, r.op_state);
                                        },
                                        t);
                                }
                                else
                                {
                                    pika::util::invoke_fused(
                                        [&](auto&... ts) mutable {
                                            r.op_state.result.template emplace<
                                                invoke_result_type>(PIKA_INVOKE(
                                                PIKA_MOVE(r.op_state.f), ts...,
                                                &request));
                                            // When the return type is non-void,
                                            // we have to forward the value to
                                            // the receiver
                                            set_value_request_callback_non_void<
                                                invoke_result_type>(
                                                request, r.op_state);
                                        },
                                        t);
                                }
                            },
                            [&](std::exception_ptr ep) {
                                pika::execution::experimental::set_error(
                                    PIKA_MOVE(r.op_state.receiver),
                                    PIKA_MOVE(ep));
                            });
                    }
                };

                using operation_state_type =
                    pika::execution::experimental::connect_result_t<
                        std::decay_t<Sender>, transform_mpi_receiver>;
                operation_state_type op_state;

                using ts_type = pika::util::detail::prepend_t<
                    typename pika::execution::experimental::sender_traits<
                        std::decay_t<Sender>>::template value_types<pika::tuple,
                        pika::variant>,
                    pika::monostate>;
                ts_type ts;

                // We store the return value of f in a variant. We know that
                // value_types of the transform_mpi_sender contains packs of at
                // most one element (the return value of f), so we only
                // specialize result_types_helper for zero or one value. For
                // empty packs we use pika::monostate since we don't need to
                // store anything in that case.
                //
                // All in all, we:
                // - transform one-element packs to the single element, and
                //   empty packs to pika::monostate
                // - add pika::monostate to the pack in case it wasn't there
                //   already
                // - remove duplicates in case pika::monostate has been added
                //   twice
                // - change the outer pack to a pika::variant
                template <typename Tuple>
                struct result_types_helper;

                template <template <typename...> class Tuple, typename T>
                struct result_types_helper<Tuple<T>>
                {
                    using type = std::decay_t<T>;
                };

                template <template <typename...> class Tuple>
                struct result_types_helper<Tuple<>>
                {
                    using type = pika::monostate;
                };

                using result_type = pika::util::detail::change_pack_t<
                    pika::variant,
                    pika::util::detail::unique_t<pika::util::detail::prepend_t<
                        pika::util::detail::transform_t<
                            transform_mpi_sender::value_types<pika::util::pack,
                                pika::util::pack>,
                            result_types_helper>,
                        pika::monostate>>>;

                result_type result;

                template <typename Receiver_, typename F_, typename Sender_>
                operation_state(Receiver_&& receiver, F_&& f, Sender_&& sender)
                  : receiver(PIKA_FORWARD(Receiver_, receiver))
                  , f(PIKA_FORWARD(F_, f))
                  , op_state(pika::execution::experimental::connect(
                        PIKA_FORWARD(Sender_, sender),
                        transform_mpi_receiver{*this}))
                {
                }

                friend constexpr auto tag_invoke(
                    pika::execution::experimental::start_t,
                    operation_state& os) noexcept
                {
                    return pika::execution::experimental::start(os.op_state);
                }
            };

            template <typename Receiver>
            friend constexpr auto tag_invoke(
                pika::execution::experimental::connect_t,
                transform_mpi_sender& s, Receiver&& receiver)
            {
                return operation_state<Receiver>(
                    PIKA_FORWARD(Receiver, receiver), s.f, s.sender);
            }

            template <typename Receiver>
            friend constexpr auto tag_invoke(
                pika::execution::experimental::connect_t,
                transform_mpi_sender&& s, Receiver&& receiver)
            {
                return operation_state<Receiver>(
                    PIKA_FORWARD(Receiver, receiver), PIKA_MOVE(s.f),
                    PIKA_MOVE(s.sender));
            }
        };
    }    // namespace detail

    inline constexpr struct transform_mpi_t final
      : pika::functional::detail::tag_fallback<transform_mpi_t>
    {
    private:
        template <typename Sender, typename F,
            PIKA_CONCEPT_REQUIRES_(
                pika::execution::experimental::is_sender_v<Sender>)>
        friend constexpr PIKA_FORCEINLINE auto tag_fallback_invoke(
            transform_mpi_t, Sender&& sender, F&& f)
        {
            return detail::transform_mpi_sender<Sender, F>{
                PIKA_FORWARD(Sender, sender), PIKA_FORWARD(F, f)};
        }

        template <typename F>
        friend constexpr PIKA_FORCEINLINE auto tag_fallback_invoke(
            transform_mpi_t, F&& f)
        {
            return ::pika::execution::experimental::detail::partial_algorithm<
                transform_mpi_t, F>{PIKA_FORWARD(F, f)};
        }
    } transform_mpi{};
}}}    // namespace pika::mpi::experimental
