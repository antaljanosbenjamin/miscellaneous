#pragma once

#include <memory>
#include <unordered_set>
#include <vector>

#include "Entity.hpp"
#include "EntityPredicate.hpp"
#include "IStore.hpp"
#include "Properties.hpp"
#include "StoreExceptions.hpp"

namespace EntityStore {

// This class provides checked access to the IStore interface. It translates the query calls to functor objects to
// receive the requested id-s from the IStore.
class Store {
private:
  explicit Store(std::unique_ptr<IStore> store);

public:
  // It is not copy constructible or copy assignable because of the unique_ptr that holds the IStore: it could be a
  // drawback of virtuality of IStore interface, but at the end I think it is better to not be able to copied, because
  // that can prevent a lot of bugs where a Store ends up passed by value. If it is necessary, then it can be achieved
  // either by defining a clone() method or by using other polymorphic technique, e.g.: https://github.com/ldionne/dyno
  // . As inheritance fully suited my needs I chose to use it: it is provided by the standard and well-known.

  // The main principle behind the API was the to provide a simple, but convinient way to access the Entities. Use the
  // return value for error handling instead of exceptions everywhere it is possible.

  Store() = delete;
  Store(const Store &) = delete;
  Store(Store &&) = default;
  Store &operator=(const Store &) = delete;
  Store &operator=(Store &&) = default;
  ~Store() = default;

  [[nodiscard]] static Store create();

  // Getting the Entity id as const lvalue might not make sense at first glance, but:
  //  * The entity id must always have a value => get it by value or by reference
  //  * Inside the function, it mustn't be changed => get it by const (value|reference)
  //  * On x64 machines copying an int64_t value is similar then passing it by reference, but the former one requires
  //  less indirection and maybe bigger space of compilers to optimize (because they don't have to care about aliasing
  //  etc.) => const value
  template <SameAsProperties TProperties>
  bool insert(const EntityId id, TProperties &&properties) {
    return m_store->insert(id, std::forward<TProperties>(properties));
  }

  // Some people might be freaked out when they see a raw pointer in modern C++. I (and Herb Sutter
  // https://www.youtube.com/watch?v=xnqTKD8uD64) think they are not that bad. If they aren't used for ownership
  // management, then they are good. E.g.: the mental model of a raw pointer could be a nullable refernce: it never
  // owns the pointed object similar to a reference, but it can be null in order to indicate some errors. So if the
  // return value of update is nullptr, that means there is no element with the specified id.
  template <SameAsProperties TProperties>
  const Properties *update(const EntityId id, TProperties &&properties) {
    return m_store->update(id, std::forward<TProperties>(properties));
  }
  // TODO(antaljanosbenjamin) Add functionality to delete a property

  [[nodiscard]] bool contains(const EntityId id) const;
  [[nodiscard]] const Properties *tryGet(const EntityId id) const;
  [[nodiscard]] const Properties &get(const EntityId id) const;

  bool remove(const EntityId id);

  void shrink();

  [[nodiscard]] Store createChild();

  // TODO(antaljanosbenjamin) Conceptify comparable types
  // Similarly to the setter/getter of Properties class, the query functions of this class is type checked. The ones
  // which get the property id as a template argument can offer compile time type checking, while the queryAs and
  // rangeQuery functions doesn't. In return the property id can be determined in runtime for them.
  template <PropertyId Id, typename TQueryValue>
  [[nodiscard]] std::unordered_set<EntityId> query(const TQueryValue &queryValue) const {

    return queryWithoutPropertyTypeCheck<PropertyValueType<Id>, TQueryValue>(Id, queryValue);
  }

  template <typename TProperty, typename TQueryValue>
  [[nodiscard]] std::unordered_set<EntityId> queryAs(const PropertyId propertyId, const TQueryValue &queryValue) const {
    static_assert(isPropertyMember<TProperty>(), "the requested type cannot be contained by Property");

    checkPropertyType<TProperty>(propertyId);

    return queryWithoutPropertyTypeCheck<TProperty, TQueryValue>(propertyId, queryValue);
  }

  template <PropertyId Id, typename TMinQueryValue, typename TMaxQueryValue>
  [[nodiscard]] std::unordered_set<EntityId> rangeQuery(const TMinQueryValue &minValue,
                                                        const TMaxQueryValue &maxValue) const {

    return rangeQueryWithoutPropertyTypeCheck<PropertyValueType<Id>, TMinQueryValue, TMaxQueryValue>(Id, minValue,
                                                                                                     maxValue);
  }

  // TODO(antaljanosbenjamin) Make the parameters similar to rangeQuery
  template <PropertyId Id>
  [[nodiscard]] std::unordered_set<EntityId> checkedRangeQuery(PropertyConstRefType<Id> minValue,
                                                               PropertyConstRefType<Id> maxValue) const {
    // TODO(antaljanosbenjamin) Make equal values valid
    if (minValue >= maxValue) {
      throw InvalidRangeException();
    }

    return rangeQueryWithoutPropertyTypeCheck<PropertyValueType<Id>, PropertyValueType<Id>, PropertyValueType<Id>>(
        Id, minValue, maxValue);
  }

  template <typename TProperty, typename TMinQueryValue, typename TMaxQueryValue>
  [[nodiscard]] std::unordered_set<EntityId> rangeQueryAs(const PropertyId propertyId, const TMinQueryValue &minValue,
                                                          const TMaxQueryValue &maxValue) const {
    static_assert(isPropertyMember<TProperty>(), "the requested type cannot be contained by Property");

    checkPropertyType<TProperty>(propertyId);

    return rangeQueryWithoutPropertyTypeCheck<TProperty, TMinQueryValue, TMaxQueryValue>(propertyId, minValue,
                                                                                         maxValue);
  }

  // TODO(antaljanosbenjamin) Make the parameters similar to rangeQueryAs
  template <typename TProperty>
  [[nodiscard]] std::unordered_set<EntityId> checkedRangeQueryAs(const PropertyId propertyId, const TProperty &minValue,
                                                                 const TProperty &maxValue) const {
    static_assert(isPropertyMember<TProperty>(), "the requested type cannot be contained by Property");

    checkPropertyType<TProperty>(propertyId);

    if (minValue >= maxValue) {
      throw InvalidRangeException();
    }

    return rangeQueryAs<TProperty, TProperty, TProperty>(propertyId, minValue, maxValue);
  }
  void commit();
  void rollback();

private:
  template <typename TProperty, typename TQueryValue>
  [[nodiscard]] std::unordered_set<EntityId> queryWithoutPropertyTypeCheck(const PropertyId propertyId,
                                                                           const TQueryValue &queryValue) const {
    static_assert(isPropertyMember<TProperty>(), "the requested type cannot be contained by Property");

    SimpleQueryEntityPredicate<TProperty, TQueryValue> predicate(propertyId, queryValue);
    return m_store->filterIds(predicate);
  }

  template <typename TProperty, typename TMinQueryValue, typename TMaxQueryValue>
  [[nodiscard]] std::unordered_set<EntityId> rangeQueryWithoutPropertyTypeCheck(const PropertyId propertyId,
                                                                                const TMinQueryValue &minValue,
                                                                                const TMaxQueryValue &maxValue) const {
    static_assert(isPropertyMember<TProperty>(), "the requested type cannot be contained by Property");

    RangeQueryEntityPredicate<TProperty, TMinQueryValue, TMaxQueryValue> predicate(propertyId, minValue, maxValue);
    return m_store->filterIds(predicate);
  }

  std::unique_ptr<IStore> m_store;
};

} // namespace EntityStore
