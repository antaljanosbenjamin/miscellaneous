#include "RootStore.hpp"

#include <cassert>

#include "StoreExceptions.hpp"

namespace EntityStore {

template <SameAsProperties TProperties>
const Properties *doUpdate(std::vector<std::optional<Entity>> &entities,
                           std::unordered_map<EntityId, size_t> &entityIndexById, const EntityId id,
                           TProperties &&properties) {
  auto it = entityIndexById.find(id);
  if (it == entityIndexById.end()) {
    return nullptr;
  }
  auto &entityHolder = entities[it->second];
  entityHolder->update(std::forward<TProperties>(properties));
  return &entityHolder->properties();
}

void doInsert(std::vector<std::optional<Entity>> &entities, std::unordered_map<EntityId, size_t> &entityIndexById,
              std::set<size_t> &emptyIndices, Entity &&entity) {
  auto insertEntity = [&](Entity &&entity) {
    size_t usedIndex = entities.size();
    entityIndexById.emplace(entity.id(), usedIndex);
    entities.push_back(std::move(entity));
  };

  auto assignEntityToEmptyIndex = [&](Entity &&entity) {
    auto firstEmptyIndex = emptyIndices.begin();
    size_t usedIndex = *firstEmptyIndex;
    emptyIndices.erase(firstEmptyIndex);
    entityIndexById.emplace(entity.id(), usedIndex);

    entities[usedIndex] = std::move(entity);
  };

  if (emptyIndices.empty()) {
    insertEntity(std::move(entity));
  } else {
    assignEntityToEmptyIndex(std::move(entity));
  }
}

template <SameAsProperties TProperties>
bool doInsert(std::vector<std::optional<Entity>> &entities, std::unordered_map<EntityId, size_t> &entityIndexById,
              std::set<size_t> &emptyIndices, const EntityId id, TProperties &&properties) {
  auto it = entityIndexById.find(id);
  if (it != entityIndexById.end()) {
    return false;
  };
  doInsert(entities, entityIndexById, emptyIndices, Entity{id, std::forward<TProperties>(properties)});
  return true;
}

bool RootStore::insert(const EntityId id, Properties &&properties) {
  return doInsert(m_entities, m_entityIndexById, m_emptyIndices, id, std::move(properties));
}

bool RootStore::insert(const EntityId id, const Properties &properties) {
  return doInsert(m_entities, m_entityIndexById, m_emptyIndices, id, properties);
}

const Properties *RootStore::update(const EntityId id, Properties &&properties) {
  return doUpdate(m_entities, m_entityIndexById, id, std::move(properties));
}

const Properties *RootStore::update(const EntityId id, const Properties &properties) {
  return doUpdate(m_entities, m_entityIndexById, id, properties);
}

bool RootStore::contains(const EntityId id) const {
  return m_entityIndexById.find(id) != m_entityIndexById.end();
}

const Properties *RootStore::tryGet(const EntityId id) const {
  auto it = m_entityIndexById.find(id);
  if (it == m_entityIndexById.end()) {
    return nullptr;
  }
  return &m_entities[it->second]->properties();
}

const Properties &RootStore::get(const EntityId id) const {
  const auto *propertiesPtr = tryGet(id);
  if (propertiesPtr == nullptr) {
    throw DoesNotHaveEntityException(id);
  }
  return *propertiesPtr;
}

bool RootStore::remove(const EntityId id) {
  auto it = m_entityIndexById.find(id);
  if (it == m_entityIndexById.end()) {
    return false;
  }

  auto index = it->second;
  m_entityIndexById.erase(it);
  m_entities[index] = std::nullopt;
  m_emptyIndices.insert(index);

  return true;
}

std::unordered_set<EntityId> RootStore::filterIds(const EntityPredicate &predicate) const {
  std::unordered_set<EntityId> result;

  for (const auto &entityHolder: m_entities) {
    if (!entityHolder.has_value()) {
      continue;
    }
    const auto &entity = *entityHolder;
    if (predicate(entity.id(), entity.properties())) {
      result.insert(entity.id());
    }
  }
  return result;
}

void RootStore::commit() {
}

void RootStore::rollback() {
}

void RootStore::shrink() {
  if (m_entities.size() == m_entityIndexById.size()) {
    return;
  }

  if (m_entityIndexById.empty()) {
    m_entities.clear();
    m_entities.shrink_to_fit();
    return;
  }

  if constexpr (std::is_nothrow_swappable_v<EntityVector::value_type>) {
    auto shrinkWithMove = [&]() {
      auto updateToNextNotEmpty = [this](EntityVector::reverse_iterator &it) {
        while (it != m_entities.rend() && !it->has_value()) {
          ++it;
        }
      };

      auto updateToNextEmpty = [this](EntityVector::iterator &it) {
        while (it != m_entities.end() && it->has_value()) {
          ++it;
        }
      };

      auto getIndexByIt = [this](EntityVector::iterator &it) { return static_cast<size_t>(it - m_entities.begin()); };

      auto emptyHolderIt = m_entities.begin();
      updateToNextEmpty(emptyHolderIt);
      auto notEmptyHolderIt = m_entities.rbegin();
      updateToNextNotEmpty(notEmptyHolderIt);
      while (emptyHolderIt < notEmptyHolderIt.base()) {
        auto newIndex = getIndexByIt(emptyHolderIt);

        m_entityIndexById.at((*notEmptyHolderIt)->id()) = newIndex;
        std::swap(*emptyHolderIt, *notEmptyHolderIt);
        updateToNextEmpty(emptyHolderIt);
        updateToNextNotEmpty(notEmptyHolderIt);
      }
      m_entities.erase(emptyHolderIt, m_entities.end());
      m_entities.shrink_to_fit();
    };
    shrinkWithMove();
  } else {
    static_assert(std::is_nothrow_swappable_v<EntityVector>, "Possible dataloss if the EntityVector::swap throws!");
    static_assert(std::is_nothrow_swappable_v<IdToIndexMap>, "Possible dataloos if the IdToIndexMap::swap throws!");
    auto shrinkWithCopy = [&] {
      EntityVector newEntities;
      newEntities.reserve(m_entityIndexById.size());
      IdToIndexMap newEntityIndexById;
      for (const auto &entityHolder: m_entities) {
        if (entityHolder.has_value()) {
          newEntityIndexById.emplace(entityHolder->id(), newEntities.size());
          newEntities.push_back(*entityHolder);
        }
      }
      m_entities.swap(newEntities);
      m_entityIndexById.swap(newEntityIndexById);
    };
    shrinkWithCopy();
  }
  assert(m_entities.size() == m_entityIndexById.size());
  m_emptyIndices.clear();
}
RootStore::Iterator RootStore::begin() {
  return m_entities.begin();
}

RootStore::Iterator RootStore::end() {
  return m_entities.end();
}

RootStore::ConstIterator RootStore::begin() const {
  return m_entities.begin();
}

RootStore::ConstIterator RootStore::end() const {
  return m_entities.end();
}

} // namespace EntityStore
