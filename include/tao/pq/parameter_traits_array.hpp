// Copyright (c) 2021-2025 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef TAO_PQ_PARAMETER_TRAITS_ARRAY_HPP
#define TAO_PQ_PARAMETER_TRAITS_ARRAY_HPP

#include <cstddef>
#include <string>

#include <tao/pq/is_array.hpp>
#include <tao/pq/oid.hpp>
#include <tao/pq/parameter_traits.hpp>

namespace tao::pq
{
   namespace internal
   {
      template< typename T >
      concept array_parameter_type = pq::is_array_parameter< T > && ( pq::is_array_parameter< typename T::value_type > || ( parameter_traits< typename T::value_type >::columns == 1 ) );

      template< typename T >
      void to_array( std::string& data, const T& v )
      {
         parameter_traits< T >( v ).template element< 0 >( data );
      }

      template< typename T >
         requires pq::is_array_parameter< T >
      void to_array( std::string& data, const T& v )
      {
         data += '{';
         if( v.empty() ) {
            data += '}';
         }
         else {
            for( const auto& e : v ) {
               internal::to_array( data, e );
               data += ',';
            }
            *data.rbegin() = '}';
         }
      }

   }  // namespace internal

   template< internal::array_parameter_type T >
   struct parameter_traits< T >
   {
   private:
      std::string m_data;

   public:
      explicit parameter_traits( const T& v )
      {
         internal::to_array( m_data, v );
      }

      static constexpr std::size_t columns = 1;
      static constexpr bool self_contained = true;

      template< std::size_t I >
      [[nodiscard]] static constexpr auto type() noexcept -> oid
      {
         return oid::invalid;
      }

      template< std::size_t I >
      [[nodiscard]] auto value() const noexcept -> const char*
      {
         return m_data.c_str();
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

      template< std::size_t I >
      void element( std::string& data ) const
      {
         internal::array_append( data, m_data );
      }

      template< std::size_t I >
      void copy_to( std::string& data ) const
      {
         internal::table_writer_append( data, m_data );
      }
   };

}  // namespace tao::pq

#endif
