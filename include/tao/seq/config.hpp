// Copyright (c) 2015-2018 Daniel Frey
// Please see LICENSE for license or visit https://github.com/taocpp/sequences/

#ifndef TAO_SEQ_CONFIG_HPP
#define TAO_SEQ_CONFIG_HPP

#include <utility>

#ifndef TAO_USE_STD_INTEGER_SEQUENCE
#if defined( __cpp_lib_integer_sequence )
#define TAO_USE_STD_INTEGER_SEQUENCE
#elif defined( _LIBCPP_VERSION ) && ( __cplusplus >= 201402L )
#define TAO_USE_STD_INTEGER_SEQUENCE
#elif defined( _MSC_VER )
#define TAO_USE_STD_INTEGER_SEQUENCE
#endif
#endif

#ifndef TAO_USE_STD_MAKE_INTEGER_SEQUENCE
#if defined( _GLIBCXX_RELEASE ) && ( _GLIBCXX_RELEASE >= 8 ) && ( __cplusplus >= 201402L )
#define TAO_USE_STD_MAKE_INTEGER_SEQUENCE
#elif defined( _LIBCPP_VERSION ) && ( __cplusplus >= 201402L )
#define TAO_USE_STD_MAKE_INTEGER_SEQUENCE
#elif defined( _MSC_VER ) && ( _MSC_FULL_VER >= 190023918 )
#define TAO_USE_STD_MAKE_INTEGER_SEQUENCE
#endif
#endif

#if defined( __cpp_fold_expressions )
#define TAO_FOLD_EXPRESSIONS
#endif

#endif
