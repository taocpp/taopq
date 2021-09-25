// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_NOTIFICATION_HPP
#define TAO_PQ_NOTIFICATION_HPP

#include <cassert>
#include <memory>

#include <libpq-fe.h>

namespace tao::pq
{
   class connection;

   class notification final
   {
   private:
      friend connection;

      const std::unique_ptr< PGnotify, decltype( &PQfreemem ) > m_pgnotify;

      explicit notification( PGnotify* notify ) noexcept
         : m_pgnotify( notify, &PQfreemem )
      {
         assert( notify );
      }

   public:
      [[nodiscard]] auto channel() const noexcept -> const char*
      {
         return m_pgnotify->relname;
      }

      [[nodiscard]] auto payload() const noexcept -> const char*
      {
         return m_pgnotify->extra;
      }

      [[nodiscard]] auto underlying_raw_ptr() noexcept -> PGnotify*
      {
         return m_pgnotify.get();
      }

      [[nodiscard]] auto underlying_raw_ptr() const noexcept -> const PGnotify*
      {
         return m_pgnotify.get();
      }
   };

}  // namespace tao::pq

#endif
