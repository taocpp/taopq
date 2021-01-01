# Advanced Features

* [Connection Pools](#connection-pools)
* [Nested Transactions](#nested-transactions)
* [Transaction Isolation](#transaction-isolation)
* [Table Writers](#table-writers)

## Connection Pools

TODO - here, or create one page with everything on connections?

## Nested Transactions

TODO - here, or create one page with everything on transaction?

A nested, or sub-transaction can be created by calling `tr->subtransaction()`.

We refer to the PostgreSQL documentation for the semantics of nested transactions.

Subtransactions can not be created with different transaction isolation levels, however:

The automatic rollback in the destructor also works for sub-transactions, only the statements executed in the sub-transaction are rolled back.

Note that in addition to keeping the connection alive, nested transactions also keep the transactions they were created from alive.

## Transaction Isolation

TODO - here, or create one page with everything on transaction?

Regular top-level transactions can be created with different [isolation levels](https://www.postgresql.org/docs/current/transaction-iso.html) from `tao::pq::transaction::isolation_level` by calling `c->transaction( level )` where `level` is an enumerator from `tao::pq::transaction::isolation_level`.

```c++
enum class isolation_level
{
   default_isolation_level,
   serializable,
   repeatable_read,
   read_committed,
   read_uncommitted
};
```

## Table Writers

TODO - here?

Copyright (c) 2019-2021 Daniel Frey and Dr. Colin Hirsch
