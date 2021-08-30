// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <tao/pq/parameter_traits.hpp>

namespace tao::pq::internal
{
   void array_append( std::string& buffer, std::string_view data )
   {
      if( data.empty() ) {
         buffer += "\"\"";
      }
      else if( data == "NULL" ) {
         buffer += "\"NULL\"";
      }
      else if( data.find_first_of( "\\\"{},; \t" ) != std::string_view::npos ) {
         buffer += '"';
         while( true ) {
            const auto n = data.find_first_of( "\\\"" );
            if( n == std::string_view::npos ) {
               buffer += data;
               break;
            }
            buffer.append( data.data(), n );
            buffer += '\\';
            buffer += data[ n ];
            data.remove_prefix( n + 1 );
         }
         buffer += '"';
      }
      else {
         buffer += data;
      }
   }

   void table_writer_append( std::string& buffer, std::string_view data )
   {
      while( true ) {
         const auto n = data.find_first_of( "\b\f\n\r\t\v\\" );
         if( n == std::string_view::npos ) {
            buffer += data;
            return;
         }
         buffer.append( data.data(), n );
         buffer += '\\';
         buffer += data[ n ];
         data.remove_prefix( n + 1 );
      }
   }

}  // namespace tao::pq::internal
