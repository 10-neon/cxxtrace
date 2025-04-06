#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "cxxtrace/cxxtrace.hpp"
#include "catch2/catch.hpp"

using namespace cxxtrace;

TEST_CASE( "add_one", "" ){
    REQUIRE(1 + 1 == 2);
}