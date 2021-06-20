#pragma once

#include <concepts>
#include <memory>
#include <unordered_map>
#include <utility>

#include "EntityStore/Property.hpp"

namespace EntityStore {

class DoesNotHavePropertyException : public std::out_of_range {
public:
  explicit DoesNotHavePropertyException(const std::string &propertyName);
};

// Represents a set of properties in a (key, value) format. It's interface is type safe in a way that a value identified
// by a key can hold only the type that the key is associated with. The setter/getter where the property id is a
// template parameter will cause a compile time error if one of the set/get calls don't conform the type system. On top
// of that, the setter also offer conversion to the property's type if necessary and possible. Because of this it is
// the recommended way to interact with Properties. If the runtime checked setter/getter is used where the property id
// is a usual function parameter, then an exception will thrown in case of invalid type operations.

// It does not support the text based lookups, although it can be done based on the propertyInfo array in Property.hpp.
// If this functionality is really needed, then it can be done. The reason behind this decision is in that way it is
// much clearer how to use this class, and its also reduces the possibility of runtime errors.

class Properties {
public:
  using PropertyMap = std::unordered_map<PropertyId, Property>;

  Properties() = default;
  Properties(const Properties &) = default;
  Properties(Properties &&) = default;
  Properties &operator=(const Properties &) = default;
  Properties &operator=(Properties &&) = default;
  ~Properties() = default;

  [[nodiscard]] bool hasProperty(const PropertyId propertyId) const;
  void update(const Properties &properties);
  void update(Properties &&properties);

  friend bool operator==(const Properties &lhs, const Properties &rhs);

  template <PropertyId Id>
  Properties &set(const PropertyValueType<Id> &value) {
    m_propertyMap.insert_or_assign(Id, value);
    return *this;
  }

  template <PropertyId Id>
  Properties &set(PropertyValueType<Id> &&value) {
    m_propertyMap.insert_or_assign(Id, std::move(value));
    return *this;
  }

  template <typename TProperty>
  Properties &setAs(PropertyId propertyId, TProperty &&value) {
    using TPropertyValueType = std::decay_t<TProperty>;
    static_assert(isPropertyMember<TPropertyValueType>(), "the requested type cannot be contained by Property");

    checkPropertyType<TPropertyValueType>(propertyId);

    m_propertyMap.insert_or_assign(propertyId, std::forward<TProperty>(value));
    return *this;
  }

  template <PropertyId Id>
  [[nodiscard]] PropertyConstRefType<Id> get() const {
    return getWithoutPropertyTypeCheck<PropertyValueType<Id>>(Id);
  }

  template <PropertyId Id>
  [[nodiscard]] const PropertyValueType<Id> *tryGet() const {
    return tryGetWithoutPropertyTypeCheck<PropertyValueType<Id>>(Id);
  }

  template <typename TProperty>
  [[nodiscard]] const TProperty &getAs(const PropertyId &propertyId) const {
    // TODO(antaljanosbenjamin) check if this works for arrays and function pointers
    static_assert(std::is_same_v<std::decay_t<TProperty>, TProperty>,
                  "getAs can be used only with lvalue types as template parameter!");

    checkPropertyType<TProperty>(propertyId);

    return getWithoutPropertyTypeCheck<TProperty>(propertyId);
  }

  template <typename TProperty>
  [[nodiscard]] const TProperty *tryGetAs(const PropertyId &propertyId) const {
    static_assert(std::is_same_v<std::decay_t<TProperty>, TProperty>,
                  "tryGetAs can be used only with lvalue types as template parameter!");

    checkPropertyType<TProperty>(propertyId);

    return tryGetWithoutPropertyTypeCheck<TProperty>(propertyId);
  }

  template <typename TVisitor>
  auto visit(PropertyId propertyId, TVisitor &&visitor) const {
    return std::visit(std::forward<TVisitor>(visitor), getProperty(propertyId));
  }

  template <typename TVisitor>
  void tryVisit(PropertyId propertyId, TVisitor &&visitor) {
    const auto *propertyPtr = tryGetProperty(propertyId);

    if (propertyPtr != nullptr) {
      std::visit(std::forward<TVisitor>(visitor), *propertyPtr);
    }
  }

private:
  [[nodiscard]] const Property *tryGetProperty(PropertyId propertyId) const {
    auto it = m_propertyMap.find(propertyId);
    if (it == m_propertyMap.end()) {
      return nullptr;
    }
    return &it->second;
  }

  [[nodiscard]] const Property &getProperty(PropertyId propertyId) const {
    const auto *propertyPtr = tryGetProperty(propertyId);
    if (propertyPtr == nullptr) {
      throw DoesNotHavePropertyException(getPropertyName(propertyId));
    }
    return *propertyPtr;
  }

  template <typename TProperty>
  [[nodiscard]] const TProperty *tryGetWithoutPropertyTypeCheck(const PropertyId &propertyId) const {
    static_assert(std::is_same_v<std::decay_t<TProperty>, TProperty>,
                  "tryGetWithoutPropertyTypeCheck can be used only with lvalue types as template parameter!");

    const auto *propertyPtr = tryGetProperty(propertyId);
    if (propertyPtr == nullptr) {
      return nullptr;
    }
    return &std::get<TProperty>(*propertyPtr);
  }

  template <typename TProperty>
  [[nodiscard]] const TProperty &getWithoutPropertyTypeCheck(const PropertyId &propertyId) const {
    static_assert(std::is_same_v<std::decay_t<TProperty>, TProperty>,
                  "getWithoutPropertyTypeCheck can be used only with lvalue types as template parameter!");
    // Theoretically it could hold a different type, but if the logic works fine, it should hold the exact same type.
    // In worst case, when the logic is broken and this get will throw a bad_variant_access. This is very unlikely
    // (that means a bug in a logic), so there is no need for custom exception.
    return std::get<TProperty>(getProperty(propertyId));
  }

  PropertyMap m_propertyMap;
};

template <typename T>
concept SameAsProperties = std::same_as<std::decay_t<T>, Properties>;

} // namespace EntityStore
