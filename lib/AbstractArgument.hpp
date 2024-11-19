#pragma once

#include <optional>
#include <string>
#include <unordered_map>

#include "General.hpp"

namespace ArgumentParser {

class AbstractArgument {
 public:
  virtual ~AbstractArgument() = default;

  virtual std::string Help(
      std::shared_ptr<std::unordered_map<std::string, std::string>>
          argument_types) const = 0;
  virtual void Reset() = 0;
  virtual void SetRawArgument(const std::string_view& value) = 0;
  virtual void SetRawArgument(const std::string& value) = 0;
  virtual bool IsFlag() const = 0;

  virtual void SetShortName(char short_name) = 0;
  virtual void SetLongName(const std::string_view& long_name) = 0;
  virtual void SetDescription(const std::string_view& description_) = 0;
  virtual void MakeFlag() = 0;
  virtual void MakePositional() = 0;
  virtual void SetMultivalue(std::size_t min_amout) = 0;

  virtual bool IsPositional() const = 0;
  virtual bool IsMultivalue() const = 0;
  virtual std::optional<char> GetShortName() const = 0;
  virtual std::optional<std::string> GetLongName() const = 0;
  virtual std::optional<std::string> GetDescription() const = 0;
  virtual std::optional<std::size_t> GetMultivalue() const = 0;

  virtual std::size_t GetValuesNumber() const = 0;
  virtual std::size_t GetDefaultNumber() const = 0;
};

}  // namespace ArgumentParser

class ArgumentBase;

template <ArgumentParser::Parsable T>
class Argument;
