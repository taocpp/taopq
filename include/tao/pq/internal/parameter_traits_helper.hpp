// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
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
      explicit constexpr char_pointer_helper( const char* p ) noexcept
         : m_p( p )
      {}

   public:
      static constexpr std::size_t columns = 1;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] constexpr auto value() const noexcept -> const char*
      {
         return m_p;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
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
      [[nodiscard]] static constexpr auto type() noexcept -> Oid
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_s.c_str();
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto length() noexcept -> int
      {
         return 0;
      }

      template< std::size_t I >
      [[nodiscard]] static constexpr auto format() noexcept -> int
      {
         return 0;
      }
   };

}  // namespace tao::pq::internal

#endif
