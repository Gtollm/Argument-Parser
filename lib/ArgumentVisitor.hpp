#pragma once

#include <string>
#include <unordered_set>

#include "AbstractArgument.hpp"
#include "General.hpp"

namespace ArgumentParser {

class ArgumentVisitor {
 public:
  virtual bool Visit(const AbstractArgument& argument) = 0;
  virtual ~ArgumentVisitor() = default;
};

class UniquenessVisitor : public ArgumentVisitor {
  std::unordered_set<std::string> long_args_;
  std::unordered_set<char> short_args_;

 public:
  ~UniquenessVisitor() = default;
  bool Visit(const AbstractArgument& argument) override;
};

class ValueSpecifiedVisitor : public ArgumentVisitor {
 public:
  ~ValueSpecifiedVisitor() = default;
  bool Visit(const AbstractArgument& argument) override;
};

class MultiValidVisitor : public ArgumentVisitor {
 public:
  ~MultiValidVisitor() = default;
  bool Visit(const AbstractArgument& argument) override;
};

}  // namespace ArgumentParser

#include "ArgumentVisitor.tpp"
