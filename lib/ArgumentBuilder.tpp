#include <memory>
#include <utility>
#include <vector>

#include "ArgParser.hpp"
#include "ArgParserContext.hpp"
#include "ArgumentBuilder.hpp"
#include "Arguments.hpp"
#include "General.hpp"

namespace ArgumentParser {

template <Parsable T>
ConcreteArgumentBuilder<T>::ConcreteArgumentBuilder() {
  this->Reset();
}

template <Parsable T>
ConcreteArgumentBuilder<T>::ConcreteArgumentBuilder(
    std::shared_ptr<ParserContext> context)
    : parser_context_(context) {
  this->Reset();
}

template <Parsable T>
ConcreteArgumentBuilder<T>::~ConcreteArgumentBuilder() = default;

template <Parsable T>
void ConcreteArgumentBuilder<T>::Reset() {
  this->argument_ = std::make_shared<Argument<T>>();
  if (std::is_same_v<T, bool>) {
    this->argument_->MakeFlag();
  }
}

template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::Positional() {
  this->argument_->MakePositional();
  this->parser_context_->AddToPos(this->argument_);
  return *this;
}
template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::MultiValue(
    std::size_t min_amount) {
  this->argument_->SetMultivalue(min_amount);
  return *this;
}
template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::WithShortName(
    const char short_name) {
  this->argument_->SetShortName(short_name);
  this->parser_context_->AddToShort(this->argument_);
  return *this;
}

template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::WithLongName(
    const std::string_view& long_name) {
  this->argument_->SetLongName(long_name);
  this->parser_context_->AddToLong(this->argument_);
  return *this;
}

template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::WithDescription(
    const std::string_view& description) {
  this->argument_->SetDescription(description);
  return *this;
}

template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::Default(const T& value) {
  this->argument_->SetDefault(value);
  return *this;
}

template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::StoreValue(T& storage) {
  this->argument_->SetStorage(storage);
  return *this;
}

template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::StoreValues(
    std::vector<T>& storage) {
  this->argument_->SetStorage(storage);
  return *this;
}

template <Parsable T>
ArgumentBuilder<T>& ConcreteArgumentBuilder<T>::Flag() {
  this->argument_->MakeFlag();
  return *this;
}

template <Parsable T>
std::shared_ptr<Argument<T>> ConcreteArgumentBuilder<T>::GetBuiltArgument() {
  return this->argument_;
}

}  // namespace ArgumentParser
