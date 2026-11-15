# Statement

:warning: Before showing you how to execute statements with taoPQ, we'd like to take a moment to talk about [SQL injection➚](https://en.wikipedia.org/wiki/SQL_injection).
SQL injection is a large family of security issues that has plagued the internet for decades.
We designed taoPQ to allow you to *safely* and *conveniently* write code that does not allow SQL injection.
With that said, let's start executing statements with taoPQ.

## `execute()`

All statements are executed by calling an `execute()`-method, either on a transaction, connection, or connection pool directly.
The synopsis of the execute methods of those types (`Type` being `tao::pq::transaction`, `tao::pq::connection`, or `tao::pq::connection_pool`) is:

```c++
template< typename... As >
auto Type::execute( const tao::pq::internal::zsv statement, As&&... as ) -> tao::pq::result;
```

For the purposes of this chapter, it makes no difference which `Type` is used.
The method takes the statement itself as its first parameter, followed by additional parameters that will be passed to the SQL statement.
It returns a `tao::pq::result`, documented in the [Result](Result.md) chapter.
If statement execution fails, an exception is thrown as documented in the [Error Handling](Error-Handling.md) chapter.

### `tao::pq::internal::zsv`

The `tao::pq::internal::zsv` type (zero-terminated string view) that is used for the `statement` parameter ensures that you pass a zero-terminated string.
It is a non-owning type which has non-explicit constructor overloads for `const char*` and `const std::string&`.
For safety reasons, there is another overload for `std::nullptr_t` that is deleted.
Note that we do not accept `std::string_view`, as the underlying C-API of `libpq` requires a zero-terminated string.

## Positional Parameters

To reference the parameters you supplied in the statement, you use [positional parameters➚](https://www.postgresql.org/docs/current/sql-expressions.html).
Positional parameters are of the form `$n` where `n` is a number starting at 1.
Here's an example of how you can insert a row with two columns into the database:

```c++
tr->execute( "INSERT INTO user ( name, age ) VALUES ( $1, $2 )", "Daniel", 42 );
```

The actual data is separated from the statement itself.
The use of positional parameters makes passing strings and other types safe, as there is no need to manually escape the data.
The library can distinguish the SQL statement from the data you want to send, which protects you from SQL injection.
The transfer of data between client and server is also more efficient.

:warning: The only thing you have to remember is to **never** concatenate strings together to create the SQL statement including the data manually.
As this is such an important point, we will illustrate how it should **not** be done:

```c++
auto find_user( const std::string& name )
{
   return connection->execute( "SELECT FROM user WHERE name = '" + name + "'" );
   //                                                         ~~~~~~~~~~~~~~       WRONG!!!
}
```

:no_entry: Never concatenate SQL statements manually!
Consider `name` to be an input field coming from untrusted user input.
What happens, if a user enters the following "name":

`Robert'; DELETE FROM user WHERE name <> 'Little Bobby Tables`

Yes, all users other than [`Little Bobby Tables`➚](https://xkcd.com/327/) have just been deleted from the database.

You might have seen other libraries where you should [escape the data explicitly➚](https://www.postgresql.org/docs/current/libpq-exec.html#LIBPQ-EXEC-ESCAPE-STRING), something like:

```c++
auto find_user( const std::string& name )
{
   return connection->execute( "SELECT FROM user WHERE name = '" + connection->escape( name ) + "'" );
}
```

This is cumbersome and error-prone.
It is easy to forget to call the escape method, and the compiler will not catch that for you.
For non-string types, the code would need to call explicit conversion methods to string, turning longer SQL statements into a messy tangle.

Positional parameters solve all of these problems, and for that reason taoPQ does not offer any escaping methods.
To be safe and to make your life easier, with taoPQ always use positional parameters:

```c++
auto find_user( const std::string& name )
{
   return connection->execute( "SELECT FROM user WHERE name = $1", name );
}
```

## Multi-Query Commands

Some SQL client libraries allow multi-query commands, i.e. the command string can include multiple SQL statements.
In `libpq`, this is supported by the [`PQexec()`➚](https://www.postgresql.org/docs/current/libpq-exec.html)-function.
As an extra defense against SQL injection, taoPQ *never* calls the `PQexec()`-function.
We allow at most one SQL command in the given statement passed to an `execute()`-method.

## Prepared Statements

In the [Connection](Connection.md) chapter we have shown how you can prepare (and deallocate) prepared statements.
To execute prepared statements, you simply pass the name of the prepared statement to the `execute()`-method instead of the SQL statement.
This might look like this:

```c++
connection->prepare( "insert_user", "INSERT INTO user ( name, age ) VALUES ( $1, $2 )" );

connection->execute( "insert_user", "Daniel", 42 );
connection->execute( "insert_user", "Tom", 41 );
connection->execute( "insert_user", "Jerry", 29 );
```

This is both more efficient and allows you to change the statements in a central place if needed, without touching any of the call sites.

You might also consider wrapping calls to a (prepared) statement in an application-specific wrapper, adding C++'s type safety for the rest of the application that calls that wrapper (and receives the result).

Another idea that we already used in practice is to read the SQL statements from a configuration file.
When you then need to change a statement, e.g. to work around a performance issue with the database or because you renamed a column in the database, all you need to do is adapt the configuration.
No need to recompile the application.

## Type Conversion

The above example also shows that you can use different data types as parameters.
Besides basic types like strings or integers, you can also use more complex types like `std::tuple` which will decay into multiple parameters.
You can even register your own data types to generate one or more parameters from them.
The [Parameter Type Conversion](Parameter-Type-Conversion.md) chapter explains which data types are supported out-of-the-box, and how you can register your own custom data types.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2026 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
