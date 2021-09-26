// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

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
