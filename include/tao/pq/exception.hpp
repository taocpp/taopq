// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_EXCEPTION_HPP
#define TAO_PQ_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <string_view>

namespace tao::pq
{
   struct sql_error
      : std::runtime_error
   {
      // https://www.postgresql.org/docs/current/errcodes-appendix.html
      std::string sqlstate;

      sql_error( const char* what, const std::string_view in_sqlstate )
         : std::runtime_error( what ),
           sqlstate( in_sqlstate )
      {}
   };

   struct broken_connection  // 08xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct feature_not_supported  // 0Axxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct data_exception  // 22xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct integrity_constraint_violation  // 23xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct restrict_violation  // 23001
      : integrity_constraint_violation
   {
      using integrity_constraint_violation::integrity_constraint_violation;
   };

   struct not_null_violation  // 23502
      : integrity_constraint_violation
   {
      using integrity_constraint_violation::integrity_constraint_violation;
   };

   struct foreign_key_violation  // 23503
      : integrity_constraint_violation
   {
      using integrity_constraint_violation::integrity_constraint_violation;
   };

   struct unique_violation  // 23505
      : integrity_constraint_violation
   {
      using integrity_constraint_violation::integrity_constraint_violation;
   };

   struct check_violation  // 23514
      : integrity_constraint_violation
   {
      using integrity_constraint_violation::integrity_constraint_violation;
   };

   struct exclusion_violation  // 23P01
      : integrity_constraint_violation
   {
      using integrity_constraint_violation::integrity_constraint_violation;
   };

   struct invalid_cursor_state  // 24xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_transaction_state  // 25xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct transaction_rollback  // 40xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct serialization_failure  // 40001
      : transaction_rollback
   {
      using transaction_rollback::transaction_rollback;
   };

   struct transaction_integrity_constraint_violation  // 40002
      : transaction_rollback
   {
      using transaction_rollback::transaction_rollback;
   };

   struct statement_completion_unknown  // 40003
      : transaction_rollback
   {
      using transaction_rollback::transaction_rollback;
   };

   struct deadlock_detected  // 40P01
      : transaction_rollback
   {
      using transaction_rollback::transaction_rollback;
   };

   struct syntax_error_or_access_rule_violation  // 42xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct insufficient_privilege  // 42501
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct syntax_error  // 42601
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_column  // 42703
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_function  // 42883
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_table  // 42P01
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct insufficient_resources  // 53xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct disk_full  // 53100
      : insufficient_resources
   {
      using insufficient_resources::insufficient_resources;
   };

   struct out_of_memory  // 53200
      : insufficient_resources
   {
      using insufficient_resources::insufficient_resources;
   };

   struct too_many_connections  // 53300
      : insufficient_resources
   {
      using insufficient_resources::insufficient_resources;
   };

   struct configuration_limit_exceeded  // 53400
      : insufficient_resources
   {
      using insufficient_resources::insufficient_resources;
   };

   struct operator_intervention  // 57xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct query_canceled  // 57014
      : operator_intervention
   {
      using operator_intervention::operator_intervention;
   };

   struct admin_shutdown  // 57P01
      : operator_intervention
   {
      using operator_intervention::operator_intervention;
   };

   struct crash_shutdown  // 57P02
      : operator_intervention
   {
      using operator_intervention::operator_intervention;
   };

   struct cannot_connect_now  // 57P03
      : operator_intervention
   {
      using operator_intervention::operator_intervention;
   };

   struct database_dropped  // 57P04
      : operator_intervention
   {
      using operator_intervention::operator_intervention;
   };

}  // namespace tao::pq

#endif
