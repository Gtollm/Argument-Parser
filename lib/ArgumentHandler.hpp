#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ArgParserContext.hpp"
#include "Arguments.hpp"
#include "General.hpp"

namespace ArgumentParser {

struct ParsingChainContext {
  ParsingChainContext(const std::vector<std::string>& tokens,
                      std::size_t& index)
      : tokens(tokens), index(index) {}

  enum class ArgPrefixType { SHORT, LONG, POSITIONAL };

  const std::vector<std::string>& tokens;
  std::size_t& index;
  std::string key;
  std::string value;
  ArgPrefixType type;
  std::shared_ptr<ArgumentBase> matched_argument;

  std::size_t short_index = 0;

  bool Advance() { return ++index < tokens.size(); }
  bool Back() { return index == tokens.size() - 1 ? false : --index > 0; }
  const std::string& CurrentToken() const { return tokens[index]; }
  void Reset() {
    this->key = "";
    this->value = "";
    this->matched_argument = nullptr;
  }
};

class ArgumentHandler {
 public:
  virtual ~ArgumentHandler() = default;
  virtual bool Handle(ParsingChainContext& context) = 0;
  void SetNext(std::shared_ptr<ArgumentHandler> next);

 protected:
  std::shared_ptr<ArgumentHandler> next_;
};

class LongArgHandler : public ArgumentHandler {
 public:
  explicit LongArgHandler();

  bool Handle(ParsingChainContext& context) override;
};

class ShortArgHandler : public ArgumentHandler {
 public:
  explicit ShortArgHandler();

  bool Handle(ParsingChainContext& context) override;
};

class HelpArgHandler : public ArgumentHandler {
  std::shared_ptr<Argument<bool>> helper_;

 public:
  explicit HelpArgHandler(std::shared_ptr<Argument<bool>> helper);

  bool Handle(ParsingChainContext& context) override;
};

class PositionalArgHandler : public ArgumentHandler {
 public:
  PositionalArgHandler();

  bool Handle(ParsingChainContext& context) override;
};

class EqualSeparaterArgHandler : public ArgumentHandler {
 public:
  EqualSeparaterArgHandler();

  bool Handle(ParsingChainContext& context) override;
};

class EmptySeparaterArgHandler : public ArgumentHandler {
 public:
  EmptySeparaterArgHandler();

  bool Handle(ParsingChainContext& context) override;
};

class SpaceSeparaterArgHandler : public ArgumentHandler {
 public:
  SpaceSeparaterArgHandler();

  bool Handle(ParsingChainContext& context) override;
};

class TypeArgHandler : public ArgumentHandler {
  std::shared_ptr<ParserContext> parser_context_;

 public:
  TypeArgHandler(std::shared_ptr<ParserContext> parser_context);

  bool Handle(ParsingChainContext& context) override;
};

class ValueArgHandler : public ArgumentHandler {
 public:
  ValueArgHandler();

  bool Handle(ParsingChainContext& context) override;
  void SetBranch(std::shared_ptr<ArgumentHandler> branch);

 private:
  std::shared_ptr<ArgumentHandler> branch_;
};

}  // namespace ArgumentParser

#include "ArgumentHandler.tpp"
