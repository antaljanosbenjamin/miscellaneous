#include <catch2/catch.hpp>

#include "KickStartAlarm.hpp"

using MyInt = uint64_t;

constexpr MyInt defaultResultModulo = 1000000007;

TEST_CASE("Small") {
  constexpr MyInt N{2};
  constexpr MyInt K{3};
  constexpr MyInt x1{1};
  constexpr MyInt y1{2};
  constexpr MyInt C{1};
  constexpr MyInt D{2};
  constexpr MyInt E1{1};
  constexpr MyInt E2{1};
  constexpr MyInt F{9};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 52U);
}

TEST_CASE("Large") {
  constexpr MyInt N{10};
  constexpr MyInt K{10};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{89273};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 739786670U);
}

TEST_CASE("Huge") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{89273};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 483888854U);
}

TEST_CASE("Huge with smaller x1 different y1 C and F") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{1001};
  constexpr MyInt y1{10602};
  constexpr MyInt C{13203};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{98773};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 508935585U);
}

TEST_CASE("Huge with different F") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{42577};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 755463306U);
}

TEST_CASE("Huge with different F v2") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{32424};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 774738409U);
}

TEST_CASE("Huge with different F v3") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{22321};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 97359778U);
}

TEST_CASE("Huge with N=1") {
  constexpr MyInt N{1};
  constexpr MyInt K{1000};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{22321};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 20003000U);
}

TEST_CASE("Huge with K=1") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{22321};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 873765382U);
}

TEST_CASE("Huge with E2=1") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{1};
  constexpr MyInt F{22321};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 38293317U);
}

TEST_CASE("Huge with smaller N and K=1") {
  constexpr MyInt N{1000};
  constexpr MyInt K{1};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{22321};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 711862847U);
}

TEST_CASE("Huge with biggest F") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{10001};
  constexpr MyInt y1{10002};
  constexpr MyInt C{10003};
  constexpr MyInt D{10004};
  constexpr MyInt E1{10005};
  constexpr MyInt E2{10006};
  constexpr MyInt F{99999};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 19750270U);
}

TEST_CASE("Limits") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{99991};
  constexpr MyInt y1{99992};
  constexpr MyInt C{99993};
  constexpr MyInt D{99994};
  constexpr MyInt E1{99995};
  constexpr MyInt E2{99996};
  constexpr MyInt F{99999};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 614036517U);
}

TEST_CASE("Limits with resultModulo=67") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{99991};
  constexpr MyInt y1{99992};
  constexpr MyInt C{99993};
  constexpr MyInt D{99994};
  constexpr MyInt E1{99995};
  constexpr MyInt E2{99996};
  constexpr MyInt F{99999};
  constexpr MyInt resultModulo{67U};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);
  CHECK(POWER_K == 7U);
}

TEST_CASE("Limits with resultModulo=17") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{99991};
  constexpr MyInt y1{99992};
  constexpr MyInt C{99993};
  constexpr MyInt D{99994};
  constexpr MyInt E1{99995};
  constexpr MyInt E2{99996};
  constexpr MyInt F{99999};
  constexpr MyInt resultModulo{17U};
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);
  CHECK(POWER_K == 9U);
}

TEST_CASE("Limits with resultModulo=479001600") {
  constexpr MyInt N{1000000};
  constexpr MyInt K{1000};
  constexpr MyInt x1{99991};
  constexpr MyInt y1{99992};
  constexpr MyInt C{99993};
  constexpr MyInt D{99994};
  constexpr MyInt E1{99995};
  constexpr MyInt E2{99996};
  constexpr MyInt F{99999};
  constexpr MyInt resultModulo{479001600U}; // 2 * 3 * ... * 12
  const MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);
  CHECK(POWER_K == 303022164U);
}