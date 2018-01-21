// The Art of C++ / Utility
// Copyright (c) 2016-2018 Daniel Frey

#include "../macros.hpp"

#include <tao/utility/getenv.hpp>

int main()
{
   TEST_ASSERT( !tao::utility::getenv( "USER" ).empty() );
   TEST_THROWS( tao::utility::getenv( "TAO_DOESNOTEXIST" ) );

   TEST_ASSERT( !tao::utility::getenv( "USER", "" ).empty() );
   TEST_ASSERT( tao::utility::getenv( "TAO_DOESNOTEXIST", "" ).empty() );
   TEST_ASSERT( tao::utility::getenv( "TAO_DOESNOTEXIST", "DEFAULT VALUE" ) == "DEFAULT VALUE" );
}
