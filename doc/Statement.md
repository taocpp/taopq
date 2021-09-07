# Statement

Before showing you how to execute statements with taoPQ, we'd like to take a moment to talk about [SQL injection](https://en.wikipedia.org/wiki/SQL_injection).
SQL injection is a large family of security issues that has plagued the internet for decades.
We designed taoPQ to allow you to *safely* and *conveniently* write code that does not allow SQL injection.
With that said, let's start executing statements with taoPQ.

## `execute()`

All statements are executed by calling an `execute()`-method, either on a transaction, connection, or connection pool directly.
The synopsis of the execute methods of those types (`Type` being `tao::pq::transaction`, `tao::pq::connection`, or `tao::pq::connection_pool`) is:

```c++
template< typename... As >
auto Type::execute( const char* statement, As&&... as )
   -> tao::pq::result;

template< typename... As >
auto Type::execute( const std::string& statement, As&&... as )
   -> tao::pq::result;
```

For the purpose of this chapter, it makes no difference which type is used.
The method takes the statement itself as its first parameter and you can add additional parameters with your values that will be used in the statement.
Note that we do not accept `std::string_view` for the statement, as `libpq` requires a zero-terminated string.

## Positional Parameters

To reference the parameters you supplied in the statement, you use [positional parameters](https://www.postgresql.org/docs/current/sql-expressions.html).
Positional parameters are of the form `$n` where `n` is a number starting at 1.
Here's an example of how you can insert a row with two columns into the database:

```c++
tr->execute( "INSERT INTO user ( name, age ) VALUES ( $1, $2 )", "Daniel", 42 );
```

The actual data is separated from the statement itself.
The use of positional parameters makes passing strings and other types safe, as there is no need for manually escaping the data.
Our library now knows what is the actual SQL statement you want to send and what is the data you want to send.
This protects you from SQL injections and it is also quiet convenient.
The way the data is now transferred between the client and the server is also more efficient.

The only thing you have to remember is to **never** concatenate strings together to create the SQL statement including the data manually.
As this is such an important point, we will illustrate how it should **not** be done:

```c++
auto find_user( const std::string& name )
{
   return connection->execute( "SELECT FROM user WHERE name = '" + name + "'" );
   //                                                         ~~~~~~~~~~~~~~       WRONG!!!
}
```

Never concatenate SQL statements manually!
Consider `name` to be an input field coming from untrusted user input.
What happens, if a user enters the following "name":

`Robert'; DELETE FROM user WHERE name <> 'Little Bobby Tables`

Yupp, all users other than [`Little Bobby Tables`](https://xkcd.com/327/) have just been deleted from the database.

You might have seen other libraries where you should [escape the data explicitly](https://www.postgresql.org/docs/current/libpq-exec.html#LIBPQ-EXEC-ESCAPE-STRING), something like:

```c++
auto find_user( const std::string& name )
{
   return connection->execute( "SELECT FROM user WHERE name = '" + connection->escape( name ) + "'" );
}
```

This is cumbersome and error-prone.
It is easy to forget calling the escape method, which the compiler will *not* catch for you, and for non-strings, the code needs to call explicit conversion methods to string.
This turns longer SQL statements into a long, ugly mess.

Positional parameters solve all of those problems and therefore taoPQ does not even offer any escaping methods.
To be safe and to make your life easier, with taoPQ always use positional parameters:

```c++
auto find_user( const std::string& name )
{
   return connection->execute( "SELECT FROM user WHERE name = $1", name );
}
```

## Multi-Query Commands

Some SQL client libraries allow multi-query commands, i.e. the command string can include multiple SQL statements.
In `libpq`, this is supported by the [`PQexec`](https://www.postgresql.org/docs/current/libpq-exec.html)-function.
As an extra defense against SQL injection, taoPQ *never* calls the `PQexec()`-function.
We allow at most one SQL command in the given statement passed to an `execute()`-method.

## Prepared Statements

In the [Connection](Connection.md) chapter we have shown how you can prepare (and deallocate) prepared statements.
In order to execute prepared statements, you simply pass the name of the prepared statement to the `execute()`-method instead of the actual SQL statement.
This might look like this:

```c++
connection->prepare( "insert_user", "INSERT INTO user ( name, age ) VALUES ( $1, $2 )" );

connection->execute( "insert_user", "Daniel", 42 );
connection->execute( "insert_user", "Tom", 41 );
connection->execute( "insert_user", "Jerry", 29 );
```

This is both more efficient and also allows you to change the statements in a central place if need be, without touching any of the places where it is actually used.

You might want to wrap calls to a (prepared) statement into an application-specific wrapper, that way you add C++'s type safety for the rest of the application calling that method (and also receiving the result).

Another idea that we already used in practice is to read the SQL statements from a configuration file.
When you then need to change a statement, e.g. to work around a performance issue with the database or because you renamed a column in the database, all you need to do is adapt the configuration.
No need to recompile the application.

## Type Conversion

The above example also shows that you can use different data types as parameters.
Besides basic types like strings or integers, you can also use more complex types like `std::tuple` which will decay into multiple parameters.
You can even register your own data types to generate one or more parameters from them.
The [Parameter Type Conversion](Parameter-Type-Conversion.md) chapter explains which data types are supported out-of-the-box, and how you can register your own custom data types.

Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
