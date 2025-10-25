#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

#include <tao/pq.hpp>

static std::string get_env(std::string_view name) {
#if defined(_MSC_VER)
      std::size_t required = 1024;
      std::vector<char> buffer(required);
      const auto err = ::getenv_s(&required, buffer.data(), buffer.size(), std::string(name).c_str());
      if (err != 0) {
            return {};
      }
      return std::string(buffer.data());
#else
      if (const char* v = std::getenv(std::string(name).c_str())) {
            return std::string(v);
      }
      return {};
#endif
}

int main() {
      const auto database = get_env("PGDATABASE");
      const auto connection = tao::pq::connection::create( database );
      const auto result = connection->execute( "SELECT version()" );
      std::cout << "PostgreSQL version: " << result.as< std::string >() << std::endl;
      return EXIT_SUCCESS;
}
