#include <memory>
#include <string>

#include "ArgParserContext.hpp"
#include "Arguments.hpp"
#include "General.hpp"
namespace ArgumentParser {

ParserContext::ParserContext() : pos_index_(0) {}

void ParserContext::AddToLong(const std::shared_ptr<ArgumentBase>& arg) {
  this->long_args_.insert({std::string(*arg->GetLongName()), arg});
}

void ParserContext::AddToShort(const std::shared_ptr<ArgumentBase>& arg) {
  this->short_args_.insert({*arg->GetShortName(), arg});
}

void ParserContext::AddToPos(const std::shared_ptr<ArgumentBase>& arg) {
  this->pos_args_.push_back(arg);
}

const std::unordered_map<std::string, std::shared_ptr<ArgumentBase>>&
ParserContext::GetLongArguments() const {
  return this->long_args_;
}

const std::unordered_map<char, std::shared_ptr<ArgumentBase>>&
ParserContext::GetShortArguments() const {
  return this->short_args_;
}

const std::vector<std::shared_ptr<ArgumentBase>>&
ParserContext::GetPosArguments() const {
  return this->pos_args_;
}

std::size_t ParserContext::GetPosIndex() const { return pos_index_; }

void ParserContext::IncrementPosIndex() { pos_index_++; }

}  // namespace ArgumentParser
