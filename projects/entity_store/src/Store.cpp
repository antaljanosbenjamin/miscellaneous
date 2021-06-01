#include "Store.hpp"

#include "NestedStore.hpp"
#include "RootStore.hpp"

namespace EntityStore {

Store::Store(std::unique_ptr<IStore> store)
  : m_store{std::move(store)} {
}

Store Store::create() {
  return Store(std::make_unique<RootStore>());
}

bool Store::contains(const TodoId id) const {
  return m_store->contains(id);
}

const Properties *Store::tryGet(const TodoId id) const {
  return m_store->tryGet(id);
}

const Properties &Store::get(const TodoId id) const {
  return m_store->get(id);
}

bool Store::remove(const TodoId id) {
  return m_store->remove(id);
}

void Store::shrink() {
  m_store->shrink();
}

Store Store::createChild() {
  return Store(std::make_unique<NestedStore>(*m_store));
}

void Store::commit() {
  m_store->commit();
}

void Store::rollback() {
  m_store->rollback();
}

} // namespace EntityStore
