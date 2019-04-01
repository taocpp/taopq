# Result Sets

In the following We assume `rs` to be an instance of `tao::pq::result` obtained by executing a `SELECT` statement.

Some general functions provide information about the result set.

The number of columns is returned by `rs.columns()`.
The name of a column can be obtained by calling `rs.name( index )` where `index` is zero-based so `index` must be less than `rs.columns()`.
Conversely the index of a column can be obtained by calling `rs.index( name )` where `name` is a string with the name of the column.

Note that name matching is done according to the SQL standard.

The class `tao::pq::result` can also be understood as container of database rows.

Calling `rs.size()` returns the number of rows in the result set, and `rs.empty()` checks whether that number is zero.
The functions `rs.begin()` and `rs.end()` return iterators to [rows](#rows) which allow both manual iteration over a result set or using a range-based for-loop.
Random access to the rows is provided by `rs[ row ]` or `rs.at( row )` where the former is an unsafe version of the latter, just as for `std::vector`.
As with columns, rows are zero-based so `row` must be less than `rs.size()`.

Low-level access to the field data is available via `rs.is_null( row, column )` and `rs.get( row, column )`.
The latter is only allowed when the former would return `false`.

The `get` function returns a `const char*`, which is valid for the lifetime of (any copy of) the result.
...

## Rows

## Fields

## ...

Copyright (c) 2019 Daniel Frey and Dr. Colin Hirsch
