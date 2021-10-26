// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_HPP
#define TAO_PQ_HPP

#if __cplusplus < 201703L
#error "taoPQ requires C++17 or newer"
#else

#include <tao/pq/version.hpp>

#include <tao/pq/binary.hpp>
#include <tao/pq/null.hpp>
#include <tao/pq/oid.hpp>

#include <tao/pq/connection.hpp>
#include <tao/pq/connection_pool.hpp>
#include <tao/pq/transaction.hpp>

#include <tao/pq/parameter_traits.hpp>
#include <tao/pq/parameter_traits_array.hpp>
#include <tao/pq/parameter_traits_optional.hpp>
#include <tao/pq/parameter_traits_pair.hpp>
#include <tao/pq/parameter_traits_tuple.hpp>

#include <tao/pq/exception.hpp>
#include <tao/pq/result.hpp>

#include <tao/pq/result_traits.hpp>
#include <tao/pq/result_traits_array.hpp>
#include <tao/pq/result_traits_optional.hpp>
#include <tao/pq/result_traits_pair.hpp>
#include <tao/pq/result_traits_tuple.hpp>

#include <tao/pq/table_reader.hpp>
#include <tao/pq/table_writer.hpp>

#include <tao/pq/large_object.hpp>

#endif
#endif
