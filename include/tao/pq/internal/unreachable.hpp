// Copyright (c) 2020-2022 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_INTERNAL_UNREACHABLE_HPP
#define TAO_PQ_INTERNAL_UNREACHABLE_HPP

#if defined( _MSC_VER )
#define TAO_PQ_UNREACHABLE __assume( false )
#else
#define TAO_PQ_UNREACHABLE __builtin_unreachable()
#endif

#endif
