// Copyright (c) 2016-2020 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_INTERNAL_PARAMETER_TRAITS_HELPER_HPP
#define TAO_PQ_INTERNAL_PARAMETER_TRAITS_HELPER_HPP

#include <cstddef>
#include <string>
#include <utility>

#include <libpq-fe.h>

namespace tao::pq::internal
{
   class char_pointer_helper
   {
   private:
      const char* const m_p;

   protected:
      explicit char_pointer_helper( const char* p ) noexcept
         : m_p( p )
      {}

   public:
      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept
      {
         return m_p;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
      {
         return 0;
      }
   };

   class string_helper
   {
   private:
      const std::string m_s;

   protected:
      template< typename... Ts >
      explicit string_helper( Ts&&... ts ) noexcept( noexcept( std::string( std::forward< Ts >( ts )... ) ) )
         : m_s( std::forward< Ts >( ts )... )
      {}

   public:
      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr Oid type() noexcept
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] const char* value() const noexcept
      {
         return m_s.c_str();
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int length() noexcept
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr int format() noexcept
      {
         return 0;
      }
   };

}  // namespace tao::pq::internal

#endif
