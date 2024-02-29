// Copyright (c) 2021-2024 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include <tao/pq/result.hpp>

static_assert( !tao::pq::internal::has_reserve< std::list< int > > );
static_assert( tao::pq::internal::has_reserve< std::vector< int > > );

auto main() -> int
{}
