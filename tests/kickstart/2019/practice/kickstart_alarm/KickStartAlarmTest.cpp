#include <catch2/catch.hpp>

#include "KickStartAlarm.hpp"

using MyInt = uint64_t;

MyInt defaultResultModulo = 1000000007;

TEST_CASE("Small") {
  MyInt N{2};
  MyInt K{3};
  MyInt x1{1};
  MyInt y1{2};
  MyInt C{1};
  MyInt D{2};
  MyInt E1{1};
  MyInt E2{1};
  MyInt F{9};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 52u);
}

TEST_CASE("Large") {
  MyInt N{10};
  MyInt K{10};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{89273};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 739786670u);
}

TEST_CASE("Huge") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{89273};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 483888854u);
}

TEST_CASE("Huge with smaller x1 different y1 C and F") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{1001};
  MyInt y1{10602};
  MyInt C{13203};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{98773};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 508935585u);
}

TEST_CASE("Huge with different F") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{42577};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 755463306u);
}

TEST_CASE("Huge with different F v2") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{32424};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 774738409u);
}

TEST_CASE("Huge with different F v3") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{22321};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 97359778u);
}

TEST_CASE("Huge with N=1") {
  MyInt N{1};
  MyInt K{1000};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{22321};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 20003000u);
}

TEST_CASE("Huge with K=1") {
  MyInt N{1000000};
  MyInt K{1};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{22321};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 873765382u);
}

TEST_CASE("Huge with E2=1") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{1};
  MyInt F{22321};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 38293317u);
}

TEST_CASE("Huge with smaller N and K=1") {
  MyInt N{1000};
  MyInt K{1};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{22321};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 711862847u);
}

TEST_CASE("Huge with biggest F") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{10001};
  MyInt y1{10002};
  MyInt C{10003};
  MyInt D{10004};
  MyInt E1{10005};
  MyInt E2{10006};
  MyInt F{99999};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 19750270u);
}

TEST_CASE("Limits") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{99991};
  MyInt y1{99992};
  MyInt C{99993};
  MyInt D{99994};
  MyInt E1{99995};
  MyInt E2{99996};
  MyInt F{99999};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, defaultResultModulo);
  CHECK(POWER_K == 614036517u);
}

TEST_CASE("Limits with resultModulo=67") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{99991};
  MyInt y1{99992};
  MyInt C{99993};
  MyInt D{99994};
  MyInt E1{99995};
  MyInt E2{99996};
  MyInt F{99999};
  MyInt resultModulo{67u};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);
  CHECK(POWER_K == 7u);
}

TEST_CASE("Limits with resultModulo=17") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{99991};
  MyInt y1{99992};
  MyInt C{99993};
  MyInt D{99994};
  MyInt E1{99995};
  MyInt E2{99996};
  MyInt F{99999};
  MyInt resultModulo{17u};
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);
  CHECK(POWER_K == 9u);
}

TEST_CASE("Limits with resultModulo=479001600") {
  MyInt N{1000000};
  MyInt K{1000};
  MyInt x1{99991};
  MyInt y1{99992};
  MyInt C{99993};
  MyInt D{99994};
  MyInt E1{99995};
  MyInt E2{99996};
  MyInt F{99999};
  MyInt resultModulo{479001600u}; // 2 * 3 * ... * 12
  MyInt POWER_K = KickStartAlarm::calculatePOWER_K(N, K, x1, y1, C, D, E1, E2, F, resultModulo);
  CHECK(POWER_K == 303022164);
}