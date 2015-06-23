#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "events.h"

using namespace events;
using namespace events::literals;

class Events :
  public EventEmitter<
    Declaration<"void1"_e, void()>,
    Declaration<"void2"_e, void()>,
    Declaration<"int"_e, void(int)>
  >
{
};

TEST_CASE("Callbacks are called when events are triggered") {
  auto count = 0;
  
  Events e;
  e.on<"void1"_e>([&count] { ++count; });

  REQUIRE(count == 0);

  e.trigger<"void1"_e>();

  REQUIRE(count == 1);
}

TEST_CASE("Correct callbacks are called when events are triggered") {
  auto void1 = 0;
  auto void2 = 0;

  Events e;
  e.on<"void1"_e>([&void1] { ++void1; });
  e.on<"void2"_e>([&void2] { ++void2; });

  REQUIRE(void1 == 0);
  REQUIRE(void2 == 0);

  e.trigger<"void1"_e>();

  REQUIRE(void1 == 1);
  REQUIRE(void2 == 0);

  e.trigger<"void2"_e>();

  REQUIRE(void1 == 1);
  REQUIRE(void2 == 1);
}

TEST_CASE("Events can be triggered with primitive type arguments") {
  auto value = 0;

  Events e;
  e.on<"int"_e>([&value](int arg) { value += arg; });

  REQUIRE(value == 0);

  e.trigger<"int"_e>(113);

  REQUIRE(value == 113);

  e.trigger<"int"_e>(-112);

  REQUIRE(value == 1);
}
