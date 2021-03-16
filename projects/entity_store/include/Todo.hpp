#pragma once

#include "Properties.hpp"
#include "Property.hpp"

// Design principle: The more compile time check you do, the less runtime error you get!
//
//   I. Every property of a Todo has a fixed type (the title is std::string, the
//        timestamp is double, etc.).
//  II. The id property is special, every Todo has an id which cannot be changed.
// III. Every modification on Todo objects should be done through the store, but the properties of a Todo should be
//        readable throught the Todo object itself.

namespace EntityStore {

// With "using" it will much easier to change the type of the id of Todo, if necessary.

using TodoId = int64_t;

class Todo final {
public:
  explicit Todo(const TodoId id_);
  Todo(const TodoId id_, Properties properties_);

  Todo(const Todo &) = default;
  Todo(Todo &&) = default;
  Todo &operator=(const Todo &) = default;
  Todo &operator=(Todo &&) = default;
  ~Todo() = default;

  const TodoId &id() const;

  void update(const Properties &properties_);
  void update(Properties &&properties_);

  Properties properties;

private:
  TodoId m_id;
};

} // namespace EntityStore
