# Overview

Using this library starts with obtaining a **connection** to the database, either by creating a singular connection or by means of a **connection pool**.

Next, SQL statements can be executed on the connection, either in auto-commit mode or inside of a **transaction**, and either directly or using a **prepared statement**.

Transactions can use any of the transaction isolation levels defined by PostgreSQL, or be a "fake" auto-commit transaction.

Statement parameters are passed as additional parameters, the **parameter traits** determine how a parameter is presented to the database.

Predefined types include `std::optional` for NULL-able values, custom types can be added with custom conversion functions.
Use `tao::pq::null` as parameter for an explicit SQL `NULL`.

Statement return results, either with the number of rows affected by an `INSERT`, `UPDATE`, `DELETE`, ... statement, or the result set of a `SELECT` query statement.

Result sets can be iterated or conveniently converted into a C++ data structure where the **result traits** determine the conversion.
Predefined types include all STL containers, `std::pair`/`std::tuple`, `std::optional` for NULL-able values.
Again custom types can be added with custom conversion functions..

## Important

Please **always** pass statement parameters as individual function arguments to `execute()`, i.e. **never** create SQL statements by pasting the parameters to the statement body with string concatenation!!!

*Concatenating the string representation of statement parameters with the SQL statement opens the door to SQL-injection attacks, a large family of security issues that has plagued the internet for decades.*

Copyright (c) 2019 Daniel Frey and Dr. Colin Hirsch
