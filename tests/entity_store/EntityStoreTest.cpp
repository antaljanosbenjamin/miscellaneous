#include <numeric>
#include <set>
#include <sstream>

#include <catch2/catch.hpp>

#include "Store.hpp"
#include "TodoUtils.hpp"

using namespace EntityStore;

const Todo todo1{1, Properties().set<PropertyId::Title>("The 1 Todo")};
const Todo todo2{2, Properties().set<PropertyId::Title>("The 2 Todo").set<PropertyId::Description>("Chapter 2")};
const Todo todo3{3, Properties()
                        .set<PropertyId::Title>("The 3 Todo")
                        .set<PropertyId::Description>("Chapter 3")
                        .setAs(PropertyId::Timestamp, 3.0)};
const Todo todo4{4, Properties()
                        .set<PropertyId::Title>("The 4 Todo")
                        .set<PropertyId::Description>("Chapter 4")
                        .setAs(PropertyId::Timestamp, 4.0)};
const Todo todo5{5, Properties()
                        .set<PropertyId::Title>("The 5 Todo")
                        .set<PropertyId::Description>("Chapter 5")
                        .setAs(PropertyId::Timestamp, 5.0)};
const Todo todo6{6, Properties()
                        .set<PropertyId::Title>("The 6 Todo")
                        .set<PropertyId::Description>("Chapter 6")
                        .setAs(PropertyId::Timestamp, 6.0)};

void insertAndCheck(Store &s, const Todo &todo) {
  CHECK_FALSE(s.contains(todo.id()));
  CHECK_FALSE(s.remove(todo.id()));
  s.insert(todo.id(), todo.properties);
  CHECK(s.contains(todo.id()));
  auto props = s.get(todo.id());
  auto propsPtr = s.tryGet(todo.id());
  CHECK(props == todo.properties);
  CHECK(*propsPtr == todo.properties);
}

void removeAndCheck(Store &s, const TodoId id) {
  CHECK(s.remove(id));
  CHECK_FALSE(s.contains(id));
  auto propsPtr = s.tryGet(id);
  CHECK(propsPtr == nullptr);
  CHECK_THROWS(s.get(id));
  if (id % 2 == 1) {
    CHECK_FALSE(s.remove(id));
  }
}

void checkStoreContainsTodoWithSameIdAndTimestamp(const Store &store, const TodoId id) {
  CHECK(store.contains(id));
  auto propsPtr = store.tryGet(id);
  if (propsPtr != nullptr) {
    auto timestampPtr = propsPtr->tryGet<PropertyId::Timestamp>();
    CHECK(*timestampPtr == Approx(id));
  };
}

void checkStoreContainsMatchingTodo(const Store &s, const Todo &todo) {
  {
    // ugly hack, but handy to check insert functionality when the store already contains
    auto &mutableStore = const_cast<Store &>(s);
    CHECK_FALSE(mutableStore.insert(todo.id(), Properties()));
    Properties properties;
    CHECK_FALSE(mutableStore.insert(todo.id(), properties));
    CHECK_FALSE(mutableStore.insert(todo.id(), const_cast<const Properties &>(properties)));
  }
  CHECK(s.contains(todo.id()));
  auto tryGetResult = s.tryGet(todo.id());
  auto &getResult = s.get(todo.id());
  CHECK(*tryGetResult == getResult);
  CHECK(todo.properties == getResult);
}

void checkStoreDoesNotContainTodo(const Store &store, const TodoId id) {
  CHECK_FALSE(store.contains(id));
  CHECK(store.tryGet(id) == nullptr);
  CHECK_THROWS(store.get(id));
}

void checkEmptyStore(const Store &store, const TodoId start = 0, const TodoId end = 10) {
  for (TodoId id = start; id <= end; ++id) {
    checkStoreDoesNotContainTodo(store, id);
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

    auto checkAfterChanges = [&childCommitType, &checkOriginal, &checkUpdated](const Store &store) {
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
  Store store = Store::create();

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
  operator std::string() const {
    return std::string("CONVERTED");
  }
};

TEST_CASE("SimpleInsert") {
  const auto init = [](Store & /*store*/) {};

  auto update = [&](Store &cs) {
    insertAndCheck(cs, todo1);
    CHECK_FALSE(cs.insert(todo1.id(), todo1.properties));
    insertAndCheck(cs, todo2);
    insertAndCheck(cs, todo3);
  };

  const auto checkUpdated = [&](const Store &store) {
    checkStoreContainsMatchingTodo(store, todo1);
    checkStoreContainsMatchingTodo(store, todo2);
    checkStoreContainsMatchingTodo(store, todo3);
  };

  const auto checkOriginal = [&](const Store &store) { checkEmptyStore(store); };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("SimpleUpdate") {
  const auto init = [](Store &s) {
    insertAndCheck(s, todo1);
    insertAndCheck(s, todo2);
    insertAndCheck(s, todo3);
    insertAndCheck(s, todo4);
    insertAndCheck(s, todo5);
  };

  auto updatedTodo2 = todo2;
  auto todo2TitleUpdate = Properties().setAs(PropertyId::Title, std::string("Title changed"));
  updatedTodo2.properties.update(todo2TitleUpdate);

  auto updatedTodo3v1 = todo3;
  auto todo3TitleUpdate = Properties().setAs(PropertyId::Title, std::string("Title changed 3"));
  updatedTodo3v1.properties.update(todo3TitleUpdate);

  auto updatedTodo3v2 = updatedTodo3v1;
  auto todo3DescriptionUpdate = Properties().setAs(PropertyId::Description, std::string("Description changed 3"));
  updatedTodo3v2.properties.update(todo3DescriptionUpdate);

  auto updatedTodo5 = todo5;
  auto todo5DescriptionUpdate = Properties().setAs(PropertyId::Description, std::string("Description changed 5"));
  updatedTodo5.properties.update(todo5DescriptionUpdate);

  auto update = [&](Store &cs) {
    removeAndCheck(cs, todo1.id());

    auto resultPtr = cs.update(todo2.id(), todo2TitleUpdate);
    CHECK(*resultPtr == updatedTodo2.properties);

    resultPtr = cs.update(todo3.id(), updatedTodo3v1.properties);
    CHECK(*resultPtr == updatedTodo3v1.properties);

    resultPtr = cs.update(todo3.id(), updatedTodo3v2.properties);
    CHECK(*resultPtr == updatedTodo3v2.properties);

    removeAndCheck(cs, todo5.id());
    CHECK(cs.update(todo5.id(), updatedTodo3v2.properties) == nullptr);
    insertAndCheck(cs, updatedTodo5);
    resultPtr = cs.update(todo5.id(), Properties(updatedTodo5.properties));
    CHECK(*resultPtr == updatedTodo5.properties);

    resultPtr = cs.update(todo4.id(), Properties(updatedTodo5.properties));
    CHECK(resultPtr->get<PropertyId::Title>() == todo5.properties.get<PropertyId::Title>());
    CHECK(resultPtr->get<PropertyId::Timestamp>() == todo5.properties.get<PropertyId::Timestamp>());
    CHECK(resultPtr->get<PropertyId::Description>() == updatedTodo5.properties.get<PropertyId::Description>());
    removeAndCheck(cs, todo4.id());
    insertAndCheck(cs, todo4);
  };

  const auto checkUpdated = [&](const Store &store) {
    checkStoreDoesNotContainTodo(store, todo1.id());
    checkStoreContainsMatchingTodo(store, updatedTodo2);
    checkStoreContainsMatchingTodo(store, updatedTodo3v2);
    checkStoreContainsMatchingTodo(store, todo4);
    checkStoreContainsMatchingTodo(store, updatedTodo5);
  };

  const auto checkOriginal = [&](const Store &store) {
    checkStoreContainsMatchingTodo(store, todo1);
    checkStoreContainsMatchingTodo(store, todo2);
    checkStoreContainsMatchingTodo(store, todo3);
    checkStoreContainsMatchingTodo(store, todo4);
    checkStoreContainsMatchingTodo(store, todo5);
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("SimpleRemove") {
  const auto init = [](Store &s) {
    insertAndCheck(s, todo1);
    insertAndCheck(s, todo2);
    insertAndCheck(s, todo3);
    insertAndCheck(s, todo4);
    insertAndCheck(s, todo5);
  };

  auto update = [](Store &cs) {
    removeAndCheck(cs, todo1.id());

    removeAndCheck(cs, todo2.id());
    insertAndCheck(cs, todo2);
    CHECK(cs.update(todo2.id(), todo3.properties) != nullptr);
    removeAndCheck(cs, todo2.id());

    CHECK(cs.update(todo3.id(), todo2.properties) != nullptr);
    removeAndCheck(cs, todo3.id());
    CHECK_FALSE(cs.remove(todo3.id()));

    removeAndCheck(cs, todo4.id());
    CHECK(cs.update(todo4.id(), todo4.properties) == nullptr);
    CHECK_FALSE(cs.remove(todo4.id()));

    CHECK_FALSE(cs.insert(todo5.id(), todo5.properties));
    removeAndCheck(cs, todo5.id());

    insertAndCheck(cs, todo6);
    CHECK(cs.update(todo6.id(), todo2.properties) != nullptr);
    removeAndCheck(cs, todo6.id());
  };

  const auto checkUpdated = [](const Store &store) { checkEmptyStore(store); };

  const auto checkOriginal = [&](const Store &store) {
    checkStoreContainsMatchingTodo(store, todo1);
    checkStoreContainsMatchingTodo(store, todo2);
    checkStoreContainsMatchingTodo(store, todo3);
    checkStoreContainsMatchingTodo(store, todo4);
    checkStoreContainsMatchingTodo(store, todo5);
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("SimpleQuery") {
  constexpr TodoId todo7Id = 7;
  constexpr auto todo7Timestamp = 1.0;

  const auto init = [](Store &s) {
    insertAndCheck(s, todo1);
    insertAndCheck(s, todo2);
    insertAndCheck(s, todo3);
    insertAndCheck(s, todo4);
    insertAndCheck(s, todo5);
    insertAndCheck(s, todo6);
  };

  const auto checkOneId = [](const Store &store, TodoId id, double timestamp) {
    auto timestampResult = store.query<PropertyId::Timestamp>(timestamp);
    CHECK(timestampResult.size() == 1);
    auto it = timestampResult.begin();
    if (it != timestampResult.end()) {
      CHECK(*timestampResult.begin() == id);
    }
  };

  const auto checkUpdated = [&todo7Id, &todo7Timestamp, &checkOneId](const Store &store) {
    checkOneId(store, todo1.id(), -1);
    checkOneId(store, todo2.id(), 4);
    constexpr auto nonExistingTimestamp = 3.0;
    CHECK(store.queryAs<double>(PropertyId::Timestamp, nonExistingTimestamp).empty());
    checkOneId(store, todo4.id(), 2);
    constexpr auto anOtherNonExistingTimestamp = 5.0;
    CHECK(store.query<PropertyId::Timestamp>(anOtherNonExistingTimestamp).empty());

    auto timestamp6Result = store.query<PropertyId::Timestamp>(todo6.properties.get<PropertyId::Timestamp>());
    CHECK(timestamp6Result.size() == 2);
    CHECK(timestamp6Result.count(todo5.id()) == 1);
    CHECK(timestamp6Result.count(todo6.id()) == 1);

    checkOneId(store, todo7Id, todo7Timestamp);
  };

  const auto checkOriginal = [&checkOneId](const Store &store) {
    CHECK(store.queryAs<double>(PropertyId::Timestamp, 1.0).empty());
    CHECK(store.query<PropertyId::Timestamp>(2).empty());

    constexpr TodoId maxInsertedId = 6;
    for (auto i{3}; i <= maxInsertedId; ++i) {
      checkOneId(store, i, i);
    }
  };

  auto update = [&todo7Id, &todo7Timestamp](Store &cs) {
    const auto constProps = Properties()
                                .set<PropertyId::Timestamp>(-1)
                                .set<PropertyId::Title>("Changed Title One")
                                .setAs(PropertyId::Description, std::string("Change Description One"));
    auto resultPropsPtr = cs.update(1, constProps);
    CHECK(constProps == *resultPropsPtr);

    auto nonConstProps = Properties()
                             .set<PropertyId::Timestamp>(4)
                             .set<PropertyId::Title>("Changed Title Two")
                             .set<PropertyId::Description>("Changed Description Two");
    resultPropsPtr = cs.update(2, nonConstProps);
    REQUIRE(resultPropsPtr != nullptr);
    CHECK(nonConstProps == *resultPropsPtr);

    REQUIRE(cs.remove(todo3.id()));
    REQUIRE(cs.tryGet(todo3.id()) == nullptr);
    REQUIRE(cs.update(todo4.id(), Properties().set<PropertyId::Timestamp>(2)) != nullptr);
    REQUIRE(cs.update(todo5.id(), Properties().set<PropertyId::Timestamp>(6)) != nullptr);
    REQUIRE(cs.insert(todo7Id, Properties().set<PropertyId::Timestamp>(todo7Timestamp)));
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("RangeQuery") {
  const auto init = [](Store &s) {
    insertAndCheck(s, todo1);
    insertAndCheck(s, todo2);
    insertAndCheck(s, todo3);
    insertAndCheck(s, todo4);
    insertAndCheck(s, todo5);
    insertAndCheck(s, todo6);
  };

  constexpr auto minTimestamp = 1;
  constexpr auto maxTimestamp = 5;
  constexpr TodoId todo7Id = 7;
  const auto checkUpdated = [&minTimestamp, &maxTimestamp, &todo7Id](const Store &store) {
    auto checkQueryResult = [&todo7Id](const auto &result) {
      CHECK(result.size() == 3);
      CHECK(result.count(todo2.id()) == 1);
      CHECK(result.count(todo4.id()) == 1);
      CHECK(result.count(todo7Id) == 1);
    };
    checkQueryResult(store.rangeQuery<PropertyId::Timestamp>(minTimestamp, maxTimestamp));
    checkQueryResult(store.checkedRangeQuery<PropertyId::Timestamp>(minTimestamp, maxTimestamp));
  };

  const auto checkOriginal = [&minTimestamp, &maxTimestamp](const Store &store) {
    auto checkQueryResult = [](const auto &result) {
      CHECK(result.size() == 2);
      CHECK(result.count(todo3.id()) == 1);
      CHECK(result.count(todo4.id()) == 1);
    };
    checkQueryResult(store.rangeQueryAs<double>(PropertyId::Timestamp, minTimestamp, maxTimestamp));
    checkQueryResult(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, minTimestamp, maxTimestamp));
  };

  const auto update = [&todo7Id](Store &cs) {
    const auto constProps =
        Properties().set<PropertyId::Timestamp>(-1).setAs(PropertyId::Title, std::string("Change Title One"));
    auto resultPropsPtr = cs.update(todo1.id(), constProps);
    CHECK(constProps == *resultPropsPtr);

    auto nonConstProps = Properties().set<PropertyId::Timestamp>(4);
    resultPropsPtr = cs.update(todo2.id(), nonConstProps);
    REQUIRE(resultPropsPtr != nullptr);
    nonConstProps.set<PropertyId::Title>("The 2 Todo").set<PropertyId::Description>("Chapter 2");
    CHECK(nonConstProps == *resultPropsPtr);
    REQUIRE(cs.update(todo1.id(), Properties().set<PropertyId::Timestamp>(-1)) != nullptr);
    REQUIRE(cs.update(todo2.id(), Properties().set<PropertyId::Timestamp>(4)) != nullptr);
    REQUIRE(cs.remove(todo3.id()));
    REQUIRE(cs.update(todo4.id(), Properties().set<PropertyId::Timestamp>(2)) != nullptr);
    REQUIRE(cs.update(todo5.id(), Properties().set<PropertyId::Timestamp>(6)) != nullptr);
    REQUIRE(cs.insert(todo7Id, Properties().set<PropertyId::Timestamp>(1)));
  };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("CheckedRangeQueryViolation") {
  const auto init = [](Store & /*store*/) {};

  auto update = [&](Store &cs) { insertAndCheck(cs, todo3); };

  const auto checkUpdated = [&](const Store &store) {
    constexpr auto min = 1.0;
    constexpr auto max = 4;

    CHECK_THROWS(store.checkedRangeQuery<PropertyId::Timestamp>(min, min));
    CHECK_THROWS(store.checkedRangeQuery<PropertyId::Timestamp>(max, max));
    CHECK_THROWS(store.checkedRangeQuery<PropertyId::Timestamp>(max, min));
    const auto result = store.checkedRangeQuery<PropertyId::Timestamp>(min, max);
    CHECK(result.size() == 1);
    if (!result.empty()) {
      CHECK(*result.begin() == todo3.id());
    }

    CHECK_THROWS(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, min, min));
    CHECK_THROWS(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, max, max));
    CHECK_THROWS(store.checkedRangeQueryAs<double>(PropertyId::Timestamp, max, min));
    const auto resultAs = store.checkedRangeQueryAs<double>(PropertyId::Timestamp, min, max);
    CHECK(resultAs.size() == 1);
    if (!resultAs.empty()) {
      CHECK(*resultAs.begin() == todo3.id());
    }

    checkStoreContainsMatchingTodo(store, todo3);
  };

  const auto checkOriginal = [&](const Store &store) { checkEmptyStore(store); };

  checkConfig(init, update, checkOriginal, checkUpdated);
}

TEST_CASE("ExceptionWhenCommitting") {
  auto s = Store::create();
  auto child = s.createChild();
  auto insertAndCommit = [&s](Store cs) {
    cs.insert(todo1.id(), todo1.properties);
    s.insert(todo1.id(), todo1.properties);
    cs.commit();
  };
  CHECK_THROWS(insertAndCommit(std::move(child)));
}

TEST_CASE("Shrink") {
  auto checkShrink = [](const std::vector<TodoId> &idsToInsert, const std::vector<TodoId> &idsToRemove) {
    std::set<TodoId> inserted{idsToInsert.begin(), idsToInsert.end()};
    std::set<TodoId> removed{idsToRemove.begin(), idsToRemove.end()};
    std::set<TodoId> remaining;
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
      checkStoreContainsTodoWithSameIdAndTimestamp(s, id);
    }
    for (const auto &id: idsToRemove) {
      s.remove(id);
      checkStoreDoesNotContainTodo(s, id);
    }
    s.shrink();
    for (const auto &id: remaining) {
      checkStoreContainsTodoWithSameIdAndTimestamp(s, id);
    }
  };
  constexpr size_t insertSize = 100u;
  {
    INFO("Insert and remove all");
    std::vector<TodoId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    checkShrink(ids, ids);
  }
  {
    INFO("Insert and remove first half");
    std::vector<TodoId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<TodoId> firstHalf{ids.begin(), ids.begin() + insertSize / 2};
    checkShrink(ids, firstHalf);
  }
  {
    INFO("Insert and remove second half");
    std::vector<TodoId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<TodoId> secondHalf{ids.begin() + insertSize / 2, ids.begin() + insertSize};
    checkShrink(ids, secondHalf);
  }
  {
    INFO("Insert and remove every even elements");
    std::vector<TodoId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<TodoId> evens(insertSize / 2);
    for (auto index{0u}; index < insertSize / 2; ++index) {
      evens[index] = 2 * index;
    }
    checkShrink(ids, evens);
  }
  {
    INFO("Insert and remove every odd elements");
    std::vector<TodoId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);
    std::vector<TodoId> odds(insertSize / 2);
    for (auto index{1u}; index < insertSize / 2; ++index) {
      odds[index] = 2 * index - 1;
    }
    checkShrink(ids, odds);
  }
  {
    INFO("Insert and remove one element");
    std::vector<TodoId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);

    checkShrink(ids, {0});
    checkShrink(ids, {1});
    checkShrink(ids, {insertSize - 1});
    checkShrink(ids, {insertSize - 2});
  }
  {
    INFO("Insert and remove one element");
    std::vector<TodoId> ids(insertSize);
    std::iota(ids.begin(), ids.end(), 0);

    checkShrink(ids, {0});
    checkShrink(ids, {1});
    checkShrink(ids, {insertSize - 1});
    checkShrink(ids, {insertSize - 2});
  }
}

TEST_CASE("InsertAfterShrink") {
  constexpr TodoId numberOfTodos = 10;
  for (TodoId removedTodoId{1}; removedTodoId <= numberOfTodos; ++removedTodoId) {
    auto init = [&numberOfTodos](Store &store) {
      for (TodoId id{1u}; id <= numberOfTodos; ++id) {
        store.insert(id, Properties().set<PropertyId::Timestamp>(static_cast<double>(id)));
      }
    };

    auto update = [&removedTodoId, &numberOfTodos](Store &store) {
      store.remove(removedTodoId);
      store.shrink();
      store.insert(numberOfTodos + 1, Properties().set<PropertyId::Timestamp>(static_cast<double>(numberOfTodos + 1)));
    };

    auto checkOriginal = [&numberOfTodos](const Store &store) {
      for (TodoId id{1u}; id <= numberOfTodos; ++id) {
        checkStoreContainsTodoWithSameIdAndTimestamp(store, id);
      }
    };

    auto checkUpdated = [&removedTodoId, &numberOfTodos](const Store &store) {
      for (TodoId id{1u}; id <= numberOfTodos + 1; ++id) {
        if (id == removedTodoId) {
          checkStoreDoesNotContainTodo(store, id);
        } else {
          checkStoreContainsTodoWithSameIdAndTimestamp(store, id);
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
  auto propsForTodo1 = Properties().set<PropertyId::Title>("Title");
  const TodoId todo1Id{1};
  const TodoId todo2Id{2};
  const TodoId todo5Id{5};
  const TodoId todo9Id{9};
  const TodoId todo60Id{60};
  s.insert(todo1Id, propsForTodo1);
  s.insert(todo9Id, Properties()
                        .set<PropertyId::Title>(cts)
                        .setAs(PropertyId::Description, std::string("VeryNice"))
                        .set<PropertyId::Timestamp>(3));
  s.insert(todo2Id, Properties()
                        .setAs(PropertyId::Title, std::string("Title and title"))
                        .setAs(PropertyId::Description, std::string("This is a description"))
                        .set<PropertyId::Timestamp>(3));
  s.insert(todo5Id, Properties()
                        .set<PropertyId::Title>(std::string("Everything is awesome!"))
                        .setAs(PropertyId::Description, orNot)
                        .set<PropertyId::Timestamp>(4));
  ss << "Size: " << s.queryAs<double>(PropertyId::Timestamp, 1).size() << "\n";
  ss << "Size: " << s.rangeQuery<PropertyId::Title>("R", std::string("U")).size() << "\n";
  printTodo(ss, s, todo1Id);
  ss << '\n';
  printTodo(ss, s, todo9Id);
  ss << '\n';
  printTodo(ss, s, todo2Id);
  ss << '\n';
  printTodo(ss, s, todo5Id);

  s.remove(todo2Id);
  s.update(todo1Id,
           Properties().set<PropertyId::Description>("AdditionalDescription").set<PropertyId::Title>("Updated title"));
  s.update(4, Properties().setAs(PropertyId::Description, std::string("This has no effect, because there is no todo")));
  s.update(todo9Id, Properties()
                        .setAs(PropertyId::Description, std::string("Unfortunately..."))
                        .set<PropertyId::Description>(std::string("... the same prop can be updated multiple times")));

  printTodo(ss, s, todo1Id);
  ss << '\n';
  printTodo(ss, s, todo9Id);
  ss << '\n';
  printTodo(ss, s, todo5Id);

  {
    auto cs = s.createChild();
    ss << "1: " << cs.remove(todo1Id) << '\n';
    cs.commit();
    ss << s.contains(todo1Id) << '\n';
  }
  {
    constexpr TodoId todo12Id = 12;
    {
      auto cs = s.createChild();
      cs.insert(todo12Id, Properties().set<PropertyId::Title>("Twelve"));
    }
    printTodo(ss, s, todo12Id);
  }
  {
    auto cs = s.createChild();
    cs.remove(todo5Id);
    cs.insert(todo5Id, Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.commit();
    printTodo(ss, s, todo5Id);
  }
  {
    constexpr TodoId todo15Id = 15;
    auto cs = s.createChild();
    cs.insert(todo15Id, Properties().set<PropertyId::Title>("Shouldn't be there").set<PropertyId::Timestamp>(-3));
    cs.remove(todo15Id);
    cs.commit();
    ss << s.contains(todo15Id) << '\n';
  }
  {
    auto cs = s.createChild();
    cs.remove(todo5Id);
    cs.insert(todo5Id, Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(todo5Id);
    cs.insert(todo5Id, Properties().set<PropertyId::Title>("Only title"));
    cs.commit();
    printTodo(ss, s, todo5Id);
  }
  {
    constexpr TodoId todo6Id = 6;
    auto cs = s.createChild();
    cs.insert(todo6Id, Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(todo6Id);
    cs.insert(todo6Id, Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(todo6Id);
    ss << s.contains(todo6Id) << '\n';
  }
  {
    {
      auto cs = s.createChild();
      cs.insert(todo60Id,
                Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
      cs.remove(todo60Id);
      cs.insert(todo60Id, Properties().set<PropertyId::Title>("ShouldBeThere"));
    }
    printTodo(ss, s, todo60Id);
  }
  {
    auto cs = s.createChild();
    cs.remove(todo5Id);
    cs.insert(todo5Id, Properties().set<PropertyId::Title>("Only title and timestamp").set<PropertyId::Timestamp>(-3));
    cs.remove(todo5Id);
    cs.commit();
    ss << s.contains(todo5Id) << '\n';
  }
  {
    {
      auto cs = s.createChild();
      cs.update(todo60Id, Properties().setAs(PropertyId::Description, std::string("Road 60")));
    }
    printTodo(ss, s, todo60Id);
  }

  std::string expectedOutput = "Size: 0\n"
                               "Size: 2\n"
                               "Todo{1}\n"
                               "\ttitle => Title\n"
                               "\n"
                               "Todo{9}\n"
                               "\ttitle => CONVERTED\n"
                               "\tdescription => VeryNice\n"
                               "\ttimestamp => 3\n"
                               "\n"
                               "Todo{2}\n"
                               "\ttitle => Title and title\n"
                               "\tdescription => This is a description\n"
                               "\ttimestamp => 3\n"
                               "\n"
                               "Todo{5}\n"
                               "\ttitle => Everything is awesome!\n"
                               "\tdescription => or maybe not?\n"
                               "\ttimestamp => 4\n"
                               "Todo{1}\n"
                               "\ttitle => Updated title\n"
                               "\tdescription => AdditionalDescription\n"
                               "\n"
                               "Todo{9}\n"
                               "\ttitle => CONVERTED\n"
                               "\tdescription => ... the same prop can be updated multiple times\n"
                               "\ttimestamp => 3\n"
                               "\n"
                               "Todo{5}\n"
                               "\ttitle => Everything is awesome!\n"
                               "\tdescription => or maybe not?\n"
                               "\ttimestamp => 4\n"
                               "1: 1\n"
                               "0\n"
                               "Todo{12}\n"
                               "\ttitle => Twelve\n"
                               "Todo{5}\n"
                               "\ttitle => Only title and timestamp\n"
                               "\ttimestamp => -3\n"
                               "0\n"
                               "Todo{5}\n"
                               "\ttitle => Only title\n"
                               "0\n"
                               "Todo{60}\n"
                               "\ttitle => ShouldBeThere\n"
                               "0\n"
                               "Todo{60}\n"
                               "\ttitle => ShouldBeThere\n"
                               "\tdescription => Road 60\n";
  CHECK(expectedOutput == ss.str());
}
