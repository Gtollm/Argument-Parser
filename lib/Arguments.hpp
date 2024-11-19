#pragma once

#include <concepts>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "AbstractArgument.hpp"
#include "ArgumentVisitor.hpp"

namespace ArgumentParser {

class ArgumentBase : public AbstractArgument {
 public:
  virtual ~ArgumentBase() = default;

  bool Accept(ArgumentVisitor& visitor) const;

  void SetShortName(char short_name) override;
  void SetLongName(const std::string_view& long_name) override;
  void SetDescription(const std::string_view& description) override;
  void MakeFlag() override;
  void MakePositional() override;
  void SetMultivalue(std::size_t min_amout) override;

  bool IsPositional() const override;
  bool IsMultivalue() const override;
  bool IsFlag() const override;
  std::optional<char> GetShortName() const override;
  std::optional<std::string> GetLongName() const override;
  std::optional<std::string> GetDescription() const override;
  std::optional<std::size_t> GetMultivalue() const override;

 protected:
  std::optional<char> short_name_;
  std::optional<std::string> long_name_;
  std::optional<std::string> description_;
  bool positional_ = false;
  bool flag_ = false;
  std::optional<std::size_t> miltivalue_;
};

template <Parsable T>
class Argument : public ArgumentBase {
 public:
  Argument();
  virtual ~Argument() = default;
  void SetDefault(const T& value);
  void SetDefault(const std::initializer_list<T>& values);
  void SetDefault(const std::vector<T>& values);

  void SetStorage(T& storage);
  void SetStorage(std::vector<T>& storage);

  void SetArgument(const T& value);
  std::optional<T> GetArgument();
  std::optional<T> GetArgument(std::size_t index);
  std::optional<const std::reference_wrapper<std::vector<T>>> GetArguments();

  std::string Help(
      std::shared_ptr<std::unordered_map<std::string, std::string>>
          argument_types) const override;
  void Reset() override;

  void SetRawArgument(const std::string_view& value) override;
  void SetRawArgument(const std::string& value) override;

  std::size_t GetValuesNumber() const override;
  std::size_t GetDefaultNumber() const override;

 private:
  std::vector<T> value_;
  std::optional<std::vector<T>> default_;

  std::variant<std::monostate, std::reference_wrapper<T>,
               std::reference_wrapper<std::vector<T>>>
      storage_;
};

}  // namespace ArgumentParser

#include "Arguments.tpp"
