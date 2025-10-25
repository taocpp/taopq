#include <iostream>
#include <string>

#include <tao/pq.hpp>

int main()
{
   const auto connection = tao::pq::connection::create( "dbname=template1" );
   const auto result = connection->execute( "SELECT version()" );
   std::cout << result.as< std::string >() << std::endl;
   return 0;
}
