#include <concepts>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "ArgParser.hpp"
#include "ArgParserContext.hpp"
#include "ArgumentBuilder.hpp"
#include "ArgumentHandler.hpp"
#include "ArgumentVisitor.hpp"
#include "Arguments.hpp"
#include "General.hpp"

namespace ArgumentParser {

ArgParser::ArgParser() : unique_visitor_({}) {
  this->parser_chain_ = BuildHandlerChain(this->context_);
  this->context_ = std::make_shared<ParserContext>();

  this->argument_types_ =
      std::make_shared<std::unordered_map<std::string, std::string>>(
          std::unordered_map<std::string, std::string>(
              {{typeid(int).name(), "Int"},
               {typeid(std::string).name(), "String"},
               {typeid(double).name(), "Double"},
               {typeid(long).name(), "Long"},
               {typeid(long long).name(), "LongLong"},
               {typeid(unsigned int).name(), "UnsignedInt"},
               {typeid(unsigned long).name(), "UnsignedLong"},
               {typeid(unsigned long long).name(), "UnsignedLongLong"},
               {typeid(char).name(), "Char"},
               {typeid(bool).name(), "Bool"},
               {typeid(unsigned char).name(), "UnsignedChar"}}));
}

ArgParser::ArgParser(const std::string_view& name)
    : ArgumentParser::ArgParser() {
  this->name_ = std::string(name);
}

ArgParser::ArgParser(const ArgParser& other)
    : name_(other.name_),
      unique_visitor_(other.unique_visitor_),
      builder_(other.builder_),
      context_(other.context_),
      parser_chain_(other.parser_chain_),
      help_argument_(other.help_argument_),
      argument_types_(other.argument_types_) {}

ArgParser& ArgParser::operator=(const ArgParser& parser) {
  if (this == &parser) return *this;
  name_ = parser.name_;
  unique_visitor_ = parser.unique_visitor_;
  builder_ = parser.builder_;
  context_ = parser.context_;
  parser_chain_ = parser.parser_chain_;
  help_argument_ = parser.help_argument_;
  argument_types_ = parser.argument_types_;
  this->Reset();
  return *this;
}

ArgParser::ArgParser(ArgParser&& other) noexcept
    : name_(std::move(other.name_)),
      unique_visitor_(std::move(other.unique_visitor_)),
      builder_(std::move(other.builder_)),
      context_(std::move(other.context_)),
      parser_chain_(std::move(other.parser_chain_)),
      help_argument_(std::move(other.help_argument_)),
      argument_types_(std::move(other.argument_types_)) {
  this->Reset();
}

ArgParser& ArgParser::operator=(ArgParser&& other) noexcept {
  if (this == &other) return *this;
  name_ = std::move(other.name_);
  unique_visitor_ = std::move(other.unique_visitor_);
  builder_ = std::move(other.builder_);
  context_ = std::move(other.context_);
  parser_chain_ = std::move(other.parser_chain_);
  help_argument_ = std::move(other.help_argument_);
  argument_types_ = std::move(other.argument_types_);

  return *this;
}

bool ArgParser::Parse(const std::vector<std::string>& tokens) {
  this->Reset();
  size_t index = 1;
  ParsingChainContext context{tokens, index};

  while (index < tokens.size()) {
    if (!parser_chain_->Handle(context)) {
      return false;
    }
    ++index;
    context.Reset();
  }
  if (this->Help()) {
    return true;
  }
  bool valid = true;
  ValueSpecifiedVisitor value_visitor;
  MultiValidVisitor multi_visitor;
  for (auto& [_, arg] : context_->GetLongArguments()) {
    valid &= arg->Accept(value_visitor) && arg->Accept(multi_visitor);
  }

  for (auto& [_, arg] : context_->GetShortArguments()) {
    valid &= arg->Accept(value_visitor) && arg->Accept(multi_visitor);
  }
  for (auto& arg : context_->GetPosArguments()) {
    valid &= arg->Accept(value_visitor) && arg->Accept(multi_visitor);
  }
  return valid;
}

bool ArgParser::Parse(int argc, char** argv) {
  std::vector<std::string> args = ArgvToVec(argc, argv);
  return this->Parse(args);
}

void ArgParser::Reset() {
  this->parser_chain_ = BuildHandlerChain(this->context_);

  for (auto& [_, arg] : this->context_->GetLongArguments()) {
    arg->Reset();
  }
  for (auto& [_, arg] : this->context_->GetShortArguments()) {
    arg->Reset();
  }
  for (auto& arg : this->context_->GetPosArguments()) {
    arg->Reset();
  }
}

bool ArgParser::Help() const {
  if (help_argument_) {
    return help_argument_->GetArgument().value_or(false);
  }
  return false;
}

std::shared_ptr<ArgumentHandler> ArgParser::BuildHandlerChain(
    std::shared_ptr<ParserContext> context) {
  auto long_handler = std::make_shared<LongArgHandler>();
  auto short_handler = std::make_shared<ShortArgHandler>();
  auto helper_handler =
      std::make_shared<HelpArgHandler>(this->help_argument_);
  auto positional_handler = std::make_shared<PositionalArgHandler>();
  auto space_handler = std::make_shared<SpaceSeparaterArgHandler>();
  auto equal_handlern = std::make_shared<EqualSeparaterArgHandler>();
  auto emptyHandler = std::make_shared<EmptySeparaterArgHandler>();
  auto type_handler =
      std::make_shared<TypeArgHandler>(TypeArgHandler(context));
  auto value_handler = std::make_shared<ValueArgHandler>();

  long_handler->SetNext(short_handler);
  short_handler->SetNext(helper_handler);
  helper_handler->SetNext(positional_handler);
  positional_handler->SetNext(equal_handlern);
  equal_handlern->SetNext(emptyHandler);
  emptyHandler->SetNext(type_handler);
  type_handler->SetNext(space_handler);
  space_handler->SetNext(value_handler);
  value_handler->SetBranch(emptyHandler);
  return long_handler;
}

std::string ArgParser::HelpDescription() const {
  std::ostringstream oss;
  if (this->name_.has_value()) {
    oss << *(this->name_) << kNewLine;
  }
  if (this->help_argument_ != nullptr) {
    if (this->help_argument_->GetDescription().has_value()) {
      oss << this->help_argument_->GetDescription().value() << kNewLine;
    }
  }
  oss << kNewLine;
  for (const auto& [_, arg] : context_->GetLongArguments())
    oss << arg->Help(argument_types_);
  if (this->help_argument_ != nullptr) {
    if (this->help_argument_->GetShortName().has_value()) {
      oss << kHyphen << this->help_argument_->GetShortName().value()
          << kComma << kSpace;
    }
    if (this->help_argument_->GetShortName().has_value()) {
      oss << kDoubleHyphen << this->help_argument_->GetLongName().value()
          << kComma << kSpace;
    }
    if (this->help_argument_->GetDescription().has_value()) {
      oss << this->help_argument_->GetDescription().value() << kNewLine;
    }
  }
  return oss.str();
}

template <Parsable T>
void ArgParser::RegisterCustomType(const std::string& description) {
  (*this->argument_types_)[typeid(T).name()] = description;
}

template <Parsable T>
ArgumentBuilder<T>& ArgParser::AddArgument(char short_name) {
  builder_ = std::make_shared<ConcreteArgumentBuilder<T>>(
      ConcreteArgumentBuilder<T>(context_));
  builder_->WithShortName(short_name);

  std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_)
      ->GetBuiltArgument()
      ->Accept(unique_visitor_);

  return *std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_);
}

template <Parsable T>
ArgumentBuilder<T>& ArgParser::AddArgument(
    char short_name, const std::string_view& long_name) {
  builder_ = std::make_shared<ConcreteArgumentBuilder<T>>(
      ConcreteArgumentBuilder<T>(context_));
  builder_->WithShortName(short_name).WithLongName(long_name);

  std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_)
      ->GetBuiltArgument()
      ->Accept(unique_visitor_);

  return *std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_);
}

template <Parsable T>
ArgumentBuilder<T>& ArgParser::AddArgument(
    const std::string_view& long_name) {
  builder_ = std::make_shared<ConcreteArgumentBuilder<T>>(
      ConcreteArgumentBuilder<T>(context_));
  builder_->WithLongName(long_name);

  std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_)
      ->GetBuiltArgument()
      ->Accept(unique_visitor_);

  return *std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_);
}

template <Parsable T>
ArgumentBuilder<T>& ArgParser::AddArgument(
    const std::string_view& long_name, const std::string_view& description) {
  builder_ = std::make_shared<ConcreteArgumentBuilder<T>>(
      ConcreteArgumentBuilder<T>(context_));
  builder_->WithLongName(long_name).WithDescription(description);

  std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_)
      ->GetBuiltArgument()
      ->Accept(unique_visitor_);

  return *std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_);
}

template <Parsable T>
ArgumentBuilder<T>& ArgParser::AddArgument(
    const char short_name, const std::string_view& long_name,
    const std::string_view& description) {
  builder_ = std::make_shared<ConcreteArgumentBuilder<T>>(
      ConcreteArgumentBuilder<T>(context_));
  builder_->WithShortName(short_name)
      .WithLongName(long_name)
      .WithDescription(description);

  std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_)
      ->GetBuiltArgument()
      ->Accept(unique_visitor_);

  return *std::dynamic_pointer_cast<ArgumentBuilder<T>>(builder_);
}

template <Parsable T>
std::optional<T> ArgParser::GetAnyFlag(const char short_name) {
  if (context_->GetShortArguments().find(short_name) !=
      context_->GetShortArguments().end()) {
    std::shared_ptr<ArgumentBase> base_ptr =
        context_->GetShortArguments().find(short_name)->second;
    std::shared_ptr<Argument<T>> arg =
        std::static_pointer_cast<Argument<T>>(base_ptr);

    if (!std::same_as<T, bool> && arg->IsMultivalue()) {
      return arg->GetArgument();
    }
  }
  throw std::runtime_error(std::string("Flag has no value ") + short_name);
}

template <Parsable T>
std::optional<T> ArgParser::GetValue(char short_name, std::size_t n) {
  if (context_->GetShortArguments().find(short_name) !=
      context_->GetShortArguments().end()) {
    std::shared_ptr<Argument<T>> arg = std::static_pointer_cast<Argument<T>>(
        context_->GetShortArguments().find(short_name)->second);
    if (arg->IsMultivalue() && arg->GetArguments().has_value() &&
        n < arg->GetArguments().value().get().size()) {
      return arg->GetArgument(n);
    }
    return arg->GetArgument();
  }
  return std::nullopt;
}

template <Parsable T>
std::optional<T> ArgParser::GetValue(const std::string_view& long_name,

                                     std::size_t n) {
  if (context_->GetLongArguments().find(std::string(long_name)) !=
      context_->GetLongArguments().end()) {
    std::shared_ptr<Argument<T>> arg = std::static_pointer_cast<Argument<T>>(
        context_->GetLongArguments().find(std::string(long_name))->second);
    if (arg->IsMultivalue() && arg->GetArguments().has_value() &&
        n < arg->GetArguments().value().get().size()) {
      return arg->GetArgument(n);
    }
    return arg->GetArgument();
  }
  return std::nullopt;
}

ArgumentBuilder<bool>& ArgParser::AddFlag(char short_name) {
  return AddArgument<bool>(short_name).Flag().Default(false);
}
ArgumentBuilder<bool>& ArgParser::AddFlag(
    char short_name, const std::string_view& long_name) {
  return AddArgument<bool>(short_name, long_name).Flag().Default(false);
}
ArgumentBuilder<bool>& ArgParser::AddFlag(
    const std::string_view& long_name) {
  return AddArgument<bool>(long_name).Flag().Default(false);
}

ArgumentBuilder<bool>& ArgParser::AddFlag(
    const std::string_view& long_name, const std::string_view& description) {
  return AddArgument<bool>(long_name, description).Flag().Default(false);
}

ArgumentBuilder<bool>& ArgParser::AddFlag(
    const char short_name, const std::string_view& long_name,
    const std::string_view& description) {
  return AddArgument<bool>(short_name, long_name, description)
      .Flag()
      .Default(false);
}

bool ArgParser::GetFlag(char short_name) {
  return *GetValue<bool>(short_name);
}

bool ArgParser::GetFlag(const std::string_view& long_name) {
  return *GetValue<bool>(long_name);
}

void ArgParser::AddHelp(const std::string_view& long_name,
                        const std::string_view& description) {
  this->help_argument_ = std::make_shared<Argument<bool>>();
  this->help_argument_->SetLongName(long_name);

  this->help_argument_->SetDescription(description);
  this->help_argument_->SetDefault(false);
}

void ArgParser::AddHelp(const char short_name,
                        const std::string_view& long_name,
                        const std::string_view& description) {
  AddHelp(long_name, description);
  this->help_argument_->SetShortName(short_name);
}

std::vector<std::string> ArgParser::ArgvToVec(int argc, char** argv) {
  std::vector<std::string> args;
  args.reserve(argc);
  for (std::int64_t i = 0; i < argc; ++i) {
    args.emplace_back(argv[i]);
  }
  return args;
}

}  // namespace ArgumentParser
