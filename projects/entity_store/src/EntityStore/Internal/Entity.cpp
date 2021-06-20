#include "EntityStore/Internal/Entity.hpp"

namespace EntityStore {

Entity::Entity(const EntityId id_)
  : m_id{id_}
  , m_properties{} {
}

Entity::Entity(const EntityId id_, Properties properties_)
  : m_id{id_}
  , m_properties{std::move(properties_)} {
}

const EntityId &Entity::id() const {
  return m_id;
}

const Properties &Entity::properties() const & {
  return m_properties;
}

Properties &&Entity::properties() && {
  return std::move(m_properties);
}

void Entity::update(const Properties &properties_) {
  m_properties.update(properties_);
}

void Entity::update(Properties &&properties_) {
  m_properties.update(std::move(properties_));
}

} // namespace EntityStore
