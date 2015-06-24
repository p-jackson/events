#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "events.h"

using namespace events;
using namespace events::literals;

class Events :
  public EventEmitter<Events,
    Declaration<"void1"_e, void()>,
    Declaration<"void2"_e, void()>,
    Declaration<"int"_e, void(int)>
  >
{
};

TEST_CASE("Listeners are called when events are emitted") {
  auto count = 0;
  
  Events e;
  e.on<"void1"_e>([&count] { ++count; });

  REQUIRE(count == 0);

  e.emit<"void1"_e>();

  REQUIRE(count == 1);
}

TEST_CASE("`addListener` is an alias for `on`") {
  auto count = 0;

  Events e;
  e.addListener<"void1"_e>([&count] { ++count; });

  REQUIRE(count == 0);

  e.emit<"void1"_e>();

  REQUIRE(count == 1);
}

TEST_CASE("Correct listeners are called when events are emitted") {
  auto void1 = 0;
  auto void2 = 0;

  Events e;
  e.on<"void1"_e>([&void1] { ++void1; });
  e.on<"void2"_e>([&void2] { ++void2; });

  REQUIRE(void1 == 0);
  REQUIRE(void2 == 0);

  e.emit<"void1"_e>();

  REQUIRE(void1 == 1);
  REQUIRE(void2 == 0);

  e.emit<"void2"_e>();

  REQUIRE(void1 == 1);
  REQUIRE(void2 == 1);
}

TEST_CASE("Events can be emitted with primitive type arguments") {
  auto value = 0;

  Events e;
  e.on<"int"_e>([&value](int arg) { value += arg; });

  REQUIRE(value == 0);

  e.emit<"int"_e>(113);

  REQUIRE(value == 113);

  e.emit<"int"_e>(-112);

  REQUIRE(value == 1);
}

TEST_CASE("The adding of listeners can be chained") {
  auto count1 = 0;
  auto count2 = 1;

  Events e;
  e.on<"void1"_e>([&count1] { ++count1; })
   .on<"void1"_e>([&count2] { ++count2; });

  REQUIRE(count1 == 0);
  REQUIRE(count2 == 1);

  e.emit<"void1"_e>();

  REQUIRE(count1 == 1);
  REQUIRE(count2 == 2);
}

TEST_CASE("`emit` returns true if event had listeners") {
  Events e;
  e.on<"void1"_e>([] {});

  REQUIRE(e.emit<"void1"_e>());
  REQUIRE_FALSE(e.emit<"void2"_e>());

  e.on<"void2"_e>([] {});

  REQUIRE(e.emit<"void1"_e>());
  REQUIRE(e.emit<"void2"_e>());
}
