#pragma once

#include <memory>
#include <vector>

#include "ArgParserContext.hpp"
#include "Arguments.hpp"
#include "General.hpp"

namespace ArgumentParser {

class AbstractBuilder {
 public:
  virtual ~AbstractBuilder() = default;
  virtual AbstractBuilder& Positional() = 0;
  virtual AbstractBuilder& MultiValue(std::size_t min_amount = 0) = 0;
  virtual AbstractBuilder& WithShortName(const char short_name) = 0;
  virtual AbstractBuilder& WithLongName(
      const std::string_view& long_name) = 0;
  virtual AbstractBuilder& WithDescription(
      const std::string_view& description_) = 0;

  virtual AbstractBuilder& Flag() = 0;
};

template <Parsable T>
class ArgumentBuilder : public AbstractBuilder {
 public:
  virtual ~ArgumentBuilder() = default;
  virtual ArgumentBuilder<T>& Positional() = 0;
  virtual ArgumentBuilder<T>& MultiValue(std::size_t min_amount = 0) = 0;
  virtual ArgumentBuilder<T>& WithShortName(const char short_name) = 0;
  virtual ArgumentBuilder<T>& WithLongName(
      const std::string_view& long_name) = 0;
  virtual ArgumentBuilder<T>& WithDescription(
      const std::string_view& description_) = 0;
  virtual ArgumentBuilder<T>& Default(const T& value) = 0;
  virtual ArgumentBuilder<T>& StoreValue(T& storage) = 0;
  virtual ArgumentBuilder<T>& StoreValues(std::vector<T>& storage) = 0;
  virtual ArgumentBuilder<T>& Flag() = 0;

  virtual std::shared_ptr<Argument<T>> GetBuiltArgument() = 0;
};

template <Parsable T>
class ConcreteArgumentBuilder : public ArgumentBuilder<T> {
 private:
  std::shared_ptr<Argument<T>> argument_;
  std::shared_ptr<ParserContext> parser_context_;

 public:
  ConcreteArgumentBuilder();

  explicit ConcreteArgumentBuilder(std::shared_ptr<ParserContext> context);

  ~ConcreteArgumentBuilder();

  void Reset();

  ArgumentBuilder<T>& Positional() override;
  ArgumentBuilder<T>& MultiValue(std::size_t min_amount = 0) override;
  ArgumentBuilder<T>& WithShortName(char short_name) override;
  ArgumentBuilder<T>& WithLongName(
      const std::string_view& long_name) override;
  ArgumentBuilder<T>& WithDescription(
      const std::string_view& description) override;
  ArgumentBuilder<T>& Default(const T& value) override;
  ArgumentBuilder<T>& StoreValue(T& storage) override;
  ArgumentBuilder<T>& StoreValues(std::vector<T>& storage) override;
  ArgumentBuilder<T>& Flag() override;
  std::shared_ptr<Argument<T>> GetBuiltArgument() override;
};

}  // namespace ArgumentParser

#include "ArgumentBuilder.tpp"
