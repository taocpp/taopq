# Performance

This chapter gives practical hints on how to make the most out of taoPQ when performance matters, as well as some gotchas you might encounter when using the library.

## Reuse Connections

Opening a new connection to the database server typically consists of several time-consuming steps (network round-trips, authentication, TLS negotiation).
If your application performs many short-lived operations, this overhead can dominate the runtime.

* Keep connections open and reuse them instead of opening a new one for every statement.
* For concurrent workloads, use a [connection pool](Connection-Pool.md).
  Pools keep a set of active connections alive and hand them out on demand, so the cost of opening a connection is amortized over many operations instead of being paid on each one.

## Use Prepared Statements

When a statement is executed repeatedly with different parameters, [prepared statements](Statement.md#prepared-statements) are usually the better choice:

* The server parses and plans the statement only once, and the cached plan is reused for subsequent executions.
* Parameters are sent separately from the statement text, which reduces the amount of data transferred per execution.
* The statement text lives in a single place, which also makes it easier to fix performance regressions without touching the call sites.

```c++
conn->prepare( "insert_user", "INSERT INTO users ( name, age ) VALUES ( $1, $2 )" );

tr->execute( "insert_user", "Daniel", 42 );
tr->execute( "insert_user", "Tom", 41 );
```

See also the [Statement](Statement.md) chapter for details on positional parameters.

## Bulk Transfer

For loading or downloading large amounts of data row by row, use [bulk transfer](Bulk-Transfer.md) via `table_writer` and `table_reader`.
These types transfer data in large chunks instead of issuing a separate round-trip per row, which dramatically reduces the number of network interactions and the per-row overhead of parameter and result conversion.

## Pipeline Mode

When many independent requests can be sent without waiting for each response, the `tao::pq::pipeline` transaction type lets you queue several operations and process the results as they arrive, hiding latency by overlapping client-side work with the network round-trips.

## Type Conversion

The [parameter](Parameter-Type-Conversion.md) and [result](Result-Type-Conversion.md) conversion traits convert between C++ types and SQL types at the call sites.
When converting aggregates or custom types, prefer direct [aggregate support](Aggregate-Support.md) over manual field-by-field conversion, as it avoids repeated per-field dispatch.

---

This document is part of [taoPQ](https://github.com/taocpp/taopq).

Copyright (c) 2021-2026 Daniel Frey and Dr. Colin Hirsch<br>
Distributed under the Boost Software License, Version 1.0<br>
See accompanying file [LICENSE_1_0.txt](../LICENSE_1_0.txt) or copy at https://www.boost.org/LICENSE_1_0.txt
