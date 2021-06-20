#include <numeric>
#include <set>
#include <sstream>

#include <catch2/catch.hpp>
#include "EntityStore/EntityUtils.hpp"
#include "EntityStore/Store.hpp"

// TODO(antaljanosbenjamin) Add proper unit tests for Property, Properties, RootStore and NestedStore
using Store = EntityStore::Store;
using Properties = EntityStore::Properties;
using PropertyId = EntityStore::PropertyId;
using Entity = EntityStore::Entity;
using EntityId = EntityStore::EntityId;

const Entity entity1{1, Properties().set<PropertyId::Title>("The 1 Entity")};
const Entity entity2{2, Properties().set<PropertyId::Title>("The 2 Entity").set<PropertyId::Description>("Chapter 2")};
const Entity entity3{3, Properties()
                            .set<PropertyId::Title>("The 3 Entity")
                            .set<PropertyId::Description>("Chapter 3")
                            .setAs(PropertyId::Timestamp, 3.0)};
const Entity entity4{4, Properties()
                            .set<PropertyId::Title>("The 4 Entity")
                            .set<PropertyId::Description>("Chapter 4")
                            .setAs(PropertyId::Timestamp, 4.0)};
const Entity entity5{5, Properties()
                            .set<PropertyId::Title>("The 5 Entity")
                            .set<PropertyId::Description>("Chapter 5")
                            .setAs(PropertyId::Timestamp, 5.0)};
const Entity entity6{6, Properties()
                            .set<PropertyId::Title>("The 6 Entity")
                            .set<PropertyId::Description>("Chapter 6")
                            .setAs(PropertyId::Timestamp, 6.0)};

void insertAndCheck(Store &s, const Entity &entity) {
  CHECK_FALSE(s.contains(entity.id()));
  CHECK_FALSE(s.remove(entity.id()));
  s.insert(entity.id(), entity.properties());
  CHECK(s.contains(entity.id()));
  auto props = s.get(entity.id());
  const auto *propsPtr = s.tryGet(entity.id());
  CHECK(props == entity.properties());
  CHECK(*propsPtr == entity.properties());
}

void removeAndCheck(Store &s, const EntityId id) {
  CHECK(s.remove(id));
  CHECK_FALSE(s.contains(id));
  const auto *propsPtr = s.tryGet(id);
  CHECK(propsPtr == nullptr);
  CHECK_THROWS(s.get(id));
  if (id % 2 == 1) {
    CHECK_FALSE(s.remove(id));
  }
}

void checkStoreContainsEntityWithSameIdAndTimestamp(const Store &store, const EntityId id) {
  CHECK(store.contains(id));
  const auto *propsPtr = store.tryGet(id);
  if (propsPtr != nullptr) {
    const auto *timestampPtr = propsPtr->tryGet<PropertyId::Timestamp>();
    CHECK(*timestampPtr == Approx(id));
  };
}

void checkStoreContainsMatchingEntity(Store &s, const Entity &entity) {
  {
    CHECK_FALSE(s.insert(entity.id(), Properties()));
    Properties properties;
    CHECK_FALSE(s.insert(entity.id(), properties));
    const auto &constProperties = properties;
    CHECK_FALSE(s.insert(entity.id(), constProperties));
  }
  CHECK(s.contains(entity.id()));
  const auto *tryGetResult = s.tryGet(entity.id());
  const auto &getResult = s.get(entity.id());
  CHECK(*tryGetResult == getResult);
  CHECK(entity.properties() == getResult);
}

void checkStoreDoesNotContainEntity(const Store &store, const EntityId id) {
  CHECK_FALSE(store.contains(id));
  CHECK(store.tryGet(id) == nullptr);
  CHECK_THROWS(store.get(id));
}

void checkEmptyStore(const Store &store, const EntityId start = 0, const EntityId end = 10) {
  for (EntityId id = start; id <= end; ++id) {
    checkStoreDoesNotContainEntity(store, id);
  }
}

enum class CommitType {
  Commit,
  CommitByDtor,
  DoubleCommit,
  Rollback,
};

template <typename TInit, typename TUpdate, typename TCheckOriginal, typename TCheckUpdated>
void checkWithChild(Store &s, const CommitType commitType, TInit &init, TUpdate &update, TCheckOriginal &checkOriginal,
                    TCheckUpdated &checkUpdated) {

  INFO("checkWithChild");
  init(s);

  {
    auto cs = s.createChild();
    update(cs);

    checkOriginal(s);
    checkUpdated(cs);
    if (commitType == CommitType::Rollback) {
      INFO("checkWithChild Rollback");
      cs.rollback();
      checkOriginal(cs);
      checkOriginal(s);
      INFO("checkWithChild Rollback with commit");
      cs.commit();
      checkOriginal(cs);
      checkOriginal(s);
    } else if (commitType == CommitType::Commit || commitType == CommitType::DoubleCommit) {
      INFO("checkWithChild Commit");
      cs.commit();
      checkUpdated(s);
      checkUpdated(cs);
    }
    if (commitType == CommitType::DoubleCommit) {
      INFO("checkWithChild DoubleCommit");
      cs.commit();
      checkUpdated(s);
      checkUpdated(cs);
    }
  }
  if (commitType != CommitType::Rollback) {
    INFO("checkWithChild CommitByDtor");
    checkUpdated(s);
  }
}

template <typename TInit, typename TUpdate, typename TCheckOriginal, typename TCheckUpdated>
void checkWithGrandChild(const CommitType commitType, TInit &init, TUpdate &update, TCheckOriginal &checkOriginal,
                         TCheckUpdated &checkUpdated) {
  auto doCheck = [&commitType, &init, &update, &checkOriginal, &checkUpdated](const CommitType childCommitType) {
    Store s = Store::create();

    auto checkAfterChanges = [&childCommitType, &checkOriginal, &checkUpdated](Store &store) {
      if (childCommitType == CommitType::Rollback) {
        checkOriginal(store);
      } else {
        checkUpdated(store);
      }
    };
    {
      Store cs = s.createChild();
      checkWithChild(cs, childCommitType, init, update, checkOriginal, checkUpdated);
      checkAfterChanges(cs);
      if (commitType == CommitType::Rollback) {
        INFO("checkWithGrandChild Rollback");
        cs.rollback();
        checkEmptyStore(cs);
        checkEmptyStore(s);
        INFO("checkWithGrandChild Rollback with commit");
        cs.commit();
        checkEmptyStore(cs);
        checkEmptyStore(s);
      } else if (commitType == CommitType::Commit || commitType == CommitType::DoubleCommit) {
        INFO("checkWithGrandChild Commit");
        cs.commit();
        checkAfterChanges(cs);
        checkAfterChanges(s);
      }
      if (commitType == CommitType::DoubleCommit) {
        INFO("checkWithGrandChild DoubleCommit");
        cs.commit();
        checkAfterChanges(s);
        checkAfterChanges(cs);
      }
    }
    if (commitType != CommitType::Rollback) {
      checkAfterChanges(s);
    }
  };
  doCheck(CommitType::Commit);
  doCheck(CommitType::CommitByDtor);
  doCheck(CommitType::DoubleCommit);
  doCheck(CommitType::Rollback);
}

template <typename TInit, typename TUpdate, typename TCheck>
void checkWithStore(TInit &init, TUpdate &update, TCheck &check) {
  auto store = Store::create();

  init(store);
  update(store);
  check(store);
}

template <typename TInit, typename TUpdate, typename TCheckOriginal, typename TCheckUpdated>
void checkConfig(TInit &init, TUpdate &update, TCheckOriginal &checkOriginal, TCheckUpdated &checkUpdated) {
  {
    INFO("checkConfig checkWithChild Commit");
    Store s = Store::create();
    checkWithChild(s, CommitType::Commit, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithChild CommitByDtor");
    Store s = Store::create();
    checkWithChild(s, CommitType::CommitByDtor, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithChild DoubleCommit");
    Store s = Store::create();
    checkWithChild(s, CommitType::DoubleCommit, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithChild Rollback");
    Store s = Store::create();
    checkWithChild(s, CommitType::Rollback, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithGrandChild Commit");
    checkWithGrandChild(CommitType::Commit, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithGrandChild CommitByDtor");
    checkWithGrandChild(CommitType::CommitByDtor, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithGrandChild DoubleCommit");
    checkWithGrandChild(CommitType::DoubleCommit, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithGrandChild Rollback");
    checkWithGrandChild(CommitType::Rollback, init, update, checkOriginal, checkUpdated);
  }
  {
    INFO("checkConfig checkWithStore");
    checkWithStore(init, update, checkUpdated);
  }
}

class ConvertibleToString {
public:
  // NOLINTNEXTLINE(google-explicit-constructor)
  operator std::string() const {
    return std::string("CONVERTED");
  }
};

TEST_CASE("SimpleInsert") {
  const auto init = [](Store & /*store*/) {};

  auto update = [&](Store &cs) {
    insertAndCheck(cs, entity1);
    CHECK_FALSE(cs.insert(entity1.id(), entity1.properties()));
    insertAndCheck(cs, entity2);
    insertAndCheck(cs, entity3);
  };

  const auto checkUpdated = [&](Store &store) {
    checkStoreContainsMatchingEntity(store, entity1);
    checkStoreContainsMatchingEntity(store, entity2);
    checkStoreContainsMatchingEntity(store, entity3);
  };

  const auto checkOriginal = [&](const Store &store) { checkEmptyStore(store); };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("SimpleUpdate") {
  const auto init = [](Store &s) {
    insertAndCheck(s, entity1);
    insertAndCheck(s, entity2);
    insertAndCheck(s, entity3);
    insertAndCheck(s, entity4);
    insertAndCheck(s, entity5);
  };

  auto updatedEntity2 = entity2;
  auto entity2TitleUpdate = Properties().setAs(PropertyId::Title, std::string("Title changed"));
  updatedEntity2.update(entity2TitleUpdate);

  auto updatedEntity3v1 = entity3;
  auto entity3TitleUpdate = Properties().setAs(PropertyId::Title, std::string("Title changed 3"));
  updatedEntity3v1.update(entity3TitleUpdate);

  auto updatedEntity3v2 = updatedEntity3v1;
  auto entity3DescriptionUpdate = Properties().setAs(PropertyId::Description, std::string("Description changed 3"));
  updatedEntity3v2.update(entity3DescriptionUpdate);

  auto updatedEntity5 = entity5;
  auto entity5DescriptionUpdate = Properties().setAs(PropertyId::Description, std::string("Description changed 5"));
  updatedEntity5.update(entity5DescriptionUpdate);

  auto update = [&](Store &cs) {
    removeAndCheck(cs, entity1.id());

    const auto *resultPtr = cs.update(entity2.id(), entity2TitleUpdate);
    CHECK(*resultPtr == updatedEntity2.properties());

    resultPtr = cs.update(entity3.id(), updatedEntity3v1.properties());
    CHECK(*resultPtr == updatedEntity3v1.properties());

    resultPtr = cs.update(entity3.id(), updatedEntity3v2.properties());
    CHECK(*resultPtr == updatedEntity3v2.properties());

    removeAndCheck(cs, entity5.id());
    CHECK(cs.update(entity5.id(), updatedEntity3v2.properties()) == nullptr);
    insertAndCheck(cs, updatedEntity5);
    resultPtr = cs.update(entity5.id(), Properties(updatedEntity5.properties()));
    CHECK(*resultPtr == updatedEntity5.properties());

    resultPtr = cs.update(entity4.id(), Properties(updatedEntity5.properties()));
    CHECK(resultPtr->get<PropertyId::Title>() == entity5.properties().get<PropertyId::Title>());
    CHECK(resultPtr->get<PropertyId::Timestamp>() == entity5.properties().get<PropertyId::Timestamp>());
    CHECK(resultPtr->get<PropertyId::Description>() == updatedEntity5.properties().get<PropertyId::Description>());
    removeAndCheck(cs, entity4.id());
    insertAndCheck(cs, entity4);
  };

  const auto checkUpdated = [&](Store &store) {
    checkStoreDoesNotContainEntity(store, entity1.id());
    checkStoreContainsMatchingEntity(store, updatedEntity2);
    checkStoreContainsMatchingEntity(store, updatedEntity3v2);
    checkStoreContainsMatchingEntity(store, entity4);
    checkStoreContainsMatchingEntity(store, updatedEntity5);
  };

  const auto checkOriginal = [&](Store &store) {
    checkStoreContainsMatchingEntity(store, entity1);
    checkStoreContainsMatchingEntity(store, entity2);
    checkStoreContainsMatchingEntity(store, entity3);
    checkStoreContainsMatchingEntity(store, entity4);
    checkStoreContainsMatchingEntity(store, entity5);
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("SimpleRemove") {
  const auto init = [](Store &s) {
    insertAndCheck(s, entity1);
    insertAndCheck(s, entity2);
    insertAndCheck(s, entity3);
    insertAndCheck(s, entity4);
    insertAndCheck(s, entity5);
  };

  auto update = [](Store &cs) {
    removeAndCheck(cs, entity1.id());

    removeAndCheck(cs, entity2.id());
    insertAndCheck(cs, entity2);
    CHECK(cs.update(entity2.id(), entity3.properties()) != nullptr);
    removeAndCheck(cs, entity2.id());

    CHECK(cs.update(entity3.id(), entity2.properties()) != nullptr);
    removeAndCheck(cs, entity3.id());
    CHECK_FALSE(cs.remove(entity3.id()));

    removeAndCheck(cs, entity4.id());
    CHECK(cs.update(entity4.id(), entity4.properties()) == nullptr);
    CHECK_FALSE(cs.remove(entity4.id()));

    CHECK_FALSE(cs.insert(entity5.id(), entity5.properties()));
    removeAndCheck(cs, entity5.id());

    insertAndCheck(cs, entity6);
    CHECK(cs.update(entity6.id(), entity2.properties()) != nullptr);
    removeAndCheck(cs, entity6.id());
  };

  const auto checkUpdated = [](const Store &store) { checkEmptyStore(store); };

  const auto checkOriginal = [&](Store &store) {
    checkStoreContainsMatchingEntity(store, entity1);
    checkStoreContainsMatchingEntity(store, entity2);
    checkStoreContainsMatchingEntity(store, entity3);
    checkStoreContainsMatchingEntity(store, entity4);
    checkStoreContainsMatchingEntity(store, entity5);
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("SimpleQuery") {
  constexpr EntityId entity7Id = 7;
  constexpr auto entity7Timestamp = 1.0;

  const auto init = [](Store &s) {
    insertAndCheck(s, entity1);
    insertAndCheck(s, entity2);
    insertAndCheck(s, entity3);
    insertAndCheck(s, entity4);
    insertAndCheck(s, entity5);
    insertAndCheck(s, entity6);
  };

  const auto checkOneId = [](const Store &store, EntityId id, double timestamp) {
    auto timestampResult = store.query<PropertyId::Timestamp>(timestamp);
    CHECK(timestampResult.size() == 1);
    auto it = timestampResult.begin();
    if (it != timestampResult.end()) {
      CHECK(*timestampResult.begin() == id);
    }
  };

  const auto checkUpdated = [&entity7Id, &entity7Timestamp, &checkOneId](const Store &store) {
    checkOneId(store, entity1.id(), -1);
    checkOneId(store, entity2.id(), 4);
    constexpr auto nonExistingTimestamp = 3.0;
    CHECK(store.queryAs<double>(PropertyId::Timestamp, nonExistingTimestamp).empty());
    checkOneId(store, entity4.id(), 2);
    constexpr auto anOtherNonExistingTimestamp = 5.0;
    CHECK(store.query<PropertyId::Timestamp>(anOtherNonExistingTimestamp).empty());

    auto timestamp6Result = store.query<PropertyId::Timestamp>(entity6.properties().get<PropertyId::Timestamp>());
    CHECK(timestamp6Result.size() == 2);
    CHECK(timestamp6Result.count(entity5.id()) == 1);
    CHECK(timestamp6Result.count(entity6.id()) == 1);

    checkOneId(store, entity7Id, entity7Timestamp);
  };

  const auto checkOriginal = [&checkOneId](const Store &store) {
    CHECK(store.queryAs<double>(PropertyId::Timestamp, 1.0).empty());
    CHECK(store.query<PropertyId::Timestamp>(2).empty());

    constexpr EntityId maxInsertedId = 6;
    for (auto i{3}; i <= maxInsertedId; ++i) {
      checkOneId(store, i, i);
    }
  };

  auto update = [&entity7Id, &entity7Timestamp](Store &cs) {
    const auto constProps = Properties()
                                .set<PropertyId::Timestamp>(-1)
                                .set<PropertyId::Title>("Changed Title One")
                                .setAs(PropertyId::Description, std::string("Change Description One"));
    const auto *resultPropsPtr = cs.update(1, constProps);
    CHECK(constProps == *resultPropsPtr);

    auto nonConstProps = Properties()
                             .set<PropertyId::Timestamp>(4)
                             .set<PropertyId::Title>("Changed Title Two")
                             .set<PropertyId::Description>("Changed Description Two");
    resultPropsPtr = cs.update(2, nonConstProps);
    REQUIRE(resultPropsPtr != nullptr);
    CHECK(nonConstProps == *resultPropsPtr);

    REQUIRE(cs.remove(entity3.id()));
    REQUIRE(cs.tryGet(entity3.id()) == nullptr);
    REQUIRE(cs.update(entity4.id(), Properties().set<PropertyId::Timestamp>(2)) != nullptr);
    REQUIRE(cs.update(entity5.id(), Properties().set<PropertyId::Timestamp>(6)) != nullptr);
    REQUIRE(cs.insert(entity7Id, Properties().set<PropertyId::Timestamp>(entity7Timestamp)));
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("RangeQuery") {
  const auto init = [](Store &s) {
    insertAndCheck(s, entity1);
    insertAndCheck(s, entity2);
    insertAndCheck(s, entity3);
    insertAndCheck(s, entity4);
    insertAndCheck(s, entity5);
    insertAndCheck(s, entity6);
  };

  constexpr auto minTimestamp = 1;
  constexpr auto maxTimestamp = 5;
  constexpr EntityId entity7Id = 7;
  const auto checkUpdated = [&minTimestamp, &maxTimestamp, &entity7Id](const Store &store) {
    auto checkQueryResult = [&entity7Id](const auto &result) {
      CHECK(result.size() == 3);
      CHECK(result.count(entity2.id()) == 1);
      CHECK(result.count(entity4.id()) == 1);
      CHECK(result.count(entity7Id) == 1);
    };
    checkQueryResult(store.rangeQuery<PropertyId::Timestamp>(minTimestamp, maxTimestamp));
    checkQueryResult(store.checkedRangeQuery<PropertyId::Timestamp>(minTimestamp, maxTimestamp));
  };

  const auto checkOriginal = [&minTimestamp, &maxTimestamp](const Store &store) {
    auto checkQueryResult = [](const auto &result) {
      CHECK(result.size() == 2);
      CHECK(result.count(entity3.id()) == 1);
      CHECK(result.count(entity4.id()) == 1);
    };
    checkQueryResult(store.rangeQueryAs<double>(PropertyId::Timestamp, minTimestamp, maxTimestamp));
    checkQueryResult(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, minTimestamp, maxTimestamp));
  };

  const auto update = [&entity7Id](Store &cs) {
    const auto constProps =
        Properties().set<PropertyId::Timestamp>(-1).setAs(PropertyId::Title, std::string("Change Title One"));
    const auto *resultPropsPtr = cs.update(entity1.id(), constProps);
    CHECK(constProps == *resultPropsPtr);

    auto nonConstProps = Properties().set<PropertyId::Timestamp>(4);
    resultPropsPtr = cs.update(entity2.id(), nonConstProps);
    REQUIRE(resultPropsPtr != nullptr);
    nonConstProps.set<PropertyId::Title>("The 2 Entity").set<PropertyId::Description>("Chapter 2");
    CHECK(nonConstProps == *resultPropsPtr);
    REQUIRE(cs.update(entity1.id(), Properties().set<PropertyId::Timestamp>(-1)) != nullptr);
    REQUIRE(cs.update(entity2.id(), Properties().set<PropertyId::Timestamp>(4)) != nullptr);
    REQUIRE(cs.remove(entity3.id()));
    REQUIRE(cs.update(entity4.id(), Properties().set<PropertyId::Timestamp>(2)) != nullptr);
    REQUIRE(cs.update(entity5.id(), Properties().set<PropertyId::Timestamp>(6)) != nullptr);
    REQUIRE(cs.insert(entity7Id, Properties().set<PropertyId::Timestamp>(1)));
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("CheckedRangeQueryViolation") {
  const auto init = [](Store & /*store*/) {};

  auto update = [&](Store &cs) { insertAndCheck(cs, entity3); };

  const auto checkUpdated = [&](Store &store) {
    constexpr auto min = 1.0;
    constexpr auto max = 4;

    CHECK_THROWS(store.checkedRangeQuery<PropertyId::Timestamp>(min, min));
    CHECK_THROWS(store.checkedRangeQuery<PropertyId::Timestamp>(max, max));
    CHECK_THROWS(store.checkedRangeQuery<PropertyId::Timestamp>(max, min));
    const auto result = store.checkedRangeQuery<PropertyId::Timestamp>(min, max);
    CHECK(result.size() == 1);
    if (!result.empty()) {
      CHECK(*result.begin() == entity3.id());
    }

    CHECK_THROWS(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, min, min));
    CHECK_THROWS(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, max, max));
    CHECK_THROWS(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, max, min));
    const auto resultAs = store.checkedRangeQueryAs<double>(PropertyId::Timestamp, min, max);
    CHECK(resultAs.size() == 1);
    if (!resultAs.empty()) {
      CHECK(*resultAs.begin() == entity3.id());
    }

    checkStoreContainsMatchingEntity(store, entity3);
  };

  const auto checkOriginal = [&](const Store &store) { checkEmptyStore(store); };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("ExceptionWhenCommitting") {
  auto s = Store::create();
  auto child = s.createChild();
  auto insertAndCommit = [&s](Store cs) {
    cs.insert(entity1.id(), entity1.properties());
    s.insert(entity1.id(), entity1.properties());
    cs.commit();
  };
  CHECK_THROWS(insertAndCommit(std::move(child)));
}

TEST_CASE("Shrink") {
  auto checkShrink = [](const std::vector<EntityId> &idsToInsert, const std::vector<EntityId> &idsToRemove) {
    std::set<EntityId> inserted{idsToInsert.begin(), idsToInsert.end()};
    std::set<EntityId> removed{idsToRemove.begin(), idsToRemove.end()};
    std::set<EntityId> remaining;
    std::set_difference(inserted.begin(), inserted.end(), removed.begin(), removed.end(),
                        std::inserter(remaining, remaining.begin()));

    CHECK(inserted.size() == idsToInsert.size());
    CHECK(removed.size() == idsToRemove.size());

    Store s = Store::create();
    for (const auto &id: idsToInsert) {
      s.insert(id, Properties().set<PropertyId::Timestamp>(static_cast<double>(id)));
    }
    s.shrink();
    for (const auto &id: idsToInsert) {
      checkStoreContainsEntityWithSameIdAndTimestamp(s, id);
    }
    for (const auto &id: idsToRemove) {
      s.remove(id);
      checkStoreDoesNotContainEntity(s, id);
    }
    s.shrink();
    for (const auto &id: remaining) {
      checkStoreContainsEntityWithSameIdAndTimestamp(s, id);
    }
  };
  constexpr size_t insertSize = 100U;
  {
    INFO("Insert and remove all");
    std::vector<EntityId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    checkShrink(ids, ids);
  }
  {
    INFO("Insert and remove first half");
    std::vector<EntityId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<EntityId> firstHalf{ids.begin(), ids.begin() + insertSize / 2};
    checkShrink(ids, firstHalf);
  }
  {
    INFO("Insert and remove second half");
    std::vector<EntityId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<EntityId> secondHalf{ids.begin() + insertSize / 2, ids.begin() + insertSize};
    checkShrink(ids, secondHalf);
  }
  {
    INFO("Insert and remove every even elements");
    std::vector<EntityId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<EntityId> evens(insertSize / 2);
    for (auto index{0U}; index < insertSize / 2; ++index) {
      evens[index] = 2 * index;
    }
    checkShrink(ids, evens);
  }
  {
    INFO("Insert and remove every odd elements");
    std::vector<EntityId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<EntityId> odds(insertSize / 2);
    for (auto index{1U}; index < insertSize / 2; ++index) {
      odds[index] = 2 * index - 1;
    }
    checkShrink(ids, odds);
  }
  {
    INFO("Insert and remove one element");
    std::vector<EntityId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);

    checkShrink(ids, {0});
    checkShrink(ids, {1});
    checkShrink(ids, {insertSize - 1});
    checkShrink(ids, {insertSize - 2});
  }
  {
    INFO("Insert and remove one element");
    std::vector<EntityId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);

    checkShrink(ids, {0});
    checkShrink(ids, {1});
    checkShrink(ids, {insertSize - 1});
    checkShrink(ids, {insertSize - 2});
  }
}

TEST_CASE("InsertAfterShrink") {
  constexpr EntityId numberOfEntities = 10;
  for (EntityId removedEntityId{1}; removedEntityId <= numberOfEntities; ++removedEntityId) {
    auto init = [&numberOfEntities](Store &store) {
      for (EntityId id{1u}; id <= numberOfEntities; ++id) {
        store.insert(id, Properties().set<PropertyId::Timestamp>(static_cast<double>(id)));
      }
    };

    auto update = [&removedEntityId, &numberOfEntities](Store &store) {
      store.remove(removedEntityId);
      store.shrink();
      store.insert(numberOfEntities + 1,
                   Properties().set<PropertyId::Timestamp>(static_cast<double>(numberOfEntities + 1)));
    };

    auto checkOriginal = [&numberOfEntities](const Store &store) {
      for (EntityId id{1u}; id <= numberOfEntities; ++id) {
        checkStoreContainsEntityWithSameIdAndTimestamp(store, id);
      }
    };

    auto checkUpdated = [&removedEntityId, &numberOfEntities](const Store &store) {
      for (EntityId id{1u}; id <= numberOfEntities + 1; ++id) {
        if (id == removedEntityId) {
          checkStoreDoesNotContainEntity(store, id);
        } else {
          checkStoreContainsEntityWithSameIdAndTimestamp(store, id);
        }
      }
    };

    checkConfig(init, update, checkOriginal, checkUpdated);
  }
}

TEST_CASE("SimpleOutput") {
  std::stringstream ss;
  std::string orNot("or maybe not?");
  Store s = Store::create();
  ConvertibleToString cts;
  auto propsForEntity1 = Properties().set<PropertyId::Title>("Title");
  const EntityId entity1Id{1};
  const EntityId entity2Id{2};
  const EntityId entity5Id{5};
  const EntityId entity9Id{9};
  const EntityId entity60Id{60};
  s.insert(entity1Id, propsForEntity1);
  s.insert(entity9Id, Properties()
                          .set<PropertyId::Title>(cts)
                          .setAs(PropertyId::Description, std::string("VeryNice"))
                          .set<PropertyId::Timestamp>(3));
  s.insert(entity2Id, Properties()
                          .setAs(PropertyId::Title, std::string("Title and title"))
                          .setAs(PropertyId::Description, std::string("This is a description"))
                          .set<PropertyId::Timestamp>(3));
  s.insert(entity5Id, Properties()
                          .set<PropertyId::Title>(std::string("Everything is awesome!"))
                          .setAs(PropertyId::Description, orNot)
                          .set<PropertyId::Timestamp>(4));
  ss << "Size: " << s.queryAs<double>(PropertyId::Timestamp, 1).size() << "\n";
  ss << "Size: " << s.rangeQuery<PropertyId::Title>("R", std::string("U")).size() << "\n";
  printEntity(ss, s, entity1Id);
  ss << '\n';
  printEntity(ss, s, entity9Id);
  ss << '\n';
  printEntity(ss, s, entity2Id);
  ss << '\n';
  printEntity(ss, s, entity5Id);

  s.remove(entity2Id);
  s.update(entity1Id,
           Properties().set<PropertyId::Description>("AdditionalDescription").set<PropertyId::Title>("Updated title"));
  s.update(4,
           Properties().setAs(PropertyId::Description, std::string("This has no effect, because there is no entity")));
  s.update(entity9Id,
           Properties()
               .setAs(PropertyId::Description, std::string("Unfortunately..."))
               .set<PropertyId::Description>(std::string("... the same prop can be updated multiple times")));

  printEntity(ss, s, entity1Id);
  ss << '\n';
  printEntity(ss, s, entity9Id);
  ss << '\n';
  printEntity(ss, s, entity5Id);

  {
    auto cs = s.createChild();
    ss << "1: " << cs.remove(entity1Id) << '\n';
    cs.commit();
    ss << s.contains(entity1Id) << '\n';
  }
  {
    constexpr EntityId entity12Id = 12;
    {
      auto cs = s.createChild();
      cs.insert(entity12Id, Properties().set<PropertyId::Title>("Twelve"));
    }
    printEntity(ss, s, entity12Id);
  }
  {
    auto cs = s.createChild();
    cs.remove(entity5Id);
    cs.insert(entity5Id,
              Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.commit();
    printEntity(ss, s, entity5Id);
  }
  {
    constexpr EntityId entity15Id = 15;
    auto cs = s.createChild();
    cs.insert(entity15Id, Properties().set<PropertyId::Title>("Shouldn't be there").set<PropertyId::Timestamp>(-3));
    cs.remove(entity15Id);
    cs.commit();
    ss << s.contains(entity15Id) << '\n';
  }
  {
    auto cs = s.createChild();
    cs.remove(entity5Id);
    cs.insert(entity5Id,
              Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(entity5Id);
    cs.insert(entity5Id, Properties().set<PropertyId::Title>("Only title"));
    cs.commit();
    printEntity(ss, s, entity5Id);
  }
  {
    constexpr EntityId entity6Id = 6;
    auto cs = s.createChild();
    cs.insert(entity6Id,
              Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(entity6Id);
    cs.insert(entity6Id,
              Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(entity6Id);
    ss << s.contains(entity6Id) << '\n';
  }
  {
    {
      auto cs = s.createChild();
      cs.insert(entity60Id,
                Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
      cs.remove(entity60Id);
      cs.insert(entity60Id, Properties().set<PropertyId::Title>("ShouldBeThere"));
    }
    printEntity(ss, s, entity60Id);
  }
  {
    auto cs = s.createChild();
    cs.remove(entity5Id);
    cs.insert(entity5Id,
              Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(entity5Id);
    cs.commit();
    ss << s.contains(entity5Id) << '\n';
  }
  {
    {
      auto cs = s.createChild();
      cs.update(entity60Id, Properties().setAs(PropertyId::Description, std::string("Road 60")));
    }
    printEntity(ss, s, entity60Id);
  }

  std::string expectedOutput = "Size: 0\n"
                               "Size: 2\n"
                               "Entity{1}\n"
                               "\ttitle => Title\n"
                               "\n"
                               "Entity{9}\n"
                               "\ttitle => CONVERTED\n"
                               "\tdescription => VeryNice\n"
                               "\ttimestamp => 3\n"
                               "\n"
                               "Entity{2}\n"
                               "\ttitle => Title and title\n"
                               "\tdescription => This is a description\n"
                               "\ttimestamp => 3\n"
                               "\n"
                               "Entity{5}\n"
                               "\ttitle => Everything is awesome!\n"
                               "\tdescription => or maybe not?\n"
                               "\ttimestamp => 4\n"
                               "Entity{1}\n"
                               "\ttitle => Updated title\n"
                               "\tdescription => AdditionalDescription\n"
                               "\n"
                               "Entity{9}\n"
                               "\ttitle => CONVERTED\n"
                               "\tdescription => ... the same prop can be updated multiple times\n"
                               "\ttimestamp => 3\n"
                               "\n"
                               "Entity{5}\n"
                               "\ttitle => Everything is awesome!\n"
                               "\tdescription => or maybe not?\n"
                               "\ttimestamp => 4\n"
                               "1: 1\n"
                               "0\n"
                               "Entity{12}\n"
                               "\ttitle => Twelve\n"
                               "Entity{5}\n"
                               "\ttitle => Only title and timestamp\n"
                               "\ttimestamp => -3\n"
                               "0\n"
                               "Entity{5}\n"
                               "\ttitle => Only title\n"
                               "0\n"
                               "Entity{60}\n"
                               "\ttitle => ShouldBeThere\n"
                               "0\n"
                               "Entity{60}\n"
                               "\ttitle => ShouldBeThere\n"
                               "\tdescription => Road 60\n";
  CHECK(expectedOutput == ss.str());
}
