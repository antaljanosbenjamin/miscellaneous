#pragma once

#include <algorithm>
#include <array>
#include <stdexcept>
#include <string>
#include <string_view>
#include <variant>

namespace EntityStore {

// There are at least three ways to store values/objects from different types in one place:
//  * Something custom solution: the type system of C++ is really big, to create something usable and reliable thing,
//    you need to be a very experienced and knowledgeable C++ developer. I think it is important to know our
//    limitations, so that why I was sure from the first moment that I won't create a custom solution.
//  * std::any: it could be handy if the types are not known in compile time. For the EntityStore, it is not the case.
//  * std::variant: a variant can hold values/objects from different types that are known at compile time. Its interface
//    is typesafe in a way that it is impossible to get a wrong typed value/object from it in normal situation. As a
//    very good SO answer [1] suggest, "The more things you check at compile time the fewer runtime bugs you have." so
//    that's why I chose std::variant.

// [1] https://stackoverflow.com/a/56304067/6639989

//  It was very convenient to build a safe, but somewhat flexible "type system" around it: my purpose with this was to
//  make sure that the user of the store could have compile time checks if it is necessary. On the other hand, it is
//  flexible enough to use runtime logic to create, modify and query entities.

using Property = std::variant<std::string, double, const char *>;

// Drawback of type system: if the order of these enum values is wrong, then we are screwed. Really screwed. So this
// file should be modified with the biggest care. This file is the heart and brain of the type system at the same time.
// I implemented a few checks that will produce compile time errors if the enums/type descriptor contains any extra
// values, but to detect whether the order of the enums are wrong or not is really hard. But, fortunately with proper
// tests we can assure that they are fine.

// To extend the Entity class with a property:
//  * If the new property has a different type then Property can already hold, then:
//    * Add a new type to the Property type definition above
//    * The corresponding enum value must be added to the PropertyType enumeration (update the _LAST value)
//  * A new value must be added to the PropertyId enumeration to represent the new property (update the _LAST value)
//  * The propertyInfos array must be extended with the new property's infos (compile time checked)
//  * A new PropertyTypeDescriptor must be created (compile time checked)
// And that's it! Every other stuff is handled by the framework.

enum class PropertyType : size_t {
  String = 0,
  Double,
  ConstCharPtr,
  LAST = ConstCharPtr, // etc.
};

// The continuity of these enums are essential in order to make the "type system" work.
enum class PropertyId : size_t {
  Title = 0,
  Description,
  Timestamp,
  CStyledString,
  LAST = CStyledString,
};

template <typename T, size_t index = 0>
constexpr size_t propertyTypeIndex() {
  if constexpr (index == std::variant_size_v<Property>) { // NOLINT(bugprone-branch-clone)
    return index;
  } else if constexpr (std::is_same_v<std::variant_alternative_t<index, Property>, T>) {
    return index;
  } else {
    return propertyTypeIndex<T, index + 1>();
  }
}

template <typename T>
constexpr bool isPropertyMember() {
  return propertyTypeIndex<T>() < std::variant_size_v<Property>;
}

template <typename TEnum>
constexpr std::underlying_type_t<TEnum> asUnderlying(const TEnum &value) {
  return static_cast<std::underlying_type_t<TEnum>>(value);
}

static_assert(
    std::variant_size_v<Property> == asUnderlying(PropertyType::LAST) + 1,
    "std::variant representing Property must have as many types as the maximum value of PropertyType enumeration!");

template <PropertyType type>
struct DescriptorBase {
  using ValueType = std::variant_alternative_t<asUnderlying(type), Property>;
  using ConstRefType = const ValueType &;
  static constexpr PropertyType propertyType = type;
};

template <PropertyId>
struct PropertyDescriptor {};

template <>
struct PropertyDescriptor<PropertyId::Title> : DescriptorBase<PropertyType::String> {};

template <>
struct PropertyDescriptor<PropertyId::Description> : DescriptorBase<PropertyType::String> {};

template <>
struct PropertyDescriptor<PropertyId::Timestamp> : DescriptorBase<PropertyType::Double> {};

template <>
struct PropertyDescriptor<PropertyId::CStyledString> : DescriptorBase<PropertyType::ConstCharPtr> {};

template <PropertyId Id>
using PropertyValueType = typename PropertyDescriptor<Id>::ValueType;

template <PropertyId Id>
using PropertyConstRefType = typename PropertyDescriptor<Id>::ConstRefType;

template <PropertyId Id, typename = void>
struct HasPropertyDescriptor : std::false_type {};

template <PropertyId Id>
struct HasPropertyDescriptor<Id, std::void_t<decltype(PropertyDescriptor<Id>::propertyType)>> : std::true_type {};

template <PropertyId Id>
struct AreAllNotGreaterPropertyDescriptorDefined
  : std::enable_if_t<HasPropertyDescriptor<Id>::value,
                     AreAllNotGreaterPropertyDescriptorDefined<static_cast<PropertyId>(asUnderlying(Id) - 1)>> {};

template <>
struct AreAllNotGreaterPropertyDescriptorDefined<static_cast<PropertyId>(0)>
  : HasPropertyDescriptor<static_cast<PropertyId>(0)> {};

using AreAllPropertyDescriptorDefined = AreAllNotGreaterPropertyDescriptorDefined<PropertyId::LAST>;

static_assert(AreAllPropertyDescriptorDefined::value, "not all property descriptor is defined!");

struct PropertyInfo {
  std::string_view name;
  PropertyType type;
};

constexpr std::array<PropertyInfo, asUnderlying(PropertyId::LAST) + 1> propertyInfos = {
    PropertyInfo{"title", PropertyDescriptor<PropertyId::Title>::propertyType},
    PropertyInfo{"description", PropertyDescriptor<PropertyId::Description>::propertyType},
    PropertyInfo{"timestamp", PropertyDescriptor<PropertyId::Timestamp>::propertyType},
    PropertyInfo{"cstyledstring", PropertyDescriptor<PropertyId::CStyledString>::propertyType},
};

static_assert(asUnderlying(PropertyId::LAST) + 1 == std::size(propertyInfos),
              "The propertyInfos array does not contain exactly the same number of elements as the PropertyId "
              "enumeration! Check the PropertyId::LAST and the propertyInfos array!");

static_assert(!propertyInfos.back().name.empty(), "propertyInfos is probably not fully initialized!");

constexpr bool areAllPropertyTypeUsed() {
  std::array<bool, asUnderlying(PropertyType::LAST) + 1> isTypeUsed{};
  std::fill(isTypeUsed.begin(), isTypeUsed.end(), false);

  for (std::underlying_type_t<PropertyId> index{0U}; index <= asUnderlying(PropertyId::LAST); ++index) {
    isTypeUsed[asUnderlying(propertyInfos[index].type)] = true;
  }
  for (std::underlying_type_t<PropertyType> index{0U}; index <= asUnderlying(PropertyType::LAST); ++index) {
    if (!isTypeUsed[index]) {
      return false;
    }
  }
  return true;
}

static_assert(areAllPropertyTypeUsed(), "not all value of PropertyType is used!");

constexpr PropertyType getPropertyType(const PropertyId &propertyId) {
  return propertyInfos[asUnderlying(propertyId)].type;
}

constexpr std::string_view getPropertyName(const PropertyId &propertyId) {
  return propertyInfos[asUnderlying(propertyId)].name;
}

template <typename T>
constexpr bool doesTypeMatchProperty(const PropertyId &propertyId) {
  return static_cast<PropertyType>(propertyTypeIndex<T>()) == getPropertyType(propertyId);
}

class InvalidPropertyTypeException : public std::runtime_error {
public:
  explicit InvalidPropertyTypeException(const std::string_view propertyName);
};

template <typename TProperty>
void checkPropertyType(PropertyId propertyId) {
  if (!doesTypeMatchProperty<TProperty>(propertyId)) {
    throw InvalidPropertyTypeException(getPropertyName(propertyId));
  }
}
} // namespace EntityStore
