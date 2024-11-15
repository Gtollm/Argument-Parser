
#pragma once
#include <unordered_set>

#include "Arguments.hpp"

class ArgumentVisitor {
 public:
  virtual void Visit(const ArgumentBase& argument) = 0;
  virtual ~ArgumentVisitor() = default;
};

class UniquenessVisitor : public ArgumentVisitor {
  std::unordered_set<std::string> longArgs_;
  std::unordered_set<char> shortArgs_;

 public:
  void Visit(const ArgumentBase& argument) override;
};
