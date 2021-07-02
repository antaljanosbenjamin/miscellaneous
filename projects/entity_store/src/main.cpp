#include <iomanip>
#include <iostream>

#include "EntityStore/EntityUtils.hpp"
#include "EntityStore/Properties.hpp"
#include "EntityStore/Property.hpp"
#include "EntityStore/Store.hpp"
#include "EntityStore/StoreExceptions.hpp"

using Store = EntityStore::Store;
using Properties = EntityStore::Properties;
using PropertyId = EntityStore::PropertyId;

template <typename TCollection>
void printCollection(std::ostream &os, const TCollection &collection, const std::string &prefix = "") {
  os << prefix << "{";
  if (!collection.empty()) {
    auto it = std::begin(collection);
    os << *it;
    ++it;
    while (it != std::end(collection)) {
      os << ", " << *it;
      ++it;
    }
  }
  os << "}\n";
}

void printTitle(const std::string &title) {
  std::cout << "##############################################\n";
  std::cout << title << '\n';
  std::cout << "##############################################\n\n";
}

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT(expression, postfix)                                                                                     \
  do {                                                                                                                 \
    std::cout << #expression << ';' << (postfix) << "\n";                                                              \
  } while (false)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT_AND_EXECUTE(expression)                                                                                  \
  PRINT(expression, "");                                                                                               \
  std::cout << '\n';                                                                                                   \
  expression;

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT_AND_EXECUTE_WITH_RESULT(expression)                                                                      \
  PRINT(expression, "");                                                                                               \
  do {                                                                                                                 \
    [[maybe_unused]] auto result = expression;                                                                         \
    std::cout << "  " << result << "\n\n";                                                                             \
  } while (false)

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT_AND_EXECUTE_COLLECTION(expression)                                                                       \
  PRINT(expression, " returned:");                                                                                     \
  printCollection(std::cout, (expression), "  ");                                                                      \
  std::cout << '\n';

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT_AND_EXECUTE_PROPERTIES(expression)                                                                       \
  PRINT(expression, " returned:");                                                                                     \
  printPropertiesWithPrefix(std::cout, (expression), "  ");                                                            \
  std::cout << '\n';

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT_AND_EXECUTE_BOOLEAN(expression)                                                                          \
  PRINT(expression, " is:");                                                                                           \
  std::cout << "  " << std::boolalpha << (expression) << "\n\n";

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define PRINT_AND_EXECUTE_TROWABLE(expression)                                                                         \
  PRINT(expression, " throws:");                                                                                       \
  do {                                                                                                                 \
    try {                                                                                                              \
      [[maybe_unused]] const auto &result = expression;                                                                \
    } catch (std::exception & e) {                                                                                     \
      std::cout << "  " << e.what() << "\n\n";                                                                         \
    }                                                                                                                  \
  } while (false)

void theBasicStoreExample() {
  printTitle("The basic store");

  PRINT_AND_EXECUTE(auto store = Store::create());

  std::cout << "// Pay attention to the difference between the two setter of title and description!\n";
  std::cout << "//  * The former one receives the property id as a template argument, therefore type conversion (from "
               "const chart* to std::string) and compile time type checking (e.g.: do not set a string property with a "
               "double) are possible!\n";
  std::cout << "//  * The latter one receives the property id as a runtime argument, so proper type checking cannot be "
               "done in compile time, so proper type checking is performed in runtime (exception is thrown in case of "
               "bad type)! The only possible compile time check that can be made is checking whether some of the "
               "properties can contain the specified type or not. If not, than it is definitely a wrong setter (e.g.: "
               "setAs<long long>(PropertyId::Timestamp, 3) will fail, because there is no property with type long "
               "long). It also supports type conversion if the template parameter is specified, otherwise the template "
               "parameter will be deduced from the type of the parameter.\n";
  PRINT_AND_EXECUTE_BOOLEAN(
      store.insert(2133, Properties()
                             .set<PropertyId::Title>("Darth Maul's lightsaber")
                             .setAs(PropertyId::Description, std::string("One of the best!"))
                             .setAs(PropertyId::Timestamp, 115.668))); // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE_TROWABLE(store.insert(2133, Properties().setAs<double>(PropertyId::Title, 4)));

  PRINT_AND_EXECUTE_BOOLEAN(
      store.insert(2133, Properties().set<PropertyId::Title>("The second insert will fail for the same id")));

  PRINT_AND_EXECUTE_PROPERTIES(store.get(2133));

  std::cout
      << "// It is also possible to get only one property! There are two ways to get an exact property, similarly to "
         "the setters. Note getAs<long long>(PropertyId::Timestamp) will fail for the same reason as for the setter.\n";
  PRINT_AND_EXECUTE_WITH_RESULT(store.get(2133).get<PropertyId::Title>());
  PRINT_AND_EXECUTE_WITH_RESULT(store.get(2133).getAs<std::string>(PropertyId::Title));
  PRINT_AND_EXECUTE_TROWABLE(store.get(2133).getAs<double>(PropertyId::Title));

  PRINT_AND_EXECUTE_PROPERTIES(*store.update(2133, Properties().set<PropertyId::Title>("Darth Zannah's lightsaber")));
  PRINT_AND_EXECUTE_BOOLEAN(store.update(666, Properties().set<PropertyId::Title>("This should fail!")) == nullptr);

  std::cout << "// The following contains+get requires two lookups.\n";
  PRINT_AND_EXECUTE_BOOLEAN(store.contains(2133));
  PRINT_AND_EXECUTE_PROPERTIES(store.get(2133));
  std::cout << "// The same functionality can be achieved by tryGet, which returns a pointer. If the pointer is equals "
               "to nullptr, then the store does not contain a entity with the specified value.\n";
  PRINT_AND_EXECUTE_BOOLEAN(store.tryGet(2133) == nullptr);

  PRINT_AND_EXECUTE(store.remove(2133));

  PRINT_AND_EXECUTE_BOOLEAN(store.contains(2133));
  PRINT_AND_EXECUTE_TROWABLE(store.get(2133));
  PRINT_AND_EXECUTE_BOOLEAN(store.tryGet(2133) == nullptr);
}

void queriesExample() {
  printTitle("Queries");

  PRINT_AND_EXECUTE(auto store = Store::create());

  PRINT_AND_EXECUTE(store.insert(1, Properties().set<PropertyId::Title>("Darth Zannah's lightsaber")));
  PRINT_AND_EXECUTE(store.insert(2, Properties().set<PropertyId::Title>("Darth Bane's lightsaber")));
  PRINT_AND_EXECUTE(store.insert(3, Properties()
                                        .set<PropertyId::Title>("Count Dooku's lightsaber")
                                        .set<PropertyId::Description>("It's a curved one!")));

  PRINT_AND_EXECUTE(store.insert(4, Properties().set<PropertyId::Timestamp>(4)));   // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE(store.insert(5, Properties().set<PropertyId::Timestamp>(5)));   // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE(store.insert(6, Properties().set<PropertyId::Timestamp>(666))); // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE(
      store.insert(7, Properties().set<PropertyId::Timestamp>(6.99)));            // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE(store.insert(8, Properties().set<PropertyId::Timestamp>(4))); // NOLINT(readability-magic-numbers)

  std::cout << "// The query and rangeQuery functions have two types similar to setter/getter of properties:\n";
  std::cout
      << "//  * query/rangeQuery: receives the property id as a template argument, therefore every check that must be "
         "done on the types are in done compile time (query: existence of operator== for the property's and the query "
         "parameter's type; rangeQuery: existence of operator>= for the property and the lower bound parameter's type, "
         "and operator < for property's and upper bound parameter's type). Type conversion for query parameters can be "
         "achieved by specifying additional template parameters (2nd for query, 2nd and 3rd for rangeQuery). \n";
  std::cout << "//  * queryAs/rangeQueryAs: receives the property id as a runtime argument, but requires the property "
               "type in compile time similarly to the getAs function, the performed checks are also the same. The "
               "check for necessary operators are also checked at runtime. The conversion of parameters can be "
               "achieved as the same as in case of query/rangeQuery functions.\n";
  std::cout << "// The query functions get their parameters as const&, so there is no unnecessary copy or conversion, "
               "but conversion might happen during comparison. If the conversion is expensive, then the usage of "
               "additional template  argument is recommended in order to reduce the number of conversions.\n\n";

  PRINT_AND_EXECUTE_COLLECTION(store.query<PropertyId::Title>("Master Yoda's lightsaber"));
  PRINT_AND_EXECUTE_COLLECTION(store.queryAs<std::string>(PropertyId::Title, "Darth Zannah's lightsaber"));
  PRINT_AND_EXECUTE_COLLECTION(store.queryAs<std::string>(PropertyId::Title, std::string("Count Dooku's lightsaber")));

  PRINT_AND_EXECUTE_COLLECTION(store.query<PropertyId::Description>("it doesn't exist"));
  PRINT_AND_EXECUTE_COLLECTION(store.query<PropertyId::Description>("It's a curved one!"));

  std::cout << "// Query can also return multiple ids!\n";
  PRINT_AND_EXECUTE_COLLECTION(store.query<PropertyId::Timestamp>(4.0)); // NOLINT(readability-magic-numbers)

  std::cout << "And let's see range queries!\n";
  PRINT_AND_EXECUTE_COLLECTION(store.rangeQuery<PropertyId::Timestamp>(4.0, 5)); // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE_COLLECTION(store.rangeQuery<PropertyId::Timestamp>(4.0, 7)); // NOLINT(readability-magic-numbers)

  std::cout << "// rangeQuery doesn't complain by default if the specified range is not valid. If these checks are "
               "desired by the user, then the checkedRangeQueries should be used.\n";
  PRINT_AND_EXECUTE_COLLECTION(store.rangeQuery<PropertyId::Timestamp>(4.0, 4.0)); // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE_COLLECTION(
      store.rangeQueryAs<double>(PropertyId::Timestamp, 4.0, 3.9)); // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE_TROWABLE(
      store.checkedRangeQuery<PropertyId::Timestamp>(4.0, 4.0)); // NOLINT(readability-magic-numbers)
  PRINT_AND_EXECUTE_TROWABLE(
      store.checkedRangeQueryAs<double>(PropertyId::Timestamp, 4.0, 3.9)); // NOLINT(readability-magic-numbers)
}

void childStoresExample() {
  printTitle("Child Stores");

  PRINT_AND_EXECUTE(auto store = Store::create());
  PRINT_AND_EXECUTE(std::string description2133 = "One of the best!");
  PRINT_AND_EXECUTE_BOOLEAN(store.insert(
      2133,
      Properties().set<PropertyId::Title>("Darth Maul's lightsaber").setAs(PropertyId::Description, description2133)));

  PRINT_AND_EXECUTE(Store child = store.createChild());

  PRINT_AND_EXECUTE_PROPERTIES(child.get(2133));

  PRINT_AND_EXECUTE(child.update(2133, Properties().set<PropertyId::Title>("Master Yoda's lightsaber")));

  PRINT_AND_EXECUTE_PROPERTIES(child.get(2133));

  PRINT_AND_EXECUTE_PROPERTIES(store.get(2133));

  PRINT_AND_EXECUTE_COLLECTION(child.query<PropertyId::Title>("Master Yoda's lightsaber"));

  PRINT_AND_EXECUTE(child.commit());

  PRINT_AND_EXECUTE_PROPERTIES(store.get(2133));

  // ############################################################################################
  // What would happen in a situation where the main store has two children?
  // ############################################################################################

  // First have a look at what would happend with the current implementation:

  // The meaning of different states:
  //  * Default: doesn't change anything
  //  * OnlyUpdated: only the update method was called regarding to this entity
  //  * InsertedByThis: the parent store didn't contain this entity, and the child inserted (and maybe updated after
  //  that)
  //  * ReInsertedByThis: the parent store did contain this entity, but the child removed and then inserted this entity
  //  (and maybe update sometimes)
  //  * RemovedByThis: the parent store did contain this entity, but the child removed (and maybe updated before that)

  // So let call the two child to A and B:
  //  * B Default,          A anything        : the changes from A will be done without any problem.
  //  * B OnlyUpdated,      A OnlyUpdated     : Both updates will be fine, if both updated the same property, then the
  //  latter commit will overwrite the former one.
  //  * B OnlyUpdated,      A ReInsertedByThis: If the A commited first, then the updates in B will overwrite the values
  //  inserted by A. If B committed first, then the result will be the same as committing just A.
  //  * B OnlyUpdated,      A RemovedByThis   : If A commited first, then commiting B will throw an exception. If B
  //  committed first, then A will remove the entire Entity.
  //  * B InsertedByThis,   A InsertedByThis  : The second commit will throw an exception.
  //  * B ReInsertedByThis, A ReInsertedByThis: The second commit will overwrite the other one.
  //  * B ReInsertedByThis, A RemovedByThis   : If A committed first, then committing B will throw an exception. If B
  //  committed first, the A will remove B-s changes.
  //  * B RemovedByThis,    A RemovedByThis   : The second commit will throw an exception.

  // The following situations cannot happen if they are started from the same state of the parent:
  //  * (B InsertedByThis,   A ReInsertedByThis)
  //  * (B InsertedByThis,   A RemovedByThis   )
  //  * (B OnlyUpdated,      A InsertedByThis  )

  // To summarize, sometimes it will does the correct thing, sometimes it doesn't throw an exception but does something
  // strange, and sometimes it throws an exception. This state is everything, but not consistent. The good news is, this
  // implementation doesn't support this feature, in other words having multiple childs that have pending changes to the
  // same item will result in undefind behaviour.

  // The most important thing about handling this situation properly is to not lose any data. Therefore before starting
  // the actual committing some kind of check is necessary which tells us whether the commit can be done successfully or
  // not. To do this, the store needs to maintain some kind of history about the changed elements. Based on these
  // information, a check can be implemented to decide whether the commit can be done or not. The interesting part is
  // obviously is when a conflict occur. In that case, I think the best way to go is to provide conflicting information
  // (e.g.: A removed, B updated the same version), and the user (who is a developer) of the store can implement a logic
  // how to solve the conflict. For this, the information can be per entity, or per property. The latter one offer more
  // information, and because of that it might result in a better solution. In return, it is more complex and requires
  // more memory. The former one requires simpler logic and less memory, but definitely tells less about the problem, so
  // the solution might be not as good as with the former one. In both cases, the conflict info can contain the actual
  // history chain, or just a flag that means there is a conflict with the regarding item, but don't specify which are
  // the conflicting operations. The absolute conservative solution could be to make the commit be able to fail without
  // giving any detailed information. This "solution" is not a good one, because if a conflict is detected, it is very
  // hard to do anything meaningful.
  // As a real user point of view (let assume the store is a cloud database, the user modified the same Entity on
  // different devices while they were offline) there are two real solution exists:
  //  1. Show that there is a conflict, and offer a way to save latter commited Entity as a different name/location etc.
  //  2. Show the user the conflicting information and let them decide what to do. At this point, the implementation can
  //  provide a "forced flag" while committing to ensure that the changes are saved to parent anyway.

  //  Of course, the second one is only a real solution, if the conflicting data is not to complex (remember a
  //  horrifying git merge conflict, ouch...). Knowing that an Entity can have a lot of properties, I think only the 1st
  //  one can be a real solution from the user point of view.
}

// NOLINTNEXTLINE(bugprone-exception-escape)
int main() {
  theBasicStoreExample();
  queriesExample();
  childStoresExample();
}
