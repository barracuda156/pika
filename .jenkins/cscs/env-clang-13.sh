# Copyright (c) 2020 ETH Zurich
#
# SPDX-License-Identifier: BSL-1.0
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

cxx_std="20"
clang_version="13.0.1"
boost_version="1.79.0"
hwloc_version="2.6.0"
spack_compiler="clang@${clang_version}"
spack_arch="cray-cnl7-broadwell"
stdexec_version="c0fe16f451137b00ceb85d912ec3a288990ce766"

spack_spec="pika@main arch=${spack_arch} %${spack_compiler} cxxflags=-stdlib=libc++ malloc=system cxxstd=${cxx_std} +p2300 ^boost@${boost_version} ^hwloc@${hwloc_version} ^fmt cxxflags=-stdlib=libc++ ^stdexec@${stdexec_version}"

configure_extra_options+=" -DCMAKE_CXX_FLAGS=-stdlib=libc++"
configure_extra_options+=" -DPIKA_WITH_CXX_STANDARD=${cxx_std}"
configure_extra_options+=" -DPIKA_WITH_MALLOC=system"
configure_extra_options+=" -DPIKA_WITH_SPINLOCK_DEADLOCK_DETECTION=ON"
configure_extra_options+=" -DPIKA_WITH_P2300_REFERENCE_IMPLEMENTATION=ON"
