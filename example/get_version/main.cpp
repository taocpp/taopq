#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <tao/pq.hpp>

int main() {
   std::size_t required = 1024;
   std::vector<char> buffer(required);
   const auto err = ::getenv_s(&required, buffer.data(), buffer.size(), "PGDATABASE");
   if (err != 0) {
       std::cerr << "Error retrieving environment variable PGDATABASE" << std::endl;
       return EXIT_FAILURE;
   }
   const std::string database( buffer.data() );
   const auto connection = tao::pq::connection::create( database );
   const auto result = connection->execute( "SELECT version()" );
   std::cout << "PostgreSQL version: " << result.as< std::string >() << std::endl;
   return EXIT_SUCCESS;
}
