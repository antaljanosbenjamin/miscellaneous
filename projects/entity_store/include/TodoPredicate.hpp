#pragma once

#include <unordered_set>

#include "Properties.hpp"
#include "Todo.hpp"

namespace EntityStore {

class TodoPredicate {
public:
  TodoPredicate() = default;
  TodoPredicate(const TodoPredicate &) = default;
  TodoPredicate(TodoPredicate &&) = default;
  TodoPredicate &operator=(const TodoPredicate &) = default;
  TodoPredicate &operator=(TodoPredicate &&) = default;
  virtual ~TodoPredicate() = default;

  virtual bool operator()(const TodoId &, const Properties &properties) const = 0;
};

// The range/single query predicates store a reference to the specified value(s), because in that way they can be used
// with classes that are expensive to copy. This makes these classes more fragile, but they should be used only by
// developers who works on the internals of the store. Taking into account that these classes are just "internal
// classes", prefering performance over robustness is reasonable.
template <typename TProperty, typename TQueryValue>
class SimpleQueryTodoPredicate final : public TodoPredicate {
public:
  SimpleQueryTodoPredicate(const PropertyId propertyId, const TQueryValue &queryValue)
    : m_propertyId{propertyId}
    , m_queryValue{queryValue} {};

  bool operator()(const TodoId & /*id*/, const Properties &properties) const override {
    const auto *propertyValuePtr = properties.tryGetAs<TProperty>(m_propertyId);
    return propertyValuePtr != nullptr && *propertyValuePtr == m_queryValue;
  }

private:
  const PropertyId m_propertyId;
  const TQueryValue &m_queryValue;
};

template <typename TProperty, typename TMinQueryValue, typename TMaxQueryValue>
class RangeQueryTodoPredicate final : public TodoPredicate {
public:
  RangeQueryTodoPredicate(const PropertyId propertyId, const TMinQueryValue &minValue, const TMaxQueryValue &maxValue)
    : m_propertyId{propertyId}
    , m_minValue{minValue}
    , m_maxValue{maxValue} {};

  bool operator()(const TodoId & /*id*/, const Properties &properties) const override {
    const auto *propertyValuePtr = properties.tryGetAs<TProperty>(m_propertyId);
    if (propertyValuePtr != nullptr) {
      const auto &propertyValue = *propertyValuePtr;
      // this can happen by comparing strings and string literals
      return (propertyValue >= m_minValue && propertyValue < m_maxValue);
    }
    return false;
  }

private:
  const PropertyId m_propertyId;
  const TMinQueryValue &m_minValue;
  const TMaxQueryValue &m_maxValue;
};

class IgnoreIds final : public TodoPredicate {
public:
  explicit IgnoreIds(std::unordered_set<TodoId> ignoredIds)
    : m_ignoredIds{std::move(ignoredIds)} {
  }

  bool operator()(const TodoId &id, const Properties & /*properties*/) const override {
    return m_ignoredIds.find(id) == m_ignoredIds.end();
  }

private:
  const std::unordered_set<TodoId> m_ignoredIds;
};

template <typename TLhsPredicate, typename TRhsPredicate>
class CompositePredicate final : public TodoPredicate {
public:
  CompositePredicate(TLhsPredicate &&lhs, TRhsPredicate &&rhs)
    : m_lhs{std::forward<TLhsPredicate>(lhs)}
    , m_rhs{std::forward<TRhsPredicate>(rhs)} {
  }

  bool operator()(const TodoId &id, const Properties &properties) const override {
    return m_lhs(id, properties) && m_rhs(id, properties);
  }

private:
  TLhsPredicate m_lhs;
  TRhsPredicate m_rhs;
};

template <typename TLhsPredicate, typename TRhsPredicate>
CompositePredicate(TLhsPredicate &&lhs, TRhsPredicate &&rhs) -> CompositePredicate<TLhsPredicate, TRhsPredicate>;

template <typename TLhsPredicate, typename TRhsPredicate>
CompositePredicate<TLhsPredicate, TRhsPredicate> operator|(TLhsPredicate &&lhs, TRhsPredicate &&rhs) {
  return CompositePredicate<TLhsPredicate, TRhsPredicate>{std::forward<TLhsPredicate>(lhs),
                                                          std::forward<TRhsPredicate>(rhs)};
}

} // namespace EntityStore