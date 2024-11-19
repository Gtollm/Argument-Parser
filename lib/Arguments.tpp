#include <cstddef>
#include <functional>
#include <initializer_list>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>

#include "AbstractArgument.hpp"
#include "ArgParser.hpp"
#include "ArgumentVisitor.hpp"
#include "Arguments.hpp"
#include "General.hpp"

namespace ArgumentParser {

template <Parsable T>
std::string GetDataType(
    std::shared_ptr<std::unordered_map<std::string, std::string>>
        argument_types) {
  return argument_types->at(typeid(T).name());
}

bool ArgumentBase::IsFlag() const { return this->flag_; }

void ArgumentBase::SetShortName(char short_name) {
  this->short_name_ = short_name;
}

void ArgumentBase::SetLongName(const std::string_view& long_name) {
  this->long_name_ = std::string(long_name);
}

void ArgumentBase::SetDescription(const std::string_view& description) {
  this->description_ = std::string(description);
}

void ArgumentBase::MakeFlag() { this->flag_ = true; }

void ArgumentBase::MakePositional() { this->positional_ = true; }

void ArgumentBase::SetMultivalue(std::size_t min_amout) {
  this->miltivalue_ = min_amout;
}

bool ArgumentBase::IsPositional() const { return this->positional_; }

bool ArgumentBase::IsMultivalue() const {
  return this->miltivalue_.has_value();
}

std::optional<char> ArgumentBase::GetShortName() const {
  return this->short_name_;
}

std::optional<std::string> ArgumentBase::GetLongName() const {
  return this->long_name_;
}

std::optional<std::string> ArgumentBase::GetDescription() const {
  return this->description_;
}

std::optional<std::size_t> ArgumentBase::GetMultivalue() const {
  return this->miltivalue_;
}

bool ArgumentBase::Accept(ArgumentVisitor& visitor) const {
  return visitor.Visit(*this);
}

template <Parsable T>
Argument<T>::Argument() = default;

template <Parsable T>
void Argument<T>::SetDefault(const T& value) {
  if (this->default_.has_value()) {
    (*this->default_).clear();
    (*this->default_).push_back(value);
  } else {
    this->default_.emplace(std::initializer_list<T>({value}));
  }
}

template <Parsable T>
void Argument<T>::SetDefault(const std::initializer_list<T>& values) {
  if (!this->default_.has_value()) {
    *this->default_ = values;
  }
}

template <Parsable T>
void Argument<T>::SetDefault(const std::vector<T>& values) {
  if (!this->default_.has_value()) {
    *this->default_ = {};
  }
  for (auto value : values) {
    (*this->default_).push_back(value);
  }
}
template <Parsable T>
void Argument<T>::SetStorage(T& storage) {
  this->storage_ = storage;
  std::get<std::reference_wrapper<T>>(this->storage_).get() = T();
}

template <Parsable T>
void Argument<T>::SetStorage(std::vector<T>& storage) {
  this->storage_ = storage;
  std::get<std::reference_wrapper<std::vector<T>>>(this->storage_)
      .get()
      .clear();
}

template <Parsable T>
void Argument<T>::SetArgument(const T& value) {
  this->value_.push_back(value);
  if (std::holds_alternative<std::reference_wrapper<T>>(this->storage_)) {
    std::get<std::reference_wrapper<T>>(this->storage_).get() = value;
  } else if (std::holds_alternative<std::reference_wrapper<std::vector<T>>>(
                 this->storage_)) {
    std::get<std::reference_wrapper<std::vector<T>>>(this->storage_)
        .get()
        .push_back(value);
  }
}

template <Parsable T>
std::optional<T> Argument<T>::GetArgument() {
  if (!this->miltivalue_.has_value()) {
    if (!this->value_.empty()) {
      return std::optional<T>(this->value_.at(0));
    } else if (this->default_.has_value() && !(*this->default_).empty()) {
      return std::optional<T>((*this->default_).at(0));
    }
  }
  return std::nullopt;
}

template <Parsable T>
std::optional<T> Argument<T>::GetArgument(std::size_t index) {
  if (this->miltivalue_.has_value()) {
    if (this->value_.size() > index) {
      return std::optional<T>(this->value_.at(index));
    } else if (this->default_.has_value() &&
               (*this->default_).size() > index) {
      return std::optional<T>((*this->default_).at(index));
    }
  }
  return std::nullopt;
}

template <Parsable T>
std::optional<const std::reference_wrapper<std::vector<T>>>
Argument<T>::GetArguments() {
  if (this->miltivalue_.has_value()) {
    return this->value_;
  }
  return std::nullopt;
}

template <Parsable T>
std::string Argument<T>::Help(
    std::shared_ptr<std::unordered_map<std::string, std::string>>
        argument_types) const {
  std::ostringstream help_line;

  if (this->short_name_.has_value()) {
    help_line << kHyphen;
    help_line << *(this->short_name_);
  }
  if (this->long_name_.has_value()) {
    if (!help_line.str().empty()) {
      help_line << kComma << kSpace;
    }
    help_line << kDoubleHyphen;
    help_line << *(this->long_name_);
  }
  help_line << " = <";
  help_line << GetDataType<T>(argument_types);
  help_line << ">";
  if (this->description_.has_value()) {
    if (!help_line.str().empty()) {
      help_line << kComma << kSpace;
    }
    help_line << *(this->description_);
  }
  if (this->default_.has_value()) {
    help_line << "[ default: ";
    if (this->IsMultivalue()) {
      for (auto t : this->default_.value()) {
        help_line << t;
        help_line << kSpace;
      }
    } else {
      help_line << this->default_.value()[0] << kSpace;
    }
    help_line << "]";
  }
  if (this->positional_) {
    help_line << "[ positional ]";
  }
  if (this->miltivalue_.has_value()) {
    help_line << "[ repeated, min_args = ";
    help_line << *(this->miltivalue_);
    help_line << "]";
  }

  help_line << kNewLine;

  return help_line.str();
}
//     parser.HelpDescription(),
//     "My Parser\n"
//     "Some Description about program\n"
//     "\n"
//     "-i,  --input=<string>,  File path for input file [repeated, min
//     args = 1]\n"
//     "-s,  --flag1,  Use some logic [default = true]\n"
//     "-p,  --flag2,  Use some logic\n"
//     "     --number=<int>,  Some Number\n"
//     "\n"
//     "-h, --help Display this help and exit\n"

template <Parsable T>
void Argument<T>::Reset() {
  this->value_.clear();
  if (std::holds_alternative<std::reference_wrapper<T>>(this->storage_)) {
    std::get<std::reference_wrapper<T>>(this->storage_).get() = T();
  } else if (std::holds_alternative<std::reference_wrapper<std::vector<T>>>(
                 this->storage_)) {
    std::get<std::reference_wrapper<std::vector<T>>>(this->storage_)
        .get()
        .clear();
  }
  if (this->default_.has_value()) {
    this->SetDefault(*this->default_);
  }
}

template <Parsable T>
void Argument<T>::SetRawArgument(const std::string_view& value) {
  this->SetRawArgument(std::string(value));
}

template <Parsable T>
void Argument<T>::SetRawArgument(const std::string& value) {
  std::istringstream iss(value);
  T tmp;
  iss >> tmp;
  this->SetArgument(tmp);
}

template <Parsable T>
std::size_t Argument<T>::GetValuesNumber() const {
  return this->value_.size();
}

template <Parsable T>
std::size_t Argument<T>::GetDefaultNumber() const {
  return this->default_.value_or(std::vector<T>()).size();
}

}  // namespace ArgumentParser
