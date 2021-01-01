// Copyright (c) 2020-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_UNREACHABLE_HPP
#define TAO_PQ_INTERNAL_UNREACHABLE_HPP

#if defined( _MSC_VER )
#define TAO_PQ_UNREACHABLE __assume( false )
#else
#define TAO_PQ_UNREACHABLE __builtin_unreachable()
#endif

#endif
