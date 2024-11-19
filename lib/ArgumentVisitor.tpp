#include <unordered_set>

#include "AbstractArgument.hpp"
#include "ArgumentVisitor.hpp"
#include "General.hpp"

namespace ArgumentParser {

bool UniquenessVisitor::Visit(const AbstractArgument& argument) {
  if (auto long_name = argument.GetLongName()) {
    if (!long_args_.insert(*long_name).second) {
      throw std::runtime_error("Duplicate long argument name: " +
                               *long_name);
      return false;
    }
  }
  if (auto shortName = argument.GetShortName()) {
    if (!short_args_.insert(*shortName).second) {
      throw std::runtime_error(
          std::string("Duplicate short argument name: ") + *shortName);
      return false;
    }
  }
  return true;
}

bool ValueSpecifiedVisitor::Visit(const AbstractArgument& argument) {
  if (argument.GetValuesNumber() > 0 || argument.GetDefaultNumber() > 0) {
    return true;
  }
  return false;
}

bool MultiValidVisitor::Visit(const AbstractArgument& argument) {
  if (!argument.GetMultivalue().has_value()) {
    return true;
  }
  if (argument.GetValuesNumber() > argument.GetMultivalue().value_or(-1) ||
      argument.GetDefaultNumber() > argument.GetMultivalue().value_or(-1)) {
    return true;
  }
  return false;
}

}  // namespace ArgumentParser
