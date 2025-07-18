# Table of Content

* [Requirements](Requirements.md)
  * [Operating System Support](Requirements.md#operator-system-support)
  * [Compiler Support](Requirements.md#compiler-support)
  * [Language Requirements](Requirements.md#language-requirements)
  * [Compiler Options/Warnings](Requirements.md#compiler-optionswarnings)
  * [Database Requirements](Requirements.md#database-requirements)
* [Installation](Installation.md)
  * **TODO** after properly rewriting this chapter
* [Getting Started](Getting-Started.md)
  * [Next Steps](Getting-Started.md#next-steps)
  * [Advanced Topics](Getting-Started.md#advanced-topics)
* [Connection Pool](Connection-Pool.md)
  * [Synopsis](Connection-Pool.md#synopsis)
  * [Creating Connection Pools](Connection-Pool.md#creating-connection-pools)
  * [Borrowing Connections](Connection-Pool.md#borrowing-connections)
  * [Executing Statements](Connection-Pool.md#executing-statements)
  * [Cleanup](Connection-Pool.md#cleanup)
  * [Thread Safety](Connection-Pool.md#thread-safety)
* [Connection](Connection.md)
  * [Synopsis](Connection.md#synopsis)
  * [Creating a Connection](Connection.md#creating-a-connection)
  * [Creating Transactions](Connection.md#creating-transactions)
    * [Creating a "Direct" Transaction](Connection.md#creating-a-direct-transaction)
    * [Creating a Database Transaction](Connection.md#creating-a-database-transaction)
  * [Executing Statements](Connection.md#executing-statements)
  * [Prepared Statements](Connection.md#prepared-statements)
    * [Manually Prepared Statements](Connection.md#manually-prepared-statements)
  * [Checking Status](Connection.md#checking-status)
  * [Notification Framework](Connection.md#notification-framework)
    * [Sending Messages](Connection.md#sending-messages)
    * [Receiving Messages](Connection.md#receiving-messages)
    * [Handling Messages](Connection.md#handling-messages)
    * [Per Channel Handlers](Connection.md#per-channel-handlers)
    * [Asynchronous Notifications](Connection.md#asynchronous-notifications)
    * [Event Loop](Connection.md#event-loop)
  * [Underlying Connection Pointer](Connection.md#underlying-connection-pointer)
  * [Error Messages](Connection.md#error-messages)
* [Transaction](Transaction.md)
  * [Synopsis](Transaction.md#synopsis)
  * [Creating Transactions](Transaction.md#creating-transactions)
  * [Statement Execution](Transaction.md#statement-execution)
  * [Terminate Transaction](Transaction.md#terminate-transaction)
    * [Commit a Transaction](Transaction.md#commit-a-transaction)
    * [Abort a Transaction](Transaction.md#abort-a-transaction)
  * [Transaction Ordering](Transaction.md#transaction-ordering)
  * [Direct Transactions](Transaction.md#direct-transactions)
  * [Manual Transaction Handling](Transaction.md#manual-transaction-handling)
  * [Accessing the Connection](Transaction.md#accessing-the-connection)
* [Statement](Statement.md)
  * [`execute()`](Statement.md#execute)
    * [`tao::pq::internal::zsv`](Statement.md#taopqinternalzsv)
  * [Positional Parameters](Statement.md#positional-parameters)
  * [Multi-Query Commands](Statement.md#multi-query-commands)
  * [Prepared Statements](Statement.md#prepared-statements)
  * [Type Conversion](Statement.md#type-conversion)
* [Parameter Type Conversion](Parameter-Type-Conversion.md)
  * [NULL](Parameter-Type-Conversion.md#null)
  * [Fundamental Types](Parameter-Type-Conversion.md#fundamental-types)
  * [`std::optional< T >`](Parameter-Type-Conversion.md#stdoptional-t-)
  * [`std::pair< T, U >`](Parameter-Type-Conversion.md#stdpair-t-u-)
  * [`std::tuple< Ts... >`](Parameter-Type-Conversion.md#stdtuple-ts-)
  * [Aggregates](Parameter-Type-Conversion.md#aggregates)
  * [Custom Data Types](Parameter-Type-Conversion.md#custom-data-types)
    * [`to_taopq()`](Parameter-Type-Conversion.md#to_taopq-)
      * [Intrusive Placement](Parameter-Type-Conversion.md#intrusive-placement)
      * [Non-Intrusive Placement](Parameter-Type-Conversion.md#non-intrusive-placement)
    * [`tao::pq::parameter_traits< T >`](Parameter-Type-Conversion.md#taopqparameter_traits-t-)
* [Result](Result.md)
  * [Synopsis](Result.md#synopsis)
  * [Non-Query Results](Result.md#non-query-results)
  * [Query Results](Result.md#query-results)
    * [Basics](Result.md#basics)
    * [Row Access](Result.md#row-access)
    * [Field Access](Result.md#field-access)
    * [Fields](Result.md#fields)
  * [Field Data Conversion](Result.md#field-data-conversion)
  * [Row Data Conversion](Result.md#row-data-conversion)
* [Result Type Conversion](Result-Type-Conversion.md)
  * [Fundamental Types](Result-Type-Conversion.md#fundamental-types)
  * [`std::optional< T >`](Result-Type-Conversion.md#stdoptional-t-)
  * [`std::pair< T, U >`](Result-Type-Conversion.md#stdpair-t-u-)
  * [`std::tuple< Ts... >`](Result-Type-Conversion.md#stdtuple-ts-)
  * [Aggregates](Result-Type-Conversion.md#aggregates)
  * [Custom Data Types](Result-Type-Conversion.md#custom-data-types)
    * [`from_taopq()`](Result-Type-Conversion.md#from_taopq)
      * [Intrusive Placement](Result-Type-Conversion.md#intrusive-placement)
      * [Non-Intrusive Placement](Result-Type-Conversion.md#non-intrusive-placement)
    * [`tao::pq::result_traits< T >`](Result-Type-Conversion.md#taopqresult_traits-t-)
* [Aggregate Support](Aggregate-Support.md)
  * [Status](Aggregate-Support.md#status)
  * [Requirements](Aggregate-Support.md#requirements)
  * [Registration](Aggregate-Support.md#registration)
  * [Direct Result Conversion](Aggregate-Support.md#direct-result-conversion)
  * [Example](Aggregate-Support.md#example)
* [Error Handling](Error-Handling.md)
  * [SQLSTATE](Error-Handling.md#sqlstate)
    * [Class of Error](Error-Handling.md#class-of-error)
    * [Specific Error Conditions](Error-Handling.md#specific-error-conditions)
  * [Connection Errors](Error-Handling.md#connection-errors)
  * [Other Exceptions](Error-Handling.md#other-exceptions)
* [Binary Data](Binary-Data.md)
  * [The `BYTEA` Data Type](Binary-Data.md#the-bytea-data-type)
  * [C++ Binary Data](Binary-Data.md#c-binary-data)
  * [Passing Binary Data](Binary-Data.md#passing-binary-data)
  * [Receiving Binary Data](Binary-Data.md#receiving-binary-data)
* [Bulk Transfer](Bulk-Transfer.md)
  * [Synopsis](Bulk-Transfer.md#synopsis)
* [Large Object](Large-Object.md)
  * [Synopsis](Large-Object.md#synopsis)
  * [Creating a Large Object](Large-Object.md#creating-a-large-object)
  * [Removing a Large Object](Large-Object.md#removing-a-large-object)
  * [Importing a Large Object](Large-Object.md#importing-a-large-object)
  * [Exporting a Large Object](Large-Object.md#exporting-a-large-object)
  * [Opening an Existing Large Object](Large-Object.md#opening-an-existing-large-object)
  * [Writing Data to a Large Object](Large-Object.md#writing-data-to-a-large-object)
  * [Reading Data from a Large Object](Large-Object.md#reading-data-from-a-large-object)
  * [Seeking in a Large Object](Large-Object.md#seeking-in-a-large-object)
  * [Obtaining the Seek Position of a Large Object](Large-Object.md#obtaining-the-seek-position-of-a-large-object)
  * [Truncating a Large Object](Large-Object.md#truncating-a-large-object)
* [Performance](Performance.md)
