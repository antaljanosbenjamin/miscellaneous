#pragma once

#include "EntityStore/Properties.hpp"
#include "EntityStore/Property.hpp"

// Design principle: The more compile time check you do, the less runtime error you get!
//
//   I. Every property of an Entity has a fixed type (the title is std::string, the
//        timestamp is double, etc.).
//  II. The id property is special, every Entity has an id which cannot be changed.
// III. Every modification on Entity objects should be done through the store, but the properties of an Entity should be
//        readable throught the Entity object itself.

namespace EntityStore {

using EntityId = int64_t;

class Entity final {
public:
  explicit Entity(const EntityId id_);
  Entity(const EntityId id_, Properties properties_);

  Entity(const Entity &) = default;
  Entity(Entity &&) = default;
  Entity &operator=(const Entity &) = default;
  Entity &operator=(Entity &&) = default;
  ~Entity() = default;

  const EntityId &id() const;
  const Properties &properties() const &;
  Properties &&properties() &&;

  void update(const Properties &properties_);
  void update(Properties &&properties_);

private:
  EntityId m_id;
  Properties m_properties;
};

} // namespace EntityStore
