#include <unordered_set>

#include "ArgumentVisitor.hpp"
#include "Arguments.hpp"

void UniquenessVisitor::Visit(const ArgumentBase& argument) {
  if (auto longName = argument.GetLongName()) {
    if (!longArgs_.insert(*longName).second) {
      throw std::runtime_error("Duplicate long argument name: " + *longName);
    }
  }
  if (auto shortName = argument.GetShortName()) {
    if (!shortArgs_.insert(*shortName).second) {
      throw std::runtime_error(
          std::string("Duplicate short argument name: ") + *shortName);
    }
  }
}
