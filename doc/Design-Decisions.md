# Design Decisions

The design of taoPQ is loosely based on Jeroen T. Vermeulen's [libpqxx](http://pqxx.org/development/libpqxx/), keeping the philosophy of the design while rethinking everything from the ground up and taking C++17 into account where possible.

* [Shared Pointers](#shared-pointers)
* [Direct Transaction](#direct-transactions)

## Shared Pointers

Connections and transaction are always passed by `std::shared_ptr` to allow for the following features.

* Connections obtained from a connection pool are automagically placed back into the pool by the deleter of the shared pointer.
* Transactions keeping a shared pointer to the connection ensures that the connection is not destroyed (or put back into the pool) while a transaction is active, thereby simplifying code that uses a connection for a single transaction by not requiring said code to keep the connection around, too.
* Shared pointers between transactions and connections are used to enforce correct nesting of transactions and that there is at most one active, i.e. inner-most, transaction per connection. Statements can only be executed on the connection when there is no transaction.

The connection pool related deleter could be used with a `std::unique_ptr`, albeit with the disadvantage of not being transparent to client code (since the type of the deleter is not hidden).

## Direct Transactions

"Direct" transactions, or auto-commit transactions, behave just like executing SQL statements directly on the connection, i.e. every statement is processed individually "as if" it were wrapped in its own transaction.

The reason this seemingly redundant kind of transactions exists is to allow writing non-templated code that can execute statements both within the context of a transaction or without a transaction.
Such code can simply operate on a `std::shared_ptr< tao::pq::transaction >`, which is the static type of both a "real" transaction and a "direct", i.e. "fake" auto-commit transaction.

Copyright (c) 2019 Daniel Frey and Dr. Colin Hirsch
