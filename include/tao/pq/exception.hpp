// Copyright (c) 2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#ifndef TAO_PQ_EXCEPTION_HPP
#define TAO_PQ_EXCEPTION_HPP

#include <stdexcept>
#include <string>
#include <string_view>

#include <libpq-fe.h>

namespace tao::pq
{
   // https://www.postgresql.org/docs/current/errcodes-appendix.html

   struct sql_error
      : std::runtime_error
   {
      std::string sqlstate;

      sql_error( const char* what, const std::string_view in_sqlstate );
   };

   // when a condition name from PostgreSQL is ambiguous,
   // we qualify the error class via a template parameter
   template< typename >
   struct string_data_right_truncation;

   template< typename >
   struct modifying_sql_data_not_permitted;

   template< typename >
   struct prohibited_sql_statement_attempted;

   template< typename >
   struct reading_sql_data_not_permitted;

   struct warning  // 01xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct null_value_eliminated_in_set_function  // 01003
      : warning
   {
      using warning::warning;
   };

   template<>
   struct string_data_right_truncation< warning >  // 01004
      : warning
   {
      using warning::warning;
   };

   struct privilege_not_revoked  // 01006
      : warning
   {
      using warning::warning;
   };

   struct privilege_not_granted  // 01007
      : warning
   {
      using warning::warning;
   };

   struct implicit_zero_bit_padding  // 01008
      : warning
   {
      using warning::warning;
   };

   struct dynamic_result_sets_returned  // 0100C
      : warning
   {
      using warning::warning;
   };

   struct deprecated_feature  // 01P01
      : warning
   {
      using warning::warning;
   };

   struct no_data  // 02xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct no_additional_dynamic_result_sets_returned  // 02001
      : no_data
   {
      using no_data::no_data;
   };

   struct sql_statement_not_yet_complete  // 03xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct broken_connection  // 08xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct sqlclient_unable_to_establish_sqlconnection  // 08001
      : broken_connection
   {
      using broken_connection::broken_connection;
   };

   struct connection_does_not_exist  // 08003
      : broken_connection
   {
      using broken_connection::broken_connection;
   };

   struct sqlserver_rejected_establishment_of_sqlconnection  // 08004
      : broken_connection
   {
      using broken_connection::broken_connection;
   };

   struct connection_failure  // 08006
      : broken_connection
   {
      using broken_connection::broken_connection;
   };

   struct transaction_resolution_unknown  // 08007
      : broken_connection
   {
      using broken_connection::broken_connection;
   };

   struct protocol_violation  // 08P01
      : broken_connection
   {
      using broken_connection::broken_connection;
   };

   struct triggered_action_exception  // 09xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct feature_not_supported  // 0Axxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_transaction_initiation  // 0Bxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct locator_exception  // 0Fxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_locator_specification  // 0F001
      : locator_exception
   {
      using locator_exception::locator_exception;
   };

   struct invalid_grantor  // 0Lxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_grant_operation  // 0LP01
      : invalid_grantor
   {
      using invalid_grantor::invalid_grantor;
   };

   struct invalid_role_specification  // 0Pxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct diagnostics_exception  // 0Zxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct stacked_diagnostics_accessed_without_active_handler  // 0Z002
      : diagnostics_exception
   {
      using diagnostics_exception::diagnostics_exception;
   };

   struct case_not_found  // 20xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct cardinality_violation  // 21xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct data_exception  // 22xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct array_subscript_error  // 2202E
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct character_not_in_repertoire  // 22021
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct datetime_field_overflow  // 22008
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct division_by_zero  // 22012
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct error_in_assignment  // 22005
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct escape_character_conflict  // 2200B
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct indicator_overflow  // 22022
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct interval_field_overflow  // 22015
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_argument_for_logarithm  // 2201E
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_argument_for_ntile_function  // 22014
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_argument_for_nth_value_function  // 22016
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_argument_for_power_function  // 2201F
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_argument_for_width_bucket_function  // 2201G
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_character_value_for_cast  // 22018
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_datetime_format  // 22007
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_escape_character  // 22019
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_escape_octet  // 2200D
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_escape_sequence  // 22025
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct nonstandard_use_of_escape_character  // 22P06
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_indicator_parameter_value  // 22010
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_parameter_value  // 22023
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_preceding_or_following_size  // 22013
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_regular_expression  // 2201B
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_row_count_in_limit_clause  // 2201W
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_row_count_in_result_offset_clause  // 2201X
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_tablesample_argument  // 2202H
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_tablesample_repeat  // 2202G
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_time_zone_displacement_value  // 22009
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_use_of_escape_character  // 2200C
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct most_specific_type_mismatch  // 2200G
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct null_value_not_allowed  // 22004
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct null_value_no_indicator_parameter  // 22002
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct numeric_value_out_of_range  // 22003
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct sequence_generator_limit_exceeded  // 2200H
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct string_data_length_mismatch  // 22026
      : data_exception
   {
      using data_exception::data_exception;
   };

   template<>
   struct string_data_right_truncation< data_exception >  // 22001
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct substring_error  // 22011
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct trim_error  // 22027
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct unterminated_c_string  // 22024
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct zero_length_character_string  // 2200F
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct floating_point_exception  // 22P01
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_text_representation  // 22P02
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_binary_representation  // 22P03
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct bad_copy_file_format  // 22P04
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct untranslatable_character  // 22P05
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct not_an_xml_document  // 2200L
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_xml_document  // 2200M
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_xml_content  // 2200N
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_xml_comment  // 2200S
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_xml_processing_instruction  // 2200T
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct duplicate_json_object_key_value  // 22030
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_argument_for_sql_json_datetime_function  // 22031
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_json_text  // 22032
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct invalid_sql_json_subscript  // 22033
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct more_than_one_sql_json_item  // 22034
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct no_sql_json_item  // 22035
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct non_numeric_sql_json_item  // 22036
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct non_unique_keys_in_a_json_object  // 22037
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct singleton_sql_json_item_required  // 22038
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct sql_json_array_not_found  // 22039
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct sql_json_member_not_found  // 2203A
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct sql_json_number_not_found  // 2203B
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct sql_json_object_not_found  // 2203C
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct too_many_json_array_elements  // 2203D
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct too_many_json_object_members  // 2203E
      : data_exception
   {
      using data_exception::data_exception;
   };

   struct sql_json_scalar_required  // 2203F
      : data_exception
   {
      using data_exception::data_exception;
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

   struct invalid_sql_statement_name  // 26xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct triggered_data_change_violation  // 27xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_authorization_specification  // 28xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_password  // 28P01
      : invalid_authorization_specification
   {
      using invalid_authorization_specification::invalid_authorization_specification;
   };

   struct dependent_privilege_descriptors_still_exist  // 2Bxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct dependent_objects_still_exist  // 2BP01
      : dependent_privilege_descriptors_still_exist
   {
      using dependent_privilege_descriptors_still_exist::dependent_privilege_descriptors_still_exist;
   };

   struct invalid_transaction_termination  // 2Dxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct sql_routine_exception  // 2Fxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   template<>
   struct modifying_sql_data_not_permitted< sql_routine_exception >  // 2F002
      : sql_routine_exception
   {
      using sql_routine_exception::sql_routine_exception;
   };

   template<>
   struct prohibited_sql_statement_attempted< sql_routine_exception >  // 2F003
      : sql_routine_exception
   {
      using sql_routine_exception::sql_routine_exception;
   };

   template<>
   struct reading_sql_data_not_permitted< sql_routine_exception >  // 2F004
      : sql_routine_exception
   {
      using sql_routine_exception::sql_routine_exception;
   };

   struct function_executed_no_return_statement  // 2F005
      : sql_routine_exception
   {
      using sql_routine_exception::sql_routine_exception;
   };

   struct invalid_cursor_name  // 34xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct external_routine_exception  // 38xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct containing_sql_not_permitted  // 38001
      : external_routine_exception
   {
      using external_routine_exception::external_routine_exception;
   };

   template<>
   struct modifying_sql_data_not_permitted< external_routine_exception >  // 38002
      : external_routine_exception
   {
      using external_routine_exception::external_routine_exception;
   };

   template<>
   struct prohibited_sql_statement_attempted< external_routine_exception >  // 38003
      : external_routine_exception
   {
      using external_routine_exception::external_routine_exception;
   };

   template<>
   struct reading_sql_data_not_permitted< external_routine_exception >  // 38004
      : external_routine_exception
   {
      using external_routine_exception::external_routine_exception;
   };

   struct external_routine_invocation_exception  // 39xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_sqlstate_returned  // 39001
      : external_routine_invocation_exception
   {
      using external_routine_invocation_exception::external_routine_invocation_exception;
   };

   struct external_null_value_not_allowed  // 39004
      : external_routine_invocation_exception
   {
      using external_routine_invocation_exception::external_routine_invocation_exception;
   };

   struct trigger_protocol_violated  // 39P01
      : external_routine_invocation_exception
   {
      using external_routine_invocation_exception::external_routine_invocation_exception;
   };

   struct srf_protocol_violated  // 39P02
      : external_routine_invocation_exception
   {
      using external_routine_invocation_exception::external_routine_invocation_exception;
   };

   struct event_trigger_protocol_violated  // 39P03
      : external_routine_invocation_exception
   {
      using external_routine_invocation_exception::external_routine_invocation_exception;
   };

   struct savepoint_exception  // 3Bxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_savepoint_specification  // 3B001
      : savepoint_exception
   {
      using savepoint_exception::savepoint_exception;
   };

   struct invalid_catalog_name  // 3Dxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct invalid_schema_name  // 3Fxxx
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

   struct invalid_name  // 42602
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_column_definition  // 42611
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct name_too_long  // 42622
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_column  // 42701
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct ambiguous_column  // 42702
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_column  // 42703
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_object  // 42704
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_object  // 42710
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_alias  // 42712
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_function  // 42723
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct ambiguous_function  // 42725
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct grouping_error  // 42803
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct datatype_mismatch  // 42804
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct wrong_object_type  // 42809
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_foreign_key  // 42830
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct cannot_coerce  // 42846
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_function  // 42883
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct generated_always  // 428C9
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct reserved_name  // 42939
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_table  // 42P01
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct undefined_parameter  // 42P02
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_cursor  // 42P03
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_database  // 42P04
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_prepared_statement  // 42P05
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_schema  // 42P06
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct duplicate_table  // 42P07
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct ambiguous_parameter  // 42P08
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct ambiguous_alias  // 42P09
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_column_reference  // 42P10
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_cursor_definition  // 42P11
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_database_definition  // 42P12
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_function_definition  // 42P13
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_prepared_statement_definition  // 42P14
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_schema_definition  // 42P15
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_table_definition  // 42P16
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_object_definition  // 42P17
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct indeterminate_datatype  // 42P18
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct invalid_recursion  // 42P19
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct windowing_error  // 42P20
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct collation_mismatch  // 42P21
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct indeterminate_collation  // 42P22
      : syntax_error_or_access_rule_violation
   {
      using syntax_error_or_access_rule_violation::syntax_error_or_access_rule_violation;
   };

   struct with_check_option_violation  // 44xxx
      : sql_error
   {
      using sql_error::sql_error;
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

   struct program_limit_exceeded  // 54xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct statement_too_complex  // 54001
      : program_limit_exceeded
   {
      using program_limit_exceeded::program_limit_exceeded;
   };

   struct too_many_columns  // 54011
      : program_limit_exceeded
   {
      using program_limit_exceeded::program_limit_exceeded;
   };

   struct too_many_arguments  // 54023
      : program_limit_exceeded
   {
      using program_limit_exceeded::program_limit_exceeded;
   };

   struct object_not_in_prerequisite_state  // 55xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct object_in_use  // 55006
      : object_not_in_prerequisite_state
   {
      using object_not_in_prerequisite_state::object_not_in_prerequisite_state;
   };

   struct cant_change_runtime_param  // 55P02
      : object_not_in_prerequisite_state
   {
      using object_not_in_prerequisite_state::object_not_in_prerequisite_state;
   };

   struct lock_not_available  // 55P03
      : object_not_in_prerequisite_state
   {
      using object_not_in_prerequisite_state::object_not_in_prerequisite_state;
   };

   struct unsafe_new_enum_value_usage  // 55P04
      : object_not_in_prerequisite_state
   {
      using object_not_in_prerequisite_state::object_not_in_prerequisite_state;
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

   struct system_error  // 58xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct io_error  // 58030
      : system_error
   {
      using system_error::system_error;
   };

   struct undefined_file  // 58P01
      : system_error
   {
      using system_error::system_error;
   };

   struct duplicate_file  // 58P02
      : system_error
   {
      using system_error::system_error;
   };

   struct snapshot_too_old  // 72xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct config_file_error  // F0xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct lock_file_exists  // F0001
      : config_file_error
   {
      using config_file_error::config_file_error;
   };

   struct fdw_error  // HVxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct fdw_out_of_memory  // HV001
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_dynamic_parameter_value_needed  // HV002
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_data_type  // HV004
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_column_name_not_found  // HV005
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_data_type_descriptors  // HV006
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_column_name  // HV007
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_column_number  // HV008
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_use_of_null_pointer  // HV009
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_string_format  // HV00A
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_handle  // HV00B
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_option_index  // HV00C
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_option_name  // HV00D
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_option_name_not_found  // HV00J
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_reply_handle  // HV00K
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_unable_to_create_execution  // HV00L
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_unable_to_create_reply  // HV00M
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_unable_to_establish_connection  // HV00N
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_no_schemas  // HV00P
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_schema_not_found  // HV00Q
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_table_not_found  // HV00R
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_function_sequence_error  // HV010
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_too_many_handles  // HV014
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_inconsistent_descriptor_information  // HV021
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_attribute_value  // HV024
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_string_length_or_buffer_length  // HV090
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct fdw_invalid_descriptor_field_identifier  // HV091
      : fdw_error
   {
      using fdw_error::fdw_error;
   };

   struct plpgsql_error  // P0xxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct raise_exception  // P0001
      : plpgsql_error
   {
      using plpgsql_error::plpgsql_error;
   };

   struct no_data_found  // P0002
      : plpgsql_error
   {
      using plpgsql_error::plpgsql_error;
   };

   struct too_many_rows  // P0003
      : plpgsql_error
   {
      using plpgsql_error::plpgsql_error;
   };

   struct assert_failure  // P0004
      : plpgsql_error
   {
      using plpgsql_error::plpgsql_error;
   };

   struct internal_error  // XXxxx
      : sql_error
   {
      using sql_error::sql_error;
   };

   struct data_corrupted  // XX001
      : internal_error
   {
      using internal_error::internal_error;
   };

   struct index_corrupted  // XX002
      : internal_error
   {
      using internal_error::internal_error;
   };

   namespace internal
   {
      [[noreturn]] void throw_sqlstate( PGresult* pgresult );

   }  // namespace internal

}  // namespace tao::pq

#endif
