#pragma once

#include <concepts>
#include <cstddef>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>

#include "ArgumentVisitor.hpp">

template <typename T>
concept Parsable =
    std::same_as<T, std::string> || requires(const std::string& str, T t) {
      { std::istringstream{str} >> t };
      { std::to_string(t) == str };
      { std::to_string(t) } -> std::same_as<std::string>;
    };

static_assert(Parsable<int>, "int is parsable");
static_assert(Parsable<double>, "double is parsable");
static_assert(Parsable<std::string>, "string is parsable");
static_assert(Parsable<bool>, "bool is parsable");
static_assert(!Parsable<void>, "void is not parsable");

class ArgumentBase {
 public:
  virtual ~ArgumentBase() = default;

  virtual void Accept(ArgumentVisitor& visitor) const = 0;

  virtual std::string Help() const = 0;
  virtual void Reset() = 0;
  virtual void SetArgument(const std::string_view& value) = 0;
  virtual void Positional() = 0;
  virtual void Multivalue(std::size_t start = 0) = 0;

  ArgumentBase& WithShortName(const std::string_view& short_name);
  ArgumentBase& WithLongName(const std::string_view& long_name);
  ArgumentBase& WithDescription(const std::string_view& description_);

  void SetShortName(const std::string_view& short_name);
  void SetLongName(const std::string_view& long_name);
  void SetDescription(const std::string_view& description_);

  bool IsPositional() const { return positional_; }
  std::optional<char> GetShortName() const;
  std::optional<std::string> GetLongName() const;
  std::optional<std::string> GetDescription() const;

 protected:
  std::optional<char> short_name_;
  std::optional<std::string> long_name_;
  std::optional<std::string> description_;
  bool positional_ = false;
  std::size_t start_ = 0;
};

class Flag : public ArgumentBase {
 public:
  void SetArgument(bool value);
  bool GetArgument();
  void Default(bool value);
  void StoreValue(bool* storage);

  std::string Help() const override;
  void Reset() override;

  void SetArgument(const std::string_view& value) override;
  void Positional() override;
  void Multivalue(std::size_t start = 0) override;

 private:
  bool value_;
  bool* storage_;
};

template <Parsable T>
class TypedArgument : public ArgumentBase {
 public:
  virtual void SetArgument(const T& value) = 0;
  virtual T GetArgument() = 0;
  virtual void Default(const T& value) = 0;
  virtual void StoreValue(T* storage) = 0;

  std::string Help() const override;
  void Reset() override;

  void SetArgument(const std::string_view& value) override;
  void Positional() override;
  void Multivalue(std::size_t start = 0) override;

 private:
  T value_;
  T* storage_;
};
