#include <cstddef>
#include <string>
#include <utility>

#include "ArgParserContext.hpp"
#include "ArgumentHandler.hpp"
#include "General.hpp"

namespace ArgumentParser {

const std::string kTrueStr = "1";

void ArgumentHandler::SetNext(std::shared_ptr<ArgumentHandler> next) {
  next_ = next;
}

LongArgHandler::LongArgHandler() = default;

bool LongArgHandler::Handle(ParsingChainContext& context) {
  if (context.CurrentToken().starts_with("--")) {
    context.type = ParsingChainContext::ArgPrefixType::LONG;
    context.key = context.CurrentToken().substr(2);
    return next_ ? next_->Handle(context) : false;
  }
  return next_ ? next_->Handle(context) : false;
}

ShortArgHandler::ShortArgHandler() = default;

bool ShortArgHandler::Handle(ParsingChainContext& context) {
  if (context.CurrentToken().starts_with("-") &&
      !context.CurrentToken().starts_with("--")) {
    context.type = ParsingChainContext::ArgPrefixType::SHORT;
    context.key = context.CurrentToken().substr(1);
    return next_ ? next_->Handle(context) : false;
  }
  return next_ ? next_->Handle(context) : false;
}

HelpArgHandler::HelpArgHandler(std::shared_ptr<Argument<bool>> helper)
    : helper_(helper) {}

bool HelpArgHandler::Handle(ParsingChainContext& context) {
  if (helper_ != nullptr &&
      (context.key ==
           std::to_string(helper_->GetShortName().value_or(' ')) ||
       context.key == helper_->GetLongName().value_or(""))) {
    helper_->SetArgument(true);
    return true;
  }
  return next_ ? next_->Handle(context) : false;
}

PositionalArgHandler::PositionalArgHandler() = default;

bool PositionalArgHandler::Handle(ParsingChainContext& context) {
  if (!context.CurrentToken().starts_with("-")) {
    context.type = ParsingChainContext::ArgPrefixType::POSITIONAL;
    context.value = context.CurrentToken();
    return next_ ? next_->Handle(context) : false;
  }
  return next_ ? next_->Handle(context) : false;
}

EqualSeparaterArgHandler::EqualSeparaterArgHandler() = default;

bool EqualSeparaterArgHandler::Handle(ParsingChainContext& context) {
  auto pos = context.key.find('=');
  if (pos != std::string::npos) {
    context.value = context.key.substr(pos + 1);
    context.key = context.key.substr(0, pos);
    return next_ ? next_->Handle(context) : true;
  }
  return next_ ? next_->Handle(context) : false;
}

EmptySeparaterArgHandler::EmptySeparaterArgHandler() = default;

bool EmptySeparaterArgHandler::Handle(ParsingChainContext& context) {
  if (context.type == ParsingChainContext::ArgPrefixType::SHORT &&
      context.key.size() > 1) {
    context.value = context.key.substr(context.short_index + 1);
    context.key =
        context.key.substr(context.short_index, context.short_index + 1);
    return next_ ? next_->Handle(context) : true;
  }
  return next_ ? next_->Handle(context) : false;
}

SpaceSeparaterArgHandler::SpaceSeparaterArgHandler() = default;

bool SpaceSeparaterArgHandler::Handle(ParsingChainContext& context) {
  if (context.value.empty() && context.Advance()) {
    context.value = context.CurrentToken();
    return next_ ? next_->Handle(context) : true;
  }
  return next_ ? next_->Handle(context) : false;
}

TypeArgHandler::TypeArgHandler(
    std::shared_ptr<ParserContext> parser_context) {
  parser_context_ = parser_context;
}

bool TypeArgHandler::Handle(ParsingChainContext& context) {
  switch (context.type) {
    case ParsingChainContext::ArgPrefixType::LONG:
      if (auto it =
              this->parser_context_->GetLongArguments().find(context.key);
          it != this->parser_context_->GetLongArguments().end()) {
        context.matched_argument = it->second;
        return next_ ? next_->Handle(context) : true;
      }
      break;
    case ParsingChainContext::ArgPrefixType::SHORT:
      if (auto it = this->parser_context_->GetShortArguments().find(
              context.key[0]);
          it != this->parser_context_->GetShortArguments().end()) {
        context.matched_argument = it->second;
        return next_ ? next_->Handle(context) : true;
      }
      break;
    case ParsingChainContext::ArgPrefixType::POSITIONAL:
      if (this->parser_context_->GetPosIndex() <
          this->parser_context_->GetPosArguments().size()) {
        context.matched_argument =
            this->parser_context_
                ->GetPosArguments()[this->parser_context_->GetPosIndex()];
        if (!context.matched_argument->GetMultivalue()) {
          this->parser_context_->IncrementPosIndex();
        }
        return next_ ? next_->Handle(context) : true;
      }
      break;
  }
  return false;
}

ValueArgHandler::ValueArgHandler() = default;

bool ValueArgHandler::Handle(ParsingChainContext& context) {
  if (context.matched_argument) {
    if (context.matched_argument->IsFlag()) {
      if (context.type == ParsingChainContext::ArgPrefixType::SHORT &&
          context.value.size() >= 1 && !context.value.starts_with("-")) {
        context.matched_argument->SetRawArgument(kTrueStr);
        context.key = context.value;
        context.value = context.key.substr(1);
        context.matched_argument = nullptr;
        return branch_ ? branch_->Handle(context) : true;
      }
      if (context.value.starts_with("-")) {
        context.Back();
      }
      context.matched_argument->SetRawArgument(kTrueStr);
      return next_ ? next_->Handle(context) : true;
    }
    context.matched_argument->SetRawArgument(context.value);
    return next_ ? next_->Handle(context) : true;
  }
  return false;
}

void ValueArgHandler::SetBranch(std::shared_ptr<ArgumentHandler> branch) {
  this->branch_ = branch;
}

}  // namespace ArgumentParser
