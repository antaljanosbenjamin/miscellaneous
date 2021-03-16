#include "Todo.hpp"

namespace EntityStore {

Todo::Todo(const TodoId id_)
  : properties{}
  , m_id{id_} {
}

Todo::Todo(const TodoId id_, Properties properties_)
  : properties{std::move(properties_)}
  , m_id{id_} {
}

const TodoId &Todo::id() const {
  return m_id;
}

void Todo::update(const Properties &properties_) {
  properties.update(properties_);
}

void Todo::update(Properties &&properties_) {
  properties.update(std::move(properties_));
}

} // namespace EntityStore
