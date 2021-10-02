# Error Handling

When an SQL statement is [executed](Statement.md) and the execution fails, an exception is thrown.
The base class for the exceptions thrown in this case is `tao::pq::sql_error`, which is derived from `std::runtime_error`.

```c++
namespace tao::pq
{
   struct sql_error
      : std::runtime_error
   {
      std::string sqlstate;
   };
}
```

The exception's `what()`-method will return the error message returned from the server.

## SQLSTATE

Depending on the [SQLSTATE](https://en.wikipedia.org/wiki/SQLSTATE) returned from the server as documented in [Appendix A](https://www.postgresql.org/docs/current/errcodes-appendix.html) of the PostgreSQL documentation, we throw an accordingly named exception class.

### Class of Error

The first two characters of the error code denote the class of errors, while the last three characters indicate a specific condition within that class.
Thus, an application that does not recognize the specific error code might still be able to infer what to do from the error class.

For each class of errors, there is an exception class derived from `tao::pq::sql_error`.
As an example, if the class is "02" ("no data"), the exception class that is thrown is either derived from `tao::pq::no_data` if a more specific error condition is recognized, or `tao::pq::no_data` itself will be the exception class that is thrown if only the class itself is recognized by taoPQ.

### Specific Error Conditions

If a specific error condition is recognized, an exception named after [Appendix A](https://www.postgresql.org/docs/current/errcodes-appendix.html) will be thrown, derived from the exception class of the class of error.

There are some cases in which the name can not be simply taken from that table, as they are used multiple times in different classes of errors.
In those cases the exception class is a class template and you need to add the class of error as a template parameter.

Specifically, this is necessary to distinguish the following exceptions:

* `tao::pq::string_data_right_truncation< tao::pq::warning >` (SQLSTATE "01004")
* `tao::pq::string_data_right_truncation< tao::pq::data_exception >` (SQLSTATE "22001")

and these exceptions from class "sql routine exception":

* `tao::pq::modifying_sql_data_not_permitted< tao::pq::sql_routine_exception >` (SQLSTATE "2F002")
* `tao::pq::prohibited_sql_statement_attempted< tao::pq::sql_routine_exception >` (SQLSTATE "2F003")
* `tao::pq::reading_sql_data_not_permitted< tao::pq::sql_routine_exception >` (SQLSTATE "2F004")

vs these exceptions from class "external routine exception":

* `tao::pq::modifying_sql_data_not_permitted< tao::pq::external_routine_exception >` (SQLSTATE "38002")
* `tao::pq::prohibited_sql_statement_attempted< tao::pq::external_routine_exception >` (SQLSTATE "38003")
* `tao::pq::reading_sql_data_not_permitted< tao::pq::external_routine_exception >` (SQLSTATE "38004")

## Connection Errors

PostgreSQL only delivers an SQLSTATE when a statement is executed.
In other situations, e.g. when opening a connection fails, no SQLSTATE is available.
We throw an exception of type `tao::pq::connection_error` in that case, with a dummy SQLSTATE of "08000".

The same exception can also be thrown when calling the connection's `get_notifications()`-method when the connection is broken.

## Other Exceptions

**TODO**

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
