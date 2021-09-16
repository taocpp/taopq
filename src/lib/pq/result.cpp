// Copyright (c) 2016-2021 Daniel Frey and Dr. Colin Hirsch
// Please see LICENSE for license or visit https://github.com/taocpp/taopq/

#include <cassert>
#include <charconv>
#include <cstring>
#include <stdexcept>

#include <libpq-fe.h>

#include <tao/pq/exception.hpp>
#include <tao/pq/result.hpp>

#include <tao/pq/internal/from_chars.hpp>
#include <tao/pq/internal/printf.hpp>

namespace tao::pq
{
   void result::check_has_result_set() const
   {
      if( m_columns == 0 ) {
         throw std::logic_error( "statement does not yield a result set" );
      }
   }

   void result::check_row( const std::size_t row ) const
   {
      check_has_result_set();
      if( !( row < m_rows ) ) {
         if( m_rows == 0 ) {
            throw std::out_of_range( internal::printf( "row %zu out of range, result is empty", row ) );
         }
         throw std::out_of_range( internal::printf( "row %zu out of range (0-%zu)", row, m_rows - 1 ) );
      }
   }

   result::result( PGresult* pgresult, const mode_t mode )
      : m_pgresult( pgresult, &PQclear ),
        m_columns( PQnfields( pgresult ) ),
        m_rows( PQntuples( pgresult ) )
   {
      const auto status = PQresultStatus( pgresult );
      switch( status ) {
         case PGRES_COMMAND_OK:
         case PGRES_TUPLES_OK:
            if( mode == mode_t::expect_ok ) {
               return;
            }
            break;

         case PGRES_COPY_IN:
            if( mode == mode_t::expect_copy_in ) {
               return;
            }
            break;

         case PGRES_COPY_OUT:
            if( mode == mode_t::expect_copy_out ) {
               return;
            }
            break;

         case PGRES_EMPTY_QUERY:
            throw std::runtime_error( "empty query" );

         default:
            const char* error_message = PQresultErrorMessage( pgresult );
            const std::string_view sql_state = PQresultErrorField( pgresult, PG_DIAG_SQLSTATE );
            // LCOV_EXCL_START
            switch( sql_state[ 0 ] ) {
               case '0':
                  switch( sql_state[ 1 ] ) {
                     case '1':
                        if( sql_state == "01003" ) {
                           throw null_value_eliminated_in_set_function( error_message, sql_state );
                        }
                        if( sql_state == "01004" ) {
                           throw warning_string_data_right_truncation( error_message, sql_state );
                        }
                        if( sql_state == "01006" ) {
                           throw privilege_not_revoked( error_message, sql_state );
                        }
                        if( sql_state == "01007" ) {
                           throw privilege_not_granted( error_message, sql_state );
                        }
                        if( sql_state == "01008" ) {
                           throw implicit_zero_bit_padding( error_message, sql_state );
                        }
                        if( sql_state == "0100C" ) {
                           throw dynamic_result_sets_returned( error_message, sql_state );
                        }
                        if( sql_state == "01P01" ) {
                           throw deprecated_feature( error_message, sql_state );
                        }
                        throw warning( error_message, sql_state );

                     case '2':
                        if( sql_state == "02001" ) {
                           throw no_additional_dynamic_result_sets_returned( error_message, sql_state );
                        }
                        throw no_data( error_message, sql_state );

                     case '3':
                        throw sql_statement_not_yet_complete( error_message, sql_state );

                     case '8':
                        if( sql_state == "08001" ) {
                           throw sqlclient_unable_to_establish_sqlconnection( error_message, sql_state );
                        }
                        if( sql_state == "08003" ) {
                           throw connection_does_not_exist( error_message, sql_state );
                        }
                        if( sql_state == "08004" ) {
                           throw sqlserver_rejected_establishment_of_sqlconnection( error_message, sql_state );
                        }
                        if( sql_state == "08006" ) {
                           throw connection_failure( error_message, sql_state );
                        }
                        if( sql_state == "08007" ) {
                           throw transaction_resolution_unknown( error_message, sql_state );
                        }
                        if( sql_state == "08P01" ) {
                           throw protocol_violation( error_message, sql_state );
                        }
                        throw broken_connection( error_message, sql_state );

                     case '9':
                        throw triggered_action_exception( error_message, sql_state );

                     case 'A':
                        throw feature_not_supported( error_message, sql_state );

                     case 'B':
                        throw invalid_transaction_initiation( error_message, sql_state );

                     case 'F':
                        if( sql_state == "0F001" ) {
                           throw invalid_locator_specification( error_message, sql_state );
                        }
                        throw locator_exception( error_message, sql_state );

                     case 'L':
                        if( sql_state == "0LP01" ) {
                           throw invalid_grant_operation( error_message, sql_state );
                        }
                        throw invalid_grantor( error_message, sql_state );

                     case 'P':
                        throw invalid_role_specification( error_message, sql_state );

                     case 'Z':
                        if( sql_state == "0Z002" ) {
                           throw stacked_diagnostics_accessed_without_active_handler( error_message, sql_state );
                        }
                        throw diagnostics_exception( error_message, sql_state );
                  }
                  break;

               case '2':
                  switch( sql_state[ 1 ] ) {
                     case '0':
                        throw case_not_found( error_message, sql_state );

                     case '1':
                        throw cardinality_violation( error_message, sql_state );

                     case '2':
                        if( sql_state == "2202E" ) {
                           throw array_subscript_error( error_message, sql_state );
                        }
                        if( sql_state == "22021" ) {
                           throw character_not_in_repertoire( error_message, sql_state );
                        }
                        if( sql_state == "22008" ) {
                           throw datetime_field_overflow( error_message, sql_state );
                        }
                        if( sql_state == "22012" ) {
                           throw division_by_zero( error_message, sql_state );
                        }
                        if( sql_state == "22005" ) {
                           throw error_in_assignment( error_message, sql_state );
                        }
                        if( sql_state == "2200B" ) {
                           throw escape_character_conflict( error_message, sql_state );
                        }
                        if( sql_state == "22022" ) {
                           throw indicator_overflow( error_message, sql_state );
                        }
                        if( sql_state == "22015" ) {
                           throw interval_field_overflow( error_message, sql_state );
                        }
                        if( sql_state == "2201E" ) {
                           throw invalid_argument_for_logarithm( error_message, sql_state );
                        }
                        if( sql_state == "22014" ) {
                           throw invalid_argument_for_ntile_function( error_message, sql_state );
                        }
                        if( sql_state == "22016" ) {
                           throw invalid_argument_for_nth_value_function( error_message, sql_state );
                        }
                        if( sql_state == "2201F" ) {
                           throw invalid_argument_for_power_function( error_message, sql_state );
                        }
                        if( sql_state == "2201G" ) {
                           throw invalid_argument_for_width_bucket_function( error_message, sql_state );
                        }
                        if( sql_state == "22018" ) {
                           throw invalid_character_value_for_cast( error_message, sql_state );
                        }
                        if( sql_state == "22007" ) {
                           throw invalid_datetime_format( error_message, sql_state );
                        }
                        if( sql_state == "22019" ) {
                           throw invalid_escape_character( error_message, sql_state );
                        }
                        if( sql_state == "2200D" ) {
                           throw invalid_escape_octet( error_message, sql_state );
                        }
                        if( sql_state == "22025" ) {
                           throw invalid_escape_sequence( error_message, sql_state );
                        }
                        if( sql_state == "22P06" ) {
                           throw nonstandard_use_of_escape_character( error_message, sql_state );
                        }
                        if( sql_state == "22010" ) {
                           throw invalid_indicator_parameter_value( error_message, sql_state );
                        }
                        if( sql_state == "22023" ) {
                           throw invalid_parameter_value( error_message, sql_state );
                        }
                        if( sql_state == "22013" ) {
                           throw invalid_preceding_or_following_size( error_message, sql_state );
                        }
                        if( sql_state == "2201B" ) {
                           throw invalid_regular_expression( error_message, sql_state );
                        }
                        if( sql_state == "2201W" ) {
                           throw invalid_row_count_in_limit_clause( error_message, sql_state );
                        }
                        if( sql_state == "2201X" ) {
                           throw invalid_row_count_in_result_offset_clause( error_message, sql_state );
                        }
                        if( sql_state == "2202H" ) {
                           throw invalid_tablesample_argument( error_message, sql_state );
                        }
                        if( sql_state == "2202G" ) {
                           throw invalid_tablesample_repeat( error_message, sql_state );
                        }
                        if( sql_state == "22009" ) {
                           throw invalid_time_zone_displacement_value( error_message, sql_state );
                        }
                        if( sql_state == "2200C" ) {
                           throw invalid_use_of_escape_character( error_message, sql_state );
                        }
                        if( sql_state == "2200G" ) {
                           throw most_specific_type_mismatch( error_message, sql_state );
                        }
                        if( sql_state == "22004" ) {
                           throw null_value_not_allowed( error_message, sql_state );
                        }
                        if( sql_state == "22002" ) {
                           throw null_value_no_indicator_parameter( error_message, sql_state );
                        }
                        if( sql_state == "22003" ) {
                           throw numeric_value_out_of_range( error_message, sql_state );
                        }
                        if( sql_state == "2200H" ) {
                           throw sequence_generator_limit_exceeded( error_message, sql_state );
                        }
                        if( sql_state == "22026" ) {
                           throw string_data_length_mismatch( error_message, sql_state );
                        }
                        if( sql_state == "22001" ) {
                           throw string_data_right_truncation( error_message, sql_state );
                        }
                        if( sql_state == "22011" ) {
                           throw substring_error( error_message, sql_state );
                        }
                        if( sql_state == "22027" ) {
                           throw trim_error( error_message, sql_state );
                        }
                        if( sql_state == "22024" ) {
                           throw unterminated_c_string( error_message, sql_state );
                        }
                        if( sql_state == "2200F" ) {
                           throw zero_length_character_string( error_message, sql_state );
                        }
                        if( sql_state == "22P01" ) {
                           throw floating_point_exception( error_message, sql_state );
                        }
                        if( sql_state == "22P02" ) {
                           throw invalid_text_representation( error_message, sql_state );
                        }
                        if( sql_state == "22P03" ) {
                           throw invalid_binary_representation( error_message, sql_state );
                        }
                        if( sql_state == "22P04" ) {
                           throw bad_copy_file_format( error_message, sql_state );
                        }
                        if( sql_state == "22P05" ) {
                           throw untranslatable_character( error_message, sql_state );
                        }
                        if( sql_state == "2200L" ) {
                           throw not_an_xml_document( error_message, sql_state );
                        }
                        if( sql_state == "2200M" ) {
                           throw invalid_xml_document( error_message, sql_state );
                        }
                        if( sql_state == "2200N" ) {
                           throw invalid_xml_content( error_message, sql_state );
                        }
                        if( sql_state == "2200S" ) {
                           throw invalid_xml_comment( error_message, sql_state );
                        }
                        if( sql_state == "2200T" ) {
                           throw invalid_xml_processing_instruction( error_message, sql_state );
                        }
                        if( sql_state == "22030" ) {
                           throw duplicate_json_object_key_value( error_message, sql_state );
                        }
                        if( sql_state == "22031" ) {
                           throw invalid_argument_for_sql_json_datetime_function( error_message, sql_state );
                        }
                        if( sql_state == "22032" ) {
                           throw invalid_json_text( error_message, sql_state );
                        }
                        if( sql_state == "22033" ) {
                           throw invalid_sql_json_subscript( error_message, sql_state );
                        }
                        if( sql_state == "22034" ) {
                           throw more_than_one_sql_json_item( error_message, sql_state );
                        }
                        if( sql_state == "22035" ) {
                           throw no_sql_json_item( error_message, sql_state );
                        }
                        if( sql_state == "22036" ) {
                           throw non_numeric_sql_json_item( error_message, sql_state );
                        }
                        if( sql_state == "22037" ) {
                           throw non_unique_keys_in_a_json_object( error_message, sql_state );
                        }
                        if( sql_state == "22038" ) {
                           throw singleton_sql_json_item_required( error_message, sql_state );
                        }
                        if( sql_state == "22039" ) {
                           throw sql_json_array_not_found( error_message, sql_state );
                        }
                        if( sql_state == "2203A" ) {
                           throw sql_json_member_not_found( error_message, sql_state );
                        }
                        if( sql_state == "2203B" ) {
                           throw sql_json_number_not_found( error_message, sql_state );
                        }
                        if( sql_state == "2203C" ) {
                           throw sql_json_object_not_found( error_message, sql_state );
                        }
                        if( sql_state == "2203D" ) {
                           throw too_many_json_array_elements( error_message, sql_state );
                        }
                        if( sql_state == "2203E" ) {
                           throw too_many_json_object_members( error_message, sql_state );
                        }
                        if( sql_state == "2203F" ) {
                           throw sql_json_scalar_required( error_message, sql_state );
                        }
                        throw data_exception( error_message, sql_state );

                     case '3':
                        if( sql_state == "23001" ) {
                           throw restrict_violation( error_message, sql_state );
                        }
                        if( sql_state == "23502" ) {
                           throw not_null_violation( error_message, sql_state );
                        }
                        if( sql_state == "23503" ) {
                           throw foreign_key_violation( error_message, sql_state );
                        }
                        if( sql_state == "23505" ) {
                           throw unique_violation( error_message, sql_state );
                        }
                        if( sql_state == "23514" ) {
                           throw check_violation( error_message, sql_state );
                        }
                        if( sql_state == "23P01" ) {
                           throw exclusion_violation( error_message, sql_state );
                        }
                        throw integrity_constraint_violation( error_message, sql_state );

                     case '4':
                        throw invalid_cursor_state( error_message, sql_state );

                     case '5':
                        if( sql_state == "08001" ) {
                           throw sqlclient_unable_to_establish_sqlconnection( error_message, sql_state );
                        }
                        if( sql_state == "08003" ) {
                           throw connection_does_not_exist( error_message, sql_state );
                        }
                        if( sql_state == "08004" ) {
                           throw sqlserver_rejected_establishment_of_sqlconnection( error_message, sql_state );
                        }
                        if( sql_state == "08006" ) {
                           throw connection_failure( error_message, sql_state );
                        }
                        if( sql_state == "08007" ) {
                           throw transaction_resolution_unknown( error_message, sql_state );
                        }
                        if( sql_state == "08P01" ) {
                           throw protocol_violation( error_message, sql_state );
                        }
                        throw invalid_transaction_state( error_message, sql_state );

                     case '6':
                        throw invalid_sql_statement_name( error_message, sql_state );

                     case '7':
                        throw triggered_data_change_violation( error_message, sql_state );

                     case '8':
                        if( sql_state == "28P01" ) {
                           throw invalid_password( error_message, sql_state );
                        }
                        throw invalid_authorization_specification( error_message, sql_state );

                     case 'B':
                        if( sql_state == "2BP01" ) {
                           throw dependent_objects_still_exist( error_message, sql_state );
                        }
                        throw dependent_privilege_descriptors_still_exist( error_message, sql_state );

                     case 'D':
                        throw invalid_transaction_termination( error_message, sql_state );

                     case 'F':
                        if( sql_state == "2F002" ) {
                           throw modifying_sql_data_not_permitted( error_message, sql_state );
                        }
                        if( sql_state == "2F003" ) {
                           throw prohibited_sql_statement_attempted( error_message, sql_state );
                        }
                        if( sql_state == "2F004" ) {
                           throw reading_sql_data_not_permitted( error_message, sql_state );
                        }
                        if( sql_state == "2F005" ) {
                           throw function_executed_no_return_statement( error_message, sql_state );
                        }
                        throw sql_routine_exception( error_message, sql_state );
                  }
                  break;

               case '3':
                  switch( sql_state[ 1 ] ) {
                     case '4':
                        throw invalid_cursor_name( error_message, sql_state );

                     case '8':
                        if( sql_state == "38001" ) {
                           throw containing_sql_not_permitted( error_message, sql_state );
                        }
                        if( sql_state == "38002" ) {
                           throw external_modifying_sql_data_not_permitted( error_message, sql_state );
                        }
                        if( sql_state == "38003" ) {
                           throw external_prohibited_sql_statement_attempted( error_message, sql_state );
                        }
                        if( sql_state == "38004" ) {
                           throw external_reading_sql_data_not_permitted( error_message, sql_state );
                        }
                        throw external_routine_exception( error_message, sql_state );

                     case '9':
                        if( sql_state == "39001" ) {
                           throw invalid_sqlstate_returned( error_message, sql_state );
                        }
                        if( sql_state == "39004" ) {
                           throw external_null_value_not_allowed( error_message, sql_state );
                        }
                        if( sql_state == "39P01" ) {
                           throw trigger_protocol_violated( error_message, sql_state );
                        }
                        if( sql_state == "39P02" ) {
                           throw srf_protocol_violated( error_message, sql_state );
                        }
                        if( sql_state == "39P03" ) {
                           throw event_trigger_protocol_violated( error_message, sql_state );
                        }
                        throw external_routine_invocation_exception( error_message, sql_state );

                     case 'B':
                        if( sql_state == "3B001" ) {
                           throw invalid_savepoint_specification( error_message, sql_state );
                        }
                        throw savepoint_exception( error_message, sql_state );

                     case 'D':
                        throw invalid_catalog_name( error_message, sql_state );

                     case 'F':
                        throw invalid_schema_name( error_message, sql_state );
                  }
                  break;

               case '4':
                  switch( sql_state[ 1 ] ) {
                     case '0':
                        if( sql_state == "40001" ) {
                           throw serialization_failure( error_message, sql_state );
                        }
                        if( sql_state == "40002" ) {
                           throw transaction_integrity_constraint_violation( error_message, sql_state );
                        }
                        if( sql_state == "40003" ) {
                           throw statement_completion_unknown( error_message, sql_state );
                        }
                        if( sql_state == "40P01" ) {
                           throw deadlock_detected( error_message, sql_state );
                        }
                        throw transaction_rollback( error_message, sql_state );

                     case '2':
                        if( sql_state == "42501" ) {
                           throw insufficient_privilege( error_message, sql_state );
                        }
                        if( sql_state == "42601" ) {
                           throw syntax_error( error_message, sql_state );
                        }
                        if( sql_state == "42602" ) {
                           throw invalid_name( error_message, sql_state );
                        }
                        if( sql_state == "42611" ) {
                           throw invalid_column_definition( error_message, sql_state );
                        }
                        if( sql_state == "42622" ) {
                           throw name_too_long( error_message, sql_state );
                        }
                        if( sql_state == "42701" ) {
                           throw duplicate_column( error_message, sql_state );
                        }
                        if( sql_state == "42702" ) {
                           throw ambiguous_column( error_message, sql_state );
                        }
                        if( sql_state == "42703" ) {
                           throw undefined_column( error_message, sql_state );
                        }
                        if( sql_state == "42704" ) {
                           throw undefined_object( error_message, sql_state );
                        }
                        if( sql_state == "42710" ) {
                           throw duplicate_object( error_message, sql_state );
                        }
                        if( sql_state == "42712" ) {
                           throw duplicate_alias( error_message, sql_state );
                        }
                        if( sql_state == "42723" ) {
                           throw duplicate_function( error_message, sql_state );
                        }
                        if( sql_state == "42725" ) {
                           throw ambiguous_function( error_message, sql_state );
                        }
                        if( sql_state == "42803" ) {
                           throw grouping_error( error_message, sql_state );
                        }
                        if( sql_state == "42804" ) {
                           throw datatype_mismatch( error_message, sql_state );
                        }
                        if( sql_state == "42809" ) {
                           throw wrong_object_type( error_message, sql_state );
                        }
                        if( sql_state == "42830" ) {
                           throw invalid_foreign_key( error_message, sql_state );
                        }
                        if( sql_state == "42846" ) {
                           throw cannot_coerce( error_message, sql_state );
                        }
                        if( sql_state == "42883" ) {
                           throw undefined_function( error_message, sql_state );
                        }
                        if( sql_state == "428C9" ) {
                           throw generated_always( error_message, sql_state );
                        }
                        if( sql_state == "42939" ) {
                           throw reserved_name( error_message, sql_state );
                        }
                        if( sql_state == "42P01" ) {
                           throw undefined_table( error_message, sql_state );
                        }
                        if( sql_state == "42P02" ) {
                           throw undefined_parameter( error_message, sql_state );
                        }
                        if( sql_state == "42P03" ) {
                           throw duplicate_cursor( error_message, sql_state );
                        }
                        if( sql_state == "42P04" ) {
                           throw duplicate_database( error_message, sql_state );
                        }
                        if( sql_state == "42P05" ) {
                           throw duplicate_prepared_statement( error_message, sql_state );
                        }
                        if( sql_state == "42P06" ) {
                           throw duplicate_schema( error_message, sql_state );
                        }
                        if( sql_state == "42P07" ) {
                           throw duplicate_table( error_message, sql_state );
                        }
                        if( sql_state == "42P08" ) {
                           throw ambiguous_parameter( error_message, sql_state );
                        }
                        if( sql_state == "42P09" ) {
                           throw ambiguous_alias( error_message, sql_state );
                        }
                        if( sql_state == "42P10" ) {
                           throw invalid_column_reference( error_message, sql_state );
                        }
                        if( sql_state == "42P11" ) {
                           throw invalid_cursor_definition( error_message, sql_state );
                        }
                        if( sql_state == "42P12" ) {
                           throw invalid_database_definition( error_message, sql_state );
                        }
                        if( sql_state == "42P13" ) {
                           throw invalid_function_definition( error_message, sql_state );
                        }
                        if( sql_state == "42P14" ) {
                           throw invalid_prepared_statement_definition( error_message, sql_state );
                        }
                        if( sql_state == "42P15" ) {
                           throw invalid_schema_definition( error_message, sql_state );
                        }
                        if( sql_state == "42P16" ) {
                           throw invalid_table_definition( error_message, sql_state );
                        }
                        if( sql_state == "42P17" ) {
                           throw invalid_object_definition( error_message, sql_state );
                        }
                        if( sql_state == "42P18" ) {
                           throw indeterminate_datatype( error_message, sql_state );
                        }
                        if( sql_state == "42P19" ) {
                           throw invalid_recursion( error_message, sql_state );
                        }
                        if( sql_state == "42P20" ) {
                           throw windowing_error( error_message, sql_state );
                        }
                        if( sql_state == "42P21" ) {
                           throw collation_mismatch( error_message, sql_state );
                        }
                        if( sql_state == "42P22" ) {
                           throw indeterminate_collation( error_message, sql_state );
                        }
                        throw syntax_error_or_access_rule_violation( error_message, sql_state );

                     case '4':
                        throw with_check_option_violation( error_message, sql_state );
                  }
                  break;

               case '5':
                  switch( sql_state[ 1 ] ) {
                     case '3':
                        if( sql_state == "53100" ) {
                           throw disk_full( error_message, sql_state );
                        }
                        if( sql_state == "53200" ) {
                           throw out_of_memory( error_message, sql_state );
                        }
                        if( sql_state == "53300" ) {
                           throw too_many_connections( error_message, sql_state );
                        }
                        if( sql_state == "53400" ) {
                           throw configuration_limit_exceeded( error_message, sql_state );
                        }
                        throw insufficient_resources( error_message, sql_state );

                     case '4':
                        if( sql_state == "54001" ) {
                           throw statement_too_complex( error_message, sql_state );
                        }
                        if( sql_state == "54011" ) {
                           throw too_many_columns( error_message, sql_state );
                        }
                        if( sql_state == "54023" ) {
                           throw too_many_arguments( error_message, sql_state );
                        }
                        throw program_limit_exceeded( error_message, sql_state );

                     case '5':
                        if( sql_state == "55006" ) {
                           throw object_in_use( error_message, sql_state );
                        }
                        if( sql_state == "55P02" ) {
                           throw cant_change_runtime_param( error_message, sql_state );
                        }
                        if( sql_state == "55P03" ) {
                           throw lock_not_available( error_message, sql_state );
                        }
                        if( sql_state == "55P04" ) {
                           throw unsafe_new_enum_value_usage( error_message, sql_state );
                        }
                        throw object_not_in_prerequisite_state( error_message, sql_state );

                     case '7':
                        if( sql_state == "57014" ) {
                           throw query_canceled( error_message, sql_state );
                        }
                        if( sql_state == "57P01" ) {
                           throw admin_shutdown( error_message, sql_state );
                        }
                        if( sql_state == "57P02" ) {
                           throw crash_shutdown( error_message, sql_state );
                        }
                        if( sql_state == "57P03" ) {
                           throw cannot_connect_now( error_message, sql_state );
                        }
                        if( sql_state == "57P04" ) {
                           throw database_dropped( error_message, sql_state );
                        }
                        throw operator_intervention( error_message, sql_state );

                     case '8':
                        if( sql_state == "58030" ) {
                           throw io_error( error_message, sql_state );
                        }
                        if( sql_state == "58P01" ) {
                           throw undefined_file( error_message, sql_state );
                        }
                        if( sql_state == "58P02" ) {
                           throw duplicate_file( error_message, sql_state );
                        }
                        throw system_error( error_message, sql_state );
                  }
                  break;

               case '7':
                  switch( sql_state[ 1 ] ) {
                     case '2':
                        throw snapshot_too_old( error_message, sql_state );
                  }
                  break;

               case 'F':
                  switch( sql_state[ 1 ] ) {
                     case '0':
                        if( sql_state == "F0001" ) {
                           throw lock_file_exists( error_message, sql_state );
                        }
                        throw config_file_error( error_message, sql_state );
                  }
                  break;

               case 'H':
                  switch( sql_state[ 1 ] ) {
                     case 'V':
                        if( sql_state == "HV001" ) {
                           throw fdw_out_of_memory( error_message, sql_state );
                        }
                        if( sql_state == "HV002" ) {
                           throw fdw_dynamic_parameter_value_needed( error_message, sql_state );
                        }
                        if( sql_state == "HV004" ) {
                           throw fdw_invalid_data_type( error_message, sql_state );
                        }
                        if( sql_state == "HV005" ) {
                           throw fdw_column_name_not_found( error_message, sql_state );
                        }
                        if( sql_state == "HV006" ) {
                           throw fdw_invalid_data_type_descriptors( error_message, sql_state );
                        }
                        if( sql_state == "HV007" ) {
                           throw fdw_invalid_column_name( error_message, sql_state );
                        }
                        if( sql_state == "HV008" ) {
                           throw fdw_invalid_column_number( error_message, sql_state );
                        }
                        if( sql_state == "HV009" ) {
                           throw fdw_invalid_use_of_null_pointer( error_message, sql_state );
                        }
                        if( sql_state == "HV00A" ) {
                           throw fdw_invalid_string_format( error_message, sql_state );
                        }
                        if( sql_state == "HV00B" ) {
                           throw fdw_invalid_handle( error_message, sql_state );
                        }
                        if( sql_state == "HV00C" ) {
                           throw fdw_invalid_option_index( error_message, sql_state );
                        }
                        if( sql_state == "HV00D" ) {
                           throw fdw_invalid_option_name( error_message, sql_state );
                        }
                        if( sql_state == "HV00J" ) {
                           throw fdw_option_name_not_found( error_message, sql_state );
                        }
                        if( sql_state == "HV00K" ) {
                           throw fdw_reply_handle( error_message, sql_state );
                        }
                        if( sql_state == "HV00L" ) {
                           throw fdw_unable_to_create_execution( error_message, sql_state );
                        }
                        if( sql_state == "HV00M" ) {
                           throw fdw_unable_to_create_reply( error_message, sql_state );
                        }
                        if( sql_state == "HV00N" ) {
                           throw fdw_unable_to_establish_connection( error_message, sql_state );
                        }
                        if( sql_state == "HV00P" ) {
                           throw fdw_no_schemas( error_message, sql_state );
                        }
                        if( sql_state == "HV00Q" ) {
                           throw fdw_schema_not_found( error_message, sql_state );
                        }
                        if( sql_state == "HV00R" ) {
                           throw fdw_table_not_found( error_message, sql_state );
                        }
                        if( sql_state == "HV010" ) {
                           throw fdw_function_sequence_error( error_message, sql_state );
                        }
                        if( sql_state == "HV014" ) {
                           throw fdw_too_many_handles( error_message, sql_state );
                        }
                        if( sql_state == "HV021" ) {
                           throw fdw_inconsistent_descriptor_information( error_message, sql_state );
                        }
                        if( sql_state == "HV024" ) {
                           throw fdw_invalid_attribute_value( error_message, sql_state );
                        }
                        if( sql_state == "HV090" ) {
                           throw fdw_invalid_string_length_or_buffer_length( error_message, sql_state );
                        }
                        if( sql_state == "HV091" ) {
                           throw fdw_invalid_descriptor_field_identifier( error_message, sql_state );
                        }
                        throw fdw_error( error_message, sql_state );
                  }
                  break;

               case 'P':
                  switch( sql_state[ 1 ] ) {
                     case '0':
                        if( sql_state == "P0001" ) {
                           throw raise_exception( error_message, sql_state );
                        }
                        if( sql_state == "P0002" ) {
                           throw no_data_found( error_message, sql_state );
                        }
                        if( sql_state == "P0003" ) {
                           throw too_many_rows( error_message, sql_state );
                        }
                        if( sql_state == "P0004" ) {
                           throw assert_failure( error_message, sql_state );
                        }
                        throw plpgsql_error( error_message, sql_state );
                  }
                  break;

               case 'X':
                  switch( sql_state[ 1 ] ) {
                     case 'X':
                        if( sql_state == "XX001" ) {
                           throw data_corrupted( error_message, sql_state );
                        }
                        if( sql_state == "XX002" ) {
                           throw index_corrupted( error_message, sql_state );
                        }
                        throw internal_error( error_message, sql_state );
                  }
                  break;
            }
            throw sql_error( error_message, sql_state );
            // LCOV_EXCL_STOP
      }

      const std::string res_status = PQresStatus( status );
      throw std::runtime_error( "unexpected result: " + res_status );
   }

   auto result::has_rows_affected() const noexcept -> bool
   {
      const char* str = PQcmdTuples( m_pgresult.get() );
      return str[ 0 ] != '\0';
   }

   auto result::rows_affected() const -> std::size_t
   {
      const char* str = PQcmdTuples( m_pgresult.get() );
      if( str[ 0 ] == '\0' ) {
         throw std::logic_error( "statement does not return affected rows" );
      }
      return internal::from_chars< std::size_t >( str );
   }

   auto result::name( const std::size_t column ) const -> std::string
   {
      if( column >= m_columns ) {
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
      return PQfname( m_pgresult.get(), static_cast< int >( column ) );
   }

   auto result::index( const internal::zsv in_name ) const -> std::size_t
   {
      const int column = PQfnumber( m_pgresult.get(), in_name );
      if( column < 0 ) {
         assert( column == -1 );
         check_has_result_set();
         throw std::out_of_range( "column not found: " + std::string( in_name ) );
      }
      return column;
   }

   auto result::empty() const -> bool
   {
      return size() == 0;
   }

   auto result::size() const -> std::size_t
   {
      check_has_result_set();
      return m_rows;
   }

   auto result::begin() const -> result::const_iterator
   {
      check_has_result_set();
      return const_iterator( row( *this, 0, 0, m_columns ) );
   }

   auto result::end() const -> result::const_iterator
   {
      return const_iterator( row( *this, size(), 0, m_columns ) );
   }

   auto result::is_null( const std::size_t row, const std::size_t column ) const -> bool
   {
      check_row( row );
      if( column >= m_columns ) {
         throw std::out_of_range( internal::printf( "column %zu out of range (0-%zu)", column, m_columns - 1 ) );
      }
      return PQgetisnull( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) ) != 0;
   }

   auto result::get( const std::size_t row, const std::size_t column ) const -> const char*
   {
      if( is_null( row, column ) ) {
         throw std::runtime_error( internal::printf( "unexpected NULL value in row %zu column %zu = %s", row, column, name( column ).c_str() ) );
      }
      return PQgetvalue( m_pgresult.get(), static_cast< int >( row ), static_cast< int >( column ) );
   }

   auto result::at( const std::size_t row ) const -> pq::row
   {
      check_row( row );
      return ( *this )[ row ];
   }

}  // namespace tao::pq
